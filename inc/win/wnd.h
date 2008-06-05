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
#if !defined(WND_H)
#define WND_H

#include "os.h"
#include "win/dc.h"
#include "util/thread.h"
#include "util/comptr.h"

#define WNDMAGIC       0x774e446d
#define WND_NULLCOLOR  ((COLORREF)0xFFFFFFFF)

namespace bvr20983
{
  namespace win
  {
    class Wnd;

    typedef IOleClientSite*            LPCLIENTSITE;
    typedef IOleInPlaceSiteWindowless* LPSITEWNDLESS;
    typedef IOleInPlaceSiteEx*         LPSITEEX;
    typedef IOleInPlaceSite*           LPSITE;

    /**
     *
     */
    class ICommand
    { public:
        virtual void OnCommand(WPARAM cmd)=0;
    }; // of class CommandCB

    /**
     *
     */
    struct WndFactory
    { virtual Wnd* Construct(HWND hWnd); 
    }; // of struct WndFactory


    /**
     *
     */
    struct WndCreateStruct
    { DWORD         magic;
      size_t        dwSize;
      LPSITEWNDLESS pSiteWndLess;
      LPSITEEX      pSiteEx;
      LPSITE        pSite;
      LPVOID        pCreateParams;
    }; // of struct WndCreateStruct


    /**
     *
     */
    class Wnd 
    {
      public:
        enum MouseMsgT
        { LBUTTONDOWN,
          LBUTTONUP,
          RBUTTONDOWN,
          RBUTTONUP
        };

        static Wnd* Construct(LPCTSTR       wndCls,
                              LPCTSTR       windowName,
                              int           exStyle=WS_EX_WINDOWEDGE,
                              int           style=WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
                              int           x=CW_USEDEFAULT,
                              int           y=CW_USEDEFAULT,
                              int           width=CW_USEDEFAULT,
                              int           height=CW_USEDEFAULT,
                              HWND          hWndParent=NULL,
                              LPVOID        lpParam=NULL,
                              LPCTSTR       menuName=NULL,
                              HINSTANCE     hInstance=NULL,
                              LPCLIENTSITE  pClientSite=NULL,
                              bool          shouldAsk4WndLess=false
                             );

        static Wnd* Construct(LPCREATESTRUCT lpCS,LPCLIENTSITE pClientSite=NULL,bool shouldAsk4WndLess=false);

        static void RegisterClass(HINSTANCE hInst,
                                  WORD wIcon=0,
                                  HBRUSH backgroundBrush=(HBRUSH)::GetStockObject(WHITE_BRUSH),
                                  UINT style=CS_HREDRAW | CS_VREDRAW
                                 );

        static void     HiMetricToPixel(const SIZEL * lpSizeInHiMetric, LPSIZEL lpSizeInPix);
        static void     PixelToHiMetric(const SIZEL * lpSizeInPix, LPSIZEL lpSizeInHiMetric);
        static short    SpecialKeyState();
        static COLORREF OleColor2ColorRef(const OLE_COLOR& olecolor)
        { COLORREF result = WND_NULLCOLOR;

          if( olecolor!=WND_NULLCOLOR )
            ::OleTranslateColor(olecolor,NULL,&result);

          return result;
        }

        virtual void    OnDraw(DCBase& dc,LPCRECTL prcBounds=NULL);
        virtual void    OnSetObjectRectsChangingWindowPos(DWORD *dwFlag);
        virtual bool    OnSetExtent(const SIZEL *pSizeL);

        virtual void    StartTimer(UINT uMillis);
        virtual void    StopTimer();

        virtual void    SetWindowText(const TString& s);
        virtual void    GetWindowText(TString& s) const;

        virtual HRESULT GetNaturalExtent(DWORD dwAspect,DVEXTENTINFO* pExtentInfo,LPSIZEL psizel)
        { return E_NOTIMPL; }

        virtual LRESULT WindowProcedure(UINT msg,WPARAM wParam,LPARAM lParam);

        bool      SetExtent(SIZEL* psizel);
        void      GetExtent(SIZEL* psizel) const;
        bool      SetObjectRects(LPCRECT lprcPosRect,LPCRECT lprcClipRect);

        void      DestroyWindow();
        void      CloseWindow();
        void      SetQuitOnDestroy(bool quitOnDestroy);

        void      Repaint();

        void      AddCommandCallback(WPARAM cmd,ICommand* cmdCB);

        bool      IsTimerRunning() const;

        HWND      GetWindowHandle() const;
        BOOL      ShowWindow(int nCmdShow=SW_SHOW);
        bool      IsEnabled() const;
        HINSTANCE GetWindowInstance() const;
        DWORD     GetWindowStyle() const;
        HWND      GetWindowParent() const;
        void      SetWindowParent(HWND hParentWindow);

        void      SetWindowPos(HWND hWndInsertAfter,int X,int Y,int cx,int cy,UINT uFlags);
        void      SetWindowPos(HWND hWndInsertAfter,LPCRECT prcBounds,UINT uFlags);
        void      MoveWindow(LPCRECT prcBounds);

        const SIZEL& GetSize() const
        { return m_size; }
        
        const RECT& GetLocation() const
        { return m_rcLocation; }

        void      ClientToScreen(POINT& pt) const;
        void      ScreenToClient(POINT& pt) const;

        void      GetScrollPosition(int& xPos,int& yPos) const;
        void      GetScrollHorzRange(int& minPos,int& maxPos) const;
        void      GetScrollVertRange(int& minPos,int& maxPos) const;

        bool      IsVisible() const
        { return NULL!=m_hWnd ? (GetWindowStyle() & WS_VISIBLE)!=0 : ( m_pSiteWndLess.IsNULL() ? true : false); }

        bool      IsWindowless() const
        { return !m_pSiteWndLess.IsNULL() ; }

        bool      SetFocus(bool fGrab);                    
        bool      GetFocus();                              
        HDC       GetDC();                                 
        void      ReleaseDC(HDC hdc);                      
        bool      SetCapture(bool fGrab);                  
        bool      GetCapture();                            
        bool      InvalidateRect(BOOL fErase=TRUE);
        bool      ScrollRect(LPCRECT, LPCRECT, int, int); 
        void      AddInvalidRect(LPCRECT r);

        BOOL      EnumChildren();

        friend LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
        friend BOOL    CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam);
        friend WndFactory;
  
      protected:
        typedef std::vector<ICommand*>       CmdCBList;
        typedef std::map <WPARAM,CmdCBList > CommandMap;
        typedef std::pair<WPARAM,CmdCBList > CommandPair;

        Wnd(HWND hWnd);
        virtual ~Wnd();

        virtual void OnCreate(LPCREATESTRUCT lpCS);
        virtual void OnSize(WORD w,WORD h);
        virtual void OnPaint();
        virtual void OnDestroy(bool destroyThis);
        virtual bool OnCommand(WPARAM command);
        virtual void OnTimer();
        virtual void OnFocus(bool setFocus,HWND hWnd);
        virtual void OnMouse(MouseMsgT type,WORD x,WORD y);
        virtual void OnKeyDown(WORD vKey);
        virtual void OnEnabled(bool isEnabled);
        virtual void OnHScroll(WORD request,WORD pos);
        virtual void OnVScroll(WORD request,WORD pos);
        virtual void OnContextMenu(WORD xPos,WORD yPos);
        virtual void OnInitMenu(HMENU hMenu);
        virtual void OnShellNotify(WPARAM iconId,LPARAM lParam);

        virtual BOOL EnumChild(HWND hWndChild);

        HWND          m_hWnd;
        SIZEL         m_size;
        bool          m_hasFocus;
        RECT          m_rcLocation;

        bool          m_isEnabled;
        bool          m_timerStarted;
        TString       m_wndText;

        bool          m_fUsingWindowRgn;
        HRGN          m_hRgn;

        GCOMPtr<IOleInPlaceSite>          m_pSite;
        COMPtr<IOleInPlaceSiteEx>         m_pSiteEx;
        COMPtr<IOleInPlaceSiteWindowless> m_pSiteWndLess;

        bool          m_quitOnDestroy;

        CommandMap    m_commandMap;
        
        RECT          m_invalidRect;
        LPRECT        m_pInvalidRect;

        static int    m_cxChar;
        static int    m_cxCaps;
        static int    m_cyChar;
        static int    m_iXppli;
        static int    m_iYppli;

        static Wnd* Create(HWND hWnd,LPCREATESTRUCT lpCS);
        static void InitStatic();
    }; // of class Wnd

    LRESULT CALLBACK WndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
    BOOL    CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam);
  } // of namespace win
} // of namespace bvr20983
#endif // WND_H