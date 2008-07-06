/*
 * $Id$
 * 
 * iostream stream.
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
#include <shlobj.h>
#include "util/logstream.h"
#include "util/registry.h"
#include "util/versioninfo.h"
#include "exception/bvr20983exception.h"

namespace bvr20983
{
  template< class charT,class traits >
  Loggers<charT,traits> LogStream<charT,traits>::m_loggers=Loggers<charT,traits>();

  template< class charT,class traits >
  TCHAR Loggers<charT,traits>::m_productPrefix[];

  template< class charT,class traits >
  TCHAR Loggers<charT,traits>::m_componentPrefix[];
  
  /**
   *
   */
  template< class charT,class traits >
  LogStream<charT,traits>::LogStream(LPCTSTR sourceFileName) : 
    basic_ostream<charT,traits>(&m_buf),
    m_level(LogLevel<charT,traits>::INFO_LEVEL),
    m_nextLongAsHR(false),
    m_nextIsGUID(false),
    m_sourceFileName(sourceFileName),
    m_sourceLineNo(-1),
    m_indent(0)
  { this->init(&m_buf);
  
    m_buf.SetLogStream(this);
  }

  template< class charT,class traits >
  void LogStream<charT,traits>::SetLevel(typename LogLevel<charT,traits>::LevelType l)
  { LogLevel<charT,traits> newLevel = LogLevel<charT,traits>(l);
  
    if( newLevel!=m_level )
    { if( !m_buf.ContainsEndl() )
        *this<<endl;
        
      m_sourceLineNo = -1;
      
      m_level = newLevel; 
    } // of if
  }


  /**
   *
   */
  template< class charT,class traits >
  Loggers<charT,traits>::Loggers()
  { }

  template< class charT,class traits >
  Loggers<charT,traits>::~Loggers()
  { LogStreamsType::iterator iter;
  
    for( iter=m_logStreams.begin();iter!=m_logStreams.end();iter++ )
    { (*iter)->flush();
    
      //delete *iter;
    } // of for
      
    m_logStreams.clear();
  }

  /**
   *
   */
  template< class charT,class traits >
  void Loggers<charT,traits>::ReadVersionInfo(HMODULE hModule)
  { util::VersionInfo verInfo(hModule);

    ::memset(m_productPrefix,'\0',sizeof(m_productPrefix));
    ::memset(m_componentPrefix,'\0',sizeof(m_componentPrefix));

    LPVOID prodPrefix = verInfo.GetStringInfo(_T("ProductPrefix"));
    LPVOID compPrefix = verInfo.GetStringInfo(_T("ComponentPrefix"));

    if( NULL!=prodPrefix )
      _tcscpy_s(m_productPrefix,ARRAYSIZE(m_productPrefix),(LPCTSTR)prodPrefix);

    if( NULL!=compPrefix )
      _tcscpy_s(m_componentPrefix,ARRAYSIZE(m_productPrefix),(LPCTSTR)compPrefix);

    //OutputDebugFmt(_T("Loggers::ReadVersionInfo() Product:%s Component:%s\n"),m_productPrefix,m_componentPrefix);
  } // of Loggers<charT,traits>::ReadVersionInfo()

  /**
   *
   */
  template< class charT,class traits >
  LogLevel<charT,traits> Loggers<charT,traits>::GetLoggingLevel(LPCTSTR srcFileName)
  { LogLevel<charT,traits> result;

    TString regPath(_T("HKEY_CURRENT_USER\\Software\\"));

    if( m_productPrefix[0]!=_T('\0') && m_componentPrefix[0]!=_T('\0') )
    { regPath += m_productPrefix;
      regPath += _T("\\");
      regPath += m_componentPrefix;
    } // of if
    else
      regPath += _T("BVR20983");

    regPath += _T("\\logging");

    ::OutputDebugFmt(_T("Loggers::GetLoggingLevel() registry <%s>\n"),regPath.c_str());

    try
    { RegistryKey regKey(regPath);
      TString     logLevel;

      regKey.QueryValue(_T("tracelevel"),logLevel);

      result = LogLevel<charT,traits>(logLevel.c_str());
    }
    catch(BVR20983Exception& e)
    { ::OutputDebugFmt(_T("Error reading registry %s: %s"),regPath.c_str(),e.GetErrorMessage());
    }

    return result;
  } // of Loggers<charT,traits>::GetLoggingLevel()

  /**
   *
   */
  bool CreateDirectory(LPCTSTR path)
  { DWORD fileAttribs = ::GetFileAttributes(path);

    return (fileAttribs==INVALID_FILE_ATTRIBUTES && ::CreateDirectory(path,NULL)) ||
           (fileAttribs!=INVALID_FILE_ATTRIBUTES && (fileAttribs&FILE_ATTRIBUTE_DIRECTORY))
           ;
  } // of CreateDirectory()

  /**
   *
   */
  template< class charT,class traits >
  void Loggers<charT,traits>::GetFilePath(LPTSTR path,UINT maxPathLen,LPCTSTR srcFileName)
  { if( SUCCEEDED( ::SHGetFolderPath(NULL,CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL,SHGFP_TYPE_CURRENT,path)) ) 
    { _tcscat_s(path,maxPathLen,_T("\\"));

      if( m_productPrefix[0]!=_T('\0') )
        _tcscat_s(path,maxPathLen,m_productPrefix);
      else
        _tcscat_s(path,maxPathLen,_T(verProdPrefix));
      
      //
      // directory doesnt exist
      //
      if( CreateDirectory(path) )
      { _tcscat_s(path,maxPathLen,_T("\\"));

        if( m_componentPrefix[0]!=_T('\0') )
        {  _tcscat_s(path,maxPathLen,m_componentPrefix);
          
          if( !CreateDirectory(path) )
            _tcscpy_s(path,maxPathLen,srcFileName);
          else
            _tcscat_s(path,maxPathLen,_T("\\"));
        } // of if

        _tcscat_s(path,maxPathLen,srcFileName);
      } // of else
      else
        // if path is file and not directory, use plain path as path for logging      
        _tcscpy_s(path,maxPathLen,srcFileName);
    } // of if
    else
      _tcscpy_s(path,maxPathLen,srcFileName);
  } // of Loggers<charT,traits>::GetFilePath()

  /**
   *
   */
  template< class charT,class traits >
  LogStream<charT,traits>& Loggers<charT,traits>::GetLogger(LPCTSTR srcFileName)
  { LogStream<charT,traits>* result = NULL;
  
    if( NULL!=srcFileName )
    { LogStreamsType::iterator iter;
    
      for( iter=m_logStreams.begin();iter!=m_logStreams.end();iter++ )
      { LogStream<charT,traits>* l = *iter;
      
        if( NULL!=l->GetSourceFileName() && _tcscmp(l->GetSourceFileName(),srcFileName)==0 )
        { result = l;
          
          break;
        } // of if
      } // of for
    } // of if
    else if( !m_logStreams.empty() )
      result = m_logStreams.at(0);
  
    if( NULL==result )
    { TCHAR                  szPath[MAX_PATH];
      LogAppender<charT,traits>::AppenderT appender,appender1;
      LogLevel<charT,traits>               ll = GetLoggingLevel(srcFileName);
      
      result = new LogStream<charT,traits>(srcFileName);

      LogAppender<charT,traits>::GetInstance(appender,LogAppender<charT,traits>::CONSOLE,LogLevelT(LogLevelT::INFO_LEVEL));
      
      appender->ShowLevel(true);
      appender->ShowLineNo(false);
      appender->ShowTimestamp(false);
      appender->ShowSourceFile(false);
    
      result->AddAppender(appender);

      GetFilePath(szPath,ARRAYSIZE(szPath),_T("logger.trc"));

      result->AddAppender(LogAppender<charT,traits>::GetInstance(appender1,LogAppender<charT,traits>::FILE,ll,szPath));

      appender1->ShowThreadID(true);
      
      TCHAR  trcFileName[256];
      LPCTSTR i = _tcsrchr(srcFileName,_T('\\'));
          
      if( NULL!=i && *(i+1)!=_T('\0') )
        i += 1;
      else
        i = srcFileName;      

      LPCTSTR k = _tcsrchr(i,_T('.'));
      
      if( NULL!=k )
      { LogAppender<charT,traits>::AppenderT appender2;
      
        memcpy_s(trcFileName,sizeof(trcFileName)-1,i,(k-i)*sizeof(TCHAR));
        
        trcFileName[k-i] = _T('\0');

        _tcscat_s(trcFileName,sizeof(trcFileName)/sizeof(trcFileName[0]),_T(".trc"));

        GetFilePath(szPath,ARRAYSIZE(szPath),trcFileName);

        LogAppender<charT,traits>::GetInstance(appender2,LogAppender<charT,traits>::FILE,ll,szPath);
        
        appender2->ShowSourceFile(false);

        result->AddAppender(appender2);

/*
        LogAppender<charT,traits>::AppenderT appender3;
        LogAppender<charT,traits>::GetInstance(appender3,LogAppender<charT,traits>::DEBUGOUTPUT,ll);

        result->AddAppender(appender3);
*/
      } // of if
      
      m_logStreams.push_back( result );
    } // of if
    
    return *result;
  }

  /**
   *
   */
  template< class charT,class traits >
  basic_ostream<charT,traits>& FTL( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetLevel(LogLevelT::FATAL_LEVEL);
    
    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& ERR( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetLevel(LogLevelT::ERROR_LEVEL);
    
    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& WRN( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetLevel(LogLevelT::WARN_LEVEL);
    
    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& INF( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetLevel(LogLevelT::INFO_LEVEL);
    
    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& DBG( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetLevel(LogLevelT::DEBUG_LEVEL);
    
    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& TRC( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetLevel(LogLevelT::TRACE_LEVEL);
    
    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& setHR( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetHR(true);
    
    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& setNoHR( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetHR(false);
    
    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& setGUID( basic_ostream<charT,traits>& os )
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);
    
    if( NULL!=logger )
      logger->SetGUID(true);
    
    return os;
  }

  /**
   *
   */  
  void sindent(ios_base& os,int indent)
  { LogStream<TCHAR>* logger = dynamic_cast< LogStream<TCHAR>* >(&os);
    
    if( NULL!=logger )
      logger->SetIndent(indent);
  } // of sindent

  /**
   *
   */  
  void slino(ios_base& os,int lineno)
  { LogStream<TCHAR>* logger = dynamic_cast< LogStream<TCHAR>* >(&os);
    
    if( NULL!=logger )
      logger->SetSourceLineNo(lineno);
  } // of slino

  _Smanip<int> setlineno( int lineno )
  { return _Smanip<int>(slino,lineno); }

  _Smanip<int> setindent( int indent )
  { return _Smanip<int>(sindent,indent); }
} // of namespace bvr20983

template class bvr20983::LogStream<TCHAR>;
template class bvr20983::Loggers<TCHAR>;

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::FTL<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::ERR<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::WRN<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::INF<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::DBG<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::TRC<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::setHR<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::setNoHR<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::setGUID<TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>& );


/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: OutputDebugFmt

  Summary:  Wraps the Win32 OutputDebugString API call to provide
            printf-style formatted (and variable argument) output.

  Args:     LPTSTR pszFmt,
              Format string.
            [...]
              Arguments to match those specified in the format string.

  Returns:  void
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
void OutputDebugFmt(LPTSTR pszFmt,...)
{ va_list arglist;
  va_start(arglist, pszFmt);
  TCHAR szMsg[256];

  // Use the format string and arguments to format the content text.
  wvsprintf(szMsg, pszFmt, arglist);
  // Output the newly formated message string to the debugger display.
  OutputDebugString(szMsg);
} // of OutputDebugFmt()
/*==========================END-OF-FILE===================================*/
