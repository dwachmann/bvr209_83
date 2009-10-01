/*
 * $Id$
 * 
 * Copyright (C) 2009 Dorothea Wachmann
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
#if !defined(SHAREDLIBRARY_H)
#define SHAREDLIBRARY_H

namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    class SharedLibrary
    {
      public:
        SharedLibrary(LPCTSTR fName);
        ~SharedLibrary();

        FARPROC GetProcAddress(LPCTSTR procName,bool mandatory=true);

      private:
        void   Init(LPCTSTR fName);

        HMODULE m_hModule;
    }; // of class SharedLibrary
  } // of namespace util
} // of namespace bvr20983
#endif // SHAREDLIBRARY_H
/*==========================END-OF-FILE===================================*/
