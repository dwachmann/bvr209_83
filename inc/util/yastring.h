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
        YAString(unsigned long);
        YAString(long);
        ~YAString();

        LPCTSTR c_str() const;
        LPCWSTR w_str() const;
        LPCSTR  a_str() const;
        operator LPCWSTR() const;
        operator LPCSTR() const;

        unsigned int Size() const;
        void         Append(LPCTSTR);

        YAString& operator+=(LPCTSTR);
        YAString& operator+=(const YAString&);
        YAString& operator=(LPCTSTR);
        YAString& operator=(const YAString&);

        void*     operator new(size_t);
        void      operator delete(void*);

      private: 
        std::basic_string<TCHAR> m_str;
        void*                    m_buffer;
        unsigned int             m_buffersize;

        void FreeBuffer();
        void Wide2Ansi();
        void Ansi2Wide();
    }; // of class YAString
  } // of namespace util
} // of namespace bvr20983
#endif // YASTRING_H
/*==========================END-OF-FILE===================================*/
