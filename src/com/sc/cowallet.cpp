/*
 * $Id$
 * 
 * CoWallet COM Object Class.
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
#include "com/sc/cowallet.h"
#include "util/logstream.h"
#include "util/comlogstream.h"


namespace bvr20983
{
  namespace COM
  {
    /**
     *
     */
#pragma warning(disable:4355)
    COWallet::COWallet(IUnknown* pUnkOuter) 
      : CODispatch(pUnkOuter,(IWallet*)this,LIBID_BVR20983_1_SC,IID_IWallet,CLSID_Wallet)
    { }
#pragma warning(default:4355)

    /*
     * COWallet::get_Balance
     *
     * Purpose:
     *   get the actual wallet balance.
     *
     * Parameters:
     *   short* amount
     *
     * Return: 
     *   HRESULT
     */
    STDMETHODIMP COWallet::get_Balance(short* amount)
    { *amount = 42;
    
      LOGGER_DEBUG<<_T("COWallet::get_Balance. Called. ")<<endl;
    
      return NOERROR;
    }
    
    /*
     * COWallet::Credit
     *
     * Purpose:
     *   get money from wallet.
     *
     * Parameters:
     *   short amount
     *
     * Return: 
     *   HRESULT
     */
    STDMETHODIMP COWallet::Credit(short amount)
    { LOGGER_DEBUG<<_T("COWallet::Credit. Called. amount=")<<amount<<endl;
    
      return NOERROR;
    }
    
    /*
     * COWallet::Debit
     *
     * Purpose:
     *   get money from wallet.
     *
     * Parameters:
     *   short amount
     *
     * Return: 
     *   HRESULT
     */
    STDMETHODIMP COWallet::Debit(short amount)
    { LOGGER_DEBUG<<_T("COWallet::Debit. Called. amount=")<<amount<<endl;
    
      return NOERROR;
    }
    
    /*
     * COWallet::Verify
     *
     * Purpose:
     *   verify cardholder by asking for a pin.
     *
     * Parameters:
     *   BSTR pin    PersonalIdentificationNumber
     *
     * Return: 
     *   HRESULT
     */
    STDMETHODIMP COWallet::Verify(BSTR pin)
    { LOGGER_DEBUG<<_T("COWallet::Verify. Called. pin=")<<pin<<endl;
    
      return NOERROR;
    }
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
