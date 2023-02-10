#include "Task.h"
#include <iostream>

TaskManager::TaskManager (QObject *parent) : QObject (parent)
{
  _pd = new QProgressDialog ("Copy files...", "Cancel", 0, 200000);
  _pd->setWindowModality (Qt::WindowModal);
  _pd->setMinimumDuration (0);
  connect (_pd, &QProgressDialog::canceled,
           this, &TaskManager::cancel_tasks);
}

void
TaskManager::set_total_tasks (size_t total_tasks_cnt)
{
  _remaining_cnt = total_tasks_cnt;
  _pd->setWindowModality (Qt::WindowModal);
  _pd->setMinimumDuration (0);
  _pd->setMaximum (total_tasks_cnt);
}

void
TaskManager::cancel_tasks ()
{
  qDebug () << "to be canceled.";
}

const size_t
TaskManager::remaining_cnt () const
{
  return _remaining_cnt;
}

void
TaskManager::decrement_tasks (size_t complete_cnt)
{
  _remaining_cnt -= complete_cnt;
  _processed_cnt += complete_cnt;
  ready = true;
}

void
TaskManager::update_progress ()
{
  auto cur_value = _pd->value ();
  for (auto i = 0; i < _processed_cnt; i++)
    {
      std::cout << "set value to " << i << std::endl;
      _pd->setValue (cur_value + i);
    }

  _pd->setValue (cur_value + _processed_cnt);
  _processed_cnt = 0;
  ready = false;
}

TaskManager::~TaskManager ()
{
  delete _pd;
}
