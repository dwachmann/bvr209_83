/*
 * $Id$
 * 
 * A class for handling Windows task.
 * 
 * Copyright (C) 2009 Dorothea Wachmann
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
#include "os.h"
#include "util/taskscheduler.h"
#include "util/logstream.h"
#include "util/guid.h"
#include "util/combuffer.h"
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"
#include <sstream>

using namespace bvr20983;

namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    Task::Task(TaskScheduler& taskScheduler) :
      m_taskScheduler(taskScheduler)
    { }

    /**
     *
     */
    Task::~Task()
    { 
    }

    /**
     *
     */
    void Task::NewTask(LPCTSTR taskName)
    { THROW_COMEXCEPTION( m_taskScheduler.GetTaskScheduler()->NewWorkItem(taskName,CLSID_CTask,IID_ITask,(LPUNKNOWN*)&m_task) );
    } // of Task::NewTask()

    /**
     *
     */
    void Task::Commit()
    { if( !m_task.IsNULL() )
      { COMPtr<IPersistFile> pPersistFile;

        THROW_COMEXCEPTION( m_task->QueryInterface(IID_IPersistFile,(LPVOID*)&pPersistFile) );
        THROW_COMEXCEPTION( pPersistFile->Save(NULL,TRUE) );
      } // of if
    } // of Task::Commit()

    /**
     *
     */
    TaskScheduler::TaskScheduler() :
      m_taskScheduler(CLSID_CTaskScheduler,IID_ITaskScheduler,CLSCTX_INPROC_SERVER)
    { 
    }

    /**
     *
     */
    TaskScheduler::~TaskScheduler()
    { 
    }

  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
