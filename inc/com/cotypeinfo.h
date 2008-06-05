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
#ifndef COTYPEINFO_H
#define COTYPEINFO_H

#include <vector>
#include "com/counknown.h"
#include "com/comutil.h"


namespace bvr20983
{
  namespace COM
  {
    class COTypeInfo : public ITypeInfo
    {
      public:
        COTypeInfo(ITypeInfo* pTypeInfo);
        virtual ~COTypeInfo();

        STDMETHODIMP QueryInterface(REFIID riid,void ** ppvObject);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP GetTypeAttr( 
            /* [out] */ TYPEATTR **ppTypeAttr);
        
        STDMETHODIMP GetTypeComp( 
            /* [out] */ ITypeComp **ppTComp);
        
        STDMETHODIMP GetFuncDesc( 
            /* [in] */ UINT index,
            /* [out] */ FUNCDESC **ppFuncDesc);
        
        STDMETHODIMP GetVarDesc( 
            /* [in] */ UINT index,
            /* [out] */ VARDESC **ppVarDesc);
        
        STDMETHODIMP GetNames( 
            /* [in] */ MEMBERID memid,
            /* [length_is][size_is][out] */ BSTR *rgBstrNames,
            /* [in] */ UINT cMaxNames,
            /* [out] */ UINT *pcNames);
        
        STDMETHODIMP GetRefTypeOfImplType( 
            /* [in] */ UINT index,
            /* [out] */ HREFTYPE *pRefType);
        
        STDMETHODIMP GetImplTypeFlags( 
            /* [in] */ UINT index,
            /* [out] */ INT *pImplTypeFlags);
        
        STDMETHODIMP GetIDsOfNames( 
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [size_is][out] */ MEMBERID *pMemId);
        
        STDMETHODIMP Invoke( 
            /* [in] */ PVOID pvInstance,
            /* [in] */ MEMBERID memid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        STDMETHODIMP GetDocumentation( 
            /* [in] */ MEMBERID memid,
            /* [out] */ BSTR *pBstrName,
            /* [out] */ BSTR *pBstrDocString,
            /* [out] */ DWORD *pdwHelpContext,
            /* [out] */ BSTR *pBstrHelpFile);
        
        STDMETHODIMP GetDllEntry( 
            /* [in] */ MEMBERID memid,
            /* [in] */ INVOKEKIND invKind,
            /* [out] */ BSTR *pBstrDllName,
            /* [out] */ BSTR *pBstrName,
            /* [out] */ WORD *pwOrdinal);
        
        STDMETHODIMP GetRefTypeInfo( 
            /* [in] */ HREFTYPE hRefType,
            /* [out] */ ITypeInfo **ppTInfo);
        
        STDMETHODIMP AddressOfMember( 
            /* [in] */ MEMBERID memid,
            /* [in] */ INVOKEKIND invKind,
            /* [out] */ PVOID *ppv);
        
        STDMETHODIMP CreateInstance( 
            /* [in] */ IUnknown *pUnkOuter,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ PVOID *ppvObj);
        
        STDMETHODIMP GetMops( 
            /* [in] */ MEMBERID memid,
            /* [out] */ BSTR *pBstrMops);
        
        STDMETHODIMP GetContainingTypeLib( 
            /* [out] */ ITypeLib **ppTLib,
            /* [out] */ UINT *pIndex);
        
        STDMETHODIMP_(void) ReleaseTypeAttr( 
            /* [in] */ TYPEATTR *pTypeAttr);
        
        STDMETHODIMP_(void) STDMETHODCALLTYPE ReleaseFuncDesc( 
            /* [in] */ FUNCDESC *pFuncDesc);
        
        STDMETHODIMP_(void) STDMETHODCALLTYPE ReleaseVarDesc( 
            /* [in] */ VARDESC *pVarDesc);
        
      private:
        ITypeInfo*            m_pTypeInfo;      
    };
  } // of namespace COM
} // of namespace bvr20983

#endif //COTYPEINFO_H
