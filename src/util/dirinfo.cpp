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
    struct DumpDirIterator : public DirIterator
    {
      boolean Next(DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,void* p)
      { dirInfo.DumpFindData();
  
        return true;
      } // of DumpDirIterator::Next()
    };

    /**
     *
     */
    struct RemoveFileIterator : public DirIterator
    {
      boolean Next(DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,void* p);
    };

    /**
     *
     */
    struct RemoveDirectoryIterator : public DirIterator
    {
      boolean Next(DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,void* p);
    };

    /*
     *
     */
    DirectoryInfo::DirectoryInfo(LPCSTR baseDirectory,LPCSTR fileMask,UINT maxDepth) :
      m_hFind(INVALID_HANDLE_VALUE),
      m_maxDepth(maxDepth)
    { WCHAR baseDirectoryW[MAX_PATH];
      WCHAR fileMaskW[MAX_PATH];

      THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, baseDirectory, -1,baseDirectoryW, MAX_PATH) );

      if( NULL!=fileMask )
      { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, fileMask, -1,fileMaskW, MAX_PATH) ); }
      
      Init(baseDirectoryW,fileMask!=NULL ? fileMaskW : NULL); 
    }

    /*
     *
     */
    DirectoryInfo::DirectoryInfo(LPCWSTR baseDirectory,LPCWSTR fileMask,UINT maxDepth) :
      m_hFind(INVALID_HANDLE_VALUE),
      m_maxDepth(maxDepth)
    { Init(baseDirectory,fileMask); }


    /*
     *
     */
    void DirectoryInfo::Init(LPCWSTR baseDirectory,LPCWSTR fileMask)
    { ::memset(&m_findData,'\0',sizeof(m_findData));

      LPWSTR filePart = NULL;
      
      THROW_LASTERROREXCEPTION1( ::GetFullPathNameW(baseDirectory,MAX_PATH,m_baseDirectory,&filePart) );
      
      if( NULL==fileMask )
        ::wcscpy_s(m_fileMask,MAX_PATH,L"*.*");
      else
        ::wcscpy_s(m_fileMask,MAX_PATH,fileMask);
      
      WCHAR  dir[MAX_PATH];
      
      ::wcscpy_s(dir,MAX_PATH,m_baseDirectory);
      ::wcscat_s(dir,MAX_PATH,L"\\");
      ::wcscat_s(dir,MAX_PATH,m_fileMask);

      m_hFind = ::FindFirstFileW(dir, &m_findData);

      if( INVALID_HANDLE_VALUE==m_hFind ) 
        THROW_LASTERROREXCEPTION2
    } // of DirectoryInfo::DirectoryInfo()

    /*
     *
     */
    DirectoryInfo::~DirectoryInfo()
    { if( INVALID_HANDLE_VALUE!=m_hFind ) 
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
    boolean DirectoryInfo::GetFullNameA(LPSTR path,int cPath)
    { boolean result = false;

      if( NULL!=path )
      { WCHAR pathW[MAX_PATH];
        
        result = GetFullNameW(pathW,MAX_PATH);
        
        if( result )
        { THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, pathW, MAX_PATH,path, cPath, NULL, NULL ) ); }
      } // of if

      return result;
    } // of DirectoryInfo::GetFullNameA()


    /*
     *
     */
    boolean DirectoryInfo::GetFullNameW(LPWSTR path,int cPath)
    { boolean result = false;

      if( NULL!=path )
      { ::wcscpy_s(path,cPath,m_baseDirectory);
        ::wcscat_s(path,cPath,L"\\");
        ::wcscat_s(path,cPath,m_findData.cFileName);
        
        result = true;
      } // of if

      return result;
    } // of DirectoryInfo::GetFullNameW()
      

    /*
     *
     */
    void DirectoryInfo::Iterate(DirIterator& iter,void* p)
    { do
      { 
        if( (m_findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0 && 
            (::wcscmp(m_findData.cFileName,L".")==0 || ::wcscmp(m_findData.cFileName,L"..")==0)
          )
          continue;
        
        if( (m_findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0 && m_maxDepth>0 )
        { WCHAR dir[MAX_PATH];
          
          ::wcscpy_s(dir,MAX_PATH,m_baseDirectory);
          ::wcscat_s(dir,MAX_PATH,L"\\");
          ::wcscat_s(dir,MAX_PATH,m_findData.cFileName);

          DirectoryInfo d(dir,m_fileMask,m_maxDepth-1);
        
          d.Iterate(iter,p);
        } // of if

        if( !iter.Next(*this,m_findData,p) )
          return;

      } while( ::FindNextFileW(m_hFind, &m_findData)!=0 );

      DWORD dwError = ::GetLastError();
      
      if( ERROR_NO_MORE_FILES!=dwError ) 
        THROW_LASTERROREXCEPTION3(dwError);
    } // of DirectoryInfo::Iterate()

    /*
     *
     */
    boolean DirectoryInfo::IsFileA(LPCSTR fName) 
    { WCHAR fNameW[MAX_PATH];

      if( NULL!=fName )
      { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, fName, -1,fNameW, MAX_PATH) ); }
      
      return fName!=NULL ? IsFileW(fNameW) : false;
    } // of DirectoryInfo::IsFileA()

    /*
     *
     */
    boolean DirectoryInfo::IsFileW(LPCWSTR fName) 
    { boolean          result = false;
    
      if( NULL!=fName )
      { HANDLE           hFind  = INVALID_HANDLE_VALUE;
        WIN32_FIND_DATAW findData;
  
        hFind = ::FindFirstFileW(fName, &findData);
  
        result = INVALID_HANDLE_VALUE!=hFind && (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0;
  
        if( INVALID_HANDLE_VALUE!=hFind ) 
          ::FindClose(hFind);
      } // of if
      
      return result;
    } // of DirectoryInfo::IsFileW()

    /*
     * 
     */
    boolean DirectoryInfo::GetFileSizeA(LPCSTR fName,DWORD* nFileSizeLow,DWORD* nFileSizeHigh) 
    { WCHAR fNameW[MAX_PATH];

      if( NULL!=fName )
      { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, fName, -1,fNameW, MAX_PATH) ); }
      
      return fName!=NULL ? GetFileSizeW(fNameW,nFileSizeLow,nFileSizeHigh) : false;
    } // of DirectoryInfo::GetFileSizeA()

    /*
     *
     */
    boolean DirectoryInfo::GetFileSizeW(LPCWSTR fName,DWORD* nFileSizeLow,DWORD* nFileSizeHigh) 
    { boolean result = false;
    
      if( NULL!=fName )
      { HANDLE           hFind  = INVALID_HANDLE_VALUE;
        WIN32_FIND_DATAW findData;
  
        hFind = ::FindFirstFileW(fName, &findData);
  
        result = INVALID_HANDLE_VALUE!=hFind && (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0;

        if( result )
        { if( NULL!=nFileSizeLow )
            *nFileSizeLow = findData.nFileSizeLow;

          if( NULL!=nFileSizeHigh )
            *nFileSizeHigh = findData.nFileSizeHigh;
        } // of if
  
        if( INVALID_HANDLE_VALUE!=hFind ) 
          ::FindClose(hFind);
      } // of if
      
      return result;
    } // of DirectoryInfo::GetFileSizeW()

    /*
     *
     */
    boolean DirectoryInfo::IsDirectoryA(LPCSTR dirName)
    { WCHAR dirNameW[MAX_PATH];

      if( NULL!=dirName )
      { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, dirName, -1,dirNameW, MAX_PATH) ); }
      
      return dirName!=NULL ? IsDirectoryW(dirNameW) : false;
    } // of DirectoryInfo::IsDirectoryA()

    /*
     *
     */
    boolean DirectoryInfo::IsDirectoryW(LPCWSTR dirName)
    { boolean          result = false;
      HANDLE           hFind  = INVALID_HANDLE_VALUE;
      WIN32_FIND_DATAW findData;

      hFind = ::FindFirstFileW(dirName, &findData);

      result = INVALID_HANDLE_VALUE!=hFind && (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;

      if( INVALID_HANDLE_VALUE!=hFind ) 
        ::FindClose(hFind);
        
      return result;
    } // of DirectoryInfo::IsDirectoryW()

    /*
     *
     */
    boolean DirectoryInfo::CreateDirectoryA(LPCSTR dirName)
    { WCHAR dirNameW[MAX_PATH];

      if( NULL!=dirName )
      { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, dirName, -1,dirNameW, MAX_PATH) ); }

      return dirName!=NULL ? CreateDirectoryW(dirNameW) : false;;
    } // of DirectoryInfo::CreateDirectoryW()
    
    /*
     *
     */
    boolean DirectoryInfo::CreateDirectoryW(LPCWSTR dirName)
    { boolean result = true;
      
      if( NULL!=dirName && NULL==::wcschr(dirName,L'*') && NULL==::wcschr(dirName,L'?') )
      { WCHAR   path[MAX_PATH];
        WCHAR   fullPath[MAX_PATH];
        LPCWSTR searchStart = dirName;
        LPCWSTR sep         = NULL;
        LPWSTR  filePart    = NULL;
        
        do
        { sep = ::wcschr(searchStart,L'\\');
        
          if( NULL!=sep )
          { ::wcsncpy_s(path,MAX_PATH,dirName,sep-dirName);
            
            searchStart = sep + 1;
          } // of if
          else
            ::wcscpy_s(path,MAX_PATH,dirName);

          THROW_LASTERROREXCEPTION1( ::GetFullPathNameW(path,MAX_PATH,fullPath,&filePart) );
          
          boolean isDir = IsDirectoryW(fullPath);
          
          if( !IsDirectoryW(fullPath) )
          {
            if( !::CreateDirectoryW(fullPath,NULL) )
            { result = false;
              break;
            } // of if
          
            LOGGER_DEBUG<<_T("creating directory <")<<fullPath<<_T(">")<<endl;
          } // of if
        } while( sep!=NULL );
      } // of if
    
      return result;
    } // of DirectoryInfo::CreateDirectoryW()

    /**
     *
     */
    boolean RemoveFileIterator::Next(DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,void* p)
    { boolean result = true;
      
      if( (findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0 )
      { WCHAR fullPath[MAX_PATH];
      
        dirInfo.GetFullNameW(fullPath,MAX_PATH);
        
        if( findData.dwFileAttributes&FILE_ATTRIBUTE_READONLY )
        { result = ::SetFileAttributes(fullPath,findData.dwFileAttributes & ~FILE_ATTRIBUTE_READONLY)==TRUE;
        
          if( !result )
            return result;
        } // of if
        
        result = ::DeleteFile(fullPath)==TRUE;
        
        LOGGER_DEBUG<<_T("delete file <")<<fullPath<<_T(">:")<<result<<endl;
      } // of if
      
      return result;
    } // of RemoveFileIterator::Next()

    /**
     *
     */
    boolean RemoveDirectoryIterator::Next(DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,void* p)
    { boolean result = true;
      
      if( findData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
      { WCHAR fullPath[MAX_PATH];
      
        dirInfo.GetFullNameW(fullPath,MAX_PATH);
        
        result = ::RemoveDirectory(fullPath)==TRUE;
        
        LOGGER_DEBUG<<_T("remove directory <")<<fullPath<<_T(">:")<<result<<endl;
      } // of if
      
      return result;
    } // of RemoveFileIterator::Next()

    /*
     *
     */
    boolean DirectoryInfo::RemoveDirectoryA(LPCSTR dirName,boolean recursive)
    { WCHAR dirNameW[MAX_PATH];

      if( NULL!=dirName )
      { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, dirName, -1,dirNameW, MAX_PATH) ); }

      return dirName!=NULL ? RemoveDirectoryW(dirNameW,recursive) : false;;
    } // of DirectoryInfo::RemoveDirectoryA()

    /*
     *
     */
    boolean DirectoryInfo::RemoveDirectoryW(LPCWSTR dirName,boolean recursive)
    { boolean result = true;
      
      if( NULL!=dirName && NULL==::wcschr(dirName,L'*') && NULL==::wcschr(dirName,L'?') )
      { WCHAR   fullPath[MAX_PATH];
        LPWSTR  filePart = NULL;
        
        THROW_LASTERROREXCEPTION1( ::GetFullPathNameW(dirName,MAX_PATH,fullPath,&filePart) );
        
        boolean isDir = IsDirectoryW(fullPath);
        
        if( IsDirectoryW(fullPath) )
        {
          if( recursive )
          { { DirectoryInfo dirInfo(fullPath,NULL,10);
            
              RemoveFileIterator fileIterator;
            
              dirInfo.Iterate(fileIterator);
            }
  
            { DirectoryInfo dirInfo(fullPath,NULL,10);
  
              RemoveDirectoryIterator dirIterator;
            
              dirInfo.Iterate(dirIterator);
            }
          } // of if
          
          result = ::RemoveDirectoryW(fullPath)==TRUE;

          LOGGER_DEBUG<<_T("remove directory <")<<fullPath<<_T(">:")<<result<<endl;
        } // of if
      } // of if
    
      return result;
    } // of DirectoryInfo::RemoveDirectoryW()

    /**
     *
     */
    void DirectoryInfo::StripFilenameA(LPSTR strippedFilename, int cbMaxFileName,LPCSTR fileName,LPCSTR prefix)
    { WCHAR strippedFilenameW[MAX_PATH];

      WCHAR fileNameW[MAX_PATH];
      WCHAR prefixW[MAX_PATH];

      THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, fileName, -1,fileNameW, MAX_PATH) );

      if( NULL!=prefix )
      { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, prefix, -1,prefixW, MAX_PATH) ); }
      
      StripFilenameW(strippedFilenameW,MAX_PATH,fileNameW,NULL!=prefix ? prefixW : NULL);

      THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, strippedFilenameW, MAX_PATH,strippedFilename, cbMaxFileName, NULL, NULL ) );
    } // of DirectoryInfo::StripFilenameA()

    /**
     *
     */
    void DirectoryInfo::StripFilenameW(LPWSTR strippedFilename, int cbMaxFileName,LPCWSTR fileName,LPCWSTR prefix)
    { LPCWSTR prefixStart = NULL!=prefix && prefix[0]!=L'\0' ? ::wcsstr(fileName,prefix) : NULL;
    
      if( NULL==prefixStart )
      { LPCWSTR p = ::wcsrchr(fileName, L'\\');
      
        if( p==NULL )
          ::wcscpy_s(strippedFilename,cbMaxFileName, fileName);
        else
          ::wcscpy_s(strippedFilename,cbMaxFileName, p+1);
      } // of if
      else
      { int prefixLen = ::wcslen(prefix);
        int offset    = *(prefixStart+prefixLen)==L'\\' ? prefixLen+1 : prefixLen;

        ::wcscpy_s(strippedFilename,cbMaxFileName,prefixStart+offset);
      } // of else
    } // of CabFCIParameter::StripFilenameW()

    /**
     *
     */
    void DirectoryInfo::DivideFilenameA(LPSTR dirName,LPSTR fName, int cbMaxFileName,LPCSTR fileName)
    { if( NULL!=dirName && NULL!=fName && NULL!=fileName )
      { WCHAR fileNameW[MAX_PATH];
        WCHAR dirNameW[MAX_PATH];
        WCHAR fNameW[MAX_PATH];
      
        THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, fileName, -1,fileNameW, MAX_PATH) );
        
        DivideFilenameW(dirNameW,fNameW,MAX_PATH,fileNameW);
        
        THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, dirNameW, MAX_PATH,dirName, cbMaxFileName, NULL, NULL ) );
        THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, fNameW, MAX_PATH,fName, cbMaxFileName, NULL, NULL ) );
      } // of if
    } // of DirectoryInfo::DivideFilenameA()

    /**
     *
     */
    void DirectoryInfo::DivideFilenameW(LPWSTR dirName,LPWSTR fName, int cbMaxFileName,LPCWSTR fileName)
    { if( NULL!=dirName && NULL!=fName && NULL!=fileName )
      { LPCWSTR p = ::wcsrchr(fileName, L'\\');
      
        if( p==NULL )
        { ::wcscpy_s(dirName,cbMaxFileName, L".\\");
          ::wcscpy_s(fName,cbMaxFileName, fileName);
        } // of if
        else
        { ::wcsncpy_s(dirName,cbMaxFileName,fileName,p-fileName);
          ::wcscat_s(dirName,cbMaxFileName,L"\\");
          
          ::wcscpy_s(fName,cbMaxFileName, p+1);
        } // of else
      } // of if
    } // of CabFCIParameter::DivideFilenameW()

#ifdef _UNICODE
    boolean DirectoryInfo::_GetFullName(LPTSTR path,int cPath)
    { return GetFullNameW(path,cPath); }
    
    boolean DirectoryInfo::_IsFile(LPCTSTR fName)
    { return IsFileW(fName); }
    
    boolean DirectoryInfo::_IsDirectory(LPCTSTR dirName)
    { return IsDirectoryW(dirName); }
    
    boolean DirectoryInfo::_CreateDirectory(LPCTSTR dirName)
    { return CreateDirectoryW(dirName); }

    boolean DirectoryInfo::_RemoveDirectory(LPCTSTR dirName,boolean recursive)
    { return RemoveDirectoryW(dirName,recursive); }

    void DirectoryInfo::_StripFilename(LPTSTR strippedFilename, int cbMaxFileName,LPCTSTR fileName,LPCTSTR prefix)
    { return StripFilenameW(strippedFilename,cbMaxFileName,fileName,prefix); }
    
    void DirectoryInfo::_DivideFilename(LPTSTR dirName,LPTSTR fName, int cbMaxFileName,LPCTSTR fileName)
    { return DivideFilenameW(dirName,fName,cbMaxFileName,fileName); }

    boolean DirectoryInfo::_GetFileSize(LPCTSTR fName,DWORD* nFileSizeLow,DWORD* nFileSizeHigh)
    { return GetFileSizeW(fName,nFileSizeLow,nFileSizeHigh); }
#else
    boolean DirectoryInfo::_GetFullName(LPTSTR path,int cPath)
    { return GetFullNameA(path,cPath); }
    
    boolean DirectoryInfo::_IsFile(LPCTSTR fName)
    { return IsFileA(fName); }
    
    boolean DirectoryInfo::_IsDirectory(LPCTSTR dirName)
    { return IsDirectoryA(dirName); }
    
    boolean DirectoryInfo::_CreateDirectory(LPCTSTR dirName)
    { return CreateDirectoryA(dirName); }
  
    boolean DirectoryInfo::_RemoveDirectory(LPCTSTR dirName,boolean recursive)
    { return RemoveDirectoryA(dirName); }

    void DirectoryInfo::_StripFilename(LPTSTR strippedFilename, int cbMaxFileName,LPCTSTR fileName,LPCTSTR prefix)
    { return StripFilenameA(strippedFilename,cbMaxFileName,fileName,prefix); }
    
    void DirectoryInfo::_DivideFilename(LPTSTR dirName,LPTSTR fName, int cbMaxFileName,LPCTSTR fileName)
    { return DivideFilenameA(dirName,fName,cbMaxFileName,fileName); }

    boolean DirectoryInfo::_GetFileSize(LPCTSTR fName,DWORD* nFileSizeLow,DWORD* nFileSizeHigh)
    { return GetFileSizeA(fName,nFileSizeLow,nFileSizeHigh); }
#endif        

  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
