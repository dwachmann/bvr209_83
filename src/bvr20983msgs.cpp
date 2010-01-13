/*
 * $Id$
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
#include "util/registry.h"
#include "util/registryutil.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/comptr.h"
#include "util/comstring.h"
#include "util/eventlogger.h"
#include "util/versioninfo.h"
#include "util/guid.h"
#include "util/apputil.h"
#include "com/comserver.h"
#include <sstream>
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"

using namespace bvr20983;
using namespace bvr20983::COM;
using namespace bvr20983::util;

/**
 *
 */
BOOL WINAPI DllMain(HINSTANCE hDllInst,DWORD fdwReason,LPVOID lpvReserved)
{ BOOL bResult = TRUE;

  switch( fdwReason )
  { case DLL_PROCESS_ATTACH:
      { VersionInfo verInfo((HMODULE)hDllInst);

        LPVOID prodPrefix = verInfo.GetStringInfo(_T("ProductPrefix"));

        if( NULL==COMServer::CreateInstance(hDllInst) || NULL==EventLogger::CreateInstance((LPCTSTR)prodPrefix) )
          bResult = FALSE;
      }
      break;
    case DLL_PROCESS_DETACH:
      COMServer::DeleteInstance();
      EventLogger::DeleteInstance();
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


/**
 *
 */
STDAPI DllRegisterServer()
{ HRESULT           hr                = S_OK;
  EnumRegistration* pEnumRegistration = RegistryParameter::GetEnumRegistration();

  try
  { Registry registry;

    EventLogger::RegisterInRegistry(registry,pEnumRegistration);

    if( registry.Prepare() )
      registry.Commit();
  }
  catch(BVR20983Exception e)
  { LOGGER_ERROR<<e<<endl;

    hr = SELFREG_E_CLASS;
  }
  catch(exception& e) 
  { LOGGER_ERROR<<"Exception "<<typeid(e).name()<<":"<<e.what()<<endl;

    hr = SELFREG_E_CLASS;
  }
  catch(...)
  { LOGGER_ERROR<<_T("Exception")<<endl;

    hr = SELFREG_E_CLASS;
  }

  EvtLogInstall(pEnumRegistration,TRUE,hr);

  return hr;
} // of DllRegisterServer()

/**
 *
 */
STDAPI DllUnregisterServer()
{ HRESULT           hr                = S_OK;
  EnumRegistration* pEnumRegistration = RegistryParameter::GetEnumRegistration();

  try
  { Registry registry;
      
    EventLogger::RegisterInRegistry(registry,pEnumRegistration);

    registry.DeleteKeys();

    if( registry.Prepare() )
      registry.Commit();
  }
  catch(BVR20983Exception e)
  { LOGGER_ERROR<<e<<endl;

    hr = SELFREG_E_CLASS;
  }
  catch(exception& e) 
  { LOGGER_ERROR<<"Exception "<<typeid(e).name()<<":"<<e.what()<<endl;

    hr = SELFREG_E_CLASS;
  }
  catch(...)
  { LOGGER_ERROR<<_T("Exception")<<endl;

    hr = SELFREG_E_CLASS;
  }

  EvtLogInstall(pEnumRegistration,FALSE,hr);
  
  return hr;
} // of DllUnregisterServer()

#ifdef _UNICODE
#define _DllRegistrationInfo_ DllRegistrationInfoW
#else
#define _DllRegistrationInfo_ DllRegistrationInfoA
#endif

/**
 *
 */
void PrintRegistrationInfoUsage(HWND hwnd)
{ basic_ostringstream<TCHAR> msgStream;
  msgStream<<_T("Usage: rundll32 <dllname>,DllRegistrationInfo <filename>");

  TString msg = msgStream.str();

  ::MessageBox(hwnd,msg.c_str(),_T("DllRegistrationInfo"),MB_OK | MB_ICONINFORMATION);
} // of PrintRegistrationInfoUsage()

/**
 * RUNDLL32.EXE <dllname>,DllRegistrationInfo <filename>
 *
 * hwnd        - window handle that should be used as the owner window for any windows your DLL creates
 * hinst       - your DLL's instance handle
 * lpszCmdLine - command line your DLL should parse
 * nCmdShow    - describes how your DLL's windows should be displayed
 */
STDAPI_(void) _DllRegistrationInfo_(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ try
  { OutputDebugFmt(_T("DllRegistrationInfo(): <%s>\n"),lpszCmdLine);

    HRESULT           hr                = S_OK;
    EnumRegistration* pEnumRegistration = RegistryParameter::GetEnumRegistration();
    TCHAR             filename[MAX_PATH];

    ::memset(filename,_T('\0'),MAX_PATH);

    int     i         = 0;
    bool    stop      = false;
    LPTSTR  nextToken = NULL;
    for( LPTSTR tok=_tcstok_s(lpszCmdLine,_T(" "),&nextToken);NULL!=tok && !stop;tok=_tcstok_s(NULL,_T(" "),&nextToken),i++ )
    {
      switch( i )
      { 
      case 0:
        _tcscpy_s(filename,MAX_PATH,tok);
        stop = true;
        break;
      default:
        stop = true;
        break;
      } // of switch
    } // of for

    if( filename[0]!=_T('\0') )
    { basic_ostringstream<TCHAR> msgStream;
      msgStream<<_T("file=")<<filename;

      { Registry registry;

        EventLogger::RegisterInRegistry(registry,pEnumRegistration);

#ifdef _UNICODE
        wofstream fos(filename,ios::app);
#else
        ofstream fos(filename,ios::app);
#endif

        fos<<registry;

        fos.close();
      } // of if
    } // of if
    else 
      PrintRegistrationInfoUsage(hwnd);
  }
  catch(BVR20983Exception e)
  { OutputDebugFmt(_T("DllRegistrationInfo(): Exception \"%s\" [%ld]>\n"),e.GetErrorMessage(),e.GetErrorCode());
    OutputDebugFmt(_T("  Filename \"%s\" Line %d\n"),e.GetFileName(),e.GetLineNo());
  }
  catch(exception& e) 
  { OutputDebugFmt(_T("DllRegistrationInfo(): Exception <%s,%s>\n"),typeid(e).name(),e.what()); }
  catch(...)
  { OutputDebugFmt(_T("DllRegistrationInfo(): Exception\n")); }
} // of _DllRegistrationInfo_()

/**
 *
 */
STDAPI_(void) DllEnumRegistrationInfo(EnumRegistration* pEnumRegistration)
{ try
  { if( NULL!=pEnumRegistration )
    { OutputDebugFmt(_T("DllEnumRegistrationInfo()\n"));

      Registry registry;

      EventLogger::RegisterInRegistry(registry,pEnumRegistration);

      registry.EnumRegistry(pEnumRegistration);
    } // of if
  }
  catch(BVR20983Exception e)
  { OutputDebugFmt(_T("DllEnumRegistrationInfo(): Exception \"%s\" [%ld]>\n"),e.GetErrorMessage(),e.GetErrorCode());
    OutputDebugFmt(_T("  Filename \"%s\" Line %d\n"),e.GetFileName(),e.GetLineNo());
  }
  catch(exception& e) 
  { OutputDebugFmt(_T("DllEnumRegistrationInfo(): Exception <%s,%s>\n"),typeid(e).name(),e.what()); }
  catch(...)
  { OutputDebugFmt(_T("DllEnumRegistrationInfo(): Exception\n")); }
} // of DllEnumRegistrationInfo()

#ifdef _UNICODE
#define _DllIsAdministrator_ DllIsAdministratorW
#else
#define _DllIsAdministrator_ DllIsAdministratorA
#endif

/**
 * rundll32 msgs.dll,DllIsAdministrator
 */
STDAPI_(void) _DllIsAdministrator_(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ try
  { OutputDebugFmt(_T("DllIsAdministrator(): <%s>\n"),lpszCmdLine);

    BOOL isAdmin = IsUserInAdministrationGroup();

    basic_ostringstream<TCHAR> msgStream;
    msgStream<<_T("isAdmin=")<<isAdmin;

    ::MessageBox(hwnd,msgStream.str().c_str(),_T("DllIsAdministrator"),MB_OK | MB_ICONINFORMATION);
  }
  catch(BVR20983Exception e)
  { OutputDebugFmt(_T("DllIsAdministrator(): Exception \"%s\" [%ld]>\n"),e.GetErrorMessage(),e.GetErrorCode());
    OutputDebugFmt(_T("  Filename \"%s\" Line %d\n"),e.GetFileName(),e.GetLineNo());
  }
  catch(exception& e) 
  { OutputDebugFmt(_T("DllIsAdministrator(): Exception <%s,%s>\n"),typeid(e).name(),e.what()); }
  catch(...)
  { OutputDebugFmt(_T("DllIsAdministrator(): Exception\n")); }
} // of _DllRegistrationInfo_()
/*==========================END-OF-FILE===================================*/
