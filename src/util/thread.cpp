/*
 * $Id$
 * 
 * Thread Class.
 * 
 * Copyright (C) 2008 Dorothea Wachmann
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
#include "util/logstream.h"
#include "util/thread.h"
#include "exception/bvr20983exception.h"

#define LOGGER_INFO_THREAD  LogStreamT::GetLogger(_T("threadproc"))<<INF<<setlineno(__LINE__)
#define LOGGER_DEBUG_THREAD LogStreamT::GetLogger(_T("threadproc"))<<DBG<<setlineno(__LINE__)
#define LOGGER_TRACE_THREAD LogStreamT::GetLogger(_T("threadproc"))<<TRC<<setlineno(__LINE__)
#define LOGGER_WARN_THREAD  LogStreamT::GetLogger(_T("threadproc"))<<WRN<<setlineno(__LINE__)
#define LOGGER_ERROR_THREAD LogStreamT::GetLogger(_T("threadproc"))<<ERR<<setlineno(__LINE__)
#define LOGGER_FATAL_THREAD LogStreamT::GetLogger(_T("threadproc"))<<FTL<<setlineno(__LINE__)

namespace bvr20983
{
  /*
   *
   */
  Thread::Thread(ThreadIF* pThreadIF,bool dispatchMsg) : 
    m_pThreadIF(pThreadIF),
    m_dispatchMsg(dispatchMsg),
    m_hThread(NULL)
  { m_hWaitEvent = ::CreateEvent(NULL,TRUE,FALSE,NULL);
  } // of Thread::Thread()
  
  /*
   *
   */
  Thread::~Thread()
  { LOGGER_DEBUG<<hex<<this<<_T("::~Thread() 0x")<<hex<<m_hThread<<endl; 

    StopThread(INFINITE);

    ::CloseHandle(m_hWaitEvent);

    LOGGER_DEBUG<<hex<<this<<_T("::~Thread() terminated 0x")<<hex<<m_hThread<<endl;
  } // of Thread::~Thread()

  /**   
   *    
   */
  void Thread::StartThread()
  { Critical critSec(m_critSec);

    if( NULL==m_hThread  )
    { m_hThread = (HANDLE)_beginthreadex(NULL,0,ThreadProc,this,CREATE_SUSPENDED,NULL); 

      LOGGER_DEBUG<<hex<<this<<_T("::Start() 0x")<<hex<<m_hThread<<_T(" ...")<<endl;

      ::ResetEvent(m_hWaitEvent);

      ::ResumeThread(m_hThread);
    } // of if
  } // of Thread::StartThread()
  
  /**
   *
   */
  bool Thread::StopThread(DWORD dwMilliseconds)
  { bool result = false;

    if( NULL!=m_hThread )
    { ::SetEvent(m_hWaitEvent);

      LOGGER_DEBUG<<hex<<this<<_T("::Stop() waiting for termination of thread 0x")<<hex<<m_hThread<<_T(" ...")<<endl;
      
      if( m_dispatchMsg )
      { for( ;; )
        { MSG msg;

          while( ::PeekMessage(&msg, NULL,  0, 0, PM_REMOVE) )
            ::DispatchMessage(&msg);
      
          LOGGER_DEBUG<<hex<<this<<_T("::Stop() msg loop for 0x")<<hex<<m_hThread<<endl;
            
          { Critical critSec(m_critSec);

            if( NULL!=m_hThread )
            { DWORD r = ::MsgWaitForMultipleObjects(1, &m_hThread,FALSE, dwMilliseconds, QS_ALLINPUT); 

              if( WAIT_OBJECT_0==r )
              { result = true;
                
                break;
              } // of if
            } // of if
            else
            { result = true;
              
              break;
            } // of else
          }
        } // of for
      } // of if
      else 
      { Critical critSec(m_critSec);

        if( NULL!=m_hThread )
        { DWORD r = ::WaitForMultipleObjects(1,&m_hThread,FALSE,dwMilliseconds);
        
          if( WAIT_OBJECT_0==r )
            result = true;
        } // of if
        else
          result = true;
      } // of else
    } // of if

    LOGGER_DEBUG<<hex<<this<<_T("::Stop() stopped")<<endl;

    return result;
  } // of Thread::StopThread()

  /**
   *
   */
  bool Thread::Init()
  { bool result = false;

    if( NULL!=m_pThreadIF )
      result = m_pThreadIF->InitThread();

    return result;
  } // of Thread::Init()

  /**
   *
   */
  void Thread::Exit(HRESULT hr)
  { if( NULL!=m_pThreadIF )
      m_pThreadIF->ExitThread(hr);

    { Critical critSec(m_critSec);
  
      m_hThread = NULL;
    }
  } // of Thread::Exit()

  /**
   *
   */
  void Thread::Run()
  { if( NULL!=m_pThreadIF )
      m_pThreadIF->RunThread(m_hWaitEvent);
  } // of Thread::Run()


  /**
   *
   */
  UINT WINAPI ThreadProc(LPVOID lpParam)
  { LONG    exHr    = 0;
    Thread* pThread = (Thread*)lpParam;
  
    LOGGER_DEBUG_THREAD<<_T("ThreadProc() --begin--")<<endl;
  
    try
    { if( NULL!=pThread && pThread->Init() )
        pThread->Run();
    }
    catch(BVR20983Exception& e)
    { LOGGER_ERROR_THREAD<<e<<endl; 
    
      exHr = e.GetErrorCode();
    }
    catch(exception& e) 
    { LOGGER_ERROR_THREAD<<"Exception "<<typeid(e).name()<<":"<<e.what()<<endl; 
      
      exHr = 1;
    }
    catch(LPCTSTR& e)
    { LOGGER_ERROR_THREAD<<e<<endl; 
  
      exHr = 2;
    }
    catch(...)
    { LOGGER_ERROR_THREAD<<_T("Exception")<<endl; 
  
      exHr = 3;
    }
  
    if( NULL!=pThread )
      pThread->Exit(exHr);

    LOGGER_DEBUG_THREAD<<_T("ThreadProc() --end--: ")<<exHr<<endl;
    LOGGER_DEBUG_THREAD<<flush;
    
    return (UINT)exHr;
  } // of ThreadProc()
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
