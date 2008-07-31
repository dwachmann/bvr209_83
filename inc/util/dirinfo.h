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
      virtual boolean Next(DirectoryInfo& dirInfo,const WIN32_FIND_DATA& findData,void* p)=0;
    };

    /**
     *
     */
    struct DumpDirIterator : public DirIterator
    {
      boolean Next(DirectoryInfo& dirInfo,const WIN32_FIND_DATA& findData,void* p);
    };

    /**
     *
     */
    class DirectoryInfo
    {
      public:
        DirectoryInfo(LPCTSTR baseDirectory,UINT maxDepth=0);
        ~DirectoryInfo();

        void           Dump();
        void           Iterate(DirIterator& iter,void* p=NULL);

        static boolean IsFile(LPCTSTR fName);
        static boolean IsDirectory(LPCTSTR dirName);

        typedef std::vector<WIN32_FIND_DATA> VDirInfo;

      private:
        TCHAR           m_baseDirectory[MAX_PATH];
        int             m_maxDepth;
        HANDLE          m_hFind;
        WIN32_FIND_DATA m_findData;

        friend struct DumpDirIterator;

        void            DumpFindData();
    }; // of class DirectoryInfo
  } // of namespace util
} // of namespace bvr20983
#endif // DIRINFO_H
/*==========================END-OF-FILE===================================*/
