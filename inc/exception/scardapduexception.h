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
#if !defined(SCARDAPDUEXCEPTION_H)
#define SCARDAPDUEXCEPTION_H

#include "exception/scardexception.h"

namespace bvr20983
{
  class SCardAPDUException : public SCardException
  {
    public:
      SCardAPDUException(LONG errorCode=0x0L);
      
      bool IsError() const;
      
      BYTE GetSW1() const
      { return m_sw1; }

      BYTE GetSW2() const
      { return m_sw2; }
      
    private:
      BYTE m_sw1;
      BYTE m_sw2;
  }; // of class SCardAPDUException

  template<class charT, class Traits>
  std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const SCardAPDUException& e);
} // of namespace bvr20983

#endif // SCARDAPDUEXCEPTION_H
