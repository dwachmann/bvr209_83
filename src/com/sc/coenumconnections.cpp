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
#include "com/coenumconnections.h"

namespace bvr20983
{
  namespace COM
  {
    /*
     * COEnumConnections::COEnumConnections
     * COEnumConnections::~COEnumConnections
     *
     * Parameters (Constructor):
     *  pUnkRef         LPUNKNOWN to use for reference counting.
     *  cConn           ULONG number of connections in prgpConn
     *  prgConnData     LPCONNECTDATA to the array to enumerate.
     */
    COEnumConnections::COEnumConnections(ULONG cConn, LPCONNECTDATA prgConnData)
    { m_cRef       = 0;
      m_iCur       = 0;
      m_cConn      = cConn;
      m_rgConnData = new CONNECTDATA[(UINT)cConn];
    
      UINT i;
      
      if( NULL!=m_rgConnData )
        for( i=0; i < cConn; i++ )
        { m_rgConnData[i]=prgConnData[i];
        
          if( NULL!=m_rgConnData[i].pUnk )
            m_rgConnData[i].pUnk->AddRef();
        } // of for
    } // of COEnumConnections::COEnumConnections()
    
    /**
     *
     */
    COEnumConnections::~COEnumConnections()
    { if( NULL!=m_rgConnData )
      { UINT i;
    
        for( i=0; i < m_cConn; i++ )
          RELEASE_INTERFACE( m_rgConnData[i].pUnk );
    
        delete [] m_rgConnData;
      } // of if
    } // of COEnumConnections::~COEnumConnections()
    
    /*
     * COEnumConnections::QueryInterface
     * COEnumConnections::AddRef
     * COEnumConnections::Release
     *
     * Purpose:
     *  IUnknown members for COEnumConnections object.
     */
    STDMETHODIMP COEnumConnections::QueryInterface(REFIID riid, LPVOID *ppv)
    { HRESULT result = E_NOINTERFACE;
    
      *ppv=NULL;
    
      if( IID_IUnknown==riid || IID_IEnumConnections==riid )
        *ppv=(LPVOID)this;
    
      if( NULL!=*ppv )
      { ((LPUNKNOWN)*ppv)->AddRef();
        result = NOERROR;
      }
    
      return result;
    } // of COEnumConnections::QueryInterface()
    
   /**
    *
    */    
    STDMETHODIMP_(ULONG) COEnumConnections::AddRef()
    { ++m_cRef;
    
      return m_cRef;
    }
    
   /**
    *
    */    
    STDMETHODIMP_(ULONG) COEnumConnections::Release()
    { ULONG result = --m_cRef;
    
      if( result<=0 )
        delete this;
  
      return result;
    }
    
    /*
     *
     */
    STDMETHODIMP COEnumConnections::Next(ULONG cConn, LPCONNECTDATA pConnData, ULONG *pulEnum)
    { HRESULT result  = S_FALSE;
      ULONG   cReturn = 0L;
      
      if( NULL!=m_rgConnData )
      { if( NULL==pulEnum && 1L!=cConn )
          result = E_POINTER;
        else
        { if( NULL!=pulEnum )
            *pulEnum=0L;
    
          if( NULL!=pConnData && m_iCur < m_cConn )
          { 
            while( m_iCur<m_cConn && cConn>0 )
            { *pConnData++ = m_rgConnData[m_iCur];
              
              if( NULL!=m_rgConnData[m_iCur].pUnk )
                m_rgConnData[m_iCur++].pUnk->AddRef();
                
              cReturn++;
              cConn--;
            } // of while
          
            if( NULL!=pulEnum )
              *pulEnum=cReturn;
              
            result = NOERROR;
          } // of if
        } // of else
      } // of if
      
      return result;
    } // of COEnumConnections::Next()
    
    /**
     *
     */
    STDMETHODIMP COEnumConnections::Skip(ULONG cSkip)
    { HRESULT result = S_FALSE;
      
      if( ((m_iCur+cSkip)<m_cConn) && NULL!=m_rgConnData )
      { result = NOERROR;
    
        m_iCur+=cSkip;
      } // of if
      
      return NOERROR;
    } // of COEnumConnections::Skip()
    
    /**
     *
     */
    STDMETHODIMP COEnumConnections::Reset()
    { m_iCur=0;
     
      return NOERROR;
    } // of COEnumConnections::Reset()
    
    /**
     *
     */
    STDMETHODIMP COEnumConnections::Clone(LPENUMCONNECTIONS* ppEnum)
    { HRESULT result = NOERROR;
    
      *ppEnum = NULL;
    
      //Create the clone
      COEnumConnections* pNew = new COEnumConnections( m_cConn, m_rgConnData);
    
      if( NULL!=pNew )
      { pNew->AddRef();
        pNew->m_iCur=m_iCur;
      
        *ppEnum=pNew;
      } // of else
      else
        result = E_OUTOFMEMORY;
      
      return result;
    } // of COEnumConnections::Clone()
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
