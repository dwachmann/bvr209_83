/*
 * $Id$
 * 
 * Base exception class.
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
#include "util/logstream.h"

namespace bvr20983
{
  BVR20983Exception::BVR20983Exception(LONG errorCode,LPCTSTR errorMessage,LPCTSTR fileName,int lineNo) : 
    m_errorCode(errorCode),m_errorMessage(errorMessage),m_lineNo(lineNo),m_fileName(fileName)
  { 
  }


  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const BVR20983Exception& e)
  { if( NULL!=e.GetErrorMessage() )
      os<<typeid(e).name()<<":"<< e.GetErrorMessage() <<_T(" [0x")<<hex<<setw(4)<<setfill(_T('0'))<<e.GetErrorCode()<<_T("]"); 
    else
      os<<typeid(e).name()<<":"<<_T(" [0x")<<hex<<setw(4)<<setfill(_T('0'))<<e.GetErrorCode()<<_T("]"); 

    if( e.GetLineNo()!=-1 )
      os<<_T(" linenumber=")<<dec<<e.GetLineNo();

    if( NULL!=e.GetFileName() )
      os<<_T(" filename=")<<e.GetFileName();

    return os;
  }
}

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const BVR20983Exception&);
/*==========================END-OF-FILE===================================*/
