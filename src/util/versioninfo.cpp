/*
 * $Id$
 * 
 * Windows DC class.
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
#include "exception/lasterrorexception.h"
#include "com/comserver.h"

#define DUMP_VERSION(a,b) HIWORD(a),LOWORD(a),HIWORD(b),LOWORD(b)

void OutputDebugFmt(LPTSTR pszFmt,...);

namespace bvr20983
{
  namespace util
  {

    /*
     *
     */
    VersionInfo::VersionInfo() :
      m_lpBuffer(NULL),
      m_pFileInfo(NULL),
      m_lpTranslate(NULL),
      m_cbTranslate(0)
    { Init(COM::COMServer::GetInstanceHandle()); }

    /*
     *
     */
    VersionInfo::VersionInfo(LPCTSTR fName) :
      m_lpBuffer(NULL),
      m_pFileInfo(NULL),
      m_lpTranslate(NULL),
      m_cbTranslate(0)
    { Init(fName); }

    /*
     *
     */
    VersionInfo::VersionInfo(HMODULE hModule) :
      m_lpBuffer(NULL),
      m_pFileInfo(NULL),
      m_lpTranslate(NULL),
      m_cbTranslate(0)
    { Init(hModule); }

    /*
     *
     */
    void VersionInfo::Init(HMODULE hModule)
    { TCHAR szModulePath[MAX_PATH];

      szModulePath[0] = _T('0');

      ::GetModuleFileName(hModule,szModulePath,ARRAYSIZE(szModulePath));

      Init(szModulePath);
    } // of VersionInfo::Init()

    /*
     *
     */
    void VersionInfo::Init(LPCTSTR fName)
    { TCHAR szModulePath[MAX_PATH];

      szModulePath[0] = _T('0');

      if( NULL==fName )
        ::GetModuleFileName(NULL,szModulePath,ARRAYSIZE(szModulePath));
      else
        _tcscpy_s(szModulePath,ARRAYSIZE(szModulePath),fName);

      //OutputDebugFmt(_T("VersionInfo()::Init(fName=%s)\n"),fName);

      DWORD hFileVersion = 0;
      DWORD verInfoSize  = ::GetFileVersionInfoSize(szModulePath,&hFileVersion);

      if( verInfoSize>0 )
      { m_lpBuffer = ::calloc(verInfoSize,1);

        THROW_LASTERROREXCEPTION1( GetFileVersionInfo(szModulePath,0,verInfoSize,m_lpBuffer) );

        UINT bufLen = 0;

        ::VerQueryValue(m_lpBuffer,_T("\\"),(LPVOID*)&m_pFileInfo,&bufLen);
        ::VerQueryValue(m_lpBuffer,_T("\\VarFileInfo\\Translation"),(LPVOID*)&m_lpTranslate,&m_cbTranslate);

        m_cbTranslate = m_cbTranslate / sizeof(LANGANDCODEPAGE);
      } // of if
    } // of VersionInfo::VersionInfo

    /*
     *
     */
    VersionInfo::~VersionInfo()
    { ::free(m_lpBuffer);

      m_lpBuffer = NULL;
    } // of VersionInfo::~VersionInfo()

    /**   
     *    
     */
    void VersionInfo::Dump()
    { if( NULL!=m_pFileInfo )
      { DWORD flags = m_pFileInfo->dwFileFlags & m_pFileInfo->dwFileFlagsMask;

        OutputDebugFmt(_T("%s version \"%d.%d.%d.%d\""),GetStringInfo(_T("InternalName")),DUMP_VERSION(m_pFileInfo->dwFileVersionMS,m_pFileInfo->dwFileVersionLS) );

        if( flags!=0 )
        { OutputDebugFmt(_T("("));

          if( flags & VS_FF_DEBUG )
            OutputDebugFmt(_T(" DEBUG"));

          if( flags & VS_FF_PRERELEASE )
            OutputDebugFmt(_T(" PRERELEASE"));

          if( flags & VS_FF_PATCHED )
            OutputDebugFmt(_T(" PATCHED"));

          if( flags & VS_FF_PRIVATEBUILD )
            OutputDebugFmt(_T(" PRIVATEBUILD"));

          if( flags & VS_FF_SPECIALBUILD )
            OutputDebugFmt(_T(" SPECIALBUILD"));
          
          OutputDebugFmt(_T(")\n"));
        } // of if

        OutputDebugFmt(_T("%s (build %s)\n"),GetStringInfo(_T("FileDescription")),GetStringInfo(_T("Comments")));
        OutputDebugFmt(_T("%s version \"%d.%d.%d.%d\"\n"),GetStringInfo(_T("ProductName")),DUMP_VERSION(m_pFileInfo->dwProductVersionMS,m_pFileInfo->dwProductVersionLS) );
        OutputDebugFmt(_T("%s\n"),GetStringInfo(_T("LegalCopyright")));

        OutputDebugFmt(_T("ProductPrefix=%s\n"),GetStringInfo(_T("ProductPrefix")));
        OutputDebugFmt(_T("ComponentPrefix=%s\n"),GetStringInfo(_T("ComponentPrefix")));

      } // of if
    } // of VersionInfo::Dump()

    /**
     *
     */
    LPVOID VersionInfo::GetStringInfo(LPCTSTR valName)
    { LPVOID varValue    = NULL;
      UINT   varValueLen = 0;

      if( NULL!=m_lpBuffer && NULL!=m_lpTranslate )
      { for( UINT i=0;i<m_cbTranslate;i++ )
        { TCHAR blockName[1024];

          _stprintf_s(blockName,ARRAYSIZE(blockName),
                      _T("\\StringFileInfo\\%04x%04x\\%s"),
                      m_lpTranslate[i].wLanguage,
                      m_lpTranslate[i].wCodePage,
                      valName
                     );

          ::VerQueryValue(m_lpBuffer,blockName,&varValue, &varValueLen); 
        } // of for
      } // of if

      return varValue;
    } // of VersionInfo::GetStringInfo()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
