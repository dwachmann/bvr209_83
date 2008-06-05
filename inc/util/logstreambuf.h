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
#if !defined(LOGSTREAMBUF_H)
#define LOGSTREAMBUF_H

#include "os.h"

#include <ios>
#include <streambuf>

#include "util/linked_ptr.h"
#include "util/loglevel.h"
#include "util/logappender.h"
#include "util/criticalsection.h"

namespace bvr20983
{
  template< class charT,class traits >
  class LogStream;

  template< class charT,class traits=std::char_traits<charT> >
  class LogStreamBuf : public std::basic_streambuf<charT,traits>
  {
    public:
      typedef typename LogAppender<charT,traits>::AppenderT    AppenderT;
      typedef typename LogAppender<charT,traits>::AppenderList AppenderList;
      typedef typename LogStream<charT,traits>                 LogStreamType;
    
      LogStreamBuf();
      ~LogStreamBuf();
      
      void SetLogStream(LogStreamType* pLogStream)
      { m_pLogStream = pLogStream; }
      
      bool ContainsEndl() const;
      
      void AddAppender(const AppenderT& app)
      { m_appenders.push_back( app ); }
      
    protected:
      int_type overflow(int_type c = traits_type::eof());
      int      sync();
      
    private:
      charT                  m_buffer[1024];
      LogStreamType*         m_pLogStream;
      bool                   m_dumpPrefix;
      AppenderList           m_appenders;
      
      static CriticalSection m_criticalSection;
  }; // of class LogStreamBuf


  template< class charT,class traits=std::char_traits<charT> >
  class OutputDebugStreamBuf : public std::basic_streambuf<charT,traits>
  {
    public:
      OutputDebugStreamBuf();
      
    protected:
      int_type overflow(int_type c = traits_type::eof());
      int      sync();
      
    private:
      charT          m_buffer[1024];
  }; // of class OutputDebugStreamBuf

} // of namespace bvr20983

#endif // LOGSTREAMBUF_H
//=================================END-OF-FILE==============================