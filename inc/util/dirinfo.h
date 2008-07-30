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
    struct DirInfo 
    { DWORD m_attrib;
      TCHAR m_fileName[MAX_PATH];
    };

    /**
     *
     */
    class DirectoryInfo
    {
      public:
        DirectoryInfo(LPCTSTR baseDirectory,UINT maxDepth=0);
        ~DirectoryInfo();

        void Iterate();
        void Dump();

        static boolean IsFile(LPCTSTR fName);
        static boolean IsDirectory(LPCTSTR dirName);

      private:
        TCHAR           m_baseDirectory[MAX_PATH];
        int             m_maxDepth;
        HANDLE          m_hFind;
        WIN32_FIND_DATA m_findData;

    }; // of class DirectoryInfo
  } // of namespace util
} // of namespace bvr20983
#endif // DIRINFO_H
/*==========================END-OF-FILE===================================*/
