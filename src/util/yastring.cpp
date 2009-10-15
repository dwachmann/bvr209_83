/*
 * $Id$
 * 
 * Yet Another String Wrapper Class.
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
#include "os.h"
#include "util/yastring.h"
#include "util/yanew.h"
#include "util/yaallocatorpool.h"
#include "util/logstream.h"
#include "exception/lasterrorexception.h"

namespace bvr20983
{
  namespace util
  {
    YAAllocatorBase* YAString::m_pClassAllocator = YAString::RegisterAllocator();

    /**
     *
     */
    YAAllocatorBase* YAString::RegisterAllocator()
    { YAAllocatorPool::AddAllocator(_T("YAString"),new YAAllocator<YAString>);
    
      return YAAllocatorPool::GetAllocator(_T("YAString"));
    }

    /**
     *
     */
    YAString::YAString() : m_buffer(NULL),m_buffersize(0)
    { }

    /**
     *
     */
    YAString::YAString(const YAString& str) : m_buffer(NULL),m_buffersize(0)
    { *this = str; }

    /**
     *
     */
    YAString::YAString(LPCWSTR str) : m_buffer(NULL),m_buffersize(0)
    { 
#ifdef _UNICODE
      m_str = str;

      //OutputDebugFmt(_T("YAString::YAString(%s)"),m_str.c_str());
#else
      m_buffersize = wcslen(str)+1;
      m_buffer     = ::calloc(m_buffersize,sizeof(char));

      THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, str, -1,(LPSTR)m_buffer, m_buffersize, NULL, NULL ) );

      m_str = (LPCSTR)m_buffer;

      FreeBuffer();
#endif
    } // of YAString::YAString()

    /**
     *
     */
    YAString::YAString(LPCSTR str) : m_buffer(NULL),m_buffersize(0)
    { 
#ifdef _UNICODE
      m_buffersize = strlen(str)+1;
      m_buffer     = ::calloc(m_buffersize,sizeof(WCHAR));

      THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, str, -1,(LPWSTR)m_buffer, m_buffersize) );

      m_str = (LPWSTR)m_buffer;

      FreeBuffer();
#else
      m_str = str;
#endif
    } // of YAString::YAString()

    /**
     *
     */
    YAString::YAString(unsigned long dword) : m_buffer(NULL),m_buffersize(0)
    { TCHAR dwordStr[64];

      _ultot_s(dword,dwordStr,ARRAYSIZE(dwordStr),10);

      m_str = dwordStr;
    }

    /**
     *
     */
    YAString::YAString(long dword) : m_buffer(NULL),m_buffersize(0)
    { TCHAR dwordStr[64];

      _ltot_s(dword,dwordStr,ARRAYSIZE(dwordStr),10);

      m_str = dwordStr;
    }


    /**
     *
     */
    YAString::~YAString()
    { FreeBuffer();
    } // of YAString::~YAString()

    /**
     *
     */
    LPCTSTR YAString::c_str() const
    { return m_str.c_str(); }


    /**
     *
     */
    LPCWSTR YAString::w_str() const
    { LPCWSTR result = NULL;

#ifdef _UNICODE
      result = c_str();
#else
      const_cast<YAString*>(this)->Wide2Ansi();

      result = (LPCWSTR)m_buffer;
#endif

      return result; 
    } // of YAString::w_str()

    /**
     *
     */
    LPCSTR YAString::a_str() const
    { LPCSTR result = NULL;

#ifdef _UNICODE
      const_cast<YAString*>(this)->Ansi2Wide();

      result = (LPCSTR)m_buffer;
#else
      result = c_str();
#endif

      return result; 
    }

    /**
     *
     */
    void YAString::FreeBuffer()
    { if( NULL!=m_buffer )
        ::free(m_buffer);

      m_buffer     = NULL;
      m_buffersize = 0;
    } // of YAString::FreeBuffer()

    /**
     * 
     */
    void YAString::Ansi2Wide()
    { FreeBuffer();

      m_buffersize = m_str.size()+1;
      m_buffer     = ::calloc(m_buffersize,sizeof(WCHAR));

      THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, (LPSTR)c_str(), -1,(LPWSTR)m_buffer, m_buffersize) );
    } // of YAString::convert()

    /**
     * 
     */
    void YAString::Wide2Ansi()
    { FreeBuffer();

      m_buffersize = m_str.size()+1;
      m_buffer     = ::calloc(m_buffersize,sizeof(char));

      THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, c_str(), -1,(LPSTR)m_buffer, m_buffersize, NULL, NULL ) );
    } // of YAString::Wide2Ansi()

    /**
     *
     */
    void YAString::Append(LPCTSTR s)
    { m_str.append(s);
    } // of YAString::Append()

    /**
     *
     */
    YAString& YAString::operator+=(LPCTSTR s)
    { Append(s); return *this; }

    /**
     *
     */
    YAString& YAString::operator+=(const YAString& s)
    { Append(s.c_str()); return *this; }

    /**
     *
     */
    unsigned int YAString::Size() const
    { return m_str.size();
    } // of YAString::Size()

    /**
     *
     */
    void YAString::Resize(unsigned int s)
    { m_str.resize(s);
    } // of YAString::Resize()

    /**
     *
     */
    YAString& YAString::operator=(const YAString& s)
    { FreeBuffer();
      
      m_str = s.m_str; 
      
      return *this; 
    }

    /**
     *
     */
    YAString& YAString::operator=(LPCTSTR s)
    { FreeBuffer();
      
      m_str = s; 
      
      return *this; 
    }

    /**
     *
     */
    YAString::operator LPCWSTR() const
    { return w_str(); }

    /**
     *
     */
    YAString::operator LPCSTR() const
    { return a_str(); }

    /**
     *
     */
    int YAString::IndexOf(LPCTSTR str) const
    { int result = -1;

      if( m_str.length()>0 && NULL!=str && _tcslen(str)>0 )
      { LPCTSTR foundStr = _tcsstr(m_str.c_str(),str);

        if( foundStr!=NULL )
          result = foundStr - m_str.c_str();
      } // of if

      return result;
    } // of YAString::IndexOf()

    /**
     *
     */
    int YAString::IndexOf(TCHAR c) const
    { int result = -1;

      if( m_str.length()>0 && c!=_T('\0') )
      { LPCTSTR foundChr = _tcschr(m_str.c_str(),c);

        if( foundChr!=NULL )
          result = foundChr - m_str.c_str();
      } // of if

      return result;
    } // of YAString::IndexOf()

    /**
     *
     */
    int YAString::LastIndexOf(LPCTSTR str) const
    { int result = -1;

      if( m_str.length()>0 && NULL!=str && _tcslen(str)>0 )
      { LPCTSTR foundStr = NULL;
        LPCTSTR actStr   = m_str.c_str();

        while( (foundStr=_tcsstr(actStr,str))!=NULL && *actStr!=_T('\0') )
          actStr = foundStr + 1;

        if( NULL!=foundStr )
          result = foundStr - m_str.c_str();
      } // of if

      return result;
    } // of YAString::LastIndexOf()

    /**
     *
     */
    int YAString::LastIndexOf(TCHAR c) const
    { int result = -1;

      if( m_str.length()>0 && c!=_T('\0') )
      { LPCTSTR foundChr = _tcsrchr(m_str.c_str(),c);

        if( foundChr!=NULL )
          result = foundChr - m_str.c_str();
      } // of if

      return result;
    } // of YAString::LastIndexOf()

    /**
     *
     */
    YAPtr<YAString> YAString::Substring(int beginIndex,int endIndex) const
    { YAPtr<YAString> result;

      if( endIndex!=-1 )
        result = YAPTR1(YAString,m_str.substr(beginIndex,endIndex-beginIndex).c_str());
      else
        result = YAPTR1(YAString,m_str.substr(beginIndex).c_str());

      return result;
    } // of YAString::Substring()

    /**
     *
     */
    void YAString::ToLowerCase()
    { unsigned int len = m_str.length();

      for( unsigned int i=0;i<len;i++ )
        m_str[i] = tolower(m_str[i]);
    } // of YAString::ToLower()

    void YAString::ToUpperCase()
    { unsigned int len = m_str.length();

      for( unsigned int i=0;i<len;i++ )
        m_str[i] = toupper(m_str[i]);
    } // of YAString::ToUpper()

    /**
     *
     */
    template<class charT, class Traits>
    basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const YAString& str)
    { os<<_T("\"")<<str.c_str()<<_T("\""); 

      return os;
    }

    template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const YAString&);
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
