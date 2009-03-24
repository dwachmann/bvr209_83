/*
 * $Id$
 * 
 * Windows DC class.
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
#include "os.h"
#include "util/handle.h"
#include "util/logstream.h"
#include "exception/lasterrorexception.h"

namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    FileHandle::FileHandle(LPCTSTR fileName)
    { m_hHandle = ::CreateFile(fileName, 
                               GENERIC_READ, 
                               0, 
                               NULL, 
                               OPEN_EXISTING, 
                               FILE_ATTRIBUTE_NORMAL, 
                               NULL
                              );

      if( m_hHandle==INVALID_HANDLE_VALUE )
      { THROW_LASTERROREXCEPTION2; }
    } // of FileHandle::FileHandle()

    /**
     *
     */
    void FileHandle::ReadFile(std::basic_string<CHAR>& buffer)
    { CHAR       pbBuffer[1024];
      DWORD      dwBytesRead;

      buffer.clear();

      if( m_hHandle!=INVALID_HANDLE_VALUE )
      { do
        { dwBytesRead = 0;

          THROW_LASTERROREXCEPTION1( ::ReadFile(m_hHandle, 
                                                pbBuffer, ARRAYSIZE(pbBuffer), &dwBytesRead, NULL
                                               ) 
                                   );

          if( dwBytesRead==0 ) 
            break;

          buffer.append(pbBuffer,dwBytesRead);
        } while( dwBytesRead>=sizeof(pbBuffer) );
      } // of if
    } // of AutoUpdate::ReadFile()

    /**
     *
     */
    void FileHandle::ReadFile(std::basic_string<WCHAR>& buffer)
    { std::basic_string<CHAR> value;

      buffer.clear();

      ReadFile(value);

      DWORD           valueSize = value.size()+1;
      auto_ptr<WCHAR> wBuffer(new WCHAR[valueSize]);

      THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, value.c_str(), -1,wBuffer.get(), valueSize) );

      buffer = wBuffer.get();
    } // of AutoUpdate::ReadFile()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
