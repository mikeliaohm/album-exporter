#ifndef FILECONVERTER_H
#define FILECONVERTER_H

#include <QObject>
#include <functional>

#include "Task.h"

namespace TaskExe
{
  template <typename T>
  using Func = std::function<T(T)>;

enum Job
{
  EXPORT,
  CONVERT_THEN_EXPORT,
  CONVERT_THEN_UPLOAD
};

template <typename T>
void process_bucket1 (std::list<struct Task> &task_buk, Func<T> processor,
                      Func<T> completion_handler);

void process_bucket (TaskManager &task_manager, enum Job);
}

#endif // FILECONVERTER_H
