/*
 * $Id$
 * 
 * A Byte String class used in smartcard implementation.
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
#include "sc/bytebuffer.h"
#include "exception/bvr20983exception.h"
#include "exception/lasterrorexception.h"
#include "util/logstream.h"

namespace bvr20983
{
  /*
   * ByteBuffer::ByteBuffer
   *
   * Constructor Parameters:
   *  None
   */
  ByteBuffer::ByteBuffer(const TString& buffer)
  { m_buffer.clear();
  
    if( buffer.size()>0 )
    { int   bufLen = 0;
      
#ifdef _UNICODE
      bufLen = ::WideCharToMultiByte( 28591, 0, buffer.data(), buffer.size(),NULL, 0, NULL, NULL );
      THROW_LASTERROREXCEPTION1( bufLen );
  
      auto_ptr<BYTE> buf = auto_ptr<BYTE>(new BYTE[bufLen]);
      ::memset(buf.get(),bufLen,sizeof(BYTE));
  
      THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( 28591, 0, buffer.data(), buffer.size(),(LPSTR)buf.get(), bufLen, NULL, NULL ) );
  
      m_buffer = BString(buf.get(),bufLen);
#else
      BYTE* buf = (BYTE*)buffer.data();
      
      bufLen = buffer.size();

      m_buffer = BString(buf,bufLen);
#endif
    } // of if
  }
  
  /**
   *
   */
  ByteBuffer::ByteBuffer(const ByteBuffer& buffer)
  { m_buffer = buffer.m_buffer;
  }
  
  /**
   *
   */
  ByteBuffer::ByteBuffer(const BYTE* buffer,DWORD bufferLen)
  { m_buffer = BString(buffer,bufferLen);
  }
  
  /**
   *
   */
  ByteBuffer::operator TString()
  { TString result;
  
    if( m_buffer.size()>0 )
    { 
#ifdef _UNICODE
      auto_ptr<TCHAR> buffer = auto_ptr<TCHAR>(new TCHAR[m_buffer.size()]);

      ::memset(buffer.get(),m_buffer.size(),sizeof(TCHAR));
  
      THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar(28591, MB_ERR_INVALID_CHARS, (LPCSTR)m_buffer.data(),m_buffer.size(),buffer.get(),m_buffer.size()) );

      result = TString(buffer.get(),m_buffer.size());
#else
      LPTSTR buffer = (LPTSTR)m_buffer.data();

      result = TString(buffer,m_buffer.size());
#endif
  
    } // of if
    
    return result;
  }

/**
   *
   */
  void ByteBuffer::GetBSTR(BSTR* result)
  { *result = NULL;

    if( m_buffer.size()>0 )
    { 
#ifdef _UNICODE
      auto_ptr<TCHAR> buffer = auto_ptr<TCHAR>(new TCHAR[m_buffer.size()]);

      ::memset(buffer.get(),m_buffer.size(),sizeof(TCHAR));
  
      THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar(28591, MB_ERR_INVALID_CHARS, (LPCSTR)m_buffer.data(),m_buffer.size(),buffer.get(),m_buffer.size()) );

      *result = ::SysAllocString(buffer.get());
#else
      BSTR buffer = (BSTR)m_buffer.data();

      *result = ::SysAllocString(buffer);
#endif
  
    } // of if
  }

#ifdef _UNICODE
  wostream& ByteBuffer::Dump(wostream& os) const
#else
  ostream& ByteBuffer::Dump(ostream& os) const
#endif  
  { ios_base::iostate err = 0;
  
#ifdef _UNICODE
    wostream::sentry opfx(os);
#else
    ostream::sentry opfx(os);
#endif  

    try
    {
      if( opfx && !m_buffer.empty() )
      { os<<setfill(_T('0'));
      
        DWORD lines = m_buffer.size()/16 + (m_buffer.size()%16!=0 ? 1 : 0);
        
        for( DWORD l=0;l<lines;l++ )
        { DWORD c=0;
      
          os<<setw(4)<<l*16<<_T(":");
        
          for( c=0;c<16;c++ )
          { if( l*16+c<m_buffer.size() )
            { const BYTE& b = m_buffer[l*16+c];
          
              os<<_T(" ")<<setw(2)<<hex<<b;
            } // of if
            else 
              os<<_T("___");
          } // of for
      
          os<<_T("|");
          
          for( c=0;c<16;c++ )
          { if( l*16+c<m_buffer.size() )
            { const BYTE& b = m_buffer[l*16+c];
      
              if( b>=0x20 && b<=0x7a )
              { TCHAR ch = (TCHAR)b;
              
                os<<ch;
              } // of if
              else
                os<<_T(".");
            } // of if
            else
              os<<_T("_");
          } // of for
      
          os<<_T("|")<<endl;
        } // of for
        
        os.width(0);
        os.fill(_T(' '));
      } // of if
    }
    catch(...)
    { bool flag = false;
    
      try
      { os.setstate(ios_base::failbit); }
      catch(...)
      { flag = true; }
      
      if( flag ) 
        throw;
    } 
    
    if( err )
      os.setstate(err);
    
    return os;
  }


  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const ByteBuffer& e)
  { return e.Dump(os); }
} // of namespace bvr20983

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const ByteBuffer&);
/*==========================END-OF-FILE===================================*/
