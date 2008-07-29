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
    DirectoryInfo::DirectoryInfo(LPCTSTR baseDirectory,int maxDepth) :
      m_hFind(INVALID_HANDLE_VALUE)
    { ::memset(&m_findData,'\0',sizeof(m_findData));

      m_hFind = ::FindFirstFile(baseDirectory, &m_findData);

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
    } // of DirectoryInfo::~DirectoryInfo()

    /**
     *
     */
    void DirectoryInfo::Dump()
    { LOGGER_INFO<<_T("FileName:")<<m_findData.cFileName<<endl;

      if( FILE_ATTRIBUTE_DIRECTORY & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T(" DIRECTORY ");
      else if( FILE_ATTRIBUTE_ARCHIVE & m_findData.dwFileAttributes )
        LOGGER_INFO<<_T(" ARCHIVE ");

      LOGGER_INFO<<endl;
    } // of DirectoryInfo::Dump()

    /*
     *
     */
    void DirectoryInfo::Iterate()
    { Dump();

      while( ::FindNextFile(m_hFind, &m_findData)!=0 ) 
      {
        Dump();
      } // of while

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
