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
#if !defined(VERSIONINFO_H)
#define VERSIONINFO_H


namespace bvr20983
{
  namespace util
  {
    struct LANGANDCODEPAGE 
    { WORD wLanguage;
      WORD wCodePage;
    };

    /**
     *
     */
    class VersionInfo
    {
      public:
        VersionInfo(LPCTSTR fName=NULL);
        VersionInfo(HMODULE hModule);
        ~VersionInfo();

        void   Dump();
        LPVOID GetStringInfo(LPCTSTR valName);

      private:
        void   Init(LPCTSTR fName);

        LPVOID            m_lpBuffer;
        VS_FIXEDFILEINFO* m_pFileInfo;
        LANGANDCODEPAGE*  m_lpTranslate;
        UINT              m_cbTranslate;

    }; // of class VersionInfo
  } // of namespace util
} // of namespace bvr20983
#endif // VERSIONINFO_H
/*==========================END-OF-FILE===================================*/
