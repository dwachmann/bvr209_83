/*
 * $Id$
 * 
 * windows base class.
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
#include "win/wnd.h"
#include "win/wndclass.h"
#include "exception/windowsexception.h"
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"
#include "bvr20983.h"

void OutputDebugFmt(LPTSTR pszFmt,...);

using namespace bvr20983;

namespace bvr20983
{
  namespace win
  {
    int Wnd::m_cxChar = 0;
    int Wnd::m_cxCaps = 0;
    int Wnd::m_cyChar = 0;
    int Wnd::m_iXppli = 0;
    int Wnd::m_iYppli = 0;


#pragma region Construction & Deconstruction
    /**
     *
     */
    Wnd* WndFactory::Construct(HWND hWnd)
    { return new Wnd(hWnd); }

    /*
     *
     */
    Wnd::Wnd(HWND hWnd) :
      m_pSite(IID_IOleInPlaceSite)
    { m_cxChar              = 0;
      m_cxCaps              = 0;
      m_cyChar              = 0;
      m_hWnd                = hWnd;
      m_timerStarted        = false;
      m_isEnabled           = true;
      m_quitOnDestroy       = false;
      m_fUsingWindowRgn     = false; 
      m_hasFocus            = false;
      m_hRgn                = NULL;
      m_pInvalidRect        = NULL;
      m_size.cx             = 0;
      m_size.cy             = 0;

      ::memset(&m_invalidRect, 0, sizeof(m_invalidRect));
      ::memset(&m_rcLocation, 0, sizeof(m_rcLocation));
    } // of Wnd::Wnd()

    /*
     *
     */
    Wnd::~Wnd()
    { StopTimer();

      DELETE_GDIOBJECT(m_hRgn);
    } // of Wnd::~Wnd()
#pragma endregion

#pragma region Create
    /**
     *
     */
    Wnd* Wnd::Create(HWND hWnd,LPCREATESTRUCT lpCS)
    { Wnd* result = NULL;

      if( NULL!=lpCS )
      { WndFactory* pWndFactory = WndClass::GetWindowFactory(lpCS->lpszClass);

        if( NULL!=pWndFactory )
        { Wnd* w = pWndFactory->Construct(hWnd);

          if( NULL!=w )
          { if( NULL!=hWnd )
              ::SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)w);

            WndCreateStruct* pWndCS = (WndCreateStruct*)lpCS->lpCreateParams;

            if( NULL!=pWndCS && pWndCS->magic==WNDMAGIC && pWndCS->dwSize>=sizeof(WndCreateStruct) )
            { w->m_pSiteWndLess    = pWndCS->pSiteWndLess;
              w->m_pSiteEx         = pWndCS->pSiteEx;
              w->m_pSite           = pWndCS->pSite;

              lpCS->lpCreateParams = pWndCS->pCreateParams;
            } // of if

            if( NULL!=lpCS->lpszName )
              w->m_wndText = lpCS->lpszName;

            if( lpCS->style & WS_DISABLED )
              w->m_isEnabled = false;

            w->m_size.cx = (WORD)lpCS->cx;
            w->m_size.cy = (WORD)lpCS->cy;

            InitStatic();

            result = w;
          } // of if
        } // of if
      } // of if

      return result;
    } // of Wnd::Create()

    /**
     *
     */
    void Wnd::InitStatic()
    { if( Wnd::m_cxChar==0 )
      { DC dc(NULL);

        TEXTMETRIC tm;

        dc.GetTextMetrics(&tm);

        Wnd::m_cxChar = tm.tmAveCharWidth;
        Wnd::m_cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * m_cxChar / 2;
        Wnd::m_cyChar = tm.tmHeight + tm.tmExternalLeading;
        Wnd::m_iXppli = dc.GetDeviceCaps(LOGPIXELSX);
        Wnd::m_iYppli = dc.GetDeviceCaps(LOGPIXELSY);
      } // of if
    } // of Wnd::InitStatic()

    /**
     *
     */
    Wnd* Wnd::Construct(LPCTSTR       wndCls,
                        LPCTSTR       windowName,
                        int           exStyle,
                        int           style,
                        int           x,
                        int           y,
                        int           width,
                        int           height,
                        HWND          hWndParent,
                        LPVOID        lpParam,
                        LPCTSTR       menuName,
                        HINSTANCE     hInstance,
                        LPCLIENTSITE  pClientSite,
                        bool          shouldAsk4WndLess
                       ) 
    { HMENU        hMenu = NULL;
      CREATESTRUCT cs;
      
      ::memset(&cs,'\0',sizeof(cs));
    
      if( NULL!=menuName )
        hMenu = ::LoadMenu(hInstance,menuName);

      cs.cx             = width;
      cs.cy             = height;
      cs.x              = x;
      cs.y              = y;
      cs.dwExStyle      = exStyle;
      cs.style          = style;
      cs.hInstance      = hInstance;
      cs.hMenu          = hMenu;
      cs.hwndParent     = hWndParent;
      cs.lpCreateParams = lpParam;
      cs.lpszName       = windowName;
      cs.lpszClass      = wndCls;
      
      return Construct(&cs,pClientSite,shouldAsk4WndLess); 
    } // of Wnd::Construct()

    /**
     *
     */
    Wnd* Wnd::Construct(LPCREATESTRUCT lpCS,
                        LPCLIENTSITE   pClientSite,
                        bool           shouldAsk4WndLess
                       ) 
    { Wnd*            result = NULL;
      WndCreateStruct wndCS;
      BOOL            fNoRedraw = FALSE;
      
      ::memset(&wndCS,'\0',sizeof(wndCS));

      wndCS.magic  = 0x774e446d;
      wndCS.dwSize = sizeof(WndCreateStruct);

      if( NULL!=pClientSite )
      { pClientSite->QueryInterface(IID_IOleInPlaceSite          ,(LPVOID*)&wndCS.pSite);
        pClientSite->QueryInterface(IID_IOleInPlaceSiteEx        ,(LPVOID*)&wndCS.pSiteEx);

        if( shouldAsk4WndLess )
          pClientSite->QueryInterface(IID_IOleInPlaceSiteWindowless,(LPVOID*)&wndCS.pSiteWndLess);

          if( NULL!=wndCS.pSiteWndLess && FAILED(wndCS.pSiteWndLess->CanWindowlessActivate()) )
            RELEASE_INTERFACE(wndCS.pSiteWndLess);
      } // of if

      wndCS.pCreateParams  = lpCS->lpCreateParams;
      lpCS->lpCreateParams = &wndCS;

      if( NULL!=wndCS.pSiteEx )
        wndCS.pSiteEx->OnInPlaceActivateEx(&fNoRedraw,NULL!=wndCS.pSiteWndLess ? ACTIVATE_WINDOWLESS : 0);
      else if( NULL!=wndCS.pSite )
        wndCS.pSite->OnInPlaceActivate();

      if( NULL!=wndCS.pSiteWndLess )
      { result = Wnd::Create(NULL,lpCS);

        if( NULL!=result )
        { result->OnCreate(lpCS);
          result->OnSize(lpCS->cx,lpCS->cy);
        } // of if
      } // of if
      else
      { HWND hWnd = ::CreateWindowEx(lpCS->dwExStyle,lpCS->lpszClass,lpCS->lpszName,
                                     lpCS->style,
                                     lpCS->x,lpCS->y,lpCS->cx,lpCS->cy,
                                     lpCS->hwndParent,lpCS->hMenu,
                                     lpCS->hInstance,lpCS->lpCreateParams
                                    );

        THROW_LASTERROREXCEPTION1(hWnd);

        result = (Wnd*) ::GetWindowLongPtr(hWnd,GWLP_USERDATA);
      } // of else

      RELEASE_INTERFACE(wndCS.pSite);
      RELEASE_INTERFACE(wndCS.pSiteEx);
      RELEASE_INTERFACE(wndCS.pSiteWndLess);

      return result;
    } // of Wnd::Construct()

    /**
     *
     */
    void Wnd::OnCreate(LPCREATESTRUCT lpCS)
    { if( NULL!=m_hWnd )
      { RECT clientRect;

        ::GetClientRect(m_hWnd,&clientRect);

        if( m_size.cx==CW_USEDEFAULT || m_size.cx!=clientRect.right )
          m_size.cx = clientRect.right;

        if( m_size.cy==CW_USEDEFAULT || m_size.cy!=clientRect.bottom )
          m_size.cy = (WORD)clientRect.bottom;
      } // of if
    } // of Wnd::OnCreate()

    /**
     *
     */
    void Wnd::DestroyWindow()
    { if( NULL!=m_hWnd ) 
        ::DestroyWindow(m_hWnd); 
      else
        OnDestroy(true);
    } // of Wnd::DestroyWindow()
#pragma endregion

#pragma region Implementation
    /**
     *
     */
    bool Wnd::SetExtent(SIZEL* psizel)
    { bool    result = true;
      SIZEL   sl;
      HRESULT hr;

      HiMetricToPixel(psizel, &sl);

      if( OnSetExtent(&sl) )
      { if( NULL!=m_hWnd ) 
          ::SetWindowPos(m_hWnd, NULL, 0, 0, sl.cx,sl.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
        else if( !m_pSiteWndLess.IsNULL() )
        { OnSize((WORD)sl.cx,(WORD)sl.cy);

          hr = m_pSiteWndLess->OnPosRectChange(&m_rcLocation);
        } // of else if
      } // of if
      else 
        result = false;

      return result;
    } // of Wnd::SetExtent()

    /**
     *
     */
    bool Wnd::OnSetExtent(const SIZEL* pSizeL)
    { return true; }

    /**
     *
     */
    void Wnd::GetExtent(SIZEL* pSizeLOut) const
    { PixelToHiMetric(&m_size, pSizeLOut); }

    /**
     *
     */
    bool Wnd::SetObjectRects(LPCRECT prcPos,LPCRECT prcClip)
    { bool result = true;
      bool fRemoveWindowRgn;
  
      if( NULL!=m_hWnd ) 
      { fRemoveWindowRgn = m_fUsingWindowRgn;

        if( prcClip ) 
        { RECT rcIXect;

          if( ::IntersectRect(&rcIXect, prcPos, prcClip) ) 
          { if( !::EqualRect(&rcIXect, prcPos) ) 
            { ::OffsetRect(&rcIXect, -(prcPos->left), -(prcPos->top));

              HRGN tempRgn = ::CreateRectRgnIndirect(&rcIXect);
              ::SetWindowRgn(m_hWnd, tempRgn, TRUE);

              if( m_hRgn!=NULL )
                ::DeleteObject(m_hRgn);

              m_hRgn = tempRgn;
              m_fUsingWindowRgn = true;
              fRemoveWindowRgn  = false;
            } // of if
          } // of if
        } // of if

        if( fRemoveWindowRgn ) 
        { ::SetWindowRgn(m_hWnd, NULL, TRUE);
          if( NULL!=m_hRgn )
          { ::DeleteObject(m_hRgn);
            m_hRgn = NULL;
          } // of if

          m_fUsingWindowRgn = false;
        } // of if

        // set our control's location, but don't change it's size at all
        // [people for whom zooming is important should set that up here]
        //
        DWORD dwFlag;
        OnSetObjectRectsChangingWindowPos(&dwFlag);

        int cx, cy;
        cx = prcPos->right - prcPos->left;
        cy = prcPos->bottom - prcPos->top;
        ::SetWindowPos(m_hWnd, NULL, prcPos->left, prcPos->top, cx, cy, dwFlag | SWP_NOZORDER | SWP_NOACTIVATE);
      } // of if
  
      m_rcLocation = *prcPos;

      return result;
    } // of Wnd::SetObjectRects()

    /**
     *
     */
    void Wnd::OnSetObjectRectsChangingWindowPos(DWORD *dwFlag)
    { *dwFlag = 0; }


    /**
     *
     */
    void Wnd::HiMetricToPixel(const SIZEL * lpSizeInHiMetric, LPSIZEL lpSizeInPix)
    { InitStatic();

      lpSizeInPix->cx = MAP_LOGHIM_TO_PIX(lpSizeInHiMetric->cx, m_iXppli);
      lpSizeInPix->cy = MAP_LOGHIM_TO_PIX(lpSizeInHiMetric->cy, m_iYppli);
    } // of Wnd::HiMetricToPixel()

    /**
     *
     */
    void Wnd::PixelToHiMetric(const SIZEL * lpSizeInPix, LPSIZEL lpSizeInHiMetric)
    { InitStatic();

      lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, m_iXppli);
      lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, m_iYppli);
    } // of Wnd::PixelToHiMetric()

    /**
     *
     */
    short Wnd::SpecialKeyState()
    { BOOL bShift = (::GetKeyState(VK_SHIFT)   < 0);
      BOOL bCtrl  = (::GetKeyState(VK_CONTROL) < 0);
      BOOL bAlt   = (::GetKeyState(VK_MENU)    < 0);

      return (short)(bShift + (bCtrl << 1) + (bAlt << 2));
    } // of Wnd::SpecialKeyState()

    /**
     *
     */
    void Wnd::SetQuitOnDestroy(bool quitOnDestroy)
    { m_quitOnDestroy = quitOnDestroy; }

    /**
     *
     */
    void Wnd::CloseWindow()
    { if( NULL!=m_hWnd ) ::SendMessage(m_hWnd,WM_CLOSE,0,0L); }

    /**
     *
     */
    bool Wnd::IsTimerRunning() const
    { return m_timerStarted; }

    /**
     *
     */
    HWND Wnd::GetWindowHandle() const
    { return m_hWnd; }

    /**
     *
     */
    BOOL Wnd::ShowWindow(int nCmdShow)
    { return NULL!=GetWindowHandle() ? ::ShowWindow(m_hWnd,nCmdShow) : FALSE; }

    /**
     *
     */
    bool Wnd::IsEnabled() const
    { return m_isEnabled; }

    /**
     *
     */
    HINSTANCE Wnd::GetWindowInstance() const
    { return m_hWnd!=GetWindowHandle() ? (HINSTANCE)::GetWindowLongPtr(m_hWnd,GWLP_HINSTANCE) : NULL; }

    /**
     *
     */
    DWORD Wnd::GetWindowStyle() const
    { return m_hWnd!=GetWindowHandle() ? (DWORD)::GetWindowLongPtr(m_hWnd,GWL_STYLE) : 0; }

    /**
     *
     */
    HWND Wnd::GetWindowParent() const
    { HWND hWndParent = NULL;

      if( NULL!=GetWindowHandle() )
        hWndParent = (HWND)::GetWindowLongPtr(GetWindowHandle(),GWLP_HWNDPARENT);
      else if( !m_pSite.IsNULL() )
      { COMPtr<IOleInPlaceSite> pSite(m_pSite);

        HRESULT hr = pSite->GetWindow(&hWndParent);

        if( FAILED(hr) )
          hWndParent = NULL;
      } // of else if

      return hWndParent;
    } // of Wnd::GetWindowParent()

    /**
     *
     */
    void Wnd::SetWindowParent(HWND hParentWindow)
    { if( NULL!=GetWindowHandle() )
        ::SetParent(GetWindowHandle(),hParentWindow);
    } // of Wnd::SetWindowParent()

    /**
     *
     */
    void Wnd::SetWindowPos(HWND hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags)
    { if( NULL!=GetWindowHandle() )
        ::SetWindowPos(GetWindowHandle(),hWndInsertAfter,X,Y,cx,cy,uFlags);
    } // of Wnd::SetWindowPos()

    /**
     *
     */
    void Wnd::SetWindowPos(HWND hWndInsertAfter,LPCRECT prcBounds,UINT uFlags)
    { if( NULL!=GetWindowHandle() && NULL!=prcBounds )
        ::SetWindowPos(GetWindowHandle(),hWndInsertAfter,
                       prcBounds->left, prcBounds->top,
                       prcBounds->right - prcBounds->left,
                       prcBounds->bottom - prcBounds->top,
                       uFlags
                      );
    } // of Wnd::SetWindowPos()


    /**
     *
     */
    void Wnd::MoveWindow(LPCRECT prcBounds)
    { SetWindowPos(NULL,prcBounds,SWP_NOZORDER);
    } // of Wnd::MoveWindow()

    /**
     *
     */
    void Wnd::SetWindowText(const TString& s)
    { m_wndText = s;

      if( NULL!=GetWindowHandle() )
        ::SetWindowText(GetWindowHandle(),s.c_str()); 
    } // of Wnd::SetWindowText()

    /**
     *
     */
    void Wnd::GetWindowText(TString& s) const
    { s = m_wndText; }

    /**
     *
     */
    void Wnd::ClientToScreen(POINT& pt) const
    { if( m_hWnd!=NULL )
        ::ClientToScreen(m_hWnd,&pt);
    } // of Wnd::ClientToScreen()

    /**
     *
     */
    void Wnd::ScreenToClient(POINT& pt) const
    { if( m_hWnd!=NULL )
        ::ScreenToClient(m_hWnd,&pt);
    } // of Wnd::ScreenToClient()

    /**
     *
     */
    void Wnd::StartTimer(UINT uMillis)
    { if( !IsTimerRunning() && NULL!=m_hWnd )
      { ::SetTimer(m_hWnd,100,uMillis,NULL); 

        m_timerStarted = true;
      } // of else if
    } // of Wnd::StartTimer()

    /**
     *
     */
    void Wnd::StopTimer()
    { if( IsTimerRunning() )
      { 
        if( NULL!=m_hWnd )
          ::KillTimer(m_hWnd,100);

        m_timerStarted = false;
      } // of if
    } // of Wnd::StopTimer()



    /**
     *
     */
    void Wnd::OnSize(WORD w,WORD h)
    { m_size.cx           = w;
      m_size.cy           = h;
      m_rcLocation.right  = m_rcLocation.left + w;
      m_rcLocation.bottom = m_rcLocation.top  + h;
    } // of Wnd::OnSize()

    /**
     *
     */
    void Wnd::OnPaint()
    { PaintDC dc(m_hWnd);

      OnDraw(dc);
    } // of Wnd::OnPaint()

    /**
     *
     */
    void Wnd::OnDraw(DCBase& dc,LPCRECTL prcBounds)
    { 
/*      
      dc.SetTextAlign(TA_LEFT | TA_TOP);
      dc.TextOut(0,0,_T("Hello World..."));
*/
    } // of Wnd::OnDraw()

    /**
     *
     */
    bool Wnd::OnCommand(WPARAM command)
    { bool                       result  = false;
      CommandMap::const_iterator cmdIter = m_commandMap.find(command);

      if( cmdIter!=m_commandMap.end() )
      { CmdCBList cbList = cmdIter->second;

        CmdCBList::iterator iter;

        for( iter=cbList.begin();iter!=cbList.end();iter++ )
          (*iter)->OnCommand(command);

        result = true;
      } // of if

      return result;
    } // of OnCommand()

    /**
     *
     */
    void Wnd::OnTimer()
    { 
/*
      if( calledByWorkerThread )
      { if( NULL!=m_hWnd )
        { DC dc(m_hWnd);

          dc.SetRepaintRect(m_pInvalidRect);

          OnDraw(dc,NULL);
        } // of if
        else 
        { HWND hParentWnd = GetWindowParent();
          
          if( NULL!=hParentWnd )
          { DC dc(hParentWnd);

            if( NULL!=m_pInvalidRect )
            { RECT rr = *m_pInvalidRect;

              rr.left   += m_rcLocation.left;
              rr.right  += m_rcLocation.left;
              rr.top    += m_rcLocation.top;
              rr.bottom += m_rcLocation.top;

              ::IntersectRect(&rr,&rr,&m_rcLocation);

              dc.SetRepaintRect(&rr);
            } // of if

            OnDraw(dc,(LPCRECTL)&m_rcLocation);
          } // of if
        } // of if

        AddInvalidRect(NULL);
      } // of if
      else
        InvalidateRect();
*/
      InvalidateRect(TRUE);
    } // of Wnd::OnTimer()

    /**
     *
     */
    void Wnd::Repaint()
    { if( NULL!=m_hWnd )
      { DC dc(m_hWnd);

        dc.SetRepaintRect(m_pInvalidRect);

        OnDraw(dc,NULL);
      } // of if
      else 
      { HWND hParentWnd = GetWindowParent();
        
        if( NULL!=hParentWnd )
        { DC dc(hParentWnd);

          if( NULL!=m_pInvalidRect )
          { RECT rr = *m_pInvalidRect;

            rr.left   += m_rcLocation.left;
            rr.right  += m_rcLocation.left;
            rr.top    += m_rcLocation.top;
            rr.bottom += m_rcLocation.top;

            ::IntersectRect(&rr,&rr,&m_rcLocation);

            dc.SetRepaintRect(&rr);
          } // of if

          OnDraw(dc,(LPCRECTL)&m_rcLocation);
        } // of if
      } // of if

      AddInvalidRect(NULL);
    } // of Wnd::Repaint()

    /**
     *
     */
    void Wnd::OnFocus(bool setFocus,HWND hWnd)
    { m_hasFocus = setFocus; }

    /**
     *
     */
    void Wnd::OnMouse(MouseMsgT type,WORD x,WORD y)
    { }

    /**
     *
     */
    void Wnd::OnKeyDown(WORD vKey)
    { }

    /**
     *
     */
    void Wnd::OnHScroll(WORD request,WORD pos)
    { }

    
    /**
     *
     */
    void Wnd::OnVScroll(WORD request,WORD pos)
    { }

    /**
     *
     */
    void Wnd::OnContextMenu(WORD xPos,WORD yPos)
    { }

    /**
     *
     */
    void Wnd::OnInitMenu(HMENU hMenu)
    { }

    /**
     *
     */
    void Wnd::OnEnabled(bool isEnabled)
    { m_isEnabled = isEnabled; }

    /**
     *
     */
    void Wnd::OnDestroy(bool destroyThis)
    { if( NULL!=m_hWnd )
        ::SetWindowLongPtr(m_hWnd,GWLP_USERDATA,0);

      m_hWnd = NULL;

      if( m_quitOnDestroy )
        ::PostQuitMessage(0);

      if( destroyThis )
        delete this;
    } // of Wnd::OnDestroy()

    /**
     *
     */
    void Wnd::OnShellNotify(WPARAM iconId,LPARAM lParam)
    { }

    /**
     *
     */
    void Wnd::GetScrollPosition(int& xPos,int& yPos) const
    { SCROLLINFO sib;

      sib.cbSize = sizeof(sib);
      sib.fMask  = SIF_POS;

      ::GetScrollInfo(GetWindowHandle(),SB_HORZ,&sib);

      xPos = sib.nPos;

      ::GetScrollInfo(GetWindowHandle(),SB_VERT,&sib);

      yPos = sib.nPos;
    }

    /**
     *
     */
    void Wnd::GetScrollHorzRange(int& minPos,int& maxPos) const
    { ::GetScrollRange(GetWindowHandle(),SB_HORZ,&minPos,&maxPos); }

    /**
     *
     */
    void Wnd::GetScrollVertRange(int& minPos,int& maxPos) const
    { ::GetScrollRange(GetWindowHandle(),SB_VERT,&minPos,&maxPos); }

    /**
     *
     */
    void Wnd::AddCommandCallback(WPARAM cmd,ICommand* cmdCB)
    { CommandMap::iterator cmdIter = m_commandMap.find(cmd);

      if( cmdIter==m_commandMap.end() )
      { CmdCBList cbList;
        
        cbList.push_back(cmdCB);

        m_commandMap.insert( CommandPair(cmd,cbList) );
      } // of if
      else
        (cmdIter->second).push_back(cmdCB);
    } // of AddCommandCallback

    /**
     *
     */
    void Wnd::RegisterClass(HINSTANCE hInst,
                            WORD wIcon,
                            HBRUSH backgroundBrush,
                            UINT style
                           )
    { WndClass::RegisterWindowClass(BVRWNDCLS,new WndFactory(),hInst,wIcon,backgroundBrush,style); }
#pragma endregion

#pragma region OCX Helper
    /**
     *
     */
    bool Wnd::SetFocus(bool fGrab)
    { bool result = false;

      if( fGrab && !m_pSite.IsNULL() )
      { COMPtr<IOleInPlaceSite> pSite(m_pSite);

        pSite->OnUIActivate();
      } // of if

      if( NULL!=m_hWnd )
        result = ::SetFocus(fGrab ? m_hWnd : NULL)==m_hWnd;
      else if( !m_pSiteWndLess.IsNULL() )
        result = m_pSiteWndLess->SetFocus(fGrab)==S_OK;

      return result;
    } // of Wnd::SetFocus()

    /**
     *
     */
    bool Wnd::GetFocus()
    { bool result = m_hasFocus;

/*
      if( NULL!=m_hWnd )
        result = ::GetFocus()==m_hWnd;
      else if( NULL!=m_pSiteWndLess ) 
        result = m_pSiteWndLess->GetFocus() == S_OK ? true : false;
*/

      return result;
    } // of Wnd::GetFocus()

    /**
     *
     */
    HDC Wnd::GetDC()
    { HDC hdc = NULL;
  
      if( NULL!=m_hWnd )
        hdc = ::GetDC(m_hWnd);
      else if( !m_pSiteWndLess.IsNULL() )
        m_pSiteWndLess->GetDC(NULL, 0, &hdc);
  
      return hdc;
    } // of Wnd::GetDC()
    
    /**
     *
     */
    void Wnd::ReleaseDC(HDC hdc)
    { if( NULL!=m_hWnd )
        ::ReleaseDC(m_hWnd, hdc);
        else if( !m_pSiteWndLess.IsNULL() )
        m_pSiteWndLess->ReleaseDC(hdc);
    } // of Wnd::ReleaseDC()
    
    /**
     *
     */
    bool Wnd::SetCapture(bool fGrab)
    { bool result = true;
  
      if( NULL!=m_hWnd )
        ::SetCapture(m_hWnd);
      else if( !m_pSiteWndLess.IsNULL() ) 
        result = m_pSiteWndLess->SetCapture(fGrab)==S_OK ? true : false;
  
      return result;
    } // of Wnd::SetCapture()
    
    /**
     *
     */
    bool Wnd::GetCapture()
    { bool result = true;

      if( NULL!=m_hWnd )
        ::GetCapture()==m_hWnd ? true : false;
      else if( !m_pSiteWndLess.IsNULL() )
        result = m_pSiteWndLess->GetCapture()==S_OK ? true : false;
  
      return result;
    } // of Wnd::GetCapture()
    
    /**
     *
     */
    bool Wnd::InvalidateRect(BOOL fErase)
    { bool result = true;

      if( NULL!=m_hWnd )
        result = ::InvalidateRect(m_hWnd, m_pInvalidRect, fErase) ? true : false;
      else if( !m_pSiteWndLess.IsNULL() )
      { HRESULT hr = m_pSiteWndLess->InvalidateRect(m_pInvalidRect, fErase);

        result = hr==S_OK ? true : false;
      } // of else if

      AddInvalidRect(NULL);
  
      return result;
    } // of Wnd::InvalidateRect()

    /**
     *
     */
    void Wnd::AddInvalidRect(LPCRECT r)
    { if( NULL!=r )
      { if( NULL==m_pInvalidRect )
          m_invalidRect = *r;
        else
          ::UnionRect(&m_invalidRect,&m_invalidRect,r);

        m_pInvalidRect = &m_invalidRect;
      } // of if
      else
      { ::memset(&m_invalidRect,'\0',sizeof(m_invalidRect));
        m_pInvalidRect = NULL;
      } // of else
    } // of Wnd::AddInvalidRect()
    
    /**
     *
     */
    bool Wnd::ScrollRect(LPCRECT  prcBounds,LPCRECT  prcClip,int dx,int dy)
    { bool result = true;

      if( NULL!=m_hWnd )
        ::ScrollWindowEx(m_hWnd, dx, dy, prcBounds, prcClip, NULL, NULL, SW_INVALIDATE);
      else if( !m_pSiteWndLess.IsNULL() )
      { HRESULT hr = m_pSiteWndLess->ScrollRect(dx, dy, prcBounds, prcClip);

        result = hr==S_OK ? true : false;
      } // of else if
  
      return result;
    } // of Wnd::ScrollRect()
#pragma endregion

#pragma region Windows Event Handling
    /**
     *
     */
    LRESULT Wnd::WindowProcedure(UINT msg,WPARAM wParam,LPARAM lParam)
    { LRESULT result  = 0L;
      bool    defProc = true;

      switch( msg )
      { 
      case WM_CREATE:
        OnCreate((LPCREATESTRUCT)lParam);
        break;
      case WM_SIZE:
        OnSize(LOWORD(lParam),HIWORD(lParam));
        break;
      case WM_COMMAND:
        OnCommand(LOWORD(wParam));
        break;
      case WM_PAINT:
        OnPaint();
        break;
      case WM_TIMER:
        OnTimer();
        break;
      case WM_SETFOCUS:
        OnFocus(true,(HWND)wParam);
        break;
      case WM_KILLFOCUS:
        OnFocus(false,(HWND)wParam);
        break;
      case WM_LBUTTONDOWN:
        OnMouse(LBUTTONDOWN,LOWORD(lParam),HIWORD(lParam));
        break;
      case WM_LBUTTONUP:
        OnMouse(LBUTTONUP,LOWORD(lParam),HIWORD(lParam));
        break;
      case WM_RBUTTONDOWN:
        OnMouse(RBUTTONDOWN,LOWORD(lParam),HIWORD(lParam));
        break;
      case WM_RBUTTONUP:
        OnMouse(RBUTTONUP,LOWORD(lParam),HIWORD(lParam));
        break;
      case WM_KEYDOWN:
        OnKeyDown(wParam);
        break;
      case WM_ENABLE:
        OnEnabled(wParam==TRUE);
        break;
      case WM_HSCROLL:
        OnHScroll(LOWORD(wParam),HIWORD(wParam));
        break;
      case WM_VSCROLL:
        OnVScroll(LOWORD(wParam),HIWORD(wParam));
        break;
      case WM_CONTEXTMENU:
        OnContextMenu(LOWORD(lParam),HIWORD(lParam));
        defProc = false;
        break;
      case WM_INITMENU:
        OnInitMenu((HMENU)wParam);
        break;
      case WM_DESTROY:
        OnDestroy(true);
        break;
      case WM_BVR_SHELLNOTIFY:
        OnShellNotify(wParam,lParam);
        break;
      default:
        break;
      } // of switch

      if( defProc )
      { if( m_hWnd!=NULL )
          result = ::DefWindowProc(m_hWnd,msg,wParam,lParam);
        else if( !m_pSiteWndLess.IsNULL() ) 
          m_pSiteWndLess->OnDefWindowMessage(msg,wParam,lParam, &result);
      } // of if

      return result;
    } // of Wnd::WindowProcedure()

    /**
     *
     */
    LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
    { LRESULT result = 0L;

      if( msg==WM_NCCREATE )
      { result = Wnd::Create(hWnd,(LPCREATESTRUCT)lParam)!=NULL ? TRUE : FALSE;

        if( result )
          result = ::DefWindowProc(hWnd,msg,wParam,lParam);
      } // of if
      else
      { win::Wnd* w = (win::Wnd*) ::GetWindowLongPtr(hWnd,GWLP_USERDATA);

        if( NULL!=w )
        { assert( w->GetWindowHandle()==hWnd );

          result = w->WindowProcedure(msg,wParam,lParam);
        } // of if
        else
          result = ::DefWindowProc(hWnd,msg,wParam,lParam);
      } // of else

#ifdef _DEBUG1
      switch( msg )
      {
      case WM_NCCREATE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_NCCREATE   wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_CREATE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_CREATE     wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_NCDESTROY:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_NCDESTROY  wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_DESTROY:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_DESTROY    wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_NCPAINT:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_NCPAINT    wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_SETFOCUS:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_SETFOCUS   wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_KILLFOCUS:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_KILLFOCUS  wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_ENABLE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_ENABLE     wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_QUIT:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_QUIT       wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_CONTEXTMENU:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_CONTEXTMENU Param=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_ACTIVATE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_ACTIVATE   wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_SIZE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_SIZE       wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_ERASEBKGND:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_ERASEBKGND wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_PAINT:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_PAINT      wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_NCHITTEST:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_NCHITTEST  wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_NCCALCSIZE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_NCCALCSIZE wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_MOUSEMOVE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_MOUSEMOVE  wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_LBUTTONDOWN:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_LBUTTONDOWN Param=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_LBUTTONUP:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_LBUTTONUP  wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_RBUTTONDOWN:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_RBUTTONDOWN Param=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_RBUTTONUP:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_RBUTTONUP  wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_SYNCPAINT:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_SYNCPAINT  wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_GETMINMAXINFO:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_GETMINMAXINFO ram=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_SHOWWINDOW:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_SHOWWINDOW wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_PARENTNOTIFY:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_PARENTNOTIFY aram=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_WINDOWPOSCHANGING:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_WINDOWPOSCHANGING=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_ACTIVATEAPP:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_ACTIVATEAPP Param=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_NCACTIVATE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_NCACTIVATE wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_GETICON:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_GETICON    wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_WINDOWPOSCHANGED:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_WINDOWPOSCHANGED =%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_SETCURSOR:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_SETCURSOR  wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_NCMOUSEMOVE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_NCMOUSEMOVE Param=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_NCMOUSEHOVER:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_NCMOUSEHOVE Param=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_MOUSEHOVER:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_MOUSEHOVER  Param=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_MOUSELEAVE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_MOUSELEAVE wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_MOVE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_MOVE       wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_SETTEXT:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_SETTEXT    wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_GETTEXT:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_GETTEXT    wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_CLOSE:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_CLOSE      wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_SYSCOMMAND:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_SYSCOMMAND wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_TIMER:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_TIMER      wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      case WM_COMMAND:
        OutputDebugFmt(TEXT("%2d <-%08x:WM_COMMAND    wParam=%8x lParam=%8lx"),result,hWnd,wParam,lParam);
        break;
      default:
        OutputDebugFmt(TEXT("%2x <-%08x:%13x wParam=%8x lParam=%8lx"),result,hWnd,msg,wParam,lParam);
        break;
      } // of switch
#endif

      return result;
    } // of ::WndProc()


    /**
     *
     */
    BOOL Wnd::EnumChildren()
    { return GetWindowHandle()!=NULL ? EnumChildWindows(GetWindowHandle(),EnumChildProc,reinterpret_cast<LPARAM>(this)) : FALSE;
    } // of Wnd::EnumChildren()


    /**
     *
     */
    BOOL Wnd::EnumChild(HWND hWndChild)
    { return TRUE; }

    /**
     *
     */
    BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
    { Wnd* pWnd = reinterpret_cast<Wnd*>(lParam);

      return NULL!=pWnd ? pWnd->EnumChild(hwndChild) : FALSE;
    } // of ::EnumChildProc()
#pragma endregion
  } // of namespace win
} // of namespace bvr20983

template class std::vector<win::ICommand*>;
template class std::map<WPARAM, std::vector<win::ICommand*> >;
/*==========================END-OF-FILE===================================*/
