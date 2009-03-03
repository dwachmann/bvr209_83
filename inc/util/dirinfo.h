/*
 * $Id$
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
#if !defined(DIRINFO_H)
#define DIRINFO_H


namespace bvr20983
{
  namespace util
  {
    class DirectoryInfo;

    /**
     *
     */
    struct DirIterator
    {
      virtual bool Next(DirectoryInfo& dirInfo,const _WIN32_FIND_DATAW& findData,int depth,void* p)=0;
    };

    /**
     *
     */
    class DirectoryInfo
    {
      public:
        DirectoryInfo(LPCWSTR baseDirectory,LPCWSTR fileMask=NULL,UINT maxDepth=0);
        DirectoryInfo(LPCSTR baseDirectory,LPCSTR fileMask=NULL,UINT maxDepth=0);
        ~DirectoryInfo();

        void           Dump();
        bool           Iterate(DirIterator& iter,int depth=0,void* p=NULL);

        bool           GetFullNameA(LPSTR path,int cPath);
        static bool    IsFileA(LPCSTR fName);
        static bool    IsDirectoryA(LPCSTR dirName);
        static bool    CreateDirectoryA(LPCSTR dirName);
        static bool    RemoveDirectoryA(LPCSTR dirName,bool recursive);
        static void    StripFilenameA(LPSTR strippedFilename, int cbMaxFileName,LPCSTR fileName,LPCSTR prefix=NULL);
        static void    DivideFilenameA(LPSTR dirName,LPSTR fName, int cbMaxFileName,LPCSTR fileName);
        static bool    GetFileSizeA(LPCSTR fName,DWORD* nFileSizeLow,DWORD* nFileSizeHigh);

        bool           GetFullNameW(LPWSTR path,int cPath);
        static bool    IsFileW(LPCWSTR fName);
        static bool    IsDirectoryW(LPCWSTR dirName);
        static bool    CreateDirectoryW(LPCWSTR dirName);
        static bool    RemoveDirectoryW(LPCWSTR dirName,bool recursive);
        static void    StripFilenameW(LPWSTR strippedFilename, int cbMaxFileName,LPCWSTR fileName,LPCWSTR prefix=NULL);
        static void    DivideFilenameW(LPWSTR dirName,LPWSTR fName, int cbMaxFileName,LPCWSTR fileName);
        static bool    GetFileSizeW(LPCWSTR fName,DWORD* nFileSizeLow,DWORD* nFileSizeHigh=NULL); 

        bool           _GetFullName(LPTSTR path,int cPath);
        static bool    _IsFile(LPCTSTR fName);
        static bool    _IsDirectory(LPCTSTR dirName);
        static bool    _CreateDirectory(LPCTSTR dirName);
        static bool    _RemoveDirectory(LPCTSTR dirName,bool recursive);
        static void    _StripFilename(LPTSTR strippedFilename, int cbMaxFileName,LPCTSTR fileName,LPCTSTR prefix=NULL);
        static void    _DivideFilename(LPTSTR dirName,LPTSTR fName, int cbMaxFileName,LPCTSTR fileName);
        static bool    _GetFileSize(LPCTSTR fName,DWORD* nFileSizeLow,DWORD* nFileSizeHigh=NULL); 

        typedef std::vector<_WIN32_FIND_DATAW> VDirInfo;

      private:
        WCHAR            m_baseDirectory[MAX_PATH];
        WCHAR            m_fileMask[MAX_PATH];
        int              m_maxDepth;
        HANDLE           m_hFind;
        WIN32_FIND_DATAW m_findData;

        friend struct DumpDirIterator;

        void            DumpFindData();
        void            Init(LPCWSTR baseDirectory,LPCWSTR fileMask);
    }; // of class DirectoryInfo
  } // of namespace util
} // of namespace bvr20983
#endif // DIRINFO_H
/*==========================END-OF-FILE===================================*/
