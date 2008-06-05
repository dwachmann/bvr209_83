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
#if !defined(LOGAPPENDER_H)
#define LOGAPPENDER_H

#include "os.h"

#include <vector>
#include <ostream>

#include "util/linked_ptr.h"
#include "util/loglevel.h"

namespace bvr20983
{
  template< class charT,class traits >
  class LogAppenders;

  template< class charT,class traits=char_traits<charT> >
  class LogAppender
  {
    public:
      typedef typename LogLevel<charT,traits>                LogLevelT;
      typedef typename linked_ptr<LogAppender<charT,traits>> AppenderT;
      typedef typename vector<AppenderT>                     AppenderList;
      
      enum AppenderType
      { CONSOLE,
        FILE,
        DEBUGOUTPUT
      };

      static AppenderT& GetInstance(AppenderT& app,AppenderType type,LogLevelT l,LPCTSTR fName=NULL)
      { return m_logAppenders.GetInstance(app,type,l,fName); }

      ~LogAppender()
      { delete m_pOs; }
      
      void Dump(LogLevelT logStreamLevel,
                LPCTSTR sourceFileName,
                int sourceLineNo,
                int indent,
                bool dumpPrefix,
                charT* buffer,streamsize len
               );
      
      void SetLevel(LogLevelT l)
      { m_level = l; }

      LogLevelT GetLevel() const
      { return m_level; }

      AppenderType GetType() const
      { return m_type; }

      LPCTSTR GetFName() const
      { LPCTSTR result = m_fName.c_str(); 

        if( NULL!=result && m_fName.size() ==0 )
          result = NULL;

        return result;
      }
      
      void ShowLevel(bool flag)
      { m_showLevel = flag; }

      void ShowLineNo(bool flag)
      { m_showLineNo = flag; }

      void ShowTimestamp(bool flag)
      { m_showTimestamp = flag; }

      void ShowSourceFile(bool flag)
      { m_showSourceFile = flag; }

      void ShowThreadID(bool flag)
      { m_showThreadID = flag; }

      operator basic_ostream<charT,traits>&();
      
      friend class LogAppenders<charT,traits>;

    private:
      LogAppender(AppenderType type,LogLevelT l,LPCTSTR fName=NULL) :
        m_type(type),m_pOs(NULL),m_level(l),
        m_showLevel(true),m_showLineNo(true),m_showTimestamp(true),m_showSourceFile(true),
        m_showThreadID(false)
      { if( NULL!=fName )
          m_fName = fName;
      }

      TString                      m_fName;
      LogLevelT                    m_level;
      basic_ostream<charT,traits>* m_pOs;
      AppenderType                 m_type;
      bool                         m_showLevel;
      bool                         m_showLineNo;
      bool                         m_showTimestamp;
      bool                         m_showSourceFile;
      bool                         m_showThreadID;
      
      static LogAppenders<charT,traits> m_logAppenders;
  }; // of class LogAppender

 
  template< class charT,class traits=char_traits<charT> >
  class LogAppenders
  { 
    public:
      typedef          LogLevel<charT,traits>      LogLevelT;
      typedef          LogAppender<charT,traits>   LogAppenderT;
      typedef typename LogAppenderT::AppenderType  AppenderType;
      typedef typename linked_ptr<LogAppenderT>    AppenderT;
      typedef typename vector<AppenderT>           AppenderList;
      
      LogAppenders()
      { }

      AppenderT& GetInstance(AppenderT& result,AppenderType type,LogLevelT l,LPCTSTR fName=NULL);
      
    private:
      AppenderList m_appenderList;
  }; // of class Loggers

} // of namespace bvr20983

#endif // LOGAPPENDER_H
//=================================END-OF-FILE==============================