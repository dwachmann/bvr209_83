/*
 * $Id$
 * 
 * COATR COM Object Class.
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
#include "com/sc/coatr.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace COM
  {
#pragma region Construction & Deconstruction
#pragma warning(disable:4355)
    /**
     *
     */
    COATR::COATR(IUnknown* pUnkOuter) 
      : CODispatch(pUnkOuter,(IATR*)this,LIBID_BVR20983,IID_IATR,CLSID_ATR)
    { }
#pragma warning(default:4355)
#pragma endregion

#pragma region IATR
    /**
     *
     */
    STDMETHODIMP COATR::get_Raw(BSTR *pRaw)
    { HRESULT hr = NOERROR;
    
      if( NULL==pRaw )
        hr = E_POINTER;
    
      return hr;
    } // of COATR::get_Raw()

    /**
     *
     */
    STDMETHODIMP COATR::get_History(BSTR *pHistory)
    { HRESULT hr = NOERROR;
    
      if( NULL==pHistory )
        hr = E_POINTER;
      else
      { ByteBuffer h;
        
        m_atr.GetHistory(h);

        h.GetBSTR(pHistory);
      } // of else
    
      return hr;
    } // of COATR::get_History()

    /**
     *
     */
    STDMETHODIMP COATR::get_F(DWORD* pF)
    { HRESULT hr = NOERROR;
    
      if( NULL==pF )
        hr = E_POINTER;
    
      return hr;
    } // of COATR::get_F()

    /**
     *
     */
    STDMETHODIMP COATR::get_FMax(DWORD* pFMax)
    { HRESULT hr = NOERROR;
    
      if( NULL==pFMax )
        hr = E_POINTER;
    
      return hr;
    } // of COATR::get_FMax()

    /**
     *
     */
    STDMETHODIMP COATR::get_D(DWORD* pD)
    { HRESULT hr = NOERROR;
    
      if( NULL==pD )
        hr = E_POINTER;
    
      return hr;
    } // of COATR::get_D()
#pragma endregion
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
