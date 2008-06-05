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
#if !defined(COMSTRING_H)
#define COMSTRING_H

#include "os.h"

namespace bvr20983
{
  class COMString
  {
    public:
      COMString()
      { m_str = NULL; }

      COMString(BSTR str)
      { m_str = str; }

      ~COMString()
      { ::SysFreeString(m_str); }
      
      operator const BSTR&() const
      { return m_str; }

      BSTR* operator&()
      { return &m_str; }

    private:
      BSTR m_str;
  }; // of class COMString
} // of namespace bvr20983

#endif // COMSTRING_H
//=================================END-OF-FILE==============================