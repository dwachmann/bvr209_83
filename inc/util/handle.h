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
#if !defined(UTIL_WIN_HANDLE_H)
#define UTIL_WIN_HANDLE_H


namespace bvr20983
{
  namespace util
  {
    class Handle
    {
      public:
        ~Handle()
        { if( NULL!=m_hHandle ) 
            ::CloseHandle(m_hHandle);

          m_hHandle=NULL;
        }
        
        Handle(HANDLE hHandle=NULL) :
          m_hHandle(hHandle)
        { }

        HANDLE operator*()  const
        { return m_hHandle; }

      protected: 
        HANDLE m_hHandle;
    }; // of class Handle


    class FileHandle : public Handle
    {
      public:
        FileHandle(LPCTSTR fileName);

        void ReadFile(BString& buffer);
    }; // of class FileHandle
  } // of namespace util
} // of namespace bvr20983
#endif // UTIL_WIN_HANDLE_H
/*==========================END-OF-FILE===================================*/
