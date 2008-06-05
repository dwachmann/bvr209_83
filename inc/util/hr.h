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
#if !defined(TheHRESULT_H)
#define TheHRESULT_H

#include "os.h"

namespace bvr20983
{
  class CHResult
  {
    public:
      CHResult(HRESULT hr);
      
      operator HRESULT() const
      { return m_hr; }

      operator const TString&() const
      { return m_hrStr; }

    private:
      HRESULT  m_hr;
      TString  m_hrStr;
  }; // of class CHResult
} // of namespace bvr20983
#endif // TheHRESULT_H
//=================================END-OF-FILE==============================