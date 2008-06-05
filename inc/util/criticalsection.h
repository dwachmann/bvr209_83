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
#if !defined(CRITICALSECTION_H)
#define CRITICALSECTION_H

#include "os.h"

void OutputDebugFmt(LPTSTR pszFmt,...);

namespace bvr20983
{
  class CriticalSection
  {
    public:
      CriticalSection() : m_inited(false)
      { ::InitializeCriticalSection(&m_CritSec); 

        m_inited = true;
      }
      
      ~CriticalSection()
      { if( m_inited) 
          ::DeleteCriticalSection(&m_CritSec); 

        m_inited = false;
      }

      void Enter()
      { if( m_inited )
        { //OutputDebugFmt(_T("CriticalSection::Enter(0x%X)"),::GetCurrentThreadId());
          ::EnterCriticalSection(&m_CritSec); 
        } // of if
      } // of Enter
      
      void Leave()
      { if( m_inited ) 
        { ::LeaveCriticalSection(&m_CritSec); 
          //OutputDebugFmt(_T("CriticalSection::Leave(0x%X)"),::GetCurrentThreadId());
        } // of if
      } // of Leave

    private:
      CRITICAL_SECTION m_CritSec;
      bool             m_inited;
  }; /* of class CriticalSection */

  class Critical
  {
    public:
      Critical(CriticalSection& critSect) : m_critSect(critSect)
      { m_critSect.Enter(); }
      
      ~Critical()
      { m_critSect.Leave(); }

    private:
      CriticalSection& m_critSect;
  }; /* of class Critical */
  
} // of namespace bvr20983

#endif // CRITICALSECTION_H
