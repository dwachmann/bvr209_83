/*
 * $Id$
 * 
 * Proxy implemention of ITypeInfo for debugging.
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
#include <stdarg.h>
#include "com/cotypeinfo.h"
#include "util/logstream.h"
#include "util/comlogstream.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace COM
  {

#pragma region Construction & Deconstruction
    /**
     *
     */
    COTypeInfo::COTypeInfo(ITypeInfo* pTypeInfo) 
      : m_pTypeInfo(pTypeInfo)
    { }
    
    /**
     *
     */
    COTypeInfo::~COTypeInfo()
    { }
#pragma endregion

#pragma region ITypeInfo

    /**
     *
     */
    STDMETHODIMP COTypeInfo::QueryInterface(REFIID riid,void ** ppvObject)
    { HRESULT hr = m_pTypeInfo->QueryInterface(riid,ppvObject);

      LOGGER_DEBUG<<_T("COTypeInfo::QueryInterface():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 

    /**
     *
     */
    ULONG COTypeInfo::AddRef(void)
    { ULONG result = m_pTypeInfo->AddRef();

      LOGGER_DEBUG<<_T("COTypeInfo::AddRef():")<<result<<endl;

      return result;
    } // of 

    /**
     *
     */
    ULONG COTypeInfo::Release(void)
    { ULONG result = m_pTypeInfo->Release();

      LOGGER_DEBUG<<_T("COTypeInfo::Release():")<<result<<endl;

      if( result<=0 )
      { LOGGER_DEBUG<<_T("COTypeInfo::Release(): delete this")<<endl;

        delete this;
      } // of if

      return result;
    } // of 


    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetTypeAttr(TYPEATTR **ppTypeAttr)
    { HRESULT hr = m_pTypeInfo->GetTypeAttr(ppTypeAttr);

      LOGGER_DEBUG<<_T("COTypeInfo::GetTypeAttr():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetTypeComp(ITypeComp **ppTComp)
    { HRESULT hr = m_pTypeInfo->GetTypeComp(ppTComp);

      LOGGER_DEBUG<<_T("COTypeInfo::GetTypeComp():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetFuncDesc(UINT index,FUNCDESC **ppFuncDesc)
    { HRESULT hr = m_pTypeInfo->GetFuncDesc(index,ppFuncDesc);

      LOGGER_DEBUG<<_T("COTypeInfo::GetFuncDesc():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetVarDesc(UINT index,VARDESC **ppVarDesc)
    { HRESULT hr = m_pTypeInfo->GetVarDesc(index,ppVarDesc);

      LOGGER_DEBUG<<_T("COTypeInfo::GetVarDesc():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetNames(MEMBERID memid,BSTR *rgBstrNames,UINT cMaxNames,UINT *pcNames)
    { HRESULT hr = m_pTypeInfo->GetNames(memid,rgBstrNames,cMaxNames,pcNames);

      LOGGER_DEBUG<<_T("COTypeInfo::GetNames():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetRefTypeOfImplType(UINT index,HREFTYPE *pRefType)
    { HRESULT hr = m_pTypeInfo->GetRefTypeOfImplType(index,pRefType);

      LOGGER_DEBUG<<_T("COTypeInfo::GetRefTypeOfImplType():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetImplTypeFlags(UINT index,INT *pImplTypeFlags)
    { HRESULT hr = m_pTypeInfo->GetImplTypeFlags(index,pImplTypeFlags);

      LOGGER_DEBUG<<_T("COTypeInfo::GetImplTypeFlags():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetIDsOfNames(LPOLESTR *rgszNames,UINT cNames,MEMBERID *pMemId)
    { HRESULT hr = m_pTypeInfo->GetIDsOfNames(rgszNames,cNames,pMemId);

      LOGGER_DEBUG<<_T("COTypeInfo::GetIDsOfNames():")<<setHR<<CHResult(hr)<<endl;

      if( SUCCEEDED(hr) )
      { for( UINT i=0;i<cNames;i++ )
          LOGGER_DEBUG<<_T("  ")<<rgszNames[i]<<_T(":")<<dec<<pMemId[i]<<endl;
      } // of if
      else
      { for( UINT i=0;i<cNames;i++ )
          LOGGER_DEBUG<<_T("  ")<<rgszNames[i]<<endl;
      } // of else

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::Invoke(PVOID pvInstance,
                                    MEMBERID memid,
                                    WORD wFlags,
                                    DISPPARAMS *pDispParams,
                                    VARIANT *pVarResult,
                                    EXCEPINFO *pExcepInfo,
                                    UINT *puArgErr
                                   )
    { HRESULT hr = m_pTypeInfo->Invoke(pvInstance,memid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr);

      LOGGER_DEBUG<<_T("COTypeInfo::Invoke(memid=")<<dec<<memid<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetDocumentation(MEMBERID memid,BSTR *pBstrName,BSTR *pBstrDocString,DWORD *pdwHelpContext,BSTR *pBstrHelpFile)
    { HRESULT hr = m_pTypeInfo->GetDocumentation(memid,pBstrName,pBstrDocString,pdwHelpContext,pBstrHelpFile);

      LOGGER_DEBUG<<_T("COTypeInfo::GetDocumentation(memid=")<<memid<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetDllEntry(MEMBERID memid,INVOKEKIND invKind,BSTR *pBstrDllName,BSTR *pBstrName,WORD *pwOrdinal)
    { HRESULT hr = m_pTypeInfo->GetDllEntry(memid,invKind,pBstrDllName,pBstrName,pwOrdinal);

      LOGGER_DEBUG<<_T("COTypeInfo::GetDllEntry(memid=")<<memid<<_T(")")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetRefTypeInfo(HREFTYPE hRefType,ITypeInfo **ppTInfo)
    { HRESULT hr = m_pTypeInfo->GetRefTypeInfo(hRefType,ppTInfo);

      LOGGER_DEBUG<<_T("COTypeInfo::GetRefTypeInfo():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::AddressOfMember(MEMBERID memid,INVOKEKIND invKind,PVOID *ppv)
    { HRESULT hr = m_pTypeInfo->AddressOfMember(memid,invKind,ppv);

      LOGGER_DEBUG<<_T("COTypeInfo::AddressOfMember(memid=")<<memid<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::CreateInstance(IUnknown *pUnkOuter,REFIID riid,PVOID *ppvObj)
    { HRESULT hr = m_pTypeInfo->CreateInstance(pUnkOuter,riid,ppvObj);

      LOGGER_DEBUG<<_T("COTypeInfo::CreateInstance():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetMops(MEMBERID memid,BSTR *pBstrMops)
    { HRESULT hr = m_pTypeInfo->GetMops(memid,pBstrMops);

    LOGGER_DEBUG<<_T("COTypeInfo::GetMops(memid=")<<memid<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP COTypeInfo::GetContainingTypeLib(ITypeLib **ppTLib,UINT *pIndex)
    { HRESULT hr = m_pTypeInfo->GetContainingTypeLib(ppTLib,pIndex);

      LOGGER_DEBUG<<_T("COTypeInfo::GetContainingTypeLib():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP_(void) COTypeInfo::ReleaseTypeAttr(TYPEATTR *pTypeAttr)
    { m_pTypeInfo->ReleaseTypeAttr(pTypeAttr);
      
      LOGGER_DEBUG<<_T("COTypeInfo::ReleaseTypeAttr()")<<endl;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP_(void) COTypeInfo::ReleaseFuncDesc(FUNCDESC *pFuncDesc)
    { m_pTypeInfo->ReleaseFuncDesc(pFuncDesc);
      
      LOGGER_DEBUG<<_T("COTypeInfo::ReleaseFuncDesc()")<<endl;
    } // of 
    
    /**
     *
     */
    STDMETHODIMP_(void) COTypeInfo::ReleaseVarDesc(VARDESC *pVarDesc)
    { m_pTypeInfo->ReleaseVarDesc(pVarDesc);
      
      LOGGER_DEBUG<<_T("COTypeInfo::ReleaseVarDesc()")<<endl;
    } // of 
#pragma endregion
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
