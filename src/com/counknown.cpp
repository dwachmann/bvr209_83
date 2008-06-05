/*
 * $Id$
 * 
 * a IUnknown implementation that supports aggregation.
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
#include "com/counknown.h"
#include "util/logstream.h"

namespace bvr20983
{
  namespace COM
  {
#pragma warning(disable:4355)
    /**
     *
     */
    COUnknown::COUnknown(LPUNKNOWN pUnkOuter) : 
      m_pUnkOuter((pUnkOuter) ? pUnkOuter : &m_UnkPrivate)
    { COMServer::IncrementObjectCount();
    }
#pragma warning(default:4355)

    /**
     *
     */
    COUnknown::~COUnknown() 
    { COMServer::DecrementObjectCount();
    }

    /**
     *
     */
    COUnknown* COUnknown::COUnknownImpl::GetMainUnknown()
    { COUnknown* result = (COUnknown*)((LPBYTE)this - offsetof(COUnknown,m_UnkPrivate));

      return result;
    }

    /**
     *
     */
    STDMETHODIMP COUnknown::COUnknownImpl::QueryInterface(REFIID riid,PPVOID ppv)
    { HRESULT hr = NOERROR;
    
      if( NULL==ppv )
        hr = E_POINTER;
      else if( riid==IID_IUnknown ) 
      { m_cRef++;
        
        *ppv = (LPUNKNOWN)this;
      } 
      else
      { *ppv = NULL;

        hr = GetMainUnknown()->InternalQueryInterface(riid, ppv);
      } // of else

      return hr; 
    }
    
    /**
     *
     */
    STDMETHODIMP_(ULONG) COUnknown::COUnknownImpl::AddRef()
    { return ++m_cRef; }
    
    /**
     *
     */
    STDMETHODIMP_(ULONG) COUnknown::COUnknownImpl::Release()
    { ULONG cRef = --m_cRef;
  
      if( m_cRef<=0 )
        delete GetMainUnknown();
  
      return cRef;
    }
    
    /**
     *
     */
    HRESULT COUnknown::InternalQueryInterface(REFIID  riid,PPVOID ppv)
    { HRESULT hr = NOERROR;
    
      if( NULL==ppv )
        hr = E_POINTER;
      else
      { *ppv = NULL;
  
        hr = E_NOINTERFACE;
      } // of else
      
      return hr;
    }
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
