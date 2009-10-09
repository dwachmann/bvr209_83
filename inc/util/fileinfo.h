/*
 * $Id: dirinfo.h 184 2009-03-26 16:04:08Z dwachmann $
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
#if !defined(FILEINFO_H)
#define FILEINFO_H

#include "util/yastring.h"
#include "util/yanew.h"

namespace bvr20983
{
  namespace util
  {
    class FileInfo;

    /**
     *
     */
    class FileInfo
    {
      public:
        FileInfo(LPCTSTR fileName);
        ~FileInfo();

		    YAPtr<YAString> GetFullPathName() const;

        LPCTSTR GetName() const
        { return m_fileName; }

      private:
        LPCTSTR m_fileName;

	  }; // of class FileInfo

    template<class charT, class Traits>
    std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const FileInfo& str);
  } // of namespace util
} // of namespace bvr20983
#endif // FILEINFO_H
/*==========================END-OF-FILE===================================*/
