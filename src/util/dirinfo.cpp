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
    { if( FILE_ATTRIBUTE_DIRECTORY & m_findData.dwFileAttributes )
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
   int DirectoryInfo::GetFullName(LPTSTR path,int cPath)
   { int result = 0;

     if( NULL!=path )
     { result = _tcscpy_s(path,cPath,m_baseDirectory);

       result += _tcscat_s(path,cPath,_T("\\"));
       result += _tcscat_s(path,cPath,m_findData.cFileName);
     } // of if

     return result;
   } // of DirectoryInfo::GetFullName()
      

    /*
     *
     */
    void DirectoryInfo::Iterate(DirIterator& iter,void* p)
    { do
      { 
        if( (m_findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0 && 
            (_tcscmp(m_findData.cFileName,_T("."))==0 || _tcscmp(m_findData.cFileName,_T(".."))==0)
          )
          continue;
        
        if( !iter.Next(*this,m_findData,p) )
          return;
      
        if( (m_findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0 && m_maxDepth>0 )
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
    } // of DirectoryInfo::IsFile()

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
    } // of DirectoryInfo::IsDirectory()
    
    /*
     *
     */
    boolean DirectoryInfo::CreateDirectory(LPCTSTR dirName)
    { boolean result = true;
      
      if( NULL!=dirName && NULL==_tcschr(dirName,'*') && NULL==_tcschr(dirName,'?') )
      { TCHAR   path[MAX_PATH];
        TCHAR   fullPath[MAX_PATH];
        LPCTSTR searchStart = dirName;
        LPCTSTR sep         = NULL;
        LPTSTR  filePart    = NULL;
        
        do
        { sep = _tcschr(searchStart,'\\');
        
          if( NULL!=sep )
          { _tcsncpy_s(path,MAX_PATH,dirName,sep-dirName);
            
            searchStart = sep + 1;
          } // of if
          else
            _tcscpy_s(path,MAX_PATH,dirName);

          THROW_LASTERROREXCEPTION1( ::GetFullPathName(path,MAX_PATH,fullPath,&filePart) );
          
          boolean isDir = IsDirectory(fullPath);
          
          if( !IsDirectory(fullPath) )
          {
            if( !::CreateDirectory(fullPath,NULL) )
            { result = false;
              break;
            } // of if
          
            LOGGER_INFO<<_T("creating directory <")<<fullPath<<_T(">")<<endl;
          } // of if
        } while( sep!=NULL );
      } // of if
    
      return result;
    } // of DirectoryInfo::CreateDirectory()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
