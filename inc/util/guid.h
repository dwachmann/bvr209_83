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
#if !defined(TheGUID_H)
#define TheGUID_H

#include "os.h"

namespace bvr20983
{
  class CGUID
  {
    public:
      CGUID(REFGUID guid);
      
      operator REFGUID() const
      { return m_guid; }

      operator const TString&() const
      { return m_guidStr; }

      bool operator <(const CGUID& guid) const
      { return m_guidStr<guid.m_guidStr; }

      bool operator ==(const CGUID& guid) const
      { return m_guid==guid.m_guid ? true : false; }

    private:
      REFGUID  m_guid;
      TString  m_guidStr;
  }; // of class CGUID
} // of namespace bvr20983

#endif // TheGUID_H
//=================================END-OF-FILE==============================