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
#include "util/dirinfo.h"
#include "util/logstream.h"
#include "exception/lasterrorexception.h"

namespace bvr20983
{
  namespace util
  {

    /*
     *
     */
    DirectoryInfo::DirectoryInfo(LPCTSTR baseDirectory,LPCTSTR fileMask,int maxDepth) :
      m_hFind(INVALID_HANDLE_VALUE)
    { 
      LOGGER_INFO<<_T("basedir=<")<<baseDirectory<<_T(">");
      
      if( NULL!=fileMask )
        LOGGER_INFO<<_T("fileMask=<")<<fileMask<<_T(">");
        
      LOGGER_INFO<<endl;
      
      
      ::memset(&m_findData,'\0',sizeof(m_findData));
    
      _tcscpy_s(m_baseDirectory,MAX_PATH,baseDirectory);
      
      THROW_LASTERROREXCEPTION1( ::GetCurrentDirectory(MAX_PATH,m_currentDirectory) );
      
      TCHAR d[MAX_PATH];
      
      _tcscpy_s(d,MAX_PATH,m_baseDirectory);

      if( NULL!=fileMask && fileMask[0]!=_T('\\') )
        _tcscat_s(d,MAX_PATH,_T("\\"));

      if( NULL!=fileMask )
        _tcscat_s(d,MAX_PATH,fileMask);

      m_hFind = ::FindFirstFile(d, &m_findData);

      if( INVALID_HANDLE_VALUE==m_hFind ) 
        THROW_LASTERROREXCEPTION2
    }

    /*
     *
     */
    DirectoryInfo::~DirectoryInfo()
    { if( INVALID_HANDLE_VALUE==m_hFind ) 
        ::FindClose(m_hFind);

      m_hFind = NULL;
      
      THROW_LASTERROREXCEPTION1( ::SetCurrentDirectory(m_currentDirectory) );
    } // of DirectoryInfo::~DirectoryInfo()

    /**
     *
     */
    void DirectoryInfo::Dump()
    { if( FILE_ATTRIBUTE_DIRECTORY & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T(" <DIR> ");
      else
        LOGGER_INFO<<_T("       ");
        
      LOGGER_INFO<<m_findData.cFileName;
      
      if( FILE_ATTRIBUTE_ARCHIVE & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T(" A ");

      LOGGER_INFO<<endl;
    } // of DirectoryInfo::Dump()

    /*
     *
     */
    void DirectoryInfo::Iterate()
    { 
      do
      { Dump();
      
        if( (m_findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0 && 
            _tcscmp(m_findData.cFileName,_T("."))!=0 && 
            _tcscmp(m_findData.cFileName,_T(".."))!=0
          )
        { 
/*          
          TCHAR  dir[MAX_PATH];
          LPTSTR filePart = NULL;
          
          THROW_LASTERROREXCEPTION1( ::GetFullPathName(m_findData.cFileName,MAX_PATH,dir,&filePart) );
          
          LOGGER_INFO<<_T("dir=<")<<dir<<_T(">")<<endl;
*/

          LOGGER_INFO<<_T("SetCurrentDirectory(")<<m_findData.cFileName<<_T(")");

          THROW_LASTERROREXCEPTION1( ::SetCurrentDirectory(m_findData.cFileName) );
          
          DirectoryInfo d(m_findData.cFileName,_T("*.*"));
        
          d.Iterate();
        } // of if

      } while( ::FindNextFile(m_hFind, &m_findData)!=0 );

      DWORD dwError = ::GetLastError();
      
      if( ERROR_NO_MORE_FILES!=dwError ) 
        THROW_LASTERROREXCEPTION3(dwError);
    } // of DirectoryInfo::Iterate()

    /*
     *
     */
    boolean DirectoryInfo::IsFile(LPCTSTR fName) 
    { boolean         result = false;
      HANDLE          hFind  = INVALID_HANDLE_VALUE;
      WIN32_FIND_DATA findData;

      hFind = ::FindFirstFile(fName, &findData);

      result = INVALID_HANDLE_VALUE!=hFind && (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0;

      if( INVALID_HANDLE_VALUE==hFind ) 
        ::FindClose(hFind);

      return result;
    }

    /*
     *
     */
    boolean DirectoryInfo::IsDirectory(LPCTSTR dirName)
    { boolean         result = false;
      HANDLE          hFind  = INVALID_HANDLE_VALUE;
      WIN32_FIND_DATA findData;

      hFind = ::FindFirstFile(dirName, &findData);

      result = INVALID_HANDLE_VALUE!=hFind && (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;

      if( INVALID_HANDLE_VALUE==hFind ) 
        ::FindClose(hFind);

      return result;
    }
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
