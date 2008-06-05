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
#if !defined(MEMORYEXCEPTION_H)
#define MEMORYEXCEPTION_H

#include "exception/bvr20983exception.h"

namespace bvr20983
{

  class MemoryException : public BVR20983Exception
  {
    public:
      MemoryException(LPCTSTR fileName=NULL,int lineNo=-1);

  }; // of class MemoryException
} // of namespace bvr20983

#define THROW_MEMORYEXCEPTION(p) \
  if( NULL==p ) \
  { throw MemoryException(_T(__FILE__), __LINE__); \
  } \

#endif // MEMORYEXCEPTION_H
