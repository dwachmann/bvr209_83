/*
 * $Id$
 * 
 * Install the product.
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
#include "util/versioninfo.h"
#include "com/comserver.h"
#include "util/eventlogger.h"
#include "util/logstream.h"

using namespace bvr20983;
using namespace bvr20983::COM;
using namespace bvr20983::util;

/**
 *
 */
STDAPI_(void) DllInstallW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ MessageBox(hwnd,lpszCmdLine,_T("DllInstall"),0);

  VersionInfo verInfo(COMServer::GetInstanceHandle());

  LPVOID prodPrefix = verInfo.GetStringInfo(_T("ProductPrefix"));

  OutputDebugFmt(_T("DllInstall(): <%s>\n"),prodPrefix);

  if( NULL!=prodPrefix && ((LPCTSTR)prodPrefix)[0]!=_T('\0') )
    EventLogger::RegisterInRegistry((LPCTSTR)prodPrefix);
} // of DllInstallW()

/**
 *
 */
STDAPI_(void) DllUninstallW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ MessageBox(hwnd,lpszCmdLine,_T("DllUninstall"),0);

  VersionInfo verInfo(COMServer::GetInstanceHandle());

  LPVOID prodPrefix = verInfo.GetStringInfo(_T("ProductPrefix"));

  OutputDebugFmt(_T("DllUninstall(): <%s>\n"),prodPrefix);

  if( NULL!=prodPrefix && ((LPCTSTR)prodPrefix)[0]!=_T('\0') )
    EventLogger::UnregisterInRegistry((LPCTSTR)prodPrefix);
} // of DllUninstallW()

/**
 *
 */
STDAPI_(void) DllLogW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ MessageBox(hwnd,lpszCmdLine,_T("DllLog"),0);

  EventLogger::GetInstance()->LogMessage(lpszCmdLine);
} // of DllLogW()

/**
 *
 */
STDAPI_(void) DllLogErrorW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ MessageBox(hwnd,lpszCmdLine,_T("DllLogError"),0);

  EventLogger::GetInstance()->LogError(lpszCmdLine);
} // of DllLogErrorW()
/*==========================END-OF-FILE===================================*/
