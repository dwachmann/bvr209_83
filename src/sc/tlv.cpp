/*
 * $Id$
 * 
 * Class to construct a TagLengthValue bytedump.
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
#include "sc/bytebuffer.h"
#include "sc/tlv.h"

namespace bvr20983
{

  /*
   * TLV::TLV
   *
   * Constructor Parameters:
   *  None
   */
  TLV::TLV()
  { }
  
  TLV::TLV(BYTE tag) : m_tag(tag)
  { push_back(m_tag);
    push_back(0);
  
  }
  
  TLV::TLV(BYTE tag,const ByteBuffer& data) : m_tag(tag)
  { SetData(data); }
  
  TLV::TLV(const TLV& tlv)
  { *this = tlv; }
  
  /**
   *
   */
  void TLV::SetData(const ByteBuffer& data)
  { clear();
    
    ByteBuffer::push_back(m_tag);
    ByteBuffer::push_back((BYTE)data.size());
    ByteBuffer::append(data);
  }

#ifdef _UNICODE
  wostream& TLV::Dump(wostream& os) const
#else
  ostream& TLV::Dump(ostream& os) const
#endif  
  { os<<_T("TLV[tag=0x")<<hex<<setw(2)<<GetTag()<<_T("]")<<endl;
  
    return ByteBuffer::Dump(os);;
  }
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
