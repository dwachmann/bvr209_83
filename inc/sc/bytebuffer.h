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
#if !defined(BYTEBUFFER_H)
#define BYTEBUFFER_H

#include "sc/sctypes.h"
#include <iostream>

namespace bvr20983
{
  class ByteBuffer
  {
    public:
      ByteBuffer()
      {}
      
      ByteBuffer(const TString& b);
      ByteBuffer(const ByteBuffer& b);
  
      ByteBuffer(const BYTE* buffer,DWORD bufferLen);
        
      operator TString();
      void GetBSTR(BSTR* s);
  
      operator const BString()
      { return m_buffer; }
  
      BYTE& operator[](DWORD i)
      { return m_buffer[i]; }
  
      const BYTE* data()
      { return m_buffer.data(); }
  
      void clear()
      { m_buffer.clear(); }
  
      DWORD size() const
      { return m_buffer.size(); }

      void resize(int s)
      { m_buffer.resize(s); }
  
      virtual void push_back(BYTE b)
      { m_buffer.push_back(b); }
  
      void append(const ByteBuffer& b)
      { m_buffer.append(b.m_buffer); }
      
      void append(const BString& b)
      { m_buffer.append(b); }

      void append(BYTE* b,int len)
      { for( int i=0;i<len;i++ )
          push_back(b[i]);
      }
  
      ByteBuffer& operator=(const BString& b)
      { m_buffer = b; return *this; }

      ByteBuffer& operator=(const ByteBuffer& b)
      { m_buffer = b.m_buffer; return *this; }

#ifdef _UNICODE      
      virtual wostream& Dump(wostream& os) const;
#else
      virtual ostream& Dump(ostream& os) const;
#endif
  
    protected:
      BString  m_buffer;
  }; // of class ByteBuffer

  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const ByteBuffer& e);

} // of namespace bvr20983

#endif // BYTEBUFFER_H
//=================================END-OF-FILE==============================