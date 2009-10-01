/*
 * $Id$
 * 
 * Utility Class to dynamically load shared libraries.
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
#include "util/sharedlibrary.h"
#include "util/logstream.h"
#include "exception/lasterrorexception.h"

namespace bvr20983
{
  namespace util
  {
    /*
     *
     */
    SharedLibrary::SharedLibrary(LPCTSTR fName) :
      m_hModule(NULL)
    { Init(fName); }

    /*
     *
     */
    void SharedLibrary::Init(LPCTSTR fName)
    { LOGGER_DEBUG<<_T("SharedLibrary::Init(")<<fName<<_T(")")<<endl;
      
      m_hModule = ::LoadLibraryEx(fName,NULL,0);

      THROW_LASTERROREXCEPTION1( m_hModule );
    } // of SharedLibrary::Init()

    /*
     *
     */
    SharedLibrary::~SharedLibrary()
    { if( NULL!=m_hModule )
        ::FreeLibrary(m_hModule);

      m_hModule = NULL;
    } // of SharedLibrary::~SharedLibrary()

    /**
     *
     */
    FARPROC SharedLibrary::GetProcAddress(LPCTSTR procName,bool mandatory)
    { FARPROC result = NULL;

      if( NULL!=m_hModule )
      { LOGGER_DEBUG<<_T("SharedLibrary::GetProcAddress(")<<procName<<_T(")")<<endl;

#ifdef _UNICODE
        char procNameA[MAX_PATH];

        THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, procName, -1,procNameA, MAX_PATH, NULL, NULL ) );

        result = ::GetProcAddress(m_hModule,procNameA);
#else
        result = ::GetProcAddress(m_hModule,procName);
#endif

        if( NULL==result )
        { DWORD err = ::GetLastError();

          if( err!=ERROR_PROC_NOT_FOUND || mandatory )
            THROW_LASTERROREXCEPTION3(err);
        } // of if
      } // of if

      return result;
    } // of SharedLibrary::GetProcAddress()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
