/*
 * $Id$
 * 
 * iostream streambuffer.
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
#include <assert.h>
#include "util/logstreambuf.h"
#include "util/logstream.h"

namespace bvr20983
{
  template< class charT,class traits >
  CriticalSection LogStreamBuf<charT,traits>::m_criticalSection = CriticalSection();

  /**
   *
   */
  template< class charT,class traits >
  LogStreamBuf<charT,traits>::LogStreamBuf()
  { m_pLogStream = NULL;
    m_dumpPrefix = true;
    
    setp(m_buffer,m_buffer+sizeof(m_buffer)/sizeof(m_buffer[0]));
  }

  /**
   *
   */
  template< class charT,class traits >
  LogStreamBuf<charT,traits>::~LogStreamBuf()
  { 
  }
      
  template< class charT,class traits >
  bool LogStreamBuf<charT,traits>::ContainsEndl() const
  { bool result = pptr() - pbase()>0 ? false : true;
  
    charT* m = pptr();
    for( charT* i = pbase();i<m;i++ )
      if( *i==_T('\n') )
      { result = true;
      
        break;
      } // of if
    
    return result;
  } 
  
  /**
   *
   */
  template< class charT,class traits >
  typename LogStreamBuf<charT,traits>::int_type 
  LogStreamBuf<charT,traits>::overflow(typename LogStreamBuf<charT,traits>::int_type c)
  { Critical crit(m_criticalSection);

    assert( m_pLogStream!=NULL );
    
    streamsize len = pptr() - pbase();
    
    if( len>0 )
    { AppenderList::iterator iter;
    
      for( iter=m_appenders.begin();iter!=m_appenders.end();iter++ )
      { LogAppender<TCHAR>& logAppender = **iter;
      
        logAppender.Dump(m_pLogStream->GetLevel(),
                         m_pLogStream->GetSourceFileName(),
                         m_pLogStream->GetSourceLineNo(),
                         m_pLogStream->GetIndent(),
                         m_dumpPrefix,
                         pbase(),len
                        );
      } // of for
      
      if( m_dumpPrefix )
        m_dumpPrefix = false;
          
      if( ContainsEndl() )
      { m_dumpPrefix = true;
        //m_pLogStream->SetLevel(LogLevel<charT,traits>::INFO_LEVEL);
        //m_pLogStream->SetSourceLineNo(-1);
      } // of if
    } // of if     

    pbump(-len);
    
    if( traits_type::not_eof(c) )
      sputc(c);
  
    return traits_type::not_eof(c);
  } // of LogStreamBuf<charT,traits>::overflow()

  /**
   *
   */
  template< class charT,class traits >
  int LogStreamBuf<charT,traits>::sync()
  { overflow();
  
    return 0;
  } // of int LogStreamBuf<charT,traits>::sync()


  /**
   *
   */
  template< class charT,class traits >
  OutputDebugStreamBuf<charT,traits>::OutputDebugStreamBuf()
  { setp(m_buffer,m_buffer+sizeof(m_buffer)/sizeof(m_buffer[0])); }

  /**
   *
   */
  template< class charT,class traits >
  typename OutputDebugStreamBuf<charT,traits>::int_type 
  OutputDebugStreamBuf<charT,traits>::overflow(typename OutputDebugStreamBuf<charT,traits>::int_type c)
  { streamsize len = pptr() - pbase();
    
    if( len>0 )
    { charT c = m_buffer[len];

      m_buffer[len] = _T('\0');

      OutputDebugString(pbase());

      m_buffer[len] = c;
    } // of if     

    pbump(-len);
    
    if( traits_type::not_eof(c) )
      sputc(c);
  
    return traits_type::not_eof(c);
  } // of OutputDebugStreamBuf<charT,traits>::overflow()

  /**
   *
   */
  template< class charT,class traits >
  int OutputDebugStreamBuf<charT,traits>::sync()
  { overflow();
  
    return 0;
  }
} // of namespace bvr20983

template class bvr20983::LogStreamBuf<TCHAR>;
template class bvr20983::OutputDebugStreamBuf<TCHAR>;
/*==========================END-OF-FILE===================================*/
