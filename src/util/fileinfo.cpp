/*
 * $Id: dirinfo.cpp 184 2009-03-26 16:04:08Z dwachmann $
 * 
 * Windows DC class.
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
#include "util/fileinfo.h"
#include "util/logstream.h"
#include "exception/lasterrorexception.h"

namespace bvr20983 
{
  namespace util
  {
	/**
	 *
	 */
    FileInfo::FileInfo(LPCTSTR fileName) :
      m_fileName(fileName)
    { }

 	/**
	 *
	 */
    FileInfo::~FileInfo()
    { 
    } // of FileInfo::~FileInfo()

	/**
	 *
	 */
	YAPtr<YAString> FileInfo::GetFullPathName() const
	{ YAPtr<YAString> result;

	  if( NULL!=m_fileName )
	  {
	  } // of if

	  return result;
	} // of FileInfo::GetFullPathName() 
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
