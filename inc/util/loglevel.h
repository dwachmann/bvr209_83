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
#if !defined(LOGLEVEL_H)
#define LOGLEVEL_H

#include "os.h"
#include <iomanip>
#include <ostream>

namespace bvr20983
{
  template< class charT,class traits=std::char_traits<charT> >
  class LogLevel
  {
    public:
      enum LevelType
      { OFF_LEVEL=0,
        FATAL_LEVEL,
        ERROR_LEVEL,
        WARN_LEVEL,
        INFO_LEVEL,
        DEBUG_LEVEL,
        TRACE_LEVEL,
        ALL_LEVEL
      };

      LogLevel()
      { m_level = OFF_LEVEL; }

      LogLevel(LevelType l)
      { m_level = l; }

      LogLevel(LPCTSTR l)
      { m_level = ALL_LEVEL;
        
        for( int i=(int)OFF_LEVEL;i<=(int)ALL_LEVEL;i++ )
          if( _tcsicmp(l,LevelName[i])==0 )
          { m_level = (LevelType)i;
            break;
          }
      }

      LPCTSTR GetName() const
      { return LevelName[m_level]; }
      
      LevelType GetLevel() const
      { return m_level; }

      bool operator>=(const LogLevel& l) const
      { return m_level>=l.m_level; }
      
      bool operator==(const LogLevel& l) const
      { return m_level==l.m_level; }

      bool operator==(LevelType l) const
      { return m_level==l; }

      bool operator!=(const LogLevel& l) const
      { return m_level!=l.m_level; }

      bool operator!=(LevelType l) const
      { return m_level!=l; }
      
    private:
      LevelType m_level;

      static LPCTSTR LevelName[];
  }; // of LogLevel

  template< class charT,class traits >
  std::basic_ostream<charT,traits>& operator<<(std::basic_ostream<charT,traits>& os,const LogLevel<charT,traits>& l)
  { return os<<std::setw(5)<<l.GetName(); }

} // of namespace bvr20983

#endif // LOGLEVEL_H
//=================================END-OF-FILE==============================