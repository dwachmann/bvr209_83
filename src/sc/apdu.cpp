/*
 * $Id$
 * 
 * APDU class.
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
#include "sc/sctypes.h"
#include "sc/apdu.h"
#include "sc/tlv.h"
#include "exception/smartcardexception.h"
#include "util/logstream.h"

namespace bvr20983
{

  /*
   * APDU::APDU
   *
   * Constructor Parameters:
   *  None
   */
  APDU::APDU() : m_cla(0),m_ins(0),m_p1(0),m_p2(0),m_leSet(FALSE),m_le(0),m_responseType(RAW_RESPONSE)
  { }
  
  APDU::APDU(BYTE cla,BYTE ins,BYTE p1,BYTE p2) : m_cla(cla),m_ins(ins),m_p1(p1),m_p2(p2),m_leSet(FALSE),m_le(0),m_responseType(RAW_RESPONSE)
  { }
  
  APDU::APDU(const APDU& apdu)
  { *this = apdu; }
  
  /**
   *
   */
  APDU& APDU::operator=(const APDU& b)
  { m_cla          = b.m_cla; 
    m_ins          = b.m_ins; 
    m_p1           = b.m_p1; 
    m_p2           = b.m_p2; 
    m_le           = b.m_le;
    m_leSet        = b.m_leSet;
    m_data         = b.m_data;
    m_responseType = b.m_responseType;
    m_responseCode = b.m_responseCode;
    
    return *this; 
  }
  
  
  /**
   *
   */
  APDU::operator ByteBuffer() const
  { ByteBuffer result;
  
    result.push_back(m_cla);
    result.push_back(m_ins);
    result.push_back(m_p1);
    result.push_back(m_p2);
    
    BYTE len = CalcLength();
    
    if( len>0 )
    { result.push_back(CalcLength());
  
      VLPByteBuffer::const_iterator iter;
    
      for( iter=m_data.begin();iter!=m_data.end();iter++ )
        result.append(**iter);
    } // of if
      
    if( m_leSet )
      result.push_back(m_le);
  
    return result;
  }
  
  /**
   *
   */
  BYTE APDU::CalcLength() const
  { BYTE len = 0;
    
    VLPByteBuffer::const_iterator iter;
  
    for( iter=m_data.begin();iter!=m_data.end();iter++ )
    { DWORD s = (*iter)->size();
    
      if( len+s>255 )
        THROW_SMARTCARDEXCEPTION(SmartcardException::EXCEEDS_APDU_MAXLENGTH);
        
      len += (BYTE)s;
    } // of for
  
    return len;
  } // of APDU::CalcLength()
  
  /**
   *
   */
  void APDU::SetResponse(const BYTE* buffer,DWORD bufferLen,const SCardAPDUException& ex)
  { m_responseCode = ex;
    m_response.clear();
  
    switch( m_responseType ) 
    { case TLV_RESPONSE:
        for( DWORD i=0;i<bufferLen; )
        { BYTE t        = buffer[i];
          BYTE l        = buffer[i+1];
          const BYTE* d = buffer+i+2;
          
          ByteBuffer buf = ByteBuffer(d,l);
          
          i += l+2;
          
          ByteBuffer* b = new TLV(t,buf);
          
          m_response.push_back( linked_ptr<ByteBuffer>(b) );
        } // of for
        break;
      default:
        { ByteBuffer* b = new ByteBuffer(buffer,bufferLen);
        
          m_response.push_back( linked_ptr<ByteBuffer>(b) );
        }
        break;
    } // of switch
  } // of APDU::SetResponse

  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const APDU& apdu)
  { ByteBuffer sendBuffer = (ByteBuffer)apdu;
    
    os<<_T("-----------APDU-----------")<<endl;
    os<<_T("send:")<<endl;
    os<<sendBuffer;
    
    if( apdu.GetResponseCode().GetErrorCode()!=0x0000 )
    { 
      os<<apdu.GetResponseCode()<<endl;
  
      VLPByteBuffer::const_iterator iter = apdu.GetResponseBegin();
    
      for( ;iter!=apdu.GetResponseEnd();iter++ )
      { const linked_ptr<ByteBuffer>& buf = *iter;
      
        os<<*buf;
      
      } // of for
    } // of if

    os<<_T("===========APDU===========")<<endl;
    
    return os;
  }
} // of namespace bvr20983

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const APDU&);
/*==========================END-OF-FILE===================================*/
