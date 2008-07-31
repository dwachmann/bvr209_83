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

    /**
     *
     */
    boolean DumpDirIterator::Next(DirectoryInfo& dirInfo,const WIN32_FIND_DATA& findData,void* p)
    { dirInfo.DumpFindData();

      return true;
    } // of DumpDirIterator::Next()

    /*
     *
     */
    DirectoryInfo::DirectoryInfo(LPCTSTR baseDirectory,UINT maxDepth) :
      m_hFind(INVALID_HANDLE_VALUE),
      m_maxDepth(maxDepth)
    { ::memset(&m_findData,'\0',sizeof(m_findData));

      LPTSTR filePart = NULL;
      
      THROW_LASTERROREXCEPTION1( ::GetFullPathName(baseDirectory,MAX_PATH,m_baseDirectory,&filePart) );
      
      TCHAR  dir[MAX_PATH];
      
      _tcscpy_s(dir,MAX_PATH,m_baseDirectory);
      _tcscat_s(dir,MAX_PATH,_T("\\*.*"));

      m_hFind = ::FindFirstFile(dir, &m_findData);

      if( INVALID_HANDLE_VALUE==m_hFind ) 
        THROW_LASTERROREXCEPTION2
    } // of DirectoryInfo::DirectoryInfo()

    /*
     *
     */
    DirectoryInfo::~DirectoryInfo()
    { if( INVALID_HANDLE_VALUE==m_hFind ) 
        ::FindClose(m_hFind);

      m_hFind = NULL;
    } // of DirectoryInfo::~DirectoryInfo()

    /**
     *
     */
    void DirectoryInfo::DumpFindData()
    { if( (FILE_ATTRIBUTE_DIRECTORY & m_findData.dwFileAttributes)!=0 &&  
          (_tcscmp(m_findData.cFileName,_T(".."))==0 || _tcscmp(m_findData.cFileName,_T("."))==0 )
        )
        return;
      
      if( FILE_ATTRIBUTE_DIRECTORY & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T(" <DIR> ");
      else
        LOGGER_INFO<<_T("       ");

      if( FILE_ATTRIBUTE_ARCHIVE & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T("A");
      else
        LOGGER_INFO<<_T(" ");

      if( FILE_ATTRIBUTE_HIDDEN & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T("H");
      else
        LOGGER_INFO<<_T(" ");

      if( FILE_ATTRIBUTE_READONLY & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T("R");
      else
        LOGGER_INFO<<_T(" ");

      if( FILE_ATTRIBUTE_SYSTEM & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T("S");
      else
        LOGGER_INFO<<_T(" ");

      LOGGER_INFO<<_T(" ")<<m_baseDirectory<<_T("\\")<<m_findData.cFileName;
      
      LOGGER_INFO<<endl;
    } // of DirectoryInfo::DumpFindData()

    /*
     *
     */
    void DirectoryInfo::Dump()
    { DumpDirIterator iter;

      Iterate(iter);
    } // of DirectoryInfo::Dump()
      

    /*
     *
     */
    void DirectoryInfo::Iterate(DirIterator& iter,void* p)
    { do
      { iter.Next(*this,m_findData,p);
      
        if( (m_findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0 && 
            _tcscmp(m_findData.cFileName,_T("."))!=0 && 
            _tcscmp(m_findData.cFileName,_T(".."))!=0 &&
            m_maxDepth>0
          )
        { TCHAR dir[MAX_PATH];
          
          _tcscpy_s(dir,MAX_PATH,m_baseDirectory);
          _tcscat_s(dir,MAX_PATH,_T("\\"));
          _tcscat_s(dir,MAX_PATH,m_findData.cFileName);

          DirectoryInfo d(dir,m_maxDepth-1);
        
          d.Iterate(iter,p);
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
