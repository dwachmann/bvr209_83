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

#include "os.h"
#include <shlobj.h>
#include "util/registry.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/comptr.h"
#include "util/versioninfo.h"
#include "util/apputil.h"
#include "util/autoupdate.h"
#include "util/taskscheduler.h"
#include "win/bitsprogressdlg.h"
#include <commctrl.h>
#include <sstream>
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"

using namespace bvr20983;
using namespace bvr20983::util;
using namespace bvr20983::win;

static HINSTANCE g_hDllInst = NULL;


/**
 *
 */
BOOL WINAPI DllMain(HINSTANCE hDllInst,DWORD fdwReason,LPVOID lpvReserved)
{ BOOL bResult = TRUE;

  switch( fdwReason )
  { case DLL_PROCESS_ATTACH:
      { VersionInfo verInfo((HMODULE)hDllInst);

        LPVOID prodPrefix = verInfo.GetStringInfo(_T("ProductPrefix"));

        g_hDllInst = hDllInst;
      }
      break;
    case DLL_PROCESS_DETACH:
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    default:
      break;
  }

  return bResult;
} // of DllMain


#ifdef _UNICODE
#define _admin_ adminW
#else
#define _admin_ adminA
#endif

/**
 *
 */
void PrintBtxCreateJob(HWND hwnd)
{ basic_ostringstream<TCHAR> msgStream;
  msgStream<<_T("Usage: rundll32 <dllname>,admin <command> args")<<endl;
  msgStream<<_T("       create      <jobname>: create a bits job")<<endl;
  msgStream<<_T("       update      <jobname> <url> <localname>: download a file")<<endl;
  msgStream<<_T("       checkupdate <updateurl>: check for new msi package")<<endl;

  TString msg = msgStream.str();

  ::MessageBox(hwnd,msg.c_str(),_T("Admin"),MB_OK | MB_ICONINFORMATION);
} // of PrintBtxCreateJob()

/**
 *
 */
void ParseCommandLine(LPWSTR lpszCmdLine,VTString& args)
{ int     i         = 0;
  LPTSTR  nextToken = NULL;
 
  for( LPTSTR tok=_tcstok_s(lpszCmdLine,_T(" "),&nextToken);NULL!=tok;tok=_tcstok_s(NULL,_T(" "),&nextToken),i++ )
    args.push_back(tok);
} // of ParseCommandline()


/**
 *
 */
STDAPI_(void) _admin_(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ ::CoInitialize(NULL);

  INITCOMMONCONTROLSEX icc;

  icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
  icc.dwICC = ICC_PROGRESS_CLASS;

  ::InitCommonControlsEx(&icc);

  try
  { auto_ptr<BackgroundTransfer> btx(new BackgroundTransfer());

    VTString args;
    ParseCommandLine(lpszCmdLine,args);

    if( args.empty() )
      PrintBtxCreateJob(hwnd);
    else
    {
      TString command = args[0];

      args.erase( args.begin() );

      if( _tcscmp(command.c_str(),_T("create"))==0 )
      { CGUID jobId;

        btx->CreateJob(args[0].c_str(),jobId);
      } // of if
      else if( _tcscmp(command.c_str(),_T("list"))==0 )
        btx->List();
      else if( _tcscmp(command.c_str(),_T("update"))==0 && args.size()>=3 )
      { BITSProgressDlg dlg(btx);

        dlg.Init(args[0].c_str(),args[1].c_str(),args[2].c_str());

        int result = dlg.Show(hwnd,g_hDllInst);
      } // of else if
      else if( _tcscmp(command.c_str(),_T("checkupdate"))==0 && args.size()>=1 )
      { AutoUpdate autoUpdate(btx);

        if( autoUpdate.Init(g_hDllInst,args[0].c_str()) )
          autoUpdate.Run();
      } // of else if
      else if( _tcscmp(command.c_str(),_T("createtask"))==0 && args.size()>=2 )
      { TCHAR       path[MAX_PATH];
        TaskScheduler taskScheduler;
        Task          task(taskScheduler);

        task.NewTask(args[0].c_str());

        THROW_COMEXCEPTION( task.GetTask()->SetApplicationName(_T("c:\\windows\\system32\\rundll32.exe")) );

        ::GetModuleFileName(g_hDllInst,path,MAX_PATH);

        TString parameters(path);

        parameters += _T(",admin checkupdate ");
        parameters += args[1].c_str();

        THROW_COMEXCEPTION( task.GetTask()->SetParameters(parameters.c_str()) );
        THROW_COMEXCEPTION( task.GetTask()->SetFlags(TASK_FLAG_RUN_ONLY_IF_LOGGED_ON) );
        //THROW_COMEXCEPTION( task.GetTask()->SetFlags(TASK_FLAG_DISABLED | TASK_FLAG_RUN_ONLY_IF_LOGGED_ON) );

        ::SHGetFolderPath(NULL,CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL,SHGFP_TYPE_CURRENT,path);

        THROW_COMEXCEPTION( task.GetTask()->SetWorkingDirectory(path) );
        THROW_COMEXCEPTION( task.GetTask()->SetComment(_T("Checks for updated MSI packages for bvr20983")) );

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

        task.Commit();
      } // of else if
      else
        PrintBtxCreateJob(hwnd);
    } // of if
  }
  catch(BVR20983Exception e)
  { OutputDebugFmt(_T("Admin(): Exception \"%s\" [%ld]>\n"),e.GetErrorMessage(),e.GetErrorCode());
  }
  catch(exception& e) 
  { OutputDebugFmt(_T("Admin(): Exception <%s,%s>\n"),typeid(e).name(),e.what()); }
  catch(...)
  { OutputDebugFmt(_T("Admin(): Exception\n")); }

  ::CoUninitialize();
} // of _admin_()
/*==========================END-OF-FILE===================================*/
