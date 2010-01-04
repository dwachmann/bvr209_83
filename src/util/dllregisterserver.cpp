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
#include "util/registryutil.h"
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
  { Registry registry;

    RegistryUtil::RegisterComObjectsInTypeLibrary(registry,szModulePath,RegistryUtil::AUTO);

    if( registry.Prepare() )
      registry.Commit();

/*
    as marker for registration of multiple typelibs

    RegistryUtil::RegisterTypeLib(LIBID_BVR20983_0_SC,LANG_SYSTEM_DEFAULT,_T("1"),1,0,szModulePath,szWindowsDir);
    RegistryUtil::RegisterTypeLib(LIBID_BVR20983_0_SC,MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_NEUTRAL), SORT_DEFAULT),_T("2"),1,0,szModulePath,szWindowsDir);
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
  { Registry registry;
   
    RegistryUtil::RegisterComObjectsInTypeLibrary(registry,szModulePath,RegistryUtil::AUTO);

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

  EvtLogInstall(NULL!=prodPrefix ? prodPrefix : _T("unknown"),NULL!=compPrefix ? compPrefix : _T("unknown"),FALSE,hr);
  
  return hr;
} // of DllUnregisterServer()

/**
 * regsvr32 /i:"DllInstall" <dllname> bInstall==TRUE 
 * regsvr32 /u:"DllInstall" <dllname> bInstall==FALSE 
 */
STDAPI DllInstall(BOOL bInstall,LPCTSTR pszCmdLine)
{ HRESULT hr = S_OK;

  OutputDebugFmt(_T("DllInstall(bInstall=%d,pszCmdLine=<%s>)\n"),bInstall,pszCmdLine);

  return hr;
} // of DllInstall()

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

    LPCTSTR prodPrefix = NULL;
    LPCTSTR compPrefix = NULL;
    TCHAR   szModulePath[MAX_PATH];

    COMServer::GetModuleFileName(szModulePath,ARRAYSIZE(szModulePath));

    VersionInfo verInfo(szModulePath);

    prodPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ProductPrefix"));
    compPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ComponentPrefix"));

    TCHAR filename[MAX_PATH];

    ::memset(filename,_T('\0'),MAX_PATH);

    int     i         = 0;
    boolean stop      = false;
    boolean newFormat = false;
    LPTSTR  nextToken = NULL;
    for( LPTSTR tok=_tcstok_s(lpszCmdLine,_T(" "),&nextToken);NULL!=tok && !stop;tok=_tcstok_s(NULL,_T(" "),&nextToken),i++ )
    {
      if( _tcscmp(tok,_T("-new"))==0 )
        newFormat = true;
      else
      { _tcscpy_s(filename,MAX_PATH,tok);

        stop = true;
      } // of else
    } // of for

    if( filename[0]!=_T('\0') )
    { basic_ostringstream<TCHAR> msgStream;
      msgStream<<_T("file=")<<filename;

      //::MessageBox(hwnd,msgStream.str().c_str(),_T("DllRegistrationInfo"),MB_OK | MB_ICONINFORMATION);

      { Registry registry;
        TCHAR    szModulePath[MAX_PATH];

        if( newFormat )
          registry.SetDumpType(Registry::XML);
        else
          registry.SetDumpType(Registry::MSI);

        registry.SetComponentId(compPrefix);

        COMServer::GetModuleFileName(szModulePath,ARRAYSIZE(szModulePath));

        RegistryUtil::RegisterComObjectsInTypeLibrary(registry,szModulePath);

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
STDAPI_(void) DllEnumRegistrationInfo(REGISTRYINFOPROC pEnumProc, LPARAM lParam)
{ try
  { OutputDebugFmt(_T("DllEnumRegistrationInfo()\n"));

    LPCTSTR prodPrefix = NULL;
    LPCTSTR compPrefix = NULL;
    TCHAR   szModulePath[MAX_PATH];

    COMServer::GetModuleFileName(szModulePath,ARRAYSIZE(szModulePath));

    VersionInfo verInfo(szModulePath);

    prodPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ProductPrefix"));
    compPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ComponentPrefix"));

    Registry registry;

    registry.SetComponentId(compPrefix);
    registry.SetDumpType(Registry::MSI);

    COMServer::GetModuleFileName(szModulePath,ARRAYSIZE(szModulePath));

    RegistryUtil::RegisterComObjectsInTypeLibrary(registry,szModulePath);

    registry.EnumRegistry(pEnumProc,lParam);
  }
  catch(BVR20983Exception e)
  { OutputDebugFmt(_T("DllRegistrationInfo(): Exception \"%s\" [%ld]>\n"),e.GetErrorMessage(),e.GetErrorCode());
    OutputDebugFmt(_T("  Filename \"%s\" Line %d\n"),e.GetFileName(),e.GetLineNo());
  }
  catch(exception& e) 
  { OutputDebugFmt(_T("DllRegistrationInfo(): Exception <%s,%s>\n"),typeid(e).name(),e.what()); }
  catch(...)
  { OutputDebugFmt(_T("DllRegistrationInfo(): Exception\n")); }
} // of DllEnumRegistrationInfo()


#ifdef _UNICODE
#define _DllRegEdit_ DllRegEditW
#else
#define _DllRegEdit_ DllRegEditA
#endif

/**
 *
 */
void PrintRegEditUsage(HWND hwnd)
{ basic_ostringstream<TCHAR> msgStream;
  msgStream<<_T("Usage: rundll32 <dllname>,DllRegEdit <read|write> [keyname] [valuename] [value]");

  TString msg = msgStream.str();

  ::MessageBox(hwnd,msg.c_str(),_T("DllRegEdit"),MB_OK | MB_ICONINFORMATION);
} // of PrintRegEditUsage()

/**
 * RUNDLL32.EXE <dllname>,DllRegEdit <read|write> [keyname] [valuename] [value]
 *
 * hwnd        - window handle that should be used as the owner window for any windows your DLL creates
 * hinst       - your DLL's instance handle
 * lpszCmdLine - command line your DLL should parse
 * nCmdShow    - describes how your DLL's windows should be displayed
 */
STDAPI_(void) _DllRegEdit_(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ try
  { OutputDebugFmt(_T("DllRegEdit(): <%s>\n"),lpszCmdLine);

    TCHAR cmd[256];
    TCHAR regkey[MAX_KEY_LENGTH];
    TCHAR regvalname[MAX_KEY_LENGTH];
    TCHAR regval[MAX_KEY_LENGTH];

    ::memset(cmd   ,_T('\0'),ARRAYSIZE(cmd));
    ::memset(regkey,_T('\0'),MAX_KEY_LENGTH);
    ::memset(regvalname,_T('\0'),MAX_KEY_LENGTH);
    ::memset(regval,_T('\0'),MAX_KEY_LENGTH);

    int     i         = 0;
    boolean stop      = false;
    LPTSTR  nextToken = NULL;
    for( LPTSTR tok=_tcstok_s(lpszCmdLine,_T(" "),&nextToken);NULL!=tok && !stop;tok=_tcstok_s(NULL,_T(" "),&nextToken),i++ )
    {
      switch( i )
      { 
      case 0:
        if( _tcscmp(tok,_T("read"))==0 || _tcscmp(tok,_T("write"))==0 )
          _tcscpy_s(cmd,ARRAYSIZE(cmd),tok);
        else
          stop = true;
        break;
      case 1:
        _tcscpy_s(regkey,MAX_KEY_LENGTH,tok);

        break;
      case 2:
        _tcscpy_s(regvalname,MAX_KEY_LENGTH,tok);
        break;
      case 3:
        _tcscpy_s(regval,MAX_KEY_LENGTH,tok);
        break;
      default:
        stop = true;
        break;
      } // of switch
    } // of for

    if( cmd[0]!=_T('\0') )
    { basic_ostringstream<TCHAR> msgStream;
      msgStream<<_T("command=")<<cmd<<_T("\n");

      ::MessageBox(hwnd,msgStream.str().c_str(),_T("DllRegEdit"),MB_OK | MB_ICONINFORMATION);

      if( _tcscmp(cmd,_T("read"))==0 )
      { Registry      registry(regkey);
        RegistryValue regVal; 

        if( registry.QueryValue(NULL,regvalname,regVal) )
        {
          basic_ostringstream<TCHAR> msgStream1;
          msgStream1<<regkey<<_T("\n");
          msgStream1<<regVal<<_T("\n");

          ::MessageBox(hwnd,msgStream1.str().c_str(),_T("DllRegEdit"),MB_OK | MB_ICONINFORMATION);
        } // of if
      } // of if
    } // of if
    else 
      PrintRegEditUsage(hwnd);
  }
  catch(BVR20983Exception e)
  { OutputDebugFmt(_T("DllRegEdit(): Exception \"%s\" [%ld]>\n"),e.GetErrorMessage(),e.GetErrorCode());
    OutputDebugFmt(_T("  Filename \"%s\" Line %d\n"),e.GetFileName(),e.GetLineNo());
  }
  catch(exception& e) 
  { OutputDebugFmt(_T("DllRegEdit(): Exception <%s,%s>\n"),typeid(e).name(),e.what()); }
  catch(...)
  { OutputDebugFmt(_T("DllRegEdit(): Exception\n")); }
} // of _DllRegEdit_()
/*==========================END-OF-FILE===================================*/
