#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QItemSelection>
#include <QMainWindow>
#include <QPushButton>
#include <QProgressDialog>

#include "CheckableSqlQueryModel.h"
#include "DbManager.h"
#include "KeyPressEater.h"
#include "TaskExe.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow (QWidget *parent = nullptr);
  ~MainWindow ();
private slots:
  void fetch_album_list ();
  void fetch_photos (const QItemSelection &, const QItemSelection &);
  void act_on_next ();

private:
  Ui::MainWindow *ui;
  CheckableSqlQueryModel *_album_list_model{};
  QSqlQueryModel *_photos_model{};
  KeyPressEater *_key_press_eater{};
  DbManager _db;

  /* Collects relevant information about how to process
     the tasks in TASK_MANAGER. */
  const enum TaskExe::Job prepare_job (TaskManager &task_manager);
};
#endif // MAINWINDOW_H
