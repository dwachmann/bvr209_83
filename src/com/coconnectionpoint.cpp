/*
 * $Id$
 * 
 * Connection point object that manages IUnknown pointers.  
 * This is a stand-alone object created from the implementation of IConnectionPointContainer.
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
#include "com/coconnectionpoint.h"
#include "com/coenum.h"
#include "com/codispatch.h"
#include "util/logstream.h"
#include "util/comlogstream.h"

namespace bvr20983
{
  namespace COM
  {

    /*
     * COConnectionPoint::COConnectionPoint
     *
     * Parameters (Constructor):
     *  pObj            PCConnObject of the object we're in.  We can
     *                  query this for the IConnectionPointContainer
     *                  interface we might need.
     *  riid            REFIID of the interface we're supporting
     */
    COConnectionPoint::COConnectionPoint(IUnknown* pObj,REFGUID typelibGUID, REFIID riid)
      : m_typelibGUID(typelibGUID),
        m_iid(riid)
    { m_cRef         = 1;
      m_cConn        = 0;
      m_pObj         = pObj;
      m_pITypeInfo   = NULL;
    
      for( UINT i=0; i < CCONNMAX; i++)
        m_gpUnknown[i].SetIID(m_iid);
    
      CODispatch::LoadTypeInfo(m_typelibGUID,m_iid,&m_pITypeInfo);
    } // of COConnectionPoint::COConnectionPoint()
    
    /**
     * COConnectionPoint::~COConnectionPoint
     *
     */
    COConnectionPoint::~COConnectionPoint()
    { RELEASE_INTERFACE(m_pITypeInfo);
    } // of COConnectionPoint::~COConnectionPoint()
    
    
    /*
     * COConnectionPoint::QueryInterface
     * COConnectionPoint::AddRef
     * COConnectionPoint::Release
     *
     * Purpose:
     *  Non-delegating IUnknown members for COConnectionPoint.
     */
    STDMETHODIMP COConnectionPoint::QueryInterface(REFIID riid, LPVOID *ppv)
    { HRESULT result = E_NOINTERFACE;
      
      *ppv=NULL;
      
      if( IID_IUnknown==riid || IID_IConnectionPoint==riid )
        *ppv=(LPVOID)this;
      else
        LOGGER_DEBUG<<_T("COConnectionPoint::QueryInterface(")<<riid<<_T(") unknown interface ")<<endl;
    
      if( NULL!=*ppv )
      { ((LPUNKNOWN)*ppv)->AddRef();
        
        result = NOERROR;
      } // of if
    
      return result;
    }
    
    /**
     *
     */
    STDMETHODIMP_(ULONG) COConnectionPoint::AddRef()
    { return ++m_cRef; }
    
    /**
     *
     */
    STDMETHODIMP_(ULONG) COConnectionPoint::Release()
    { ULONG result = --m_cRef;

      if( result<=0 )
        delete this;

      return result;
    }
    
    /*
     * COConnectionPoint::GetConnectionInterface
     *
     * Purpose:
     *  Returns the IID of the outgoing interface supported through
     *  this connection point.
     *
     * Parameters:
     *  pIID            IID * in which to store the IID.
     */
    STDMETHODIMP COConnectionPoint::GetConnectionInterface(IID *pIID)
    { HRESULT result = E_POINTER;
    
      if( NULL!=pIID )
      { *pIID=m_iid;
      
        LOGGER_DEBUG<<_T("COConnectionPoint::GetConnectionInterface(")<<m_iid<<_T(")")<<endl;
    
        result = NOERROR;
      } // of if
      
      return result;
    }
    
    /*
     * COConnectionPoint::GetConnectionPointContainer
     *
     * Purpose:
     *  Returns a pointer to the IConnectionPointContainer that
     *  is manageing this connection point.
     *
     * Parameters:
     *  ppCPC           IConnectionPointContainer ** in which to return
     *                  the pointer after calling AddRef.
     */
    STDMETHODIMP COConnectionPoint::GetConnectionPointContainer(IConnectionPointContainer **ppCPC)
    { return m_pObj->QueryInterface(IID_IConnectionPointContainer, (void **)ppCPC); }
    
    
    /*
     * COConnectionPoint::Advise
     *
     * Purpose:
     *  Provides this connection point with a notification sink to
     *  call whenever the appropriate outgoing function/event occurs.
     *
     * Parameters:
     *  pUnkSink        LPUNKNOWN to the sink to notify.  The connection
     *                  point must QueryInterface on this pointer to obtain
     *                  the proper interface to call.  The connection
     *                  point must also insure that any pointer held has
     *                  a reference count (QueryInterface will do it).
     *  pdwCookie       DWORD * in which to store the connection key for
     *                  later calls to Unadvise.
     */
    STDMETHODIMP COConnectionPoint::Advise(LPUNKNOWN pUnkSink, DWORD *pdwCookie)
    { IUnknown* pSink=NULL;
    
      *pdwCookie=0;
      
      //Check if we're already full of sink pointers
      if( CCONNMAX==m_cConn )
        return CONNECT_E_ADVISELIMIT;
    
      /*
       * Verify that the sink has the interface it's supposed
       * to.  We don't have to know what it is because we have
       * m_iid to describe it.  If this works, then we neatly
       * have a pointer with an AddRef that we can stow away.
       */
      if( FAILED(pUnkSink->QueryInterface(m_iid, (PPVOID)&pSink)) )
        return CONNECT_E_CANNOTCONNECT;
    
      /*
       * We got the sink, now store it in our array.  We'll look for
       * the first entry that is NULL, indicating a free spot.
       */
      for( UINT i=0;i<CCONNMAX;i++ )
        if( m_gpUnknown[i].IsNULL() )
        { m_gpUnknown[i] = pUnkSink;

          *pdwCookie = m_gpUnknown[i].GetCookie();
            
          LOGGER_DEBUG<<_T("COConnectionPoint::Advise(dwCookie=0x")<<hex<<*pdwCookie<<_T(" i=)")<<dec<<i<<endl;
            
          break;
        } // of if
    
      m_cConn++;
      
      LOGGER_DEBUG<<_T("COConnectionPoint::Advise() m_cConn=")<<m_cConn<<endl;
    
      return NOERROR;
    } // of COConnectionPoint::Advise()
    
    /*
     * COConnectionPoint::Unadvise
     *
     * Purpose:
     *  Terminates the connection to the notification sink identified
     *  with dwCookie (that was returned from Advise).  The connection
     *  point has to Release any held pointers for that sink.
     *
     * Parameters:
     *  dwCookie        DWORD connection key from Advise.
     */
    STDMETHODIMP COConnectionPoint::Unadvise(DWORD dwCookie)
    { HRESULT result = CONNECT_E_NOCONNECTION;
    
      if( 0==dwCookie )
        result = E_INVALIDARG;
      else
        for( UINT i=0;i<CCONNMAX;i++ )
          if( dwCookie==m_gpUnknown[i].GetCookie() )
          { m_gpUnknown[i].Release();
            m_cConn--;

            result = NOERROR;
            
            break;
          } // of if
    
      LOGGER_DEBUG<<_T("COConnectionPoint::Unadvise(dwCookie=")<<dwCookie<<_T(") m_cConn=")<<m_cConn<<endl;
        
      return result;
    } // of COConnectionPoint::Unadvise()
    
    /*
     * COConnectionPoint::EnumConnections
     *
     * Purpose:
     *  Creates and returns an enumerator object with the
     *  IEnumConnections interface that will enumerate the IUnknown
     *  pointers of each connected sink.
     *  
     *  this member is potentially called from a different thread
     *
     * Parameters:
     *  ppEnum          LPENUMCONNECTIONS in which to store the
     *                  IEnumConnections pointer.
     */
    STDMETHODIMP COConnectionPoint::EnumConnections(LPENUMCONNECTIONS *ppEnum)
    { HRESULT            result=OLE_E_NOCONNECTION;
      LPCONNECTDATA      pCD=NULL;
      UINT               i, j;
      COEnumConnections* pEnum=NULL;
    
      if( NULL==ppEnum )
        result = E_POINTER;
      else
      {
        *ppEnum=NULL;
    
        if( 0!=m_cConn )
        { pCD=new CONNECTDATA[(UINT)m_cConn];
        
          if( NULL==pCD )
            result = E_OUTOFMEMORY;
          else
          { COMPtr<IUnknown> pUnk[CCONNMAX];

            ::memset(pCD,0,sizeof(*pCD)*m_cConn);
          
            for( i=0,j=0;i<CCONNMAX;i++ )
              if( !m_gpUnknown[i].IsNULL() )
              { LOGGER_DEBUG<<_T("COConnectionPoint::EnumConnections(): m_gpUnknown[")<<dec<<i<<_T("]=")<<m_gpUnknown[i].GetCookie()<<endl;

                pUnk[j] = m_gpUnknown[i];
              
                pCD[j].pUnk     = pUnk[j];
                pCD[j].dwCookie = m_gpUnknown[i].GetCookie();
                j++;
              } // of if
          
            /*
             * If creation works, it makes a copy pCD, so we can
             * always delete it regardless of the outcome.
             */
            if( pCD!=NULL )
            { pEnum=new COEnumConnections(IID_IEnumConnections,m_cConn, pCD);
              
              delete[] pCD;
            } // of if
          
            if( NULL!=pEnum )
            { *ppEnum = pEnum;

              result = S_OK;
            } // of if
            else
              result = E_OUTOFMEMORY;
          } // of else
        } // of if
      } // of else
    
      LOGGER_DEBUG<<_T("COConnectionPoint::EnumConnections()")<<endl;
      
      return result;
    } // of COConnectionPoint::EnumConnections()
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
