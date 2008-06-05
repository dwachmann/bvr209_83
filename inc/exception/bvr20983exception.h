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
#if !defined(BVR20983EXCEPTION_H)
#define BVR20983EXCEPTION_H

#include "os.h"
#include <iomanip>
#include <iostream>

namespace bvr20983
{
  class BVR20983Exception
  {
    public:
      BVR20983Exception(LONG errorCode=0,LPCTSTR errorMessage=NULL,LPCTSTR fileName=NULL,int lineNo=-1);

      virtual LONG GetErrorCode() const
      { return m_errorCode; }
      
      virtual LPCTSTR GetErrorMessage() const
      { return m_errorMessage; }

      virtual int GetLineNo() const
      { return m_lineNo; }

      virtual LPCTSTR GetFileName() const
      { return m_fileName; }

    protected:
      LONG    m_errorCode;
      LPCTSTR m_errorMessage;
      int     m_lineNo;
      LPCTSTR m_fileName;
  }; // of class BVR20983Exception
  
  template<class charT, class Traits>
  std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const BVR20983Exception& e);
} // of namespace bvr20983

#endif // BVR20983EXCEPTION_H
