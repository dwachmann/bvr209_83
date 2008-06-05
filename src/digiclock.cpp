/*
 * $Id$
 * 
 * A windows GUI program that tests the digital clock windows class.
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
#include "bvr20983cc.h"
#include "digiclock-res.h"
#include "digiclock.h"
#include "win/dialog.h"
#include "win/dc.h"
#include "win/wnd.h"
#include "win/wndclass.h"
#include "win/clipboard.h"
#include "win/tray.h"
#include <sstream>
#include <iomanip>

using namespace bvr20983;
using namespace bvr20983::win;

static DigiClockApp* gApp = NULL;

LONG DigiClockWnd::m_hLEDWndStyle[6] =
{ WS_VISIBLE | WS_CHILD | WS_TABSTOP | LEDS_SUPPRESS | WS_CLIPSIBLINGS | WS_DLGFRAME,
  WS_VISIBLE | WS_CHILD | WS_TABSTOP | LEDS_CLOCK | LEDS_16SEGEGMENT | LEDS_24HOUR | LEDS_SUPPRESS | WS_CLIPSIBLINGS | WS_CAPTION | WS_THICKFRAME,
  WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS | WS_CAPTION | WS_THICKFRAME,
  WS_VISIBLE | WS_CHILD | WS_TABSTOP | LEDS_16SEGEGMENT | WS_CLIPSIBLINGS | WS_THICKFRAME,
  WS_VISIBLE | WS_CHILD | WS_TABSTOP | LEDS_CLOCK | LEDS_16SEGEGMENT | WS_CLIPSIBLINGS | WS_CAPTION,
  WS_VISIBLE | WS_CHILD | WS_TABSTOP | LEDS_16SEGEGMENT | WS_DISABLED | WS_CLIPSIBLINGS
};

LPCTSTR DigiClockWnd::m_hLEDText[6] =
{ _T("1000.00 EU"),
  _T("LED"),
  _T("TEST:THE:QUICK:BROWN:FOX:JUMPS:OVER:THE:LAZY:DOG."),
  _T("LED"),
  _T("LED"),
  _T("JUMPS:OVER.")
};

namespace bvr20983
{
  namespace win
  {
#pragma region Construction & Deconstruction
    /**
     *
     */
    Wnd* DigiClockWndFactory::Construct(HWND hWnd)
    { return new DigiClockWnd(hWnd); }


    /*
     * DigiClockApp::DigiClockApp
     *
     * Purpose:
     *   Constructor.
     *
     * Parameters:
     *
     * Return: 
     *   void
     */
    DigiClockApp::DigiClockApp(HINSTANCE hInstance,LPSTR lpCmdLine,int nCmdShow) 
      : App(hInstance,lpCmdLine,nCmdShow,IDI_STATE_0,IDA_ACC,(HBRUSH)::GetStockObject(BLACK_BRUSH)),
        m_transparent(false),
        m_noWnd(6)
    { InitBVRCustomControls();

      WndClass::RegisterWindowClass(_T("DIGICLOCKWND"),
                                    new DigiClockWndFactory(),
                                    hInstance,
                                    IDI_APP_ICON,
                                    ::CreateSolidBrush(RGB(255,255,70))
//                                    (HBRUSH)::GetStockObject(NULL_BRUSH)
                                   ); 

      Init(nCmdShow);
    } // of DigiClockApp::DigiClockApp()

    /**
     *
     */
    void DigiClockApp::Init(int nCmdShow)
    { m_pWnd = (Wnd*) Wnd::Construct(_T("DIGICLOCKWND"),_T("digiclock"),
                                     m_transparent ? WS_EX_WINDOWEDGE | WS_EX_COMPOSITED : WS_EX_WINDOWEDGE,
//                                     WS_EX_WINDOWEDGE,
                                     WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL | WS_CLIPSIBLINGS,
                                     CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
                                     NULL,
                                     NULL,
                                     MAKEINTRESOURCE(IDM_APP),
                                     GetInstance()
                                    );

      ((DigiClockWnd*)m_pWnd)->SetApplication(this);

      m_pWnd->AddCommandCallback(IDM_APP_ABOUT,this);
      m_pWnd->AddCommandCallback(IDM_APP_EXIT,this);
      m_pWnd->AddCommandCallback(IDM_EDIT_COPY,this);

      m_pWnd->AddCommandCallback(IDM_WND_1,this);
      m_pWnd->AddCommandCallback(IDM_WND_2,this);
      m_pWnd->AddCommandCallback(IDM_WND_4,this);
      m_pWnd->AddCommandCallback(IDM_WND_6,this);
      m_pWnd->AddCommandCallback(IDM_TRANSPARENT,this);

      m_pWnd->AddCommandCallback(IDM_STARTCLOCK,this);
      m_pWnd->AddCommandCallback(IDM_STOPCLOCK,this);

      m_pWnd->ShowWindow(nCmdShow);
    } // of DigiClockApp::DigiClockApp()

    /**
     *
     */
    DigiClockWnd::DigiClockWnd(HWND hWnd) : Wnd(hWnd)
    { SetQuitOnDestroy(true);
      ::memset(m_hLEDWnd,'\0',sizeof(m_hLEDWnd));
      ::memset(m_hLEDWndProc,'\0',sizeof(m_hLEDWndProc));
      ::memset(m_LEDWndRect,'\0',sizeof(m_LEDWndRect));

      m_focus         = -1;
      m_childrenCount = 0;
      m_pTray         = NULL;
      m_pApplication  = NULL;
    }

    /**
     *
     */
    DigiClockWnd::~DigiClockWnd()
    { }
#pragma endregion

#pragma region Implementation
    /**
     *
     */
    LRESULT DigiClockApp::CallWindowProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
    { return ((DigiClockWnd*)m_pWnd)->CallWindowProc(hWnd,msg,wParam,lParam); }
#pragma endregion

#pragma region Events
    /**
     *
     */
    void DigiClockApp::OnCommand(WPARAM cmd)
    { switch( cmd )
      { 
      case IDM_APP_ABOUT:
        { win::Dialog dlg;

          dlg.Show(MAKEINTRESOURCE(IDD_ABOUTBOX),m_pWnd->GetWindowHandle(),m_hInstance);
        }
        break;
      case IDM_EDIT_COPY:
        ::SendMessage( ((DigiClockWnd*)m_pWnd)->GetFocusWnd(),WM_COMMAND,IDC_EDIT_COPY,0L);
        break;

      case IDM_STARTCLOCK:
        ::SendMessage( ((DigiClockWnd*)m_pWnd)->GetFocusWnd(),LEDM_STARTCLOCK,0,0L);
        break;
      case IDM_STOPCLOCK:
        ::SendMessage( ((DigiClockWnd*)m_pWnd)->GetFocusWnd(),LEDM_STOPCLOCK,0,0L);
        break;

      case IDM_WND_1:
        m_noWnd = 1;
        ((DigiClockWnd*)m_pWnd)->CreateChildren(m_noWnd);
        break;
      case IDM_WND_2:
        m_noWnd = 2;
        ((DigiClockWnd*)m_pWnd)->CreateChildren(m_noWnd);
        break;
      case IDM_WND_4:
        m_noWnd = 4;
        ((DigiClockWnd*)m_pWnd)->CreateChildren(m_noWnd);
        break;
      case IDM_WND_6:
        m_noWnd = 6;
        ((DigiClockWnd*)m_pWnd)->CreateChildren(m_noWnd);
        break;
      case IDM_TRANSPARENT:
        m_transparent = !m_transparent;

        m_pWnd->SetQuitOnDestroy(false);
        m_pWnd->CloseWindow();
        Init(SW_SHOW);
        break;
      case IDM_APP_EXIT:
        m_pWnd->CloseWindow();
        break;
      } // of switch
    } // of DigiClockApp::OnCommand()


    /**
     *
     */
    void DigiClockWnd::CalcLEDPos(int num,int maxW,int maxH)
    { bool isTransparent = NULL!=m_pApplication && m_pApplication->ShouldBeTransparent();

      if( isTransparent )
      { int w0 = 60;
        int h0 = 40;
        int w  = maxW-(num-1)*w0;
        int h  = maxH-(num-1)*h0;
        int i  = 0;

        for( int c=0;c<num;c++,i++ )
        { m_LEDWndRect[i].left   = c*w0;
          m_LEDWndRect[i].top    = c*h0;
          m_LEDWndRect[i].right  = w;
          m_LEDWndRect[i].bottom = h;
        } // of if

        SCROLLINFO sib;

        sib.cbSize = sizeof(sib);
        sib.fMask  = SIF_RANGE | SIF_DISABLENOSCROLL | SIF_PAGE;
        sib.nMin   = 0;
        sib.nMax   = maxW-1;
        sib.nPage  = w0;
        ::SetScrollInfo(GetWindowHandle(),SB_HORZ,&sib,FALSE);

        sib.nMax   = maxH-1;
        sib.nPage  = h0;
        ::SetScrollInfo(GetWindowHandle(),SB_VERT,&sib,FALSE);
      } // of if
      else
      { int  maxX          = 0;
        int  maxY          = 0;
        int  margin        = 10;
        int  cols          = num%2==0 ? 2 : 1;
        int  rows          = num/cols + (num%cols!=0) ;

        if( num==2 )
        { cols=1;
          rows=2;
        } // of if
        else if( num==4 )
        { cols=1;
          rows=4;
        } // of else if

        if( maxW<300*cols )
          maxW = 300*cols;

        if( maxH<200*rows )
          maxH = 200*rows;

        int i      = 0;

        int w0     = maxW / cols;
        int h0     = maxH / rows;

        int w      = w0 - 2*margin;
        int h      = h0 - 2*margin;

        for( int r=0;r<rows;r++ )
          for( int c=0;c<cols;c++,i++ )
            if( i<num )
            { m_LEDWndRect[i].left   = c*w0 + margin;
              m_LEDWndRect[i].top    = r*h0 + margin;
              m_LEDWndRect[i].right  = w;
              m_LEDWndRect[i].bottom = h;

              maxX = max( m_LEDWndRect[i].left + m_LEDWndRect[i].right  + margin,maxX );
              maxY = max( m_LEDWndRect[i].top  + m_LEDWndRect[i].bottom + margin,maxY );
            } // of if
            else
              break;

        SCROLLINFO sib;

        sib.cbSize = sizeof(sib);
        sib.fMask  = SIF_RANGE | SIF_DISABLENOSCROLL | SIF_PAGE;
        sib.nMin   = 0;
        sib.nMax   = maxX-1;
        sib.nPage  = m_size.cx;
        ::SetScrollInfo(GetWindowHandle(),SB_HORZ,&sib,FALSE);

        sib.nMax   = maxY-1;
        sib.nPage  = m_size.cy;
        ::SetScrollInfo(GetWindowHandle(),SB_VERT,&sib,FALSE);
      } // of else
    } // of DigiClockWnd::CalcLEDPos()

    /**
     *
     */
    void DigiClockWnd::CreateChildren(int childrenCount)
    { bool isTransparent = NULL!=m_pApplication && m_pApplication->ShouldBeTransparent();

      m_childrenCount = childrenCount;
      m_focus         = -1;

      int scrollXPos;
      int scrollYPos;

      GetScrollPosition(scrollXPos,scrollYPos);

      CalcLEDPos(childrenCount,m_size.cx,m_size.cy);

      LEDCREATESTRUCT ledCS;
      LEDCREATESTRUCT ledCS1;

      ::memset(&ledCS,'\0',sizeof(ledCS));
      ::memset(&ledCS1,'\0',sizeof(ledCS));

      ledCS.magic         = LEDWNDMAGIC;
      ledCS.dwSize        = sizeof(LEDCREATESTRUCT);
      //ledCS.bgColor       = RGB(20,20,80);
      ledCS.bgColor       = WND_NULLCOLOR;
      ledCS.fgColor       = RGB(0,128,0);
      ledCS.fgColor1      = RGB(0,20,0);
      ledCS.initialStart  = true;
      ledCS.displayLen    = 0;

      ledCS1.magic        = LEDWNDMAGIC;
      ledCS1.dwSize       = sizeof(LEDCREATESTRUCT);
      ledCS1.bgColor      = RGB(0,0,0);
      ledCS1.fgColor      = RGB(200,40,0);
      ledCS1.fgColor1     = RGB(100,20,0);
      ledCS1.initialStart = true;
      ledCS1.displayLen   = 8;

      for( int i=0;i<sizeof(m_hLEDWnd)/sizeof(m_hLEDWnd[0]);i++ )
      { if( m_hLEDWnd[i]!=NULL )
          ::DestroyWindow(m_hLEDWnd[i]);

        m_hLEDWnd[i]     = NULL;
        m_hLEDWndProc[i] = NULL;
      } // of for

      for( int i=0;i<childrenCount;i++ )
      { m_hLEDWnd[i] = ::CreateWindowEx(isTransparent ? WS_EX_TRANSPARENT: 0,LEDWNDCLS,m_hLEDText[i],m_hLEDWndStyle[i],
                                        m_LEDWndRect[i].left - scrollXPos,
                                        m_LEDWndRect[i].top  - scrollYPos,
                                        m_LEDWndRect[i].right,
                                        m_LEDWndRect[i].bottom,
                                        GetWindowHandle(),NULL,GetWindowInstance(),
                                        i==1 ? &ledCS : (i==2 ? &ledCS1 : NULL)
                                       );

        m_hLEDWndProc[i] = (WNDPROC) ::SetWindowLongPtr(m_hLEDWnd[i],GWLP_WNDPROC,(LONG)LEDWndProc);
      } // of for

      if( childrenCount==1 )
        m_pTray->Modify(1,IDI_STATE_2);
      else
        m_pTray->Modify(1,IDI_STATE_1);
    } // of DigiClockWnd::CreateChildren()

    /**
     *
     */
    LRESULT DigiClockWnd::CallWindowProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
    { LRESULT result = 0;

      for( int i=0;i<sizeof(m_hLEDWnd)/sizeof(m_hLEDWnd[0]);i++ )
      { if( m_hLEDWnd[i]==hWnd && m_hLEDWndProc[i]!=NULL )
        { 
          switch( msg )
          { 
          case WM_LBUTTONDOWN:
            ::SetWindowPos(hWnd, HWND_TOP, 0,0,0,0,SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE);
            break;
          case WM_MBUTTONDOWN:
            ::SetWindowPos(hWnd, HWND_BOTTOM, 0,0,0,0,SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE);
            break;
          } // of switch
          
          result = ::CallWindowProc(m_hLEDWndProc[i],hWnd,msg,wParam,lParam);

          if( msg==WM_SETFOCUS )
            m_focus = i;

          break;
        } // of if
      } // of for

      return result;
    }

    /**
     *
     */
    void DigiClockWnd::OnCreate(LPCREATESTRUCT lpCS)
    { Wnd::OnCreate(lpCS);
      
      m_pTray = new Tray(GetWindowHandle());

      m_pTray->Add(0,IDI_STATE_0,IDS_NOTIFY_TIP1);
      m_pTray->Add(1,IDI_STATE_1,IDS_NOTIFY_TIP2,IDS_NOTIFY_TIP3);

      CreateChildren(NULL!=m_pApplication ? m_pApplication->GetNoWindows() : ARRAYSIZE(m_hLEDWnd));
    } // of DigiClockWnd::OnCreate()

    /**
     *
     */
    void DigiClockWnd::OnDestroy(bool destroyThis)
    { DELETE_POINTER(m_pTray);

      Wnd::OnDestroy(destroyThis);
    } // of DigiClockWnd::OnDestroy()

    /**
     *
     */
    void DigiClockWnd::OnSize(WORD x,WORD y)
    { Wnd::OnSize(x,y);

      CalcLEDPos(m_childrenCount,x,y);
      MoveChildren();
    } // of DigiClockWnd::OnSize()

    /**
     *
     */
    void DigiClockWnd::OnMouse(MouseMsgT type,WORD x,WORD y)
    { switch( type )
      { 
      case LBUTTONDOWN:
        ::MessageBox(m_hWnd,_T("left button"),_T("Message Box"),MB_OK | MB_ICONINFORMATION);
        break;
      case RBUTTONDOWN:
        ::MessageBox(m_hWnd,_T("right button"),_T("Message Box"),MB_OK | MB_ICONINFORMATION);
        break;
      } // of switch
    } // of DigiClockWnd::OnMouse()

    /**
     *
     */
    void DigiClockWnd::MoveChildren()
    { int scrollXPos;
      int scrollYPos;

      GetScrollPosition(scrollXPos,scrollYPos);
      
      for( int i=0;i<m_childrenCount;i++ )
        ::MoveWindow(m_hLEDWnd[i],
                     m_LEDWndRect[i].left - scrollXPos,
                     m_LEDWndRect[i].top  - scrollYPos,
                     m_LEDWndRect[i].right,
                     m_LEDWndRect[i].bottom,
                     TRUE
                    );

      ::InvalidateRect(GetWindowHandle(),NULL,TRUE);                                     
    } // of DigiClockWnd::OnSize()

    /**
     *
     */
    void DigiClockWnd::OnFocus(bool setFocus,HWND hWnd)
    { Wnd::OnFocus(setFocus,hWnd);

      if( setFocus && m_focus!=-1 )
        ::SetFocus(m_hLEDWnd[m_focus]);
    } // of DigiClockWnd::OnFocus()

    /**
     *
     */
    void DigiClockWnd::OnKeyDown(WORD vKey)
    { switch( vKey )
      { case VK_TAB:
          { BOOL vkShift = ::GetKeyState(VK_SHIFT) & 0x8000;

            for( int i=m_childrenCount;i>0;i-- )
            { m_focus = m_focus + (vkShift ? m_childrenCount-1 : 1);
            
              m_focus %= m_childrenCount;

              HWND newWnd = ::SetFocus(m_hLEDWnd[m_focus]);

              if( newWnd!=NULL )
                break;
            } // of for
          }
          break;
      } // of switch
    } // of DigiClockWnd::OnKeyDown()

    /**
     *
     */
    void DigiClockWnd::OnInitMenu(HMENU hMenu)
    { Wnd::OnInitMenu(hMenu);

      HWND hWnd      = GetFocusWnd();
      bool isRunning = false;

      if( hWnd!=NULL )
        isRunning = ::SendMessage(hWnd,LEDM_ISRUNNING,0,0L)>0 ;

      ::EnableMenuItem(hMenu,IDM_STARTCLOCK,MF_BYCOMMAND | (hWnd!=NULL && !isRunning ? MF_ENABLED : MF_DISABLED|MF_GRAYED) );
      ::EnableMenuItem(hMenu,IDM_STOPCLOCK ,MF_BYCOMMAND | (hWnd!=NULL &&  isRunning ? MF_ENABLED : MF_DISABLED|MF_GRAYED) );

      ::CheckMenuItem(hMenu,IDM_TRANSPARENT ,MF_BYCOMMAND | ((NULL!=m_pApplication && m_pApplication->ShouldBeTransparent()) ? MF_CHECKED : MF_UNCHECKED) );
    } // of DigiClockWnd::OnInitMenu()

    /**
     *
     */
    void DigiClockWnd::OnContextMenu(WORD xPos,WORD yPos)
    { POINT pt = { xPos,yPos };

      if( xPos==0xffff && yPos==0xffff )
      { pt.x = m_size.cx/2;
        pt.y = m_size.cy/2;

        ClientToScreen(pt); 
      } // of if

      HMENU hPopupMenu  = ::LoadMenu((HINSTANCE)GetWindowInstance(),MAKEINTRESOURCE(IDM_POPUP_DIGICLOCK));
      HMENU hPopupMenu1 = ::GetSubMenu(hPopupMenu,0);

      ::TrackPopupMenu(hPopupMenu1, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, 0, GetWindowHandle(), NULL); 

      ::DestroyMenu(hPopupMenu);
    } // of DigiClockWnd::OnContextMenu()

    /**
     *
     */
    void DigiClockWnd::OnShellNotify(WPARAM iconId,LPARAM lParam)
    { POINT pt;

      switch( lParam )
      { 
      case WM_RBUTTONDOWN:
      case WM_CONTEXTMENU:
        ::SetForegroundWindow(GetWindowHandle());
        ::GetCursorPos(&pt);
        OnContextMenu((WORD)pt.x,(WORD)pt.y);
        ::PostMessage(GetWindowHandle(), WM_NULL, 0, 0);

        m_pTray->SetFocus(iconId);
        break;
      }
    } // of DigiClockWnd::OnShellNotify()

    /**
     *
     */
    void DigiClockWnd::OnHScroll(WORD request,WORD pos)
    { OnScroll(request,SB_HORZ); }

    /**
     *
     */
    void DigiClockWnd::OnVScroll(WORD request,WORD pos)
    { OnScroll(request,SB_VERT); }

    /**
     *
     */
    void DigiClockWnd::OnScroll(WORD request,int nBar)
    { SCROLLINFO sib;

      sib.cbSize = sizeof(sib);
      sib.fMask  = SIF_ALL;
      ::GetScrollInfo(GetWindowHandle(),nBar,&sib);

      int horzPos = sib.nPos;

      switch( request )
      { 
      case SB_LEFT:
        sib.nPos = sib.nMin;
        break;
      case SB_LINELEFT:
        sib.nPos--;
        break;
      case SB_LINERIGHT:
        sib.nPos++;
        break;
      case SB_PAGELEFT:
        sib.nPos -= 10;
        break;
      case SB_PAGERIGHT:
        sib.nPos += 10;
        break;
      case SB_RIGHT:
        sib.nPos = sib.nMax;
        break;
      case SB_THUMBTRACK:
        sib.nPos = sib.nTrackPos;
        break;
      } // of switch

      sib.fMask = SIF_POS;
      ::SetScrollInfo(GetWindowHandle(),nBar,&sib,TRUE);
      ::GetScrollInfo(GetWindowHandle(),nBar,&sib);

      if( sib.nPos!=horzPos )
        MoveChildren();
    } // of DigiClockWnd::OnScroll()
#pragma endregion

    /**
     *
     */
    LRESULT CALLBACK LEDWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
    { return gApp!=NULL ? gApp->CallWindowProc(hWnd,msg,wParam,lParam) : 0; }
  } // of namespace win
} // of namespace bvr20983

/**
 * This function initializes the application and processes the
 * message loop.
 */
int WINAPI ::WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{ DigiClockApp app(hInstance,lpCmdLine,nCmdShow);

  gApp = &app;

  return app.Main();
} // of WinMain()
/*==========================END-OF-FILE===================================*/
