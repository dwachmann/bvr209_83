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
#include <shlobj.h>
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

    /**
     *
     */
    void TaskScheduler::CreateTask(Task& task,LPCTSTR taskName,LPCTSTR parameters,HINSTANCE hDll,LPCTSTR comment)
    { TCHAR path[MAX_PATH];

      ::SHGetFolderPath(NULL,CSIDL_SYSTEM , NULL,SHGFP_TYPE_CURRENT,path);
      TString rundll32Path(path);
      rundll32Path += _T("\\rundll32.exe");

      ::GetModuleFileName(hDll,path,MAX_PATH);
      TString completeParameters(path);
      completeParameters += parameters;

      task.NewTask(taskName);

      THROW_COMEXCEPTION( task.GetTask()->SetApplicationName(rundll32Path.c_str()) );
      THROW_COMEXCEPTION( task.GetTask()->SetParameters(completeParameters.c_str()) );
      THROW_COMEXCEPTION( task.GetTask()->SetFlags(TASK_FLAG_RUN_ONLY_IF_LOGGED_ON) );
      //THROW_COMEXCEPTION( task.GetTask()->SetFlags(TASK_FLAG_DISABLED | TASK_FLAG_RUN_ONLY_IF_LOGGED_ON) );

      ::SHGetFolderPath(NULL,CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL,SHGFP_TYPE_CURRENT,path);
      THROW_COMEXCEPTION( task.GetTask()->SetWorkingDirectory(path) );
      THROW_COMEXCEPTION( task.GetTask()->SetComment(comment) );

      TASK_TRIGGER pTrigger;
      ::ZeroMemory(&pTrigger, sizeof (TASK_TRIGGER));
      
      // Add code to set trigger structure?
      pTrigger.wBeginDay =1;                  // Required
      pTrigger.wBeginMonth =1;                // Required
      pTrigger.wBeginYear =1999;              // Required
      pTrigger.cbTriggerSize = sizeof (TASK_TRIGGER); 
      pTrigger.wStartHour = 13;
      pTrigger.TriggerType = TASK_TIME_TRIGGER_DAILY;
      pTrigger.Type.Daily.DaysInterval = 1;

      COMPtr<ITaskTrigger> taskTrigger;
      WORD piNewTrigger=0;

      THROW_COMEXCEPTION( task.GetTask()->CreateTrigger(&piNewTrigger,&taskTrigger) );
      THROW_COMEXCEPTION( taskTrigger->SetTrigger(&pTrigger) );
    } // of TaskScheduler::CreateTask()

    /**
     *
     */
    void TaskScheduler::RemoveTask(LPCTSTR taskName)
    { GetTaskScheduler()->Delete(taskName);
    } // of TaskScheduler::RemoveTask()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
