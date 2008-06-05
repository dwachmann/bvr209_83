/*
 * $Id$
 * 
 * COLED COM Control Class.
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
#include "com/ocx/coled.h"
#include "com/comserver.h"
#include "win/dialog.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "exception/bvr20983exception.h"
#include "bvr20983cc-dispid.h"
#include "bvr20983cc-res.h"
#include "bvr20983cc.h"

namespace bvr20983
{
  namespace COM
  {

     /**
      *
      */
     COEventInfo COLED::m_eventInfo[] =
     { { DIID_DLEDEvent,DISPID_ONSTARTED,0,NULL },
       { DIID_DLEDEvent,DISPID_ONSTOPPED,0,NULL },
       { DIID_DLEDEvent,DISPID_ONDIRTY,0,NULL },
       { DIID_DLEDEvent,DISPID_READYSTATECHANGE,0,NULL }
     };

#pragma region Construction & Deconstruction
     /**
      *
      */
     HRESULT COLED::Create(REFCLSID clsid,LPUNKNOWN* ppv,LPUNKNOWN pUnkOuter)
     { HRESULT hr = NOERROR;
 
       if( NULL==ppv )
         hr = E_POINTER;
       else
       { COLED* pCOLED = new COLED(pUnkOuter);
 
         if( NULL==pCOLED )
           hr = E_OUTOFMEMORY;
         else
           *ppv = pCOLED->PrivateUnknown();
       } // of else

       return hr;
     } // of COLED::Create()

     /**
      *
      */
#pragma warning(disable:4355)
     COLED::COLED(IUnknown* pUnkOuter) 
      : COControl(_T("LEDCOControl"),
                  pUnkOuter,
                  (ILED*)this,
                  LIBID_BVR20983CC,
                  IID_ILED,
                  CLSID_LED,DIID_DLEDEvent
                 )
    { 
      GetPropertySet().AddPropertyName(_T("16segment"));
      GetPropertySet().AddPropertyName(_T("clock"));
      GetPropertySet().AddPropertyName(_T("24hour"));
      GetPropertySet().AddPropertyName(_T("suppress0"));
      GetPropertySet().AddPropertyName(_T("text"));
      GetPropertySet().AddPropertyName(_T("fgcolor"));
      GetPropertySet().AddPropertyName(_T("bgcolor"));
      GetPropertySet().AddPropertyName(_T("fillcolor"));

      GetPropertySet().AddPropertyName(_T("transparent"));
      GetPropertySet().AddPropertyName(_T("windowless"));
      GetPropertySet().AddPropertyName(_T("usethread"));
      GetPropertySet().AddPropertyName(_T("drawdirectinthread"));

      InitializeControl();
    } // of COLED::COLED()
#pragma warning(default:4355)
    
     /**
      *
      */
    COLED::~COLED()
    { }
#pragma endregion

    /**
     *
     */
    bool COLED::CreateInPlaceWindow(HWND hWndParent,const RECT& rcPos)
    { bool            result      = true;
      bool            clock       = false;
      bool            _24hour     = false;
      bool            suppress0   = false;
      bool            _16segment  = false;
      bool            windowless  = false;
      bool            usethread   = false;
      bool            directdraw  = false;
      bool            transparent = false;
      OLE_COLOR       bgColor     = WND_NULLCOLOR;
      OLE_COLOR       fgColor     = RGB(0,128,0);
      OLE_COLOR       fillColor   = WND_NULLCOLOR;
      int             wndStyle    = WS_VISIBLE | WS_CHILD | WS_TABSTOP;

      GetPropertySet().Get(_T("windowless"),windowless);
      GetPropertySet().Get(_T("transparent"),transparent);
      GetPropertySet().Get(_T("usethread"),usethread);
      GetPropertySet().Get(_T("drawdirectinthread"),directdraw);
      GetPropertySet().Get(_T("clock"),clock);
      GetPropertySet().Get(_T("24hour"),_24hour);
      GetPropertySet().Get(_T("suppress0"),suppress0);
      GetPropertySet().Get(_T("16segment"),_16segment);

      if( clock      ) wndStyle |= LEDS_CLOCK;
      if( _24hour    ) wndStyle |= LEDS_24HOUR;
      if( suppress0  ) wndStyle |= LEDS_SUPPRESS;
      if( _16segment ) wndStyle |= LEDS_16SEGEGMENT;

      windowless = m_myControlClass->m_isWindowless && windowless;

      LEDCREATESTRUCT ledCS;

      ::memset(&ledCS,'\0',sizeof(ledCS));
      ledCS.magic              = LEDWNDMAGIC;
      ledCS.dwSize             = sizeof(LEDCREATESTRUCT);

      ledCS.bgColor            = WND_NULLCOLOR;
      ledCS.fgColor            = WND_NULLCOLOR;
      ledCS.fgColor1           = WND_NULLCOLOR;

      if( bgColor!=bgColor )   ::OleTranslateColor(bgColor,NULL,&ledCS.bgColor);
      if( fgColor!=bgColor )   ::OleTranslateColor(fgColor,NULL,&ledCS.fgColor);
      if( fillColor!=bgColor ) ::OleTranslateColor(fillColor,NULL,&ledCS.fgColor1);

      ledCS.initialStart       = false;
      ledCS.useThread          = usethread;
      ledCS.drawDirectInThread = directdraw;

      m_size.cx = rcPos.right  - rcPos.left;
      m_size.cy = rcPos.bottom - rcPos.top;

      OLE_COLOR    olecolor;

      if( transparent && windowless )
        ledCS.bgColor = WND_NULLCOLOR;
      else if( ledCS.bgColor==WND_NULLCOLOR && GetAmbientProperty(DISPID_AMBIENT_BACKCOLOR,VT_I4,&olecolor) )
        ::OleTranslateColor(olecolor,NULL,&ledCS.bgColor);

      if( ledCS.fgColor==WND_NULLCOLOR && GetAmbientProperty(DISPID_AMBIENT_FORECOLOR,VT_I4,&olecolor) )
        ::OleTranslateColor(olecolor,NULL,&ledCS.fgColor);

      ledCS.pStatusCB = (ILEDStatus*)this;

      m_pWnd = Wnd::Construct(LEDWNDCLS,_T("coled"),0,
                              wndStyle,
                              rcPos.left,rcPos.top,
                              m_size.cx, m_size.cy,
                              hWndParent,
                              &ledCS,
                              NULL,
                              COMServer::GetInstanceHandle(),
                              m_pClientSite,
                              windowless
                             );

      result = m_pWnd!=NULL;

      if( NULL!=m_pWnd )
      { LEDWnd* pWnd = (LEDWnd*)m_pWnd;

        if( pWnd->IsWindowless() && !usethread )
          pWnd->SetUseThread(true);
      } // of if

      if( result && NULL!=m_pClientSite )
        m_pClientSite->ShowObject();

      PropertyNotifyOnChanged(DISPID_READYSTATE);
      TriggerEvent(&m_eventInfo[Event_ReadyStateChange]);

      return result;
    } // of COLED::CreateInPlaceWindow()

#pragma region ILED
    /**
     *
     */
    STDMETHODIMP COLED::get_Text(BSTR *pText)
    { HRESULT hr = NOERROR;

      if( NULL==pText )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else 
      { try
        { TString s;

          if( NULL!=m_pWnd )
          { ((LEDWnd*)m_pWnd)->GetText(s); 
            
            *pText = ::SysAllocString(s.c_str());
          } // of if
          else
            GetPropertySet().Get(_T("text"),pText); 
        }
        catch(const BVR20983Exception& e)
        { LOGGER_ERROR<<e<<endl;
          
          hr = e.GetErrorCode();
        }
        catch(...)
        { hr = Exception(BVR_SOURCE_LED,E_FAIL); }
      } // of else

      return hr;
    } // of COLED::get_Text()

    /**
     *
     */
    STDMETHODIMP COLED::put_Text(BSTR text)
    { HRESULT hr = NOERROR;

      try
      { GetPropertySet().Add(_T("text"),text);

        if( NULL!=m_pWnd )
          ((LEDWnd*)m_pWnd)->SetText(text); 

        PropertyNotifyOnChanged(DISPID_TEXT);
      }
      catch(const BVR20983Exception& e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_LED,E_FAIL); }

      return hr;
    } // of COLED::put_Text()

    /**
     *
     */
    STDMETHODIMP COLED::get_BackgroundColor(OLE_COLOR* pColor)
    { HRESULT hr = NOERROR;

      if( NULL==pColor )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else 
        hr = GetPropertySet().Get(_T("bgcolor"),*pColor) ? NOERROR : E_FAIL;

      return hr;
    } // of COLED::get_BackgroundColor()

    /**
     *
     */
    STDMETHODIMP COLED::put_BackgroundColor(OLE_COLOR color)
    { HRESULT hr = NOERROR;

      if( NULL!=m_pWnd )
      { try
        { GetPropertySet().Add(_T("bgcolor"),color);

          if( NULL!=m_pWnd )
          { COLORREF c;

            ::OleTranslateColor(color,NULL,&c);
            ((LEDWnd*)m_pWnd)->SetBackgroundColor(c,true); 
          } // of if

          PropertyNotifyOnChanged(DISPID_BACKCOLOR);
        }
        catch(const BVR20983Exception& e)
        { LOGGER_ERROR<<e<<endl;
          
          hr = e.GetErrorCode();
        }
        catch(...)
        { hr = Exception(BVR_SOURCE_LED,E_FAIL); }
      } // of else

      return hr;
    } // of COLED::put_BackgroundColor()

    /**
     *
     */
    STDMETHODIMP COLED::get_ForegroundColor(OLE_COLOR* pColor)
    { HRESULT hr = NOERROR;

      if( NULL==pColor )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else 
        hr = GetPropertySet().Get(_T("fgcolor"),*pColor) ? NOERROR : E_FAIL;

      return hr;
    } // of COLED::get_ForegroundColor()

    /**
     *
     */
    STDMETHODIMP COLED::put_ForegroundColor(OLE_COLOR color)
    { HRESULT hr = NOERROR;

      if( NULL!=m_pWnd )
      { try
        { GetPropertySet().Add(_T("fgcolor"),color);

          if( NULL!=m_pWnd )
          { COLORREF c;

            ::OleTranslateColor(color,NULL,&c);
            ((LEDWnd*)m_pWnd)->SetForegroundColor(c,true); 
          } // of if
        }
        catch(const BVR20983Exception& e)
        { LOGGER_ERROR<<e<<endl;
          
          hr = e.GetErrorCode();
        }
        catch(...)
        { hr = Exception(BVR_SOURCE_LED,E_FAIL); }
      } // of else

      return hr;
    } // of COLED::put_ForegroundColor()

    /**
     *
     */
    STDMETHODIMP COLED::get_FillColor(OLE_COLOR* pColor)
    { HRESULT hr = NOERROR;

      if( NULL==pColor )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else 
        hr = GetPropertySet().Get(_T("fillcolor"),*pColor) ? NOERROR : E_FAIL;

      return hr;
    } // of COLED::get_FillColor()

    /**
     *
     */
    STDMETHODIMP COLED::put_FillColor(OLE_COLOR color)
    { HRESULT hr = NOERROR;

      if( NULL!=m_pWnd )
      { try
        { GetPropertySet().Add(_T("fillcolor"),color);

          if( NULL!=m_pWnd )
          { COLORREF c;

            ::OleTranslateColor(color,NULL,&c);
            ((LEDWnd*)m_pWnd)->SetForegroundColor1(c,true); 
          } // of if
        }
        catch(const BVR20983Exception& e)
        { LOGGER_ERROR<<e<<endl;
          
          hr = e.GetErrorCode();
        }
        catch(...)
        { hr = Exception(BVR_SOURCE_LED,E_FAIL); }
      } // of else

      return hr;
    } // of COLED::put_FillColor()

    /**
     *
     */
    STDMETHODIMP COLED::get_Clock(VARIANT_BOOL *pClock)
    { HRESULT hr = NOERROR;

      if( NULL==pClock )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else
        hr = GetPropertySet().Get(_T("clock"),pClock) ? NOERROR : E_FAIL;

      return hr;
    } // of COLED::get_Clock()

    /**
     *
     */
    STDMETHODIMP COLED::put_Clock(VARIANT_BOOL clock)
    { HRESULT hr = NOERROR;

      try
      { GetPropertySet().Add(_T("clock"),clock==VARIANT_TRUE);
        
        if( NULL!=m_pWnd )
          ((LEDWnd*)m_pWnd)->SetClock( clock==VARIANT_TRUE ); 

        PropertyNotifyOnChanged(DISPID_LED_CLOCK);
      }
      catch(const BVR20983Exception& e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_LED,E_FAIL); }

      return hr;
    } // of COLED::put_Clock()


    /**
     *
     */
    STDMETHODIMP COLED::get_Suppress0(VARIANT_BOOL *pSuppress0)
    { HRESULT hr = NOERROR;

      if( NULL==pSuppress0 )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else
        hr = GetPropertySet().Get(_T("suppress0"),pSuppress0) ? NOERROR : E_FAIL;

      return hr;
    } // of COLED::get_Suppress0()

    /**
     *
     */
    STDMETHODIMP COLED::put_Suppress0(VARIANT_BOOL suppress0)
    { HRESULT hr = NOERROR;

      try
      { GetPropertySet().Add(_T("suppress0"),suppress0==VARIANT_TRUE);
        
        if( NULL!=m_pWnd )
          ((LEDWnd*)m_pWnd)->SetZeroSuppress( suppress0==VARIANT_TRUE ); 
      }
      catch(const BVR20983Exception& e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_LED,E_FAIL); }

      return hr;
    } // of COLED::put_Suppress0()

    /**
     *
     */
    STDMETHODIMP COLED::get_Display24Hour(VARIANT_BOOL *pDisplay24Hour)
    { HRESULT hr = NOERROR;

      if( NULL==pDisplay24Hour )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else
        hr = GetPropertySet().Get(_T("24hour"),pDisplay24Hour) ? NOERROR : E_FAIL;

      return hr;
    } // of COLED::get_Display24Hour()

    /**
     *
     */
    STDMETHODIMP COLED::put_Display24Hour(VARIANT_BOOL display24Hour)
    { HRESULT hr = NOERROR;

      try
      { GetPropertySet().Add(_T("24hour"),display24Hour==VARIANT_TRUE);

        if( NULL!=m_pWnd )
          ((LEDWnd*)m_pWnd)->Set24Hour( display24Hour==VARIANT_TRUE ); 
      }
      catch(const BVR20983Exception& e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_LED,E_FAIL); }

      return hr;
    } // of COLED::put_Display24Hour()

    /**
     *
     */
    STDMETHODIMP COLED::get_Display16Segment(VARIANT_BOOL *pDisplay16Segment)
    { HRESULT hr = NOERROR;

      if( NULL==pDisplay16Segment )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else 
        hr = GetPropertySet().Get(_T("16segment"),pDisplay16Segment) ? NOERROR : E_FAIL;

      return hr;
    } // of COLED::get_Display16Segment()

    /**
     *
     */
    STDMETHODIMP COLED::put_Display16Segment(VARIANT_BOOL display16Segment)
    { HRESULT hr = NOERROR;

      try
      { GetPropertySet().Add(_T("16segment"),display16Segment==VARIANT_TRUE);

        if( NULL!=m_pWnd )
          ((LEDWnd*)m_pWnd)->Set16Segment( display16Segment==VARIANT_TRUE );
      } 
      catch(const BVR20983Exception& e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_LED,E_FAIL); }

      return hr;
    } // of COLED::put_Display16Segment()

    /**
     *
     */
    STDMETHODIMP COLED::get_Run(VARIANT_BOOL *pIsRunning)
    { HRESULT hr = NOERROR;

      if( NULL==pIsRunning )
        hr = Exception(BVR_SOURCE_LED,E_POINTER);
      else if( NULL!=m_pWnd ) 
      { try
        { *pIsRunning = ((LEDWnd*)m_pWnd)->IsRunning() ? VARIANT_TRUE : VARIANT_FALSE;
        }
        catch(const BVR20983Exception& e)
        { LOGGER_ERROR<<e<<endl;
          
          hr = e.GetErrorCode();
        }
        catch(...)
        { hr = Exception(BVR_SOURCE_LED,E_FAIL); }
      } // of else

      return hr;
    } // of COLED::get_Run()
    
    /**
     *
     */
    STDMETHODIMP COLED::put_Run(VARIANT_BOOL shouldRun)
    { HRESULT hr = NOERROR;

      if( NULL!=m_pWnd )
      { try
        { if( shouldRun==VARIANT_TRUE ) 
            ((LEDWnd*)m_pWnd)->Start(1000);
          else
            ((LEDWnd*)m_pWnd)->Stop();
        }
        catch(const BVR20983Exception& e)
        { LOGGER_ERROR<<e<<endl;
          
          hr = e.GetErrorCode();
        }
        catch(...)
        { hr = Exception(BVR_SOURCE_LED,E_FAIL); }
      } // of if
      else
        hr = E_FAIL;

      return hr;
    } // of COLED::put_Run()

    /**
     *
     */
    STDMETHODIMP COLED::get_ReadyState(VARIANT* pIsReady)
    { HRESULT hr = NOERROR;

      if( NULL==pIsReady )
        hr = E_POINTER;
      else
      { V_VT(pIsReady) = VT_I4;
        V_I4(pIsReady) = NULL==m_pWnd ? READYSTATE_LOADED : ( READYSTATE_COMPLETE );
      }

      LOGGER_DEBUG<<_T("COLED::get_ReadyState(*pIsReady=")<<(V_I4(pIsReady))<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COLED::get_ReadyState() 

    /**
     *
     */
    void COLED::Refresh()
    { if( NULL!=m_pWnd )
        m_pWnd->InvalidateRect(TRUE);
    } // of COLED::Refresh()

    /**
     *
     */
    HRESULT COLED::AboutBox()
    { win::Dialog dlg;
      HWND        hWndParent = NULL!=m_pWnd ? m_pWnd->GetWindowParent() : NULL;

      dlg.Show(MAKEINTRESOURCE(IDD_ABOUTBOX),m_pWnd->GetWindowHandle(),COMServer::GetInstanceHandle());

      return S_OK;
    } // of COLED::AboutBox()

    /**
     *
     */
    HRESULT COLED::PropertyPages()
    { return ShowPropertyPages(); }
#pragma endregion
    
#pragma region Events
    /**
     *
     */
    void COLED::OnLEDStarted()
    { TriggerEvent( &m_eventInfo[Event_OnStarted] ); }

    /**
     *
     */
    void COLED::OnLEDStopped()
    { TriggerEvent( &m_eventInfo[Event_OnStopped] ); }

    /**
     *
     */
    void COLED::OnLEDChanged()
    { TriggerEvent( &m_eventInfo[Event_OnDirty] ); }
#pragma endregion

#pragma region IPersist Overloaded
    /**
     *
     */
    HRESULT COLED::LoadProperties()
    { if( NULL!=m_pWnd )
      { // do something
      } // of if

      return S_OK;
    } // of COLED::LoadProperties()


    /**
     *
     */
    HRESULT COLED::InitializeControl()
    { GetPropertySet().Add(_T("16segment"),false);
      GetPropertySet().Add(_T("24hour"),true);
      GetPropertySet().Add(_T("clock"),true);
      GetPropertySet().Add(_T("suppress0"),false);
      GetPropertySet().Add(_T("transparent"),false);
      GetPropertySet().Add(_T("windowless"),false);
      GetPropertySet().Add(_T("usethread"),false);
      GetPropertySet().Add(_T("drawdirectinthread"),false);

      GetPropertySet().Add(_T("text"),_T("LED"));
      GetPropertySet().Add(_T("fgcolor"),RGB(255,0,0));
      GetPropertySet().Add(_T("bgcolor"),RGB(0,0,0));
      GetPropertySet().Add(_T("fillcolor"),WND_NULLCOLOR);
      
      return S_OK; 
    } // of COLED::InitializeControl()
#pragma endregion

    /**
     *
     */
    STDMETHODIMP COLED::GetNaturalExtent(DWORD           dvAspect,
                                         LONG            lIndex,
                                         DVTARGETDEVICE* ptd,
                                         HDC             hicTargetDevice,
                                         DVEXTENTINFO*   pExtentInfo,
                                         LPSIZEL         pSizel
                                        )
    { HRESULT hr = E_FAIL;

      if( NULL!=m_pWnd )
        hr = m_pWnd->GetNaturalExtent(dvAspect,pExtentInfo,pSizel);

      LOGGER_DEBUG<<_T("COLED::IViewObjectEx::GetNaturalExtent(dvAspect=")<<dvAspect<<_T(",pSizel=")<<pSizel<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr; 
    } // of COLED::GetNaturalExtent()


#pragma region OnDraw
    /**
     *
     */
    STDMETHODIMP COLED::OnDraw(DWORD dvAspect, 
                               HDC hdcDraw, 
                               LPCRECTL prcBounds, 
                               LPCRECTL prcWBounds, 
                               HDC hicTargetDev, 
                               BOOL fOptimize
                              )
    { HRESULT hr=NOERROR;

      DCBase dc(hdcDraw);

      if( NULL!=m_pWnd )
        m_pWnd->OnDraw(dc,prcBounds);

/*
      if( NULL!=prcBounds )
      { RECT r;
        r.left   = prcBounds->left;
        r.top    = prcBounds->top;
        r.right  = prcBounds->right;
        r.bottom = prcBounds->bottom;

        dc.SelectObject((HGDIOBJ)::GetSysColor(BLACK_PEN));
        dc.SelectObject((HGDIOBJ)::GetStockObject(NULL_BRUSH));
        dc.Rectangle(r.left,r.top,r.right,r.bottom);
      } // of if
*/
      return hr;
    } // of COLED::OnDraw()
#pragma endregion
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
