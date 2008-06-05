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
#if !defined(COENUMVARIANT_H)
#define COENUMVARIANT_H

#include "sc/sctypes.h"

namespace bvr20983
{
  namespace COM
  {
    class COEnumVariant : public IEnumVARIANT
    {
      public:
        // IUnknown methods 
        STDMETHODIMP QueryInterface(REFIID riid, LPVOID* ppvObj);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
    
        // IEnumVARIANT methods 
        STDMETHODIMP Next(ULONG cElements, VARIANT* pvar, ULONG* pcElementFetched);
        STDMETHODIMP Skip(ULONG cElements);
        STDMETHODIMP Reset();
        STDMETHODIMP Clone(IEnumVARIANT** ppenum);
        
        static HRESULT Create(SAFEARRAY*,IUnknown**);
          
      private:
        COEnumVariant();
        ~COEnumVariant();
    
        ULONG      m_cRef;      // Reference count                                 
        ULONG      m_cElements; // Number of elements in enumerator. 
        long       m_lLBound;   // Lower bound of index.
        long       m_lCurrent;  // Current index.
        SAFEARRAY* m_psa;       // Safe array that holds elements.
    
    };  
  } // of namespace COM
} // of namespace bvr20983

#endif // COENUMVARIANT_H
