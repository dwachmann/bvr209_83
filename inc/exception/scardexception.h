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
#if !defined(SCARDEXCEPTION_H)
#define SCARDEXCEPTION_H

#include "exception/bvr20983exception.h"

namespace bvr20983
{
  class SCardException : public BVR20983Exception
  {
    public:
      SCardException(LONG errorCode,LPCTSTR fileName=NULL,int lineNo=-1);
  }; // of class SCardException
} // of namespace bvr20983

#define THROW_SCARDEXCEPTION(hr) \
  if( SCARD_S_SUCCESS!=hr ) \
  { throw SCardException(hr,_T(__FILE__), __LINE__); \
  } \

#endif // SCARDEXCEPTION_H
