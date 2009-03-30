/*
 * $Id$
 * 
 * Copyright (C) 2008-2009 Dorothea Wachmann
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include <mstask.h>
#include "util/comptr.h"
#include "util/guid.h"

namespace bvr20983
{
  namespace util
  {
    class TaskScheduler;

    /**
     *
     */
    class Task
    {
      public:
        Task(TaskScheduler& taskScheduler);
        ~Task();

        void NewTask(LPCTSTR taskName);
        void Commit();

        COMPtr<ITask>& GetTask()
        { return m_task; }

      private:
        TaskScheduler& m_taskScheduler;
        COMPtr<ITask>  m_task;
    }; // of class Task

    /**
     *
     */
    class TaskScheduler
    {
      public:
        TaskScheduler();
        ~TaskScheduler();

        COMPtr<ITaskScheduler>& GetTaskScheduler()
        { return m_taskScheduler; }

        void CreateTask(Task& task,LPCTSTR taskName,LPCTSTR parameters,HINSTANCE hDll,LPCTSTR comment);
        void RemoveTask(LPCTSTR taskName);

      private:
        COMPtr<ITaskScheduler> m_taskScheduler;

    }; // of class TaskScheduler
  } // of namespace util
} // of namespace bvr20983
#endif // TASKSCHEDULER_H
/*==========================END-OF-FILE===================================*/
