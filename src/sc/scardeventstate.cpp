/*
 * $Id$
 * 
 * Class for streaming SCard Event.
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
#include "sc/scardeventstate.h"

namespace bvr20983
{

  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const SCardEventState& e)
  { DWORD dwEventState = e.GetEventState();
  
    os<<_T("[0x")<<hex<<dwEventState<<_T("] ");
    
    if( dwEventState & SCARD_STATE_IGNORE )
      os<<_T(" SCARD_STATE_IGNORE");

    if( dwEventState & SCARD_STATE_CHANGED )
      os<<_T(" SCARD_STATE_CHANGED");

    if( dwEventState & SCARD_STATE_UNKNOWN )
      os<<_T(" SCARD_STATE_UNKNOWN");

    if( dwEventState & SCARD_STATE_UNAVAILABLE )
      os<<_T(" SCARD_STATE_UNAVAILABLE");

    if( dwEventState & SCARD_STATE_EMPTY )
      os<<_T(" SCARD_STATE_EMPTY");

    if( dwEventState & SCARD_STATE_PRESENT )
      os<<_T(" SCARD_STATE_PRESENT");

    if( dwEventState & SCARD_STATE_ATRMATCH )
      os<<_T(" SCARD_STATE_ATRMATCH");

    if( dwEventState & SCARD_STATE_EXCLUSIVE )
      os<<_T(" SCARD_STATE_EXCLUSIVE");

    if( dwEventState & SCARD_STATE_INUSE )
      os<<_T(" SCARD_STATE_INUSE");

    if( dwEventState & SCARD_STATE_MUTE )
      os<<_T(" SCARD_STATE_MUTE")<<endl;
      
    return os;
  }
}

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const SCardEventState&);
/*==========================END-OF-FILE===================================*/
