#include <QMessageBox>
#include <QProcess> /* Used to open the preview app. */
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableView>
#include <filesystem>
#include <iostream>
#include <list>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include "./ui_mainwindow.h"
#include "DbManager.h"
#include "ProjectSettings.h"
#include "Task.h"
#include "TaskExe.h"
#include "mainwindow.h"

namespace fs = std::filesystem;

/* Number of items in a bucket for the task manager. */
#define TASK_BUCKET_SIZE 10

/* Number of concurrent threads working on the tasks. */
#define WORKER_CNT 10

/* Updates progress while the tasks are processed in TASK_MANAGER.
   The function synchronizes with worker threads that are processing
   the tasks. */
static void update_progress (TaskManager &task_manager);

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent), ui (new Ui::MainWindow)
{
  ui->setupUi (this);
  _key_press_eater = new KeyPressEater (ui->photoTableView, parent);
  ui->dbPathLineEdit->setText (
      QString::fromStdString (PROJECT::PHOTO_DB_NAME));
  ui->photoTableView->installEventFilter (_key_press_eater);
  connect (ui->connectDbButton, &QPushButton::released, this,
           &MainWindow::fetch_album_list);
  connect (ui->nextStepPushButton, &QPushButton::released, this,
           &MainWindow::act_on_next);
}

void
MainWindow::fetch_album_list ()
{
  auto db_path{ ui->dbPathLineEdit->text () };
  _db = DbManager{ db_path };
  auto query = _db.album_list_query ();
  _album_list_model = new CheckableSqlQueryModel (this);
  auto db_instance = _db.db ();
  _album_list_model->setQuery (query, db_instance);
  ui->albumListView->setModel (_album_list_model);

  /* Display the ZTITLE column. */
  int col_no = _album_list_model->record ().indexOf ("ZTITLE");
  ui->albumListView->setModelColumn (col_no);

  /* Add signal to handle list view selectionChanged
     event only when query model is hooked up with
     the list view. */
  connect (ui->albumListView->selectionModel (),
           &QItemSelectionModel::selectionChanged, this,
           &MainWindow::fetch_photos);
}

void
MainWindow::fetch_photos (const QItemSelection &current,
                          const QItemSelection &previous)
{
  /* Release previous query model. */
  if (_photos_model != nullptr)
    {
      _photos_model->clear ();
      delete _photos_model;
      _photos_model = nullptr;
    }

  /* No-op if current selection is empty. */
  if (current.empty ())
    return;

  auto indices = current.indexes ();
  auto model = ui->albumListView->model ();

  /* Fetch the album PK. */
  auto data = model->data (indices.first ().siblingAtColumn (0));

  /* Build the SQL statement to query photos for the album PK. */
  auto query_str = _db.photo_query (data.toInt ());
  QSqlQuery query{ query_str, _db.db () };
  query.exec ();

  _photos_model = new QSqlQueryModel (this);
  _photos_model->setQuery (std::move (query));
  ui->photoTableView->setModel (_photos_model);
}

void
MainWindow::act_on_next ()
{
  if (_album_list_model == nullptr
      || _album_list_model->selected_items ().empty ())
    {
      QMessageBox msg;
      msg.setText ("No album is selected");
      msg.exec ();
      return;
    }

  /* Fetch the album PK. */
  auto selected_indices = _album_list_model->selected_items ();
  std::list<int> pk_list;
  std::unordered_map<int, std::string> albums_map{};

  /* To check duplicated album names. */
  std::unordered_set<std::string> album_set{};

  /* Builds the album map. */
  for (auto it = selected_indices.cbegin (); it != selected_indices.cend ();
       ++it)
    {
      QModelIndex idx = it->sibling (it->row (), 2);
      std::string album_name = _album_list_model->data (idx, Qt::DisplayRole)
                                   .toString ()
                                   .toStdString ();

      idx = it->sibling (it->row (), 0);
      int album_pk = _album_list_model->data (idx, Qt::DisplayRole).toInt ();

      /* If there is duplicated album_name, report an error. */
      if (album_set.count (album_name) > 0)
        {
          QMessageBox msg;
          msg.setText (
              QString::fromStdString ("Duplicated album name: " + album_name));
          msg.exec ();
          return;
        }
      else
        album_set.insert (album_name);
      albums_map.insert (std::make_pair (album_pk, album_name));
      pk_list.push_back (album_pk);
    }

  auto query_str = _db.bulk_photo_query (pk_list);
  QSqlQuery query{ query_str, _db.db () };

  /* Prepares for the column positions to fetch from QSqlQuery. */
  int pk_col = query.record ().indexOf (_db.album_table ());
  int direction_col = query.record ().indexOf (_db.z_asset_directory ());
  int filename_col = query.record ().indexOf (_db.z_asset_filename ());
  TaskManager task_manager (this);

  /* Build the tasks and store them in a task manager. */
  size_t counter = 0;
  while (query.next ())
    {
      auto file_dir = query.value (direction_col).toString ().toStdString ();
      auto file_name = query.value (filename_col).toString ().toStdString ();
      auto pk = query.value (pk_col).toInt ();
      struct Task task
      {
        file_dir, file_name, pk, Instruction::TO_EXPORT
      };

      /* Init a new list to store task or add new task to
         the existing list. */
      int bucket_idx = counter / TASK_BUCKET_SIZE;
      if (task_manager.tasks.size () < bucket_idx + 1)
        task_manager.tasks.push_back (std::list<struct Task>{ task });
      else
        task_manager.tasks[bucket_idx].push_back (task);
      counter++;
    }
  task_manager.set_total_tasks (counter);
  task_manager.album_map = albums_map;
  auto job = prepare_job (task_manager);

  /* Spawns threads to process the task. */
  std::thread thread_pool[WORKER_CNT];
  for (size_t worker = 0; worker < WORKER_CNT; worker++)
    {
      thread_pool[worker] = std::thread (TaskExe::process_bucket,
                                         std::ref (task_manager), job);
    }

  /* UPDATE_PROGRESS () blocks until all tasks are processed. */
  update_progress (task_manager);

  for (size_t worker = 0; worker < WORKER_CNT; worker++)
    thread_pool[worker].join ();
}

const enum TaskExe::Job
MainWindow::prepare_job (TaskManager &task_manager)
{
  // TODO: support user input for export path (i.e. edit box to store
  // OUTPUT_PREFIX)
  std::ostringstream output_prefix;
  output_prefix << getenv ("HOME") << "/Documents/export";
  fs::create_directory (output_prefix.str ());
  task_manager.output_prefix = output_prefix.str ();
  return TaskExe::Job::EXPORT;
}

MainWindow::~MainWindow ()
{
  delete _key_press_eater;
  delete _album_list_model;
  delete _photos_model;
  delete ui;
}

static void
update_progress (TaskManager &task_manager)
{
  size_t remaining_cnt;
  do
    {
      std::cout << "awaiting tasks to be processed..." << std::endl;
      std::unique_lock<std::mutex> cv_m (task_manager.cv_m);
      auto &ready = task_manager.ready;
      task_manager.cv.wait (cv_m, [&ready] { return ready; });
      task_manager.update_progress ();
      remaining_cnt = task_manager.remaining_cnt ();
    }
  while (remaining_cnt > 0);
}
