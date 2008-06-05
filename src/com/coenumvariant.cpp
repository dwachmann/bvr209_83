/*
 * $Id$
 * 
 * COEnumVariant Collection COM Object.
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
#include "com/coenumvariant.h"
#include "util/logstream.h"
#include "exception/memoryexception.h"
#include "exception/comexception.h"
#include "com/coenum.h"

namespace bvr20983
{
  namespace COM
  {

    /*
     * COEnumVariant::Create
     *
     * Purpose:
     *  Creates an instance of the IEnumVARIANT enumerator object and initializes it.
     *
     * Parameters:
     *  psa        Safe array containing items to be enumerated.
     *  cElements  Number of items to be enumerated. 
     *  ppenumvariant    Returns enumerator object.
     *
     * Return Value:
     *  HRESULT
     *
     */
    HRESULT COEnumVariant::Create(SAFEARRAY* psa, IUnknown** ppUnk) 
    { HRESULT        hr           = NOERROR;
      HRESULT        exHr         = E_OUTOFMEMORY;
      COEnumVariant* penumvariant = NULL;
      LONG           lLBound      = -1;
      LONG           lUBound      = -1;
      
      LOGGER_DEBUG<<_T("COEnumVariant::Create().1")<<endl;
      
      if( NULL==psa || NULL==ppUnk )
        return E_POINTER;
    
      try
      { penumvariant = new COEnumVariant();
        LOGGER_DEBUG<<_T("COEnumVariant::Create().2")<<endl;
          
        THROW_COMEXCEPTION( ::SafeArrayGetLBound(psa, 1, &lLBound) );
        THROW_COMEXCEPTION( ::SafeArrayGetUBound(psa, 1, &lUBound) );
    
        LOGGER_DEBUG<<_T("COEnumVariant::Create().3")<<endl;
    
        penumvariant->m_cElements = 0;
        penumvariant->m_lLBound   = lLBound;
        penumvariant->m_lCurrent  = lLBound;                  
    
        //
        // copy none empty members of safearray to enumeration
        //
        LONG i           = 0;
        LONG k           = 0;
    
        for( LONG i=lLBound;i<=lUBound;i++ )
        { VARIANT v;
        
          THROW_COMEXCEPTION( ::SafeArrayGetElement(psa,&i,&v) );
          
          if( VT_EMPTY!=V_VT(&v) )
            (penumvariant->m_cElements)++;
        } // of for
    
        SAFEARRAYBOUND sabound[1];
      
        sabound[0].lLbound   = lLBound;   
        sabound[0].cElements = penumvariant->m_cElements;
          
        penumvariant->m_psa = SafeArrayCreate(VT_VARIANT, 1, sabound);
        THROW_MEMORYEXCEPTION(penumvariant->m_psa);
    
        for( i=i=lLBound;i<=lUBound;i++ )
        { VARIANT v;
        
          THROW_COMEXCEPTION( ::SafeArrayGetElement(psa,&i,&v) );
          
          if( VT_EMPTY!=V_VT(&v) )
          { THROW_COMEXCEPTION( ::SafeArrayPutElement(penumvariant->m_psa,&k,&v) );
            
            k++;
          } // of if
        } // of for
    
        LOGGER_DEBUG<<_T("COEnumVariant::Create().5")<<endl;
        
        hr = penumvariant->QueryInterface(IID_IUnknown, (VOID**)ppUnk);
         
        LOGGER_DEBUG<<_T("COEnumVariant::Create().6")<<endl;
      }
      catch( MemoryException ex )
      { LOGGER_ERROR<<_T("COEnumVariant::Create()")<<ex;

        if( NULL!=penumvariant )
          delete penumvariant;
        
        hr = exHr;
      }
    
      return hr;
    }
    
    /*
     * COEnumVariant::COEnumVariant
     *
     * Purpose:
     *  Constructor for COEnumVariant object. Initializes members to NULL.
     *
     */
    COEnumVariant::COEnumVariant()
    { m_psa  = NULL; 
      m_cRef = 0;
    }
    
    /*
     * COEnumVariant::~COEnumVariant
     *
     * Purpose:
     *  Destructor for COEnumVariant object. 
     *
     */
    COEnumVariant::~COEnumVariant()
    { LOGGER_DEBUG<<_T("COEnumVariant::~COEnumVariant()")<<endl;
    
      if( NULL!=m_psa ) 
        ::SafeArrayDestroy(m_psa);
    }
    
    /*
     * COEnumVariant::QueryInterface, AddRef, Release
     *
     * Purpose:
     *  Implements IUnknown::QueryInterface, AddRef, Release
     *
     */
    STDMETHODIMP COEnumVariant::QueryInterface(REFIID iid, void * * ppv) 
    { *ppv = NULL;
          
      if( iid==IID_IUnknown || iid==IID_IEnumVARIANT ) 
        *ppv = this;     
      else 
        return E_NOINTERFACE; 
    
      AddRef();
      
      return NOERROR;    
    }
    
    /**
     *
     */
    STDMETHODIMP_(ULONG) COEnumVariant::AddRef()
    { return ++m_cRef; }
    
    /**
     *
     */
    STDMETHODIMP_(ULONG) COEnumVariant::Release()
    { ULONG result = --m_cRef;

      if( 0==result )
        delete this;
    
      return result;
    } // of COEnumVariant::Release()
    
    /*
     * COEnumVariant::Next
     *
     * Purpose:
     *  Retrieves the next cElements elements. Implements IEnumVARIANT::Next. 
     *
     */
    STDMETHODIMP COEnumVariant::Next(ULONG cElements, VARIANT * pvar, ULONG * pcElementFetched)
    { HRESULT hr   = NOERROR;
      HRESULT exHr = E_OUTOFMEMORY;
      long    l1   = m_lCurrent;
      ULONG   l2   = 0;
      
      try
      { if( NULL!=pcElementFetched )
          *pcElementFetched = 0;
            
        // Retrieve the next cElements elements.
        for( ; l1<(long)(m_lLBound+m_cElements) && l2<cElements; l1++, l2++ )
          THROW_COMEXCEPTION( ::SafeArrayGetElement(m_psa, &l1, &pvar[l2]) );
      
        // Set count of elements retrieved
        if( NULL!=pcElementFetched )
          *pcElementFetched = l2;
    
        m_lCurrent = l1;
        
        if( l2<cElements )
          hr = S_FALSE;
      }
      catch( COMException ex )
      { LOGGER_ERROR<<_T("COEnumVariant::Create()")<<ex;
      
        for( ULONG l=0; l<cElements; l++ )
          ::VariantClear(&pvar[l]);
        
        hr = exHr;
      }
    
      return hr;    
    }
    
    /*
     * COEnumVariant::Skip
     *
     * Purpose:
     *  Skips the next cElements elements. Implements IEnumVARIANT::Skip. 
     *
     */
    STDMETHODIMP COEnumVariant::Skip(ULONG cElements)
    { HRESULT result = NOERROR;
    
      m_lCurrent += cElements; 
    
      if( m_lCurrent>(long)(m_lLBound+m_cElements) )
      { m_lCurrent =  m_lLBound+m_cElements;
        
        result = S_FALSE;
      } 
      
      return result;
    }
    
    /*
     * COEnumVariant::Reset
     *
     * Purpose:
     *  Resets the current element in the enumerator to the beginning. Implements IEnumVARIANT::Reset. 
     *
     */
    STDMETHODIMP COEnumVariant::Reset()
    { m_lCurrent = m_lLBound;
    
      return NOERROR;
    }
    
    /*
     * COEnumVariant::Clone
     *
     * Purpose:
     *  Creates a copy of the current enumeration state. Implements IEnumVARIANT::Clone. 
     *
     */
    STDMETHODIMP COEnumVariant::Clone(IEnumVARIANT** ppenum)
    { HRESULT        hr    = NOERROR;
      IUnknown*      pUnk  = NULL;
      
      if( NULL==ppenum )
        return E_POINTER;
        
      hr = COEnumVariant::Create(m_psa, &pUnk);
      if( SUCCEEDED(hr) )
      { pUnk->QueryInterface(IID_IEnumVARIANT, (VOID**)ppenum);
        
        pUnk->Release();
        
        ((COEnumVariant*)pUnk)->m_lCurrent = m_lCurrent;
      } // of if
        
      return hr;        
    }
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
