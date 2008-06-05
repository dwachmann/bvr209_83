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
#if !defined(THREAD_H)
#define THREAD_H

#include "util/criticalsection.h"

namespace bvr20983
{
  struct ThreadIF
  { 
    virtual bool InitThread()
    { return true; }
    
    virtual void ExitThread(HRESULT hr)
    { }
    
    virtual void RunThread(HANDLE hWaitEvent)=0;
  }; // of ThreadIF

  /**
   *
   */
  class Thread
  {
    public:
      Thread(ThreadIF* pThreadIF,bool dispatchMsg=false);
      ~Thread();

      void StartThread();
      bool StopThread(DWORD dwMilliseconds=INFINITE);

    private:
      HANDLE          m_hThread;
      HANDLE          m_hWaitEvent;
      CriticalSection m_critSec;
      ThreadIF*       m_pThreadIF;
      bool            m_dispatchMsg;

      bool Init();
      void Exit(HRESULT hr);
      void Run();

      friend UINT WINAPI ThreadProc(LPVOID lpParam);
  }; // of class Thread
} // of namespace bvr20983
#endif // THREAD_H
/*==========================END-OF-FILE===================================*/
