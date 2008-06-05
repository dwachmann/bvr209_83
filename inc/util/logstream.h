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
#if !defined(LOGSTREAM_H)
#define LOGSTREAM_H

#include "os.h"
#include <vector>
#include <ios>
#include <iomanip>
#include <ostream>
#include "util/loglevel.h"
#include "util/logstreambuf.h"
#include "util/logappender.h"

#define LOGGER_INFO  bvr20983::LogStreamT::GetLogger(_T(__FILE__))<<bvr20983::INF<<bvr20983::setlineno(__LINE__)
#define LOGGER_DEBUG bvr20983::LogStreamT::GetLogger(_T(__FILE__))<<bvr20983::DBG<<bvr20983::setlineno(__LINE__)
#define LOGGER_TRACE bvr20983::LogStreamT::GetLogger(_T(__FILE__))<<bvr20983::TRC<<bvr20983::setlineno(__LINE__)
#define LOGGER_WARN  bvr20983::LogStreamT::GetLogger(_T(__FILE__))<<bvr20983::WRN<<bvr20983::setlineno(__LINE__)
#define LOGGER_ERROR bvr20983::LogStreamT::GetLogger(_T(__FILE__))<<bvr20983::ERR<<bvr20983::setlineno(__LINE__)
#define LOGGER_FATAL bvr20983::LogStreamT::GetLogger(_T(__FILE__))<<bvr20983::FTL<<bvr20983::setlineno(__LINE__)

namespace bvr20983
{
  template< class charT,class traits >
  class Loggers;

  template< class charT,class traits=char_traits<charT> >
  class LogStream : public basic_ostream<charT,traits>
  {
    public:
      typedef LogAppender<charT,traits>          LogAppenderT;
      typedef LogLevel<charT,traits>             LogLevelT;
      typedef typename LogLevelT::LevelType      LevelType;
      typedef typename LogAppenderT::AppenderT   AppenderT;
    
      void SetLevel(LevelType l);

      LogLevelT GetLevel() const
      { return m_level; }

      void SetSourceFileName(LPCTSTR fName)
      { m_sourceFileName = fName; }

      LPCTSTR GetSourceFileName() const
      { return m_sourceFileName; }

      void SetSourceLineNo(int lineno)
      { m_sourceLineNo = lineno; }

      void SetHR(bool value)
      { m_nextLongAsHR = value; }

      void SetGUID(bool value)
      { m_nextIsGUID = value; }

      bool IsHR()
      { return m_nextLongAsHR; }

      bool IsGUID()
      { return m_nextIsGUID; }

      int GetSourceLineNo() const
      { return m_sourceLineNo; }

      void SetIndent(int i)
      { m_indent = i; }

      int GetIndent() const
      { return m_indent; }

      void AddAppender(const AppenderT& app)
      { m_buf.AddAppender(app); }

      static LogStream<charT,traits>& GetLogger(LPCTSTR srcFileName=NULL)
      { return m_loggers.GetLogger(srcFileName); }

      friend class Loggers<charT,traits>;
      
    private:
      LogStream(LPCTSTR sourceFileName=NULL);
      
      LogStreamBuf<charT,traits>     m_buf;
      LogLevelT                      m_level;
      LPCTSTR                        m_sourceFileName;
      int                            m_sourceLineNo;
      bool                           m_nextLongAsHR;
      bool                           m_nextIsGUID;
      int                            m_indent;
      
      static Loggers<charT,traits>   m_loggers;
  }; // of class LogStream


  template< class charT,class traits=char_traits<charT> >
  class Loggers
  { 
    public:
      typedef vector<LogStream<charT,traits>*> LogStreamsType;

      Loggers();
      ~Loggers();
      
      LogStream<charT,traits>& GetLogger(LPCTSTR srcFileName=NULL);
      
    private:
      LogStreamsType m_logStreams;

      static LogLevel<charT,traits> GetLoggingLevel(LPCTSTR srcFileName);
      static void                   GetFilePath(LPTSTR path,UINT maxPathLen,LPCTSTR srcFileName);
  }; // of class Loggers

  template< class charT,class traits >
  basic_ostream<charT,traits>& FTL( basic_ostream<charT,traits>& os );

  template< class charT,class traits >
  basic_ostream<charT,traits>& ERR( basic_ostream<charT,traits>& os );

  template< class charT,class traits >
  basic_ostream<charT,traits>& WRN( basic_ostream<charT,traits>& os );

  template< class charT,class traits >
  basic_ostream<charT,traits>& INF( basic_ostream<charT,traits>& os );

  template< class charT,class traits >
  basic_ostream<charT,traits>& DBG( basic_ostream<charT,traits>& os );

  template< class charT,class traits >
  basic_ostream<charT,traits>& TRC( basic_ostream<charT,traits>& os );

  template< class charT,class traits >
  basic_ostream<charT,traits>& setHR( basic_ostream<charT,traits>& os );

  template< class charT,class traits >
  basic_ostream<charT,traits>& setNoHR( basic_ostream<charT,traits>& os );

  template< class charT,class traits >
  basic_ostream<charT,traits>& setGUID( basic_ostream<charT,traits>& os );

  _Smanip<int> setlineno( int lineno );
  _Smanip<int> setindent( int indent );

  typedef LogStream<TCHAR> LogStreamT;
  typedef LogLevel<TCHAR>  LogLevelT;
} // of namespace bvr20983


#define MAXLOGST 128

void OutputDebugFmt(LPTSTR pszFmt,...);

#if defined(DEBUG)

//Basic debug and logging macros

// Output debug string; literal or variable.
#define ODS(sMsg) \
{\
  TCHAR szDebug[MAXLOGST];\
  OutputDebugFmt(_T("%s"),_T(sMsg));\
  wsprintf(szDebug, _T(" [%s, %u]\r\n"), (LPTSTR) _T(__FILE__), __LINE__);\
  OutputDebugString(szDebug);\
}

// Output debug string formatted; one argument.
#define ODF1(sFmt,x) \
{\
  TCHAR szDebug[MAXLOGST];\
  OutputDebugFmt(_T(sFmt),x);\
  wsprintf(szDebug, _T(" [%s, %u]\r\n"), (LPTSTR) _T(__FILE__), __LINE__);\
  OutputDebugString(szDebug);\
}

// Output debug string formatted; two argument.
#define ODF2(sFmt,x,y) \
{\
  TCHAR szDebug[MAXLOGST];\
  OutputDebugFmt(_T(sFmt),x,y);\
  wsprintf(szDebug, _T(" [%s, %u]\r\n"), (LPTSTR) _T(__FILE__), __LINE__);\
  OutputDebugString(szDebug);\
}

// Output debug string formatted; three argument.
#define ODF3(sFmt,x,y,z) \
{\
  TCHAR szDebug[MAXLOGST];\
  OutputDebugFmt(_T(sFmt),x,y,z);\
  wsprintf(szDebug, _T(" [%s, %u]\r\n"), (LPTSTR) _T(__FILE__), __LINE__);\
  OutputDebugString(szDebug);\
}


#else  // !defined(DEBUG)
#define ODS(x)
#define ODF1(sFmt,x)
#define ODF2(sFmt,x,y)
#define ODF3(sFmt,x,y,z)
#endif // DEBUG

#endif // LOGSTREAM_H
//=================================END-OF-FILE==============================