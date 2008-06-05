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
#if !defined(COLED_H)
#define COLED_H

#include "com/cocontrol.h"
#include "win/ledwnd.h"
#include "ibvr20983cc.h"


using namespace bvr20983;

namespace bvr20983
{
  namespace COM
  { 
    class COLED : public COControl, public ILED, public ILEDStatus
    {
      public:
        // Main Object Constructor & Destructor.
        static HRESULT Create(REFCLSID clsid,LPUNKNOWN* ppv,LPUNKNOWN pUnkOuter=NULL);
        virtual ~COLED();

        DECLARE_UNKNOWN

        // ISmartcard methods
        DECLARE_DISPATCH

        STDMETHODIMP get_Text(BSTR *pText);
        STDMETHODIMP put_Text(BSTR text);

        STDMETHODIMP get_BackgroundColor(OLE_COLOR *);
        STDMETHODIMP put_BackgroundColor(OLE_COLOR);

        STDMETHODIMP get_ForegroundColor(OLE_COLOR *);
        STDMETHODIMP put_ForegroundColor(OLE_COLOR);

        STDMETHODIMP get_FillColor(OLE_COLOR *);
        STDMETHODIMP put_FillColor(OLE_COLOR);

        STDMETHODIMP get_Clock(VARIANT_BOOL *pClock);
        STDMETHODIMP put_Clock(VARIANT_BOOL clock);
        
        STDMETHODIMP get_Suppress0(VARIANT_BOOL *pSuppress0);
        STDMETHODIMP put_Suppress0(VARIANT_BOOL suppress0);

        STDMETHODIMP get_Display24Hour(VARIANT_BOOL *pDisplay24Hour);
        STDMETHODIMP put_Display24Hour(VARIANT_BOOL display24Hour);

        STDMETHODIMP get_Display16Segment(VARIANT_BOOL *pDisplay16Segment);
        STDMETHODIMP put_Display16Segment(VARIANT_BOOL display16Segment);

        STDMETHODIMP get_Run(VARIANT_BOOL *pIsRunning);
        STDMETHODIMP put_Run(VARIANT_BOOL shouldRun);

        STDMETHODIMP get_ReadyState(VARIANT* pIsReady);

        STDMETHODIMP_(void) Refresh();
        STDMETHODIMP AboutBox();
        STDMETHODIMP PropertyPages();

      private:
        COLED(IUnknown* pUnkOuter);

        HRESULT LoadProperties();
        HRESULT InitializeControl();

        STDMETHODIMP OnDraw(DWORD dvAspect, HDC hdcDraw, LPCRECTL prcBounds, LPCRECTL prcWBounds, HDC hicTargetDev, BOOL fOptimize);
        STDMETHODIMP GetNaturalExtent(DWORD dwAspect, LONG lindex, DVTARGETDEVICE* ptd, HDC hicTargetDev, DVEXTENTINFO* pExtentInfo, LPSIZEL psizel);

        virtual bool CreateInPlaceWindow(HWND hWndParent,const RECT& rcPos);

        virtual void OnLEDStarted();
        virtual void OnLEDChanged();
        virtual void OnLEDStopped();

        enum Events
        { Event_OnStarted        = 0,
          Event_OnStopped        = 1,
          Event_OnDirty          = 2,
          Event_ReadyStateChange = 3
        };

        static COEventInfo m_eventInfo[];
    }; // of class COLED
  } // of namespace COM
} // of namespace bvr20983
#endif // COLED_H
