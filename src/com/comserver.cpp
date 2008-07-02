/*
 * $Id$
 * 
 * COMServer server-related utility C++ object.  This object encapsulates the 
 * server's internal control of global server object and lock counts.
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
#include "com/comserver.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/guid.h"

using namespace bvr20983;
using namespace bvr20983::COM;

namespace bvr20983
{
  namespace COM
  {

    COMServer*      COMServer::m_pMe             = NULL;
    CriticalSection COMServer::m_criticalSection = CriticalSection();
    
    /**
     *
     */
    COMServer::COMServer(HINSTANCE hDllInst)
    { m_hDllInst   = hDllInst;
      m_cObjects   = 0;
      m_cLocks     = 0;
      m_dwTLS      = ::TlsAlloc();
    }
    
    /**
     *
     */
    COMServer::~COMServer()
    { ::TlsFree(m_dwTLS);
      m_dwTLS = 0;
    }
    
    /**
     *
     */
    void COMServer::Lock()
    { ::InterlockedIncrement((PLONG) &m_cLocks);
    
      LOGGER_DEBUG<<_T("COMServer::Lock. New cLocks=")<< m_cLocks<<endl;
    }
    
    /**
     *
     */
    void COMServer::Unlock()
    { ::InterlockedDecrement((PLONG) &m_cLocks);
    
      LOGGER_DEBUG<<_T("COMServer::Unlock. New cLocks=")<< m_cLocks<<endl;
    }
    
    /**
     *
     */
    void COMServer::ObjectsUp()
    { ::InterlockedIncrement((PLONG) &m_cObjects);
    
      LOGGER_DEBUG<<_T("COMServer::ObjectsUp. New cObjects=")<< m_cObjects<<endl;
    }
    
    /**
     *
     */
    void COMServer::ObjectsDown()
    { ::InterlockedDecrement((PLONG) &m_cObjects);
    
      LOGGER_DEBUG<<_T("COMServer::ObjectsDown. New cObjects=")<< m_cObjects<<endl;
    }
    
    /**
     *
     */
    int COMServer::LoadString(UINT uID,LPTSTR lpBuffer,int nBufferMax,HINSTANCE hInst)
    { return ::LoadString(hInst,uID,lpBuffer,nBufferMax); }
    
    /**
     *
     */
    int COMServer::LoadString(UINT uID,TString** result,HINSTANCE hInst)
    { int bufLen = 0;

      if( NULL!=result )
      { TCHAR buffer[1024];
        
        bufLen = LoadString(uID,buffer,ARRAYSIZE(buffer),hInst);

        *result = bufLen>0 ? new TString(buffer,bufLen) : NULL;
      } // of if

      return bufLen;
    } // of COMServer::LoadString()

    /**
     *
     */
    LPOLESTR COMServer::LoadString(UINT uID,HINSTANCE hInst)
    { int      bufLen = 0;
      LPOLESTR result = NULL;

      { TCHAR buffer[1024];
        
        bufLen = LoadString(uID,buffer,ARRAYSIZE(buffer),hInst);

        if( bufLen>0 )
        { result = (LPOLESTR)::CoTaskMemAlloc((bufLen+1)*sizeof(TCHAR));

          if( NULL!=result )
            _tcscpy_s(result,bufLen+1,buffer);
        } // of if
      } // of if

      return result;
    } // of COMServer::LoadString()

  
    /**
     *
     */
    int COMServer::LoadString(UINT uID,LPTSTR* result,HINSTANCE hInst)
    { int bufLen = 0;

      if( NULL!=result )
      { TCHAR buffer[1024];
      
        bufLen = LoadString(uID,buffer,ARRAYSIZE(buffer),hInst);

        if( bufLen>0 )
        { *result = new TCHAR[bufLen+1];

          if( NULL!= *result )
            _tcscpy_s(*result,bufLen,buffer);
        } // of if
        else
          *result = NULL;
      } // of if

      return bufLen;
    } // of COMServer::LoadString()

    /*
     * COMServer::LoadIcon
     *
     * Purpose:
     *   delegation to WIN32 LoadIcon using m_hDllInst
     *
     * Parameters:
     *
     * Return: 
     *   int   form WIN32 LoadString
     */
    HICON COMServer::LoadIcon(WORD wIcon,HINSTANCE hInst)
    { return ::LoadIcon(hInst,MAKEINTRESOURCE(wIcon)); }

    /*
     * COMServer::GetInstance()
     *
     * Purpose:
     *   returns the COMServer singleton
     *
     * Parameters:
     *
     * Return: 
     *   COMServer* the one and only COMServer object
     */
    COMServer* COMServer::GetInstance()
    { return m_pMe; }
    
    /*
     * COMServer::CreateInstance()
     *
     * Purpose:
     *   create the COMServer singleton
     *
     * Parameters:
     *
     * Return: 
     *   COMServer* the one and only COMServer object
     */
    COMServer* COMServer::CreateInstance(HINSTANCE hDllInst)
    { if( m_pMe==NULL )
      { Critical crit(m_criticalSection);
      
        m_pMe = new COMServer(hDllInst);

        
      } // of if
    
      return m_pMe; 
    }
    
    /*
     * COMServer::DeleteInstance()
     *
     * Purpose:
     *   Singelton
     *
     * Parameters:
     *
     * Return: 
     *   
     */
    void COMServer::DeleteInstance()
    { if( NULL!=m_pMe )
      { Critical crit(m_criticalSection);
    
        if( NULL!=m_pMe )
          delete m_pMe;
          
        m_pMe = NULL;
      } // of if
    }
        
    /**
     *
     */
    DWORD COMServer::GetModuleFileName(LPTSTR path,DWORD nSize)
    { return ::GetModuleFileName(GetInstance()->m_hDllInst,path,nSize); }
        
    /**
     *
     */
    HINSTANCE COMServer::GetInstanceHandle()
    { return GetInstance()!=NULL ? GetInstance()->m_hDllInst : NULL; }

    /**
     *
     */
    void COMServer::IncrementObjectCount()
    { COMServer* pComServer = GetInstance();

      if( NULL!=pComServer )
        pComServer->ObjectsUp();
    }

    /**
     *
     */
    void COMServer::DecrementObjectCount()
    { COMServer* pComServer = GetInstance();

      if( NULL!=pComServer )
        pComServer->ObjectsDown();
    }

    /**
     *
     */
    BOOL COMServer::ShouldUnload()
    { BOOL       result = FALSE;
      COMServer* s      = GetInstance();
      
      result = (0L==s->m_cObjects && 0L==s->m_cLocks) ? TRUE : FALSE; 
    
      LOGGER_DEBUG<<_T("COMServer::ShouldUnload() cObjects=")<<s->m_cObjects<<_T(", cLocks=")<<s->m_cLocks<<_T(" --> ")<<result<<endl;
    
      return result;
    }
    
    /**
     *
     */
    void COMServer::SetLangID(LANGID* pLangID)
    { ::TlsSetValue(GetInstance()->m_dwTLS, pLangID); }
    
    
    /**
     *
     */
    LANGID COMServer::GetLangID()
    { LANGID* pLangID=(LANGID *)::TlsGetValue(GetInstance()->m_dwTLS);
    
      return NULL!=pLangID ? *pLangID : LANG_NEUTRAL;
    }
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
