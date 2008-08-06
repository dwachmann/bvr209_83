/*
 * $Id$
 * 
 * Exception class for GetLastError.
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
#include "exception/lasterrorexception.h"
#include "util/logstream.h"

/*
 *
 */
void bvr20983::LastErrorException::LoadMessage()
{ m_IsLastError = false;

  DWORD result = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                 NULL,
                                 m_errorCode,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPTSTR) &m_errorMessage,
                                 0, NULL 
                                );
                                
  if( result>0 )
    m_IsLastError  = true;
}

/**
 *
 */
bvr20983::LastErrorException::~LastErrorException()
{ if( m_IsLastError && NULL!=m_errorMessage ) 
    ::LocalFree((LPTSTR)m_errorMessage);
}
/*==========================END-OF-FILE===================================*/
