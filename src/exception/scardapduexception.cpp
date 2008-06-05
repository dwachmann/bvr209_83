/*
 * $Id$
 * 
 * Exception class for SCARD API Errors.
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
#include "exception/bvr20983exception.h"
#include "exception/scardexception.h"
#include "exception/scardapduexception.h"

namespace bvr20983
{

  /*
   * SCardAPDUException::SCardAPDUException
   *
   * Constructor Parameters:
   *  None
   */
  SCardAPDUException::SCardAPDUException(LONG errorCode) : SCardException(errorCode)
  { m_errorCode = errorCode;
    m_sw1       = (BYTE)((0xff00&m_errorCode)>>8);
    m_sw2       = (BYTE)(0xff&m_errorCode);
  
    switch( m_errorCode )
    { case 0x0000: 
        m_errorMessage=_T(""); 
        break;
      case 0x9000: 
        m_errorMessage=_T("No Error"); 
        break;
      case 0x9802: 
        m_errorMessage=_T("no PIN defined"); 
        break;
      case 0x9804: 
        m_errorMessage=_T("authorization failed"); 
        break;
      case 0x9835: 
        m_errorMessage=_T("ASK RANDOM / GIVE RANDOM not executed"); 
        break;
      case 0x9840: 
        m_errorMessage=_T("PIN verification failed"); 
        break;
      case 0x9400: 
        m_errorMessage=_T("no EF selected"); 
        break;
      case 0x9402: 
        m_errorMessage=_T("addressrange exceeded"); 
        break;
      case 0x9404: 
        m_errorMessage=_T("FID or Record not found"); 
        break;
      case 0x9408: 
        m_errorMessage=_T("selected filetype not valid for command"); 
        break;
      case 0x6C00: 
        m_errorMessage=_T("Correct Expected Length (Le)"); 
        break;
      case 0x6D00: 
        m_errorMessage=_T("Unsupported command"); 
        break;
      case 0x6E00: 
        m_errorMessage=_T("Unsupported class"); 
        break;
      case 0x6A80: 
        m_errorMessage=_T("Wrong command parameter"); 
        break;
      case 0x6A82: 
        m_errorMessage=_T("File not found"); 
        break;
      case 0x6A84: 
        m_errorMessage=_T("Not enough memory space in the file"); 
        break;
      case 0x6983: 
        m_errorMessage=_T("File invalid"); 
        break;
      default:
        if( m_sw1==0x9f )
          m_errorMessage=_T("No Error, use GET RESPONSE for more data"); 
        else if( m_sw1==67 )
          m_errorMessage=_T("Length field wrong"); 
        else
          m_errorMessage=_T("Unknown Error"); 
        break;
    } // of switch
  }

  bool SCardAPDUException::IsError() const
  { bool result = true; 

    if( m_errorCode==0x9000 ) 
      result = false;
    else if( m_sw1==0x9F || m_sw1==0x61 )
      result = false;
      
    return result;
  }
  
  
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const SCardAPDUException& e)
  { os<<"APDU Response: \""<<e.GetErrorMessage()<<"\" [0x"<<hex<<setw(4)<<setfill(_T('0'))<<e.GetErrorCode()<<"]"; 
  
    return os;
  }
}

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const SCardAPDUException&);
/*==========================END-OF-FILE===================================*/
