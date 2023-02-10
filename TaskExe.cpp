#include <iostream>
#include <thread>
#include <filesystem>

#include "ProjectSettings.h"
#include "TaskExe.h"

namespace fs = std::filesystem;
using BUCKET = std::list<struct Task>;
using ALBUM_MAP = std::unordered_map<int, std::string>;

/* Returns the bucket index to process next in the task.
   This function alters a state in TASK_MANAGER and is protected
   by mutex. */
static const size_t pick_up_bucket (TaskManager &task_manager);

/* Returns the number of items successuflly exported in the
   BUCKET. */
static const int export_bucket (BUCKET &, const ALBUM_MAP &,
                                std::string output_prefix);

/* Decrements the remaining number of tasks in TASK_MANAGER.
   This function alters a state in TASK_MANAGER and is protected
   by mutex. */
static void complete_task (TaskManager &task_manager, const int, const int);

void
TaskExe::process_bucket (TaskManager &task_manager, enum Job job)
{
  try
  {
    /* PICK_UP_BUCKET syncs with other threads. */
    size_t idx = pick_up_bucket (task_manager);

    std::cout << "pick up bucket " << idx
              << " by thread " << std::this_thread::get_id ()
              << std::endl;
    auto &bucket = task_manager.tasks.at (idx);
    size_t success_cnt;

    /* Process the tasks based on JOB. */
    switch (job) {
      case EXPORT:
        success_cnt = export_bucket (bucket, task_manager.album_map,
                                     task_manager.output_prefix);
        break;
      case CONVERT_THEN_EXPORT:
        break;
      case CONVERT_THEN_UPLOAD:
        break;
      default:
        break;
      }

    auto error_cnt = bucket.size () - success_cnt;

    /* COMPLETE_TASK syncs with other threads. */
    complete_task (task_manager, success_cnt, error_cnt);

    /* Recursively calls PROCESS_BUCKET until all tasks are completed. */
    process_bucket (task_manager, job);
  }
  catch (std::exception)
  {
    return;
  }

}

static const size_t
pick_up_bucket (TaskManager &task_manager)
{
  const size_t last_bucket_idx = task_manager.tasks.size () - 1;
  const std::lock_guard<std::mutex> lock (std::ref (task_manager.buk_m));
  const size_t buk_idx = task_manager.buk_to_process;

  // TODO (refactor) raise custom exception instead of the standard one.
  if (buk_idx > last_bucket_idx)
    throw std::exception ();

  task_manager.buk_to_process++;

  return buk_idx;
}

static void
complete_task (TaskManager &task_manager, const int success_cnt,
               const int error_cnt)
{
  const std::lock_guard<std::mutex> lock (std::ref (task_manager.cv_m));
  auto processed_cnt = success_cnt + error_cnt;
  task_manager.decrement_tasks (processed_cnt);
  task_manager.cv.notify_one ();
}

static const int
export_bucket (BUCKET &bucket, const ALBUM_MAP &album_map,
               std::string output_prefix)
{
  int success_cnt = 0;

  for (auto &item : bucket)
    {
      auto album_name = album_map.find (item.album_pk)->second;
      const auto album_prefix = output_prefix + "/" + album_name;
      fs::create_directory (album_prefix);
      const auto input_path = PROJECT::PHOTO_ORIGINALS + "/" + item.file_path ();
      const auto output_path = album_prefix + "/" + item.file_name;
      try {
        auto result = fs::copy_file (input_path, output_path);
        if (result)
          {
            item.instruction = Instruction::COMPLETED;
            std::cout << item.file_path () << " was copied" << std::endl;
            success_cnt++;
          }
      } catch (fs::filesystem_error &e) {
        item.instruction = Instruction::TO_EXPORT_ERROR;
        std::cerr << "Copy ops fails - "
                  << e.what () << std::endl;
      }
    }

  return success_cnt;
}
