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
#if !defined(TLV_H)
#define TLV_H

#include "os.h"

namespace bvr20983
{
  class TLV : public ByteBuffer
  {
    public:
      TLV();
      TLV(BYTE tag);
      TLV(BYTE tag,const ByteBuffer& data);
      TLV(const TLV& b);
  
      void SetTag(BYTE tag)
      { m_tag = tag; }

      BYTE GetTag() const
      { return m_tag; }
  
      void SetData(const ByteBuffer& data);
  
      virtual void push_back(BYTE b)
      { ByteBuffer::push_back(b); 
      
        (*this)[1] = (BYTE)size()-2;
      }

#ifdef _UNICODE      
      virtual wostream& Dump(wostream& os) const;
#else
      virtual ostream& Dump(ostream& os) const;
#endif
  
    private:
      BYTE m_tag;
  }; // of class TLV
} // of namespace bvr20983


#endif // TLV_H
//=================================END-OF-FILE==============================