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
#if !defined(COATR_H)
#define COATR_H

#include "sc/atr.h"
#include "com/codispatch.h"
#include "ibvr20983.h"

namespace bvr20983
{
  namespace COM
  { 
    class COATR : public CODispatch, public IATR
    {
      public:
        // Main Object Constructor & Destructor.
        COATR(IUnknown* pUnkOuter=NULL);

        DECLARE_UNKNOWN

        // IATR methods.
        DECLARE_DISPATCH
        STDMETHODIMP get_Raw(BSTR *pRaw);
        STDMETHODIMP get_History(BSTR *pHistory);
        STDMETHODIMP get_F(DWORD* pF);
        STDMETHODIMP get_FMax(DWORD* pFMax);
        STDMETHODIMP get_D(DWORD* pD);

        void SetATR(const ATR& atr)
        { m_atr = atr; }

      private:
        ATR m_atr;
    };
  } // of namespace COM
} // of namespace bvr20983
#endif // COATR_H
