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
#if !defined(ATR_H)
#define ATR_H

#include "os.h"
#include <iostream>
#include "sc/bytebuffer.h"

namespace bvr20983
{
  class ATR
  {
    public:
      ATR();
      ATR(const ByteBuffer& buf);

      template<class charT, class Traits>
      friend basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const ATR& e);
      
      void clear();

      DWORD GetF() const
      { return m_F; }

      DWORD GetFMax() const
      { return m_FMax; }

      DWORD GetD() const
      { return m_D; }

      void GetHistory(ByteBuffer& h) const
      { h.append((BYTE*)m_hist,m_histLen); }

      const ByteBuffer& GetBuffer() const
      { return m_buffer; }

    private:
      ByteBuffer m_buffer;

      BYTE       m_ts;
      BYTE       m_ta[5];
      BYTE       m_tb[5];
      BYTE       m_tc[5];
      BYTE       m_td[5];
      
      DWORD      m_F;
      DWORD      m_FMax;
      DWORD      m_D;
      
      BYTE       m_histLen;
      BYTE       m_hist[128];
  }; // of class ATR

  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >&,const ATR&);

} // of namespace bvr20983

#endif // ATR_H
