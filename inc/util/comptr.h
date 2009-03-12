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
#if !defined(COMPTR_H)
#define COMPTR_H

#include "os.h"
#include "util/comptrbase.h"
#include "exception/comexception.h"

namespace bvr20983
{
  template <class ICom>
  class GCOMPtr;

  template <class ICom>
  class COMPtr : public COMPtrBase
  {
    public:
      COMPtr(ICom* pIUnk=NULL) : m_pIUnk(NULL)
      { Init(pIUnk); }

      COMPtr(LPCOLESTR lpszProgID,LPCOLESTR lpszIID,DWORD dwClsContext=CLSCTX_INPROC_SERVER,LPUNKNOWN pOutUnknown=NULL) : m_pIUnk(NULL)
      { CLSID clsId = GUID_NULL;
        IID   iid   = GUID_NULL;
      
        GetInfo(lpszProgID,lpszIID,&clsId,&iid);
        CreateCOM(clsId,iid,dwClsContext,pOutUnknown);
      } 

      COMPtr(REFIID clsId,REFIID riid,DWORD dwClsContext=CLSCTX_INPROC_SERVER,LPUNKNOWN pOutUnknown=NULL) : m_pIUnk(NULL)
      { CreateCOM(clsId,riid,dwClsContext,pOutUnknown); }

      COMPtr(const GCOMPtr<ICom>& gCOMPtr) : m_pIUnk(NULL)
      { Init(gCOMPtr.GetPtr()); }

      COMPtr(const COMPtr<ICom>& pIUnk) : m_pIUnk(NULL)
      { Init(pIUnk.m_pIUnk); }

      ~COMPtr()
      { Release(); }

      void Release()
      { 
        RELEASE_INTERFACE(m_pIUnk); 
      }

      COMPtr<ICom>& operator=(const GCOMPtr<ICom>& gCOMPtr)
      { Init(gCOMPtr.GetPtr());
  
        return *this;
      }

      COMPtr<ICom>& operator=(ICom* pIUnk)
      { Init(pIUnk);

        return *this;
      }

      bool IsNULL() const
      { return m_pIUnk==NULL; }

      operator ICom*()
      { return GetPtr(); }
      
      ICom* operator->() const
      { ICom* result = GetPtr();

        assert( NULL!=result );

        return result;
      }

      ICom& operator*() const
      { ICom* result = GetPtr();

        assert( NULL!=result );

        return *result;
      }

      ICom** operator&() 
      { Release();

        return &m_pIUnk; 
      }

      ICom* GetPtr() const
      { return m_pIUnk; }

      bool operator==(const COMPtr<ICom>& rhs) const
      { return (this.m_pIUnk==NULL && rhs.m_pIUnk==NULL) || this.m_pIUnk==rhs.m_pIUnk; }

      bool operator<(const COMPtr<ICom>& rhs) const
      { return (this.m_pIUnk==NULL && rhs.m_pIUnk!=NULL) || this.m_pIUnk<rhs.m_pIUnk; }

     COMPtr<ICom>& operator=(const COMPtr<ICom>& pIUnk)
      { Init(pIUnk.m_pIUnk);
  
        return *this;
      }

    private:
      void CreateCOM(REFIID clsId,REFIID riid,DWORD dwClsContext,LPUNKNOWN pOutUnknown)
      { ICom* pIUnk = NULL;

        HRESULT hr = ::CoCreateInstance(clsId,pOutUnknown,dwClsContext,riid,(LPVOID*)&pIUnk);

        THROW_COMEXCEPTION( hr );

        Init(pIUnk); 

        RELEASE_INTERFACE(pIUnk);
      }

      void Init(ICom* pIUnk)
      { ADDREF_INTERFACE( pIUnk );

        Release();

        m_pIUnk = pIUnk;
      } // of Init()

      ICom* m_pIUnk;
  }; // of class COMPtr

  template <class ICom>
  class GCOMPtr : public GCOMPtrBase
  {
    public:
      GCOMPtr(REFIID riid=IID_NULL) : m_iid(riid),m_cookie(0)
      { }

      ~GCOMPtr()
      { Release(); }

      void Release()
      { if( m_cookie>0 )
        { assert( NULL!=m_gpGIT );

          m_gpGIT->RevokeInterfaceFromGlobal(m_cookie);
        } // of if

        m_cookie = 0;
      }

      GCOMPtr<ICom>& operator=(const GCOMPtr<ICom>& pIUnk)
      { Init(pIUnk.m_cookie,pIUnk.m_iid);
  
        return *this;
      }

      GCOMPtr<ICom>& operator=(ICom* pIUnk)
      { Init(pIUnk,m_iid);

        return *this;
      }

      bool operator==(const GCOMPtr<ICom>& rhs) const
      { return this.m_cookie==rhs.m_cookie; }

      bool operator<(const GCOMPtr<ICom>& rhs) const
      { return this.m_cookie<rhs.m_cookie; }

      bool IsNULL() const
      { return m_cookie==0; }

      DWORD GetCookie() const
      { return m_cookie; }

      void SetIID(REFIID riid)
      { m_iid = riid; }

      friend class COMPtr<ICom>;

    private:
      ICom* GetPtr() const
      { assert( NULL!=m_gpGIT && m_iid!=IID_NULL );

        ICom* result = NULL;

        if( m_cookie>0 )
          THROW_COMEXCEPTION( m_gpGIT->GetInterfaceFromGlobal(m_cookie,m_iid,(LPVOID*)&result) );

        return result;
      } // of GetPtr();

      void Init(ICom* pIUnk,REFIID riid)
      { if( NULL!=pIUnk && m_iid!=IID_NULL )
        { DWORD cookie = 0;

          THROW_COMEXCEPTION( m_gpGIT->RegisterInterfaceInGlobal(pIUnk,riid,&cookie) );

          Release();

          m_iid    = riid;
          m_cookie = cookie;
        } // of if
      } // of Init()

      void Init(DWORD cookie,REFIID riid)
      { Release();

        m_cookie = cookie;
        m_iid    = riid;
      } // of Init()

      DWORD m_cookie;
      IID   m_iid;
  }; // of class GCOMPtr
} // of namespace bvr20983
#endif // COMPTR_H
//=================================END-OF-FILE==============================
