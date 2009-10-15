 /*
 * $Id$
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
#if !defined(YASTRING_H)
#define YASTRING_H

#include "util/yanew.h"

namespace bvr20983
{
  namespace util
  {
    class YAString
    {
      public:
        YAString();
        YAString(LPCWSTR str);
        YAString(LPCSTR  str);
        YAString(const YAString& str);
        YAString(unsigned long);
        YAString(long);
        ~YAString();

        LPCTSTR c_str()     const throw();
        LPCWSTR w_str()     const throw();
        LPCSTR  a_str()     const throw();
        operator LPCWSTR()  const throw();
        operator LPCSTR()   const throw();

        unsigned int    Size() const throw();
        void            Resize(unsigned int s);
        void            Append(LPCTSTR);

        int             IndexOf(LPCTSTR str) const throw();
        int             IndexOf(TCHAR   c) const throw();
        int             LastIndexOf(LPCTSTR str) const throw();
        int             LastIndexOf(TCHAR   c) const throw();
        void            ToLowerCase();
        void            ToUpperCase();

        YAPtr<YAString> Substring(int beginIndex,int endIndex=-1) const;

        YAString&       operator+=(LPCTSTR);
        YAString&       operator+=(const YAString&);
        YAString&       operator=(LPCTSTR);
        YAString&       operator=(const YAString&);

      private: 
        std::basic_string<TCHAR> m_str;
        void*                    m_buffer;
        unsigned int             m_buffersize;

        static YAAllocatorBase*  m_pClassAllocator;
        static YAAllocatorBase*  RegisterAllocator();

        void FreeBuffer();
        void Wide2Ansi();
        void Ansi2Wide();
    }; // of class YAString

    template<class charT, class Traits>
    std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const YAString& str);
  } // of namespace util
} // of namespace bvr20983
#endif // YASTRING_H
/*==========================END-OF-FILE===================================*/
