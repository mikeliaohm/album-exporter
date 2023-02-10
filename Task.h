#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QProgressDialog>
#include <list>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <condition_variable>

enum Instruction
{
  TO_EXPORT,
  TO_UPLOAD,
  COMPLETED,
  TO_EXPORT_ERROR,
  TO_UPLOAD_ERROR
};

struct Task
{
  std::string file_dir;         /* Directory. */
  std::string file_name;        /* Filename. */
  int album_pk;                 /* Album pk. */
  enum Instruction instruction; /* Instruction to perform. */

  std::string file_path () const { return file_dir + "/" + file_name; }
};

/* Data structure to wrap the Tasks to be processed and
   contain synchronization tools to manage multi-threading
   processing of the tasks. */
class TaskManager : public QObject
{
private:
  QProgressDialog *_pd;          /* Progress dialog. */
  size_t _remaining_cnt{};       /* Remaining number of tasks. */
  size_t _processed_cnt{};       /* Number of tasks processed. */
  size_t _error_cnt{};           /* Number of errors encountered. */
private slots:
  void cancel_tasks ();

public:
  TaskManager (QObject *parent);
  ~TaskManager ();
  std::vector<std::list<struct Task> > tasks;
  std::unordered_map<int, std::string> album_map; /* Maps album PK to name; */
  std::string output_prefix;    /* Output path of processed files (if any); */

  size_t buk_to_process = 0;    /* Bucket index to be processed. */
  std::mutex buk_m{};           /* Synchronize BUK_TO_PROCESS. */

  bool ready = false;           /* True if any task has been performed. */
  std::condition_variable cv;   /* Cond var for task processing synchronization. */
  std::mutex cv_m{};            /* Mutex for CV. */

  /* Sets the total number of task in _PD. */
  void set_total_tasks (size_t tasks_cnt);

  /* Decrements _REMAINING_CNT and increments _PROCESSED_CNT. Not
     thread-safe. */
  void decrement_tasks (size_t complete_cnt);

  /* Updates progress in _PD and restores _PROCESS_CNT to 0 and
     READY to false. Not thread-safe. */
  void update_progress ();

  /* Returns the current number of tasks to be processed. */
  const size_t remaining_cnt () const;
};

#endif // TASK_H
