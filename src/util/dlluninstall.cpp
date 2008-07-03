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
#include "util/logstream.h"

using namespace bvr20983;
using namespace bvr20983::COM;

/**
 *
 */
STDAPI_(void) DllInstallW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ LOGGER_INFO<<_T("DllInstall cmd=<")<<lpszCmdLine<<_T(">")<<endl;

  MessageBox(hwnd,lpszCmdLine,_T("DllInstall"),0);
} // of DllInstallW()

/**
 *
 */
STDAPI_(void) DllUninstallW(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ LOGGER_INFO<<_T("DllUninstall cmd=<")<<lpszCmdLine<<_T(">")<<endl;
  
  MessageBox(hwnd,lpszCmdLine,_T("DllUninstall"),0);
} // of DllUninstallW()
/*==========================END-OF-FILE===================================*/