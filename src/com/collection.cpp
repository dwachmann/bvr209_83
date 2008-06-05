/*
 * $Id$
 * 
 * Collection COM Object.
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
#include "com/collection.h"
#include "com/coenumvariant.h"
#include "exception/comexception.h"
#include "exception/memoryexception.h"

namespace bvr20983
{
  namespace COM
  {
#pragma region Construction & Deconstruction
    /*
     * Collection::Collection
     *
     * Purpose:
     *   Constructor: allocates a collection with initial size
     *
     * Parameters:
     *   ULONG initialSize
     */
    Collection::Collection(ULONG initialSize,ULONG redim)
    { m_index   = 0;
      m_size    = 0;
      m_redim   = redim;
      m_pSA     = NULL;
      
      Alloc(initialSize);
    }

    /*
     * Collection::~Collection
     *
     * Purpose:
     *   Destructor: frees memory
     *
     */
    Collection::~Collection()
    { if( NULL!=m_pSA )
        ::SafeArrayDestroy(m_pSA);
        
      m_pSA = NULL;
    }
#pragma endregion

    /**
     *
     */
    void Collection::Alloc(ULONG newSize)
    { SAFEARRAYBOUND sabound[1];
    
      sabound[0].lLbound   = 0;   
      sabound[0].cElements = newSize;
    
      VARIANT v;
      VariantInit(&v);
      V_VT(&v) = VT_EMPTY;
      
      if( NULL==m_pSA )
      { m_pSA = ::SafeArrayCreate(VT_VARIANT, 1, sabound);
        THROW_MEMORYEXCEPTION(m_pSA);
    
        for( LONG i=0;i<(LONG)newSize;i++ )
          THROW_COMEXCEPTION( ::SafeArrayPutElement(m_pSA, &i, &v) );
      } // of if
      else
      { LONG oldSize = m_pSA->rgsabound[0].cElements;
    
        THROW_COMEXCEPTION( ::SafeArrayRedim(m_pSA,sabound) );
    
        for( LONG i=oldSize;i<(LONG)newSize;i++ )
          THROW_COMEXCEPTION( ::SafeArrayPutElement(m_pSA, &i, &v) );
      } // of else
    }
    
    /*
     * Collection::Add
     *
     * Purpose:
     *   add a pointer to IUnknown to collection
     *
     * Parameters:
     *   IUnknown* pUnk
     *
     * Return:
     *   TRUE if element was added
     */
    bool Collection::Add(IDispatch* pUnk)
    { bool result = false;
    
      if( pUnk!=NULL )
      { LONG i = GetNextFreeIndex();
        
        VARIANT v;
        VariantInit(&v);
        
        V_VT(&v)       = VT_DISPATCH;
        V_DISPATCH(&v) = pUnk;
        
        THROW_COMEXCEPTION( ::SafeArrayPutElement(m_pSA, &i, &v) );
        
        m_size++;
        result = true;
      } // of if
      
      return result;
    }
    
    /*
     * Collection::Add
     *
     * Purpose:
     *   add a string
     *
     * Parameters:
     *   IUnknown* pUnk
     *
     * Return:
     *   TRUE if element was added
     */
    bool Collection::Add(LPCTSTR pStr)
    { bool result = false;
    
      if( pStr!=NULL )
      { LONG i = GetNextFreeIndex();
      
        VARIANT v;
        VariantInit(&v);
        
        V_VT(&v)   = VT_BSTR;
        V_BSTR(&v) = ::SysAllocString(pStr);
        
        THROW_COMEXCEPTION( ::SafeArrayPutElement(m_pSA, &i, &v) );
        
        m_size++;
        result = true;
      } // of if
      
      return result;
    }
    
    /*
     * Collection::GetNextFreeIndex
     *
     * Purpose:
     *   find the next free index in SafeArray
     *   realloc SafeArray, if no free index is found
     *
     * Parameters:
     *   void
     *
     * Return:
     *   LONG free index
     */
    LONG Collection::GetNextFreeIndex()
    { LONG i=0;
      LONG maxElements = m_pSA->rgsabound[0].cElements;
      
      for( ;i<maxElements;i++ )
      { VARIANT v;
      
        THROW_COMEXCEPTION( ::SafeArrayGetElement(m_pSA,&i,&v) );
        
        if( VT_EMPTY==V_VT(&v) )
          break;
      } // of for
      
      if( i>=maxElements )
        Alloc(maxElements+m_redim);
        
      return i;
    }
        
    /*
     * Collection::Remove
     *
     * Purpose:
     *   remove a pointer to IUnknown from collection
     *
     * Parameters:
     *   IUnknown* pUnk
     *
     * Return:
     *   TRUE if element was removed
     */
    bool Collection::Remove(IDispatch* pUnk)
    { bool result = false;
    
      LONG i=0;
      LONG maxElements = m_pSA->rgsabound[0].cElements;
      
      for( ;i<maxElements;i++ )
      { VARIANT v;
      
        HRESULT hr=SafeArrayGetElement(m_pSA,&i,&v);
        THROW_COMEXCEPTION(hr);
        
        if( VT_DISPATCH==V_VT(&v) && pUnk==V_DISPATCH(&v) )
        { VariantClear(&v);
          
          m_size--;
          
          result = true;
          
          break;
        } // of if
      } // of for
    
      return result;
    }
    
    /*
     * Collection::Enumeration
     *
     * Purpose:
     *   returns a IEnumVARIANT Enumeration object
     *
     * Parameters:
     *   IUnknown** ppUnk
     *
     * Return:
     *   HRESULT
     */
    HRESULT Collection::Enumeration(IUnknown** ppUnk)
    { HRESULT hr = NOERROR;
      
      if( NULL==ppUnk )
        return E_POINTER;
    
      hr = COEnumVariant::Create(m_pSA, ppUnk);
    
      return hr;
    }
    
    /*
     * Collection::Get
     *
     * Purpose:
     *   gets element at specified index
     *
     * Parameters:
     *   LONG       index
     *   IUnknown** index
     *
     * Return:
     *   TRUE if successful
     */
    bool  Collection::Get(LONG index,LPVARIANTARG pVar)
    { bool result = false;
    
      if( NULL!=pVar )
      { LONG    i=0;
        LONG    k=0;
        LONG    maxElements = m_pSA->rgsabound[0].cElements;
        VARIANT v;
        
        for( ;i<maxElements;i++ )
        { HRESULT hr = SafeArrayGetElement(m_pSA,&i,&v);
          THROW_COMEXCEPTION(hr);
          
          if( VT_EMPTY!=V_VT(&v) )
            k++;
            
          if( index==k )
          { *pVar = v;

            result = true;
            break;
          } // of if
        } // of for
      } // of if
      
      return result;
    }
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
