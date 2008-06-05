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
#if !defined(COMSERVER_H)
#define COMSERVER_H

#include "util/apputil.h"
#include "util/criticalsection.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace COM
  {
    /*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
      Class:    COMServer
    
      Summary:  Class to encapsulate control of this COM server (eg, handle
                Lock and Object counting, encapsulate otherwise global data).
    
      Methods:  none
    C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
    class COMServer
    {
      public:
        void Lock();
        void Unlock();
    
        static int        LoadString(UINT uID,LPTSTR lpBuffer,int nBufferMax,HINSTANCE hInst=GetInstanceHandle());
        static int        LoadString(UINT uID,TString** s,HINSTANCE hInst=GetInstanceHandle());
        static int        LoadString(UINT uID,LPTSTR* s,HINSTANCE hInst=GetInstanceHandle());
        static LPOLESTR   LoadString(UINT uID,HINSTANCE hInst=GetInstanceHandle());
        static HICON      LoadIcon(WORD wIcon,HINSTANCE hInst=GetInstanceHandle());
        
        static COMServer* GetInstance();
        static COMServer* CreateInstance(HINSTANCE hDllInst);
        static void       DeleteInstance();
        static DWORD      GetModuleFileName(LPTSTR path,DWORD nSize);
        static void       SetLangID(LANGID* pLangID);
        static LANGID     GetLangID();
        static BOOL       ShouldUnload();
        static HINSTANCE  GetInstanceHandle();

        static void       IncrementObjectCount();
        static void       DecrementObjectCount();

        static CriticalSection& GetCriticalSection()
        { return m_criticalSection; }
        
      private:
        COMServer(HINSTANCE hDllInst);
        ~COMServer();

        void ObjectsUp();
        void ObjectsDown();

        static COMServer*      m_pMe;
        static CriticalSection m_criticalSection;
    
        HINSTANCE m_hDllInst;      // A place to store the handle to loaded instance of this DLL module.
        LONG      m_cObjects;      // Global DLL Server living Object count.
        LONG      m_cLocks;        // Global DLL Server Client Lock count.
        DWORD     m_dwTLS;         // For thread-local storage
    }; /* of class COMServer */

  } // of namespace COM
} // of namespace bvr20983

#endif // COMSERVER_H
