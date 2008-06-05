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
#if !defined(SCARDEVENTSTATE_H)
#define SCARDEVENTSTATE_H

#include "os.h"
#include <iostream>

namespace bvr20983
{
  class SCardEventState
  {
    public:
      SCardEventState(DWORD dwEventState) : m_dwEventState(dwEventState)
      { }
      
      DWORD GetEventState() const
      { return m_dwEventState; }
      

    private:
      DWORD   m_dwEventState;
  }; // of class SCardEventState
  
  template<class charT, class Traits>
  std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const SCardEventState& e);
} // of namespace bvr20983

#endif // SCARDEVENTSTATE_H
