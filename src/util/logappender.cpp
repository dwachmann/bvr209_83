/*
 * $Id$
 * 
 * LogAppender class.
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
#include <time.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include "util/logappender.h"
#include "util/logstreambuf.h"

void OutputDebugFmt(LPTSTR pszFmt,...);

namespace bvr20983
{

  template< class charT,class traits >
  LogAppenders<charT,traits> LogAppender<charT,traits>::m_logAppenders;

  /**
   *
   */
  template< class charT,class traits >
  LogAppender<charT,traits>::operator basic_ostream<charT,traits>&()
  { 
    switch( m_type )
    { case CONSOLE:
#ifdef _UNICODE
         return wcerr;
#else
         return cerr;
#endif    
        break;
      case FILE:
        if( NULL==m_pOs )

#ifdef _UNICODE
          m_pOs = new wofstream(m_fName.c_str(),ios_base::app);
#else
          m_pOs = new ofstream(m_fName.c_str(),ios_base::app);
#endif  
        break;
      case DEBUGOUTPUT:
        if( NULL==m_pOs )
          m_pOs = new basic_ostream<charT,traits>(new OutputDebugStreamBuf<charT,traits>());
        break;
      default:
        throw invalid_argument("invalid appender type");
        break;
    } // of switch

    if( NULL!=m_pOs )
      return *m_pOs;

    throw runtime_error("no output stream set");
  } // of LogAppender<charT,traits>::operator basic_ostream<charT,traits>&()


  template< class charT,class traits >
  void LogAppender<charT,traits>::Dump(LogLevelT logStreamLevel,
                                       LPCTSTR sourceFileName,
                                       int sourceLineNo,
                                       int indent,
                                       bool dumpPrefix,
                                       charT* buffer,streamsize len
                                      )
  {     
    if( GetLevel()>=logStreamLevel )
    { basic_ostream<TCHAR>& os = *this;

      if( dumpPrefix )
      { 
        if( m_type==CONSOLE )
        { 
          switch( logStreamLevel.GetLevel() )
          { case LogLevelT::FATAL_LEVEL:
              ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
              break;
            case LogLevelT::ERROR_LEVEL:
              ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
              break;
            case LogLevelT::WARN_LEVEL:
              ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED|FOREGROUND_GREEN| FOREGROUND_INTENSITY);
              break;
            case LogLevelT::INFO_LEVEL:
              ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_BLUE);
              break;
            case LogLevelT::DEBUG_LEVEL:
              ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED|BACKGROUND_BLUE);
              break;
            default:
              ::SetConsoleTextAttribute(::GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
              break;
          } // of switch
        } // of if
        else
        {
          os<<_T("[");
  
          if( m_showSourceFile && NULL!=sourceFileName )
          { LPCTSTR i = _tcsrchr(sourceFileName,_T('\\'));
            
            if( NULL!=i && *(i+1)!=_T('\0') )
              i += 1;
            else
              i = sourceFileName;
            
            os<<setw(20)<<left<<i<<_T(",");
          } // of if
  
          if( m_showThreadID )
          { os<<std::hex<<_T("0x")<<(::GetCurrentThreadId())<<_T(","); }
  
          if( m_showLineNo )
          { if( sourceLineNo!=-1 )
              os<<std::setw(5)<<sourceLineNo<<_T(",");
            else
              os<<_T("     ,");
          } // of if
          
          if( m_showLevel )
            os<<std::setw(5)<<logStreamLevel;
          
          if( m_showTimestamp )
          {
            time_t    ltime;
            struct tm tm;
            
            time(&ltime);
            localtime_s(&tm,&ltime);
            
            TCHAR  strftimebuf[255];
            _tcsftime(strftimebuf, sizeof(strftimebuf)/sizeof(strftimebuf[0]), _T("%Y-%m-%d-%H:%M:%S"), &tm);
            
            os<<_T(",")<<strftimebuf;
          } // of if
  
          os<<_T("] ");
  
          for( int i=0;i<indent;i++ )
            os<<_T("  ");
        } // of else
      } // of if
      
      os.write(buffer,len);
/*
      { TString s(buffer,len);

        ::OutputDebugString(s.c_str());
      }
*/
    } // of if
  }

  /**
   *
   */
  template< class charT,class traits >
  typename LogAppenders<charT,traits>::AppenderT& 
  LogAppenders<charT,traits>::GetInstance(typename LogAppenders<charT,traits>::AppenderT& result,typename AppenderType type,typename LogLevelT l,LPCTSTR fName)
  { AppenderList::iterator iter;

    //OutputDebugFmt(_T("LogAppenders::GetInstance(fname=%s)\n"),fName);
  
    for( iter=m_appenderList.begin();iter!=m_appenderList.end();iter++ )
    { AppenderT& app = *iter;

      //OutputDebugFmt(_T("LogAppenders::GetInstance(): app->GetType()=%d app->GetFName()=%s\n"),app->GetType(),app->GetFName());

      if( app->GetType()==type && ((fName==NULL && app->GetFName()==NULL) || _tcscmp(fName,app->GetFName())==0) )
      { result = app;

        //OutputDebugFmt(_T("LogAppenders::GetInstance(fname=%s): found appender\n"),fName);
        
        break;
      } 
    } // of if

    if( result.get()==NULL )
    { result = linked_ptr<LogAppender<charT,traits>>(new LogAppender<charT,traits>(type,l,fName));
      
      m_appenderList.push_back(result);
    } // of if

    //OutputDebugFmt(_T("LogAppenders::GetInstance(): 0x%lx\n"),&result);
  
    return result;
  } // of LogAppenders<charT,traits>::GetInstance()
} // of namespace bvr20983


template class bvr20983::LogAppenders<TCHAR>;
template class bvr20983::LogAppender<TCHAR>;
template class bvr20983::linked_ptr<bvr20983::LogAppender<TCHAR>>;
/*==========================END-OF-FILE===================================*/
