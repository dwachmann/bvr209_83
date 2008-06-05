/*
 * $Id$
 * 
 * Exception class for COM errors.
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
#include "exception/comexception.h"
#include "util/apputil.h"

/*
 * COMException::COMException
 * COMException::~COMException
 *
 * Constructor Parameters:
 *  None
 */
bvr20983::COMException::COMException(HRESULT hr,LPCTSTR fileName,int lineNo) : BVR20983Exception((LONG)hr,NULL,fileName,lineNo)
{ m_comErrorMessage = NULL;
  m_hr              = hr;
  m_errorMessage    = _T("No COM Error");
  
  if( FAILED(hr) )
  { TCHAR szMsg[MAX_PATH];
    
    if( ::GetErrorMsg(hr,szMsg,MAX_PATH) )
    { m_comErrorMessage = (LPTSTR) new TCHAR[_tcslen(szMsg)+1];
      
      _tcscpy_s(m_comErrorMessage,_tcslen(szMsg)+1,szMsg);
      
      m_errorMessage = m_comErrorMessage;
    } // of if
  } // of if
}

/**
 *
 */
bvr20983::COMException::~COMException()
{ if( NULL!=m_comErrorMessage ) 
    delete[] m_comErrorMessage;

  m_comErrorMessage = NULL;
  m_errorMessage    = NULL;
}
/*==========================END-OF-FILE===================================*/
