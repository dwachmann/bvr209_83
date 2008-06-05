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
#if !defined(SCARDATTRIBUTE_H)
#define SCARDATTRIBUTE_H

#include "os.h"

namespace bvr20983
{
  class SCardAttribute
  {
    public:
      SCardAttribute(SCARDHANDLE hCard,DWORD dwAttrId);
      ~SCardAttribute();

      template<class charT, class Traits>
      friend basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const SCardAttribute& e);
      
    private:
      SCARDHANDLE m_hCard;
      LPBYTE      m_pbAttr;
      DWORD       m_dwAttribId;
      ByteBuffer  m_attrib;
  }; // of class SCardAttribute

  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const SCardAttribute& e);

} // of namespace bvr20983

#endif // SCARDATTRIBUTE_H
