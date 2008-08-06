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
#if !defined(LASTERROREXCEPTION_H)
#define LASTERROREXCEPTION_H

#include "exception/bvr20983exception.h"

namespace bvr20983
{

  class LastErrorException : public BVR20983Exception
  {
    public:
      LastErrorException(LPCTSTR fileName=NULL,int lineNo=-1) : BVR20983Exception(::GetLastError(),NULL,fileName,lineNo)
      { LoadMessage(); }
      
      LastErrorException(LONG errorNo,LPCTSTR fileName=NULL,int lineNo=-1) : BVR20983Exception(errorNo,NULL,fileName,lineNo)
      { LoadMessage(); }
      
      ~LastErrorException();
      
    private:
      bool m_IsLastError;
      
      void LoadMessage();
  }; // of class LastErrorException
} // of namespace bvr20983

#define THROW_LASTERROREXCEPTION(hr) \
  if( ERROR_SUCCESS!=hr ) \
  { throw LastErrorException(hr,_T(__FILE__), __LINE__); \
  } \

#define THROW_LASTERROREXCEPTION1(hr) \
  if( 0==hr ) \
  { throw LastErrorException(_T(__FILE__), __LINE__); \
  } \

#define THROW_LASTERROREXCEPTION2 \
  throw LastErrorException(_T(__FILE__), __LINE__);

#define THROW_LASTERROREXCEPTION3(err) \
  throw LastErrorException(err,_T(__FILE__), __LINE__);

#endif // LASTERROREXCEPTION_H
