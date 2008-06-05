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
#if !defined(COWALLET_H)
#define COWALLET_H

#include "com/codispatch.h"
#include "ibvr20983.h"

namespace bvr20983
{
  namespace COM
  { class COMServer;

    class COWallet : public CODispatch, public IWallet
    {
      public:
        COWallet(IUnknown* pUnkOuter=NULL);

        DECLARE_UNKNOWN

        // IWallet methods
        DECLARE_DISPATCH
        STDMETHODIMP get_Balance (short* amount);
        STDMETHODIMP Credit      (short amount);
        STDMETHODIMP Debit       (short amount);
        STDMETHODIMP Verify      (BSTR pin);
    }; // of class COWallet
  } // of namespace COM
} // of namespace bvr20983
#endif // COWALLET_H
