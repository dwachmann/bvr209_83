/*
 * $Id$
 * 
 * register OLE components.
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


/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: DllRegisterServer

  Summary:  The standard exported function that can be called to command
            this DLL server to register itself in the system registry.

  Args:     void.

  Returns:  HRESULT
              NOERROR
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
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
  { Registry::RegisterComObjectsInTypeLibrary(szModulePath,true);

/*
    as marker for registration of multiple typelibs

    Registry::RegisterTypeLib(LIBID_BVR20983TypeLibrary,LANG_SYSTEM_DEFAULT,_T("1"),1,0,szModulePath,szWindowsDir);
    Registry::RegisterTypeLib(LIBID_BVR20983TypeLibrary,MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_NEUTRAL), SORT_DEFAULT),_T("2"),1,0,szModulePath,szWindowsDir);
*/
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

  EvtLogInstall(NULL!=prodPrefix ? prodPrefix : _T("unknown"),NULL!=prodPrefix ? compPrefix : _T("unknown"),TRUE,hr);

  return hr;
} // of DllRegisterServer()

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: DllUnregisterServer

  Summary:  The standard exported function that can be called to command
            this DLL server to unregister itself from the system Registry.

  Args:     void.

  Returns:  HRESULT
              NOERROR
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
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
  { Registry::RegisterComObjectsInTypeLibrary(szModulePath,false);
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

  EvtLogInstall(NULL!=prodPrefix ? prodPrefix : _T("unknown"),NULL!=prodPrefix ? compPrefix : _T("unknown"),FALSE,hr);
  
  return hr;
} // of DllUnregisterServer()

/**
 *
 */
STDAPI DllInstall(BOOL bInstall,LPCTSTR pszCmdLine)
{ HRESULT hr = S_OK;

  OutputDebugFmt(_T("DllInstall(bInstall=%d,pszCmdLine=<%s>)\n"),bInstall,pszCmdLine);

  return hr;
} // of DllInstall()
/*==========================END-OF-FILE===================================*/
