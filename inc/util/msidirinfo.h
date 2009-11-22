/*
 * $Id:$
 * 
 * Copyright (C) 2009 Dorothea Wachmann
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
#if !defined(MSIDIRINFO_H)
#define MSIDIRINFO_H

#include "util/yastring.h"

namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    struct MSIDirectoryInfo
    {
      MSIDirectoryInfo()
      { }

      MSIDirectoryInfo(LPCTSTR dirName) : m_dirName(dirName)
      { }

      MSIDirectoryInfo(LPCTSTR dirName,LPCTSTR dirShortName) : m_dirName(dirName),m_dirShortName(dirShortName)
      { }

      YAString m_dirId;
      YAString m_parentId;
      YAString m_dirName;
      YAString m_dirShortName;
      YAString m_dirPath;
    }; // of struct MSIDirectoryInfo

    typedef std::vector<MSIDirectoryInfo> VMSIDirInfoT;
  } // of namespace util
} // of namespace bvr20983
#endif // MSIDIRINFO_H
/*==========================END-OF-FILE===================================*/
