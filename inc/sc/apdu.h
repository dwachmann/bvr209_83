/*
 * $Id$
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
#if !defined(APDU_H)
#define APDU_H

#include "os.h"
#include <iostream>
#include "util/linked_ptr.h"
#include "sc/bytebuffer.h"
#include "exception/scardapduexception.h"

namespace bvr20983
{
  typedef vector< linked_ptr<ByteBuffer> > VLPByteBuffer;

  class Smartcard;
  
  class APDU
  {
    public:
      enum ResponseType
      { RAW_RESPONSE,
        TLV_RESPONSE
      };
  
      APDU();
      APDU(const APDU& b);
      APDU(BYTE cla,BYTE ins,BYTE p1=0,BYTE p2=0);
  
      operator ByteBuffer() const;
      APDU& operator=(const APDU& b);
      
      void SetClass(BYTE cla)
      { this->m_cla = cla; }
  
      void SetInstruction(BYTE ins)
      { this->m_ins = ins; }
  
      void SetP1(BYTE p1)
      { this->m_p1 = p1; }
  
      void SetP2(BYTE p2)
      { this->m_p2 = p2; }
  
      void SetLe(BYTE le)
      { this->m_le = le; this->m_leSet=TRUE; }
      
      void AddData(const ByteBuffer& data)
      { m_data.push_back( linked_ptr<ByteBuffer>(new ByteBuffer(data) ) ); }
  
      void Clear()
      { m_response.clear(); 
        m_responseCode = SCardAPDUException();
      }

      void SetResponseType(ResponseType t)
      { m_responseType = t; }
      
      VLPByteBuffer::const_iterator GetResponseBegin() const
      { return m_response.begin(); }

      VLPByteBuffer::const_iterator GetResponseEnd() const
      { return m_response.end(); }

      const SCardAPDUException& GetResponseCode() const
      { return m_responseCode; }
  
    private:
      BYTE CalcLength() const;
      void SetResponse(const BYTE* buffer,DWORD bufferLen,const SCardAPDUException& ex);

      friend class Smartcard;
 
      BYTE               m_cla;
      BYTE               m_ins;
      BYTE               m_p1;
      BYTE               m_p2;
  
      bool               m_leSet;
      BYTE               m_le;
      
      VLPByteBuffer      m_data;
      
      ResponseType       m_responseType;
      VLPByteBuffer      m_response;
      SCardAPDUException m_responseCode;
  }; // of class APDU

  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const APDU& e);
} // of namespace bvr20983

#endif // APDU_H
//=================================END-OF-FILE==============================