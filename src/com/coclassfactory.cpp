/*
 * $Id$
 * 
 * COM ClassFactory implementation.
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
#include "com/coclassfactory.h"
#include "com/comserver.h"
#include "util/logstream.h"
#include "util/comlogstream.h"


namespace bvr20983
{
  namespace COM
  {
    /**
     *
     */
    COClassFactory::COClassFactory(REFCLSID clsid,FNCOCREATE fnCoCreate) : m_clsid(clsid)
    { m_cRefs      = 0;
      m_fnCoCreate = fnCoCreate;
    }
    
    /**
     *
     */
    STDMETHODIMP COClassFactory::QueryInterface(REFIID riid,PPVOID ppv)
    { HRESULT hr = E_NOINTERFACE;
      *ppv = NULL;
    
      if( IID_IUnknown == riid )
        *ppv = (IUnknown*)this;
      else if( IID_IClassFactory == riid )
        *ppv = (IClassFactory*)this;
    
      if( NULL != *ppv )
      { ((LPUNKNOWN)*ppv)->AddRef();
       
        hr = NOERROR;
      }
    
      return hr;
    }
    
    /**
     *
     */
    STDMETHODIMP_(ULONG) COClassFactory::AddRef()
    { m_cRefs++;

      //COMServer::IncrementObjectCount();
    
      return m_cRefs;
    }

    /**
     *
     */
    STDMETHODIMP_(ULONG) COClassFactory::Release()
    { ULONG result = --m_cRefs;

      //if( m_cRefs>0 ) 
      //  COMServer::DecrementObjectCount();
            
      return result;
    }

    /**
     *
     */
    STDMETHODIMP COClassFactory::CreateInstance(IUnknown* pUnkOuter,REFIID riid,PPVOID ppv)
    { HRESULT   hr   = NOERROR;
      LPUNKNOWN pUnk = NULL;

      assert( NULL!=m_fnCoCreate );
    
      if( NULL==ppv )
        hr = E_POINTER;
      else
      { *ppv = NULL;

        // for aggregation (pUnkOuter!=NULL) the riid should be IID_IUnknown 
        if( NULL!=pUnkOuter && riid!=IID_IUnknown )
          hr = CLASS_E_NOAGGREGATION;
        else
        { hr = m_fnCoCreate(m_clsid,&pUnk,pUnkOuter);

          if( SUCCEEDED(hr) )
          { if( riid!=IID_IUnknown )
            { hr = pUnk->QueryInterface(riid,ppv);

              pUnk->Release();
            } // of if
            else
              *ppv = pUnk;
          } // of if
        } // of else
      } // of else
    
      if( SUCCEEDED(hr) )
      { LOGGER_DEBUG<<_T("COClassFactory::CreateInstance Succeeded. *ppv=0x")<<hex<<setw(4)<<setfill(_T('0'))<<*ppv<<endl; }
      else
      { LOGGER_DEBUG<<_T("COClassFactory::CreateInstance Failed.")<<hr<<endl; }
    
      return hr;
    }
    
    /**
     *
     */
    STDMETHODIMP COClassFactory::LockServer(BOOL fLock)
    { HRESULT hr = NOERROR;

      COMServer* pComServer = COMServer::GetInstance();
    
      if( pComServer!=NULL )
      { if( fLock )
          pComServer->Lock();
        else
          pComServer->Unlock();
      } // of if
    
      return hr;
    }
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
