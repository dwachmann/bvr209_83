/*
 * $Id$
 * 
 * CoCollection Collection COM Object.
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
#include "com/cocollection.h"
#include "com/collection.h"
#include "com/comserver.h"
#include "util/logstream.h"
#include "util/comlogstream.h"

const IID IID_ICollectionInternal = 
{0x7dc6e86d,0x0001,0x0002,{0x00,0x04,0x42,0x56,0x52,0x20,0x09,0x83}};

namespace bvr20983
{
  namespace COM
  {
    LPCTSTR CoCollection::dispNames[] =
    { _T("Item"),_T("Count"),_T("_NewEnum") };

    DISPID CoCollection::dispIds[] =
    { DISPID_LISTITEM,DISPID_LISTCOUNT,DISPID_NEWENUM };

#pragma region Construction & Deconstruction
#pragma warning(disable:4355)
    /**
     *
     */
    CoCollection::CoCollection(IUnknown* pUnkOuter) :
      COUnknown(pUnkOuter)
    { }
#pragma warning(default:4355)
#pragma endregion    

#pragma region Add
    /*
     * CoCollection::Add
     *
     * Purpose:
     *   Add a IDispatch COM object to the collection.
     *
     * Parameters:
     *   IDispatch* pUnk
     *
     * Return Value:
     *   bool true if successfull, false otherwise.
     */
    bool CoCollection::Add(IDispatch* pUnk)
    { return m_collection.Add(pUnk); }
    
    /*
     * CoCollection::Add
     *
     * Purpose:
     *   Add a string object to the collection.
     *
     * Parameters:
     *   LPCTSTR pStr
     *
     * Return Value:
     *   bool true if successfull, false otherwise.
     */
    bool CoCollection::Add(LPCTSTR pStr)
    { return m_collection.Add(pStr); }
#pragma endregion

#pragma region CoCollection::QI
    /**
     *
     */
    HRESULT CoCollection::InternalQueryInterface(REFIID riid,PPVOID ppv)
    { HRESULT hr = E_NOINTERFACE;
    
      *ppv = NULL;
      
      if( IID_IUnknown==riid )
      { *ppv = (IUnknown*)this;
      
        LOGGER_DEBUG<<_T("CoCollection::QueryInterface. IID_IUnknown==riid 'this' returned.")<<endl;
      }
      else if( IID_IDispatch==riid )
      { *ppv = (IDispatch*)this;
      
        LOGGER_DEBUG<<_T("CoCollection::QueryInterface. IID_IDispatch==riid 'this' returned.")<<endl;
      }
      else if( IID_ICollectionInternal==riid )
      { *ppv = (ICollectionInternal*)this;
        LOGGER_DEBUG<<_T("CoCollection::QueryInterface. IID_ICollectionInternal==riid 'this' returned.")<<endl;
      }
      else
        LOGGER_DEBUG<<_T("CoCollection::QueryInterface(riid=%s) unsupported interface")<<riid<<endl;
    
      if( NULL!=*ppv )
      { ((IUnknown*)*ppv)->AddRef();
    
        hr = NOERROR;
      }
        
      return hr;
    } // of CoCollection::InternalQueryInterface()
#pragma endregion

#pragma region DualIDispatchImpl::IDispatch
    /**
     *
     */
    STDMETHODIMP CoCollection::GetTypeInfoCount(UINT *pctInfo)
    { *pctInfo=0;

      LOGGER_DEBUG<<_T("CoCollection::GetTypeInfoCount()")<<endl;
     
      return NOERROR;
    } // of CoCollection::GetTypeInfoCount()
    
    /*
     *
     */
    STDMETHODIMP CoCollection::GetTypeInfo(UINT itInfo,LCID lcid,ITypeInfo **ppITypeInfo)
    { HRESULT hr = E_NOTIMPL;

      if( NULL==ppITypeInfo )
        hr = E_POINTER;
      else
        *ppITypeInfo=NULL;

      LOGGER_DEBUG<<_T("CoCollection::GetTypeInfo(itInfo=")<<itInfo<<_T(",lcid=")<<lcid<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of CoCollection::GetTypeInfo()
    
    /*
     * DualIDispatchImpl::GetIDsOfNames
     *
     * Purpose:
     *  Converts text names into DISPIDs to pass to Invoke
     *
     * Parameters:
     *  riid            REFIID reserved.  Must be IID_NULL.
     *  rgszNames       OLECHAR ** pointing to the array of names to be
     *                  mapped.
     *  cNames          UINT number of names to be mapped.
     *  lcid            LCID of the locale.
     *  rgDispID        DISPID * caller allocated array containing IDs
     *                  corresponging to those names in rgszNames.
     *
     * Return Value:
     *  HRESULT         NOERROR or a general error code.
     */
    STDMETHODIMP CoCollection::GetIDsOfNames(REFIID riid, 
                                             OLECHAR **rgszNames, 
                                             UINT cNames, 
                                             LCID lcid, 
                                             DISPID *rgDispID
                                            )
    { HRESULT hr=NOERROR;
    
      if( IID_NULL!=riid )
        hr = DISP_E_UNKNOWNINTERFACE;
      else
      { if( PRIMARYLANGID(lcid)!=LANG_NEUTRAL && PRIMARYLANGID(lcid)!=LANG_ENGLISH )
          hr = DISP_E_UNKNOWNLCID;
        else
        {
          hr = DISP_E_UNKNOWNNAME;

          for( UINT i=0;i<cNames;i++ )
          { rgDispID[i] = DISPID_UNKNOWN;

            for( UINT k=0;k<ARRAYSIZE(dispNames);k++ )
              if( _tcsicmp(rgszNames[i],dispNames[k])==0 )
              { rgDispID[i] = dispIds[k];
                hr          = NOERROR;

                break;
              } // of if
          } // of for
        } // of else
      } // of else

      LOGGER_DEBUG<<_T("CoCollection::GetIDsOfNames(riid=")<<riid<<_T(",lcid=")<<lcid<<_T(")")<<endl;

      if( SUCCEEDED(hr) )
      { for( UINT i=0;i<cNames;i++ )
        { LOGGER_DEBUG<<rgszNames[i]<<_T(":")<<dec<<rgDispID[i]<<endl;
        } // of for
      }
      else
      { for( UINT i=0;i<cNames;i++ )
        { LOGGER_DEBUG<<rgszNames[i]<<endl;
        } // of for
      }

      LOGGER_DEBUG<<_T("CoCollection::GetIDsOfNames():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of CoCollection::GetIDsOfNames)
    
    /*
     *
     */
    STDMETHODIMP CoCollection::Invoke(DISPID         dispID, 
                                      REFIID         riid, 
                                      LCID           lcid, 
                                      unsigned short wFlags, 
                                      DISPPARAMS*    pDispParams, 
                                      VARIANT*       pVarResult, 
                                      EXCEPINFO*     pExcepInfo, 
                                      UINT*          puArgErr
                                     )
    { HRESULT hr = NOERROR;
    
      //riid is supposed to be IID_NULL always
      if( IID_NULL!=riid )
        return DISP_E_UNKNOWNINTERFACE;

      LOGGER_DEBUG<<_T("CoCollection::Invoke(dispID=")<<dispID<<_T(",wFlags=")<<wFlags<<_T(")")<<endl;

      switch( dispID )
      {
      case DISPID_LISTITEM:
        if( (DISPATCH_PROPERTYGET & wFlags)==0 && (DISPATCH_METHOD & wFlags)==0 )
          hr = DISP_E_MEMBERNOTFOUND;
        else
        { if( NULL==pVarResult )
            hr = E_INVALIDARG;
          else if( pDispParams->cArgs!=1 )
            hr = DISP_E_BADPARAMCOUNT;
          else
          { VARIANT index;

            ::VariantInit(&index);

            hr = ::VariantChangeType(&index,&pDispParams->rgvarg[0],0,VT_I4); 
            if( FAILED(hr) )
            { if( NULL!=puArgErr )
                *puArgErr = 0;
            } // of if
            else
            { ::VariantInit(pVarResult);

              hr = get_Item(index.lVal,pVarResult);
            } // of else
          } // of else
        } // of else
        break;
      case DISPID_LISTCOUNT:
        if( (DISPATCH_PROPERTYGET & wFlags)==0 && (DISPATCH_METHOD & wFlags)==0 )
          hr = DISP_E_MEMBERNOTFOUND;
        else
        { if( NULL==pVarResult )
            hr = E_INVALIDARG;
          else
          { VariantInit(pVarResult);
            
            V_VT(pVarResult) = VT_I4;

            get_Count( &V_I4(pVarResult) );
          } // of else
        } // of else
        break;
      case DISPID_NEWENUM:
        if( (DISPATCH_METHOD & wFlags)==0 )
          hr = DISP_E_MEMBERNOTFOUND;
        else
        { if( NULL==pVarResult )
            hr = E_INVALIDARG;
          else
          { VariantInit(pVarResult);
            
            V_VT(pVarResult) = VT_UNKNOWN;

            hr = get__NewEnum( &V_UNKNOWN(pVarResult) );
          } // of else
        } // of else
        break;
      default:
        hr = DISP_E_MEMBERNOTFOUND;
        break;
      } // of switch
    
      LOGGER_DEBUG<<_T("CoCollection::Invoke()")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of CoCollection::Invoke()
#pragma endregion

#pragma region DualIDispatchImpl::IDispatch
    /**
     *
     */
    STDMETHODIMP CoCollection::get_Item(long index,VARIANT *pVarResult)
    { HRESULT hr = NOERROR;

      if( NULL==pVarResult )
        hr = E_POINTER;
      else
        hr = m_collection.Get(index,pVarResult) ? S_OK : S_FALSE;

      return hr;
    } // of CoCollection::get_Item()

    /**
     *
     */
    STDMETHODIMP CoCollection::get_Count(long *retval)
    { HRESULT hr = NOERROR;

      if( NULL==retval )
        hr = E_POINTER;
      else
        *retval = m_collection.Size();

      return hr;
    } // of CoCollection::get_Count()

    /**
     *
     */
    STDMETHODIMP CoCollection::get__NewEnum(IUnknown **pVarResult)
    { return m_collection.Enumeration( pVarResult ); }
#pragma endregion
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
