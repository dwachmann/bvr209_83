/*
 * $Id$
 * 
 * Utility functions.
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
#include "util/apputil.h"

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: GetErrorMsg

  Summary:  Accepts a Win32 error code and retrieves a human readable
            system message for it.

  Args:     HRESULT hr
              SCODE error code.
            LPTSTR pszMsg
              Pointer string where message will be placed.
            UINT uiSize
              Max size of the msg string.

  Returns:  BOOL
              TRUE if hr was error; FALSE if not.
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
BOOL GetErrorMsg(HRESULT hr,LPTSTR pszMsg,UINT uiSize)
{
  BOOL bErr = FAILED(hr);
  DWORD dwSize;

  if (bErr)
  {
    memset(pszMsg, 0, uiSize * sizeof(TCHAR));

    dwSize = ::FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             hr,
                             MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                             pszMsg,
                             uiSize,
                             NULL
                            );

    if (dwSize>2)
    {
      // Take out the trailing CRLF.
      pszMsg[--dwSize] = 0;
      pszMsg[--dwSize] = 0;
    }
  }

  return bErr;
}
/*==========================END-OF-FILE===================================*/
