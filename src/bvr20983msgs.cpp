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
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/comptr.h"
#include "util/comstring.h"
#include "util/eventlogger.h"
#include "util/versioninfo.h"
#include "com/comserver.h"
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"
#include "util/guid.h"
#include <sstream>

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
{ HRESULT hr         = S_OK;
  LPCTSTR prodPrefix = NULL;
  LPCTSTR compPrefix = NULL;
  TCHAR   szModulePath[MAX_PATH];

  COMServer::GetModuleFileName(szModulePath,sizeof(szModulePath)/sizeof(szModulePath[0]));

  VersionInfo verInfo(szModulePath);

  prodPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ProductPrefix"));
  compPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ComponentPrefix"));

  try
  { if( NULL!=prodPrefix && ((LPCTSTR)prodPrefix)[0]!=_T('\0') )
      EventLogger::RegisterInRegistry((LPCTSTR)prodPrefix);
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

  EvtLogInstall(NULL!=prodPrefix ? prodPrefix : _T("unknown"),NULL!=compPrefix ? compPrefix : _T("unknown"),TRUE,hr);

  return hr;
} // of DllRegisterServer()

/**
 *
 */
STDAPI DllUnregisterServer()
{ HRESULT hr         = S_OK;
  LPCTSTR prodPrefix = NULL;
  LPCTSTR compPrefix = NULL;
  TCHAR   szModulePath[MAX_PATH];

  COMServer::GetModuleFileName(szModulePath,sizeof(szModulePath)/sizeof(szModulePath[0]));

  VersionInfo verInfo(szModulePath);

  prodPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ProductPrefix"));
  compPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ComponentPrefix"));

  try
  { if( NULL!=prodPrefix && ((LPCTSTR)prodPrefix)[0]!=_T('\0') )
      EventLogger::UnregisterInRegistry((LPCTSTR)prodPrefix);
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

  EvtLogInstall(NULL!=prodPrefix ? prodPrefix : _T("unknown"),NULL!=compPrefix ? compPrefix : _T("unknown"),FALSE,hr);
  
  return hr;
} // of DllUnregisterServer()
/*==========================END-OF-FILE===================================*/
