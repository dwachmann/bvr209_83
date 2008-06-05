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
#if !defined(COSMARTCARD_H)
#define COSMARTCARD_H

#include "com/codispatch.h"
#include "sc/sctypes.h"
#include "sc/smartcard.h"
#include "util/criticalsection.h"
#include "util/thread.h"
#include "ibvr20983.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace COM
  { class CoCollection;

    class COSmartcard : public CODispatch, public ISmartcard, public ThreadIF
    {
      public:
        // Main Object Constructor & Destructor.
        static HRESULT Create(REFCLSID clsid,LPUNKNOWN* ppv,LPUNKNOWN pUnkOuter=NULL);
        virtual ~COSmartcard();

        DECLARE_UNKNOWN

        // ISmartcard methods
        DECLARE_DISPATCH
        STDMETHODIMP get_ATR(IUnknown **ppATR);
        STDMETHODIMP get_CardPresent(VARIANT_BOOL *pIsCardPresent);
        STDMETHODIMP get_Applications(IUnknown **retval);
        STDMETHODIMP get_RegisteredCards(IUnknown **retval);
        STDMETHODIMP get_Readers(IUnknown **retval);
        STDMETHODIMP Attribute(Attributes attrib,BSTR *pAttribute);
        STDMETHODIMP Feature(Features feature,VARIANT_BOOL *pHasFeature);
        STDMETHODIMP WaitForInsertEvent(unsigned short timeInSeconds);
        STDMETHODIMP WaitForRemoveEvent(unsigned short timeInSeconds);

        STDMETHODIMP OutputText(BSTR text,byte timeInSeconds)
        { return E_NOTIMPL; }

        STDMETHODIMP InputText(BSTR text,byte timeInSeconds)
        { return E_NOTIMPL; }

        STDMETHODIMP Transmit(IUnknown *pAPDU,DWORD* pCmdNo,VARIANT_BOOL async,VARIANT_BOOL genMac)
        { return E_NOTIMPL; }

      private:
        COSmartcard(IUnknown* pUnkOuter);

        // Event methods
        void    OnInserted();
        void    OnRemoved();

        // Thread methods
        bool    InitThread();
        void    ExitThread(HRESULT hr);
        void    RunThread(HANDLE hWaitEvent);
    
        CoCollection*    m_pApplications;
        CoCollection*    m_pRegisteredCards;
        CoCollection*    m_pReaders;
        Thread           m_workingThread;

        CriticalSection  m_SmartCardCS;
        Smartcard*       m_pSmartCard;

        enum Events
        { Event_OnInserted = 0,
          Event_OnRemoved  = 1,
          Event_OnReceived = 2
        };

        static COEventInfo m_eventInfo[];
    }; // of class COSmartcard
  } // of namespace COM
} // of namespace bvr20983
#endif // COSMARTCARD_H
