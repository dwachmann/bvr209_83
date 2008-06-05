/*
 * $Id$
 * 
 * a digital clock window class.
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
#include <sstream>
#include <iomanip>
#include "win/ledwnd.h"
#include "win/wndclass.h"
#include "win/clipboard.h"
#include "win/colordialog.h"
#include "util/logstream.h"
#include "bvr20983cc-res.h"

using namespace bvr20983;
using namespace bvr20983::win;

POINT bvr20983::win::LEDWnd::m_ptSegment[7][6] = 
{ { 5, 4, 9, 0,29, 0,33, 4,29, 8, 9, 8}, // A
  { 4, 5, 8, 9, 8,29, 4,33, 0,29, 0, 9}, // F
  {34, 5,38, 9,38,29,34,33,30,29,30, 9}, // B
  { 5,34, 9,30,29,30,33,34,29,38, 9,38}, // G
  { 4,35, 8,39, 8,59, 4,63, 0,59, 0,39}, // E
  {34,35,38,39,38,59,34,63,30,59,30,39}, // C
  { 5,64, 9,60,29,60,33,64,29,68, 9,68}  // D
};

/*
 .-A-.     
 |   |     
 F   B
 |   |
 .-G-.
 |   |
 E   C
 |   |
 .-D-.
 */

BOOL bvr20983::win::LEDWnd::m_fSevenSegment[37][7] = 
{ 
// A F B G E C D
  {0,0,0,0,0,0,0}, // blank
  {1,1,1,0,1,1,1}, // 0
  {0,0,1,0,0,1,0}, // 1
  {1,0,1,1,1,0,1}, // 2
  {1,0,1,1,0,1,1}, // 3
  {0,1,1,1,0,1,0}, // 4
  {1,1,0,1,0,1,1}, // 5
  {1,1,0,1,1,1,1}, // 6
  {1,0,1,0,0,1,0}, // 7
  {1,1,1,1,1,1,1}, // 8
  {1,1,1,1,0,1,1}, // 9
  {1,1,1,1,1,1,0}, // A
  {1,1,1,1,1,1,1}, // B
  {1,1,0,0,1,0,1}, // C
  {0,0,1,1,1,1,1}, // D
  {1,1,0,1,1,0,1}, // E
  {1,1,0,1,1,0,0}, // F
  {1,1,1,1,0,1,1}, // G
  {0,1,1,1,1,1,0}, // H
  {0,0,1,0,0,1,0}, // I
  {0,0,1,0,0,1,1}, // J
  {1,1,0,1,1,1,0}, // K
  {0,1,0,0,1,0,1}, // L
  {1,1,1,0,1,1,0}, // M
  {1,1,1,0,1,1,0}, // N
  {1,1,1,0,1,1,1}, // O
  {1,1,1,1,1,0,0}, // P
  {1,1,1,1,0,1,0}, // Q
  {0,0,0,1,1,0,0}, // R
  {1,1,0,1,0,1,1}, // S
  {0,1,0,1,1,0,1}, // T
  {0,1,1,0,1,1,1}, // U
  {0,1,1,0,1,1,1}, // V 
  {0,1,1,0,1,1,1}, // W
  {0,1,1,1,1,1,0}, // X
  {0,1,1,1,0,1,1}, // Y
  {1,0,1,1,1,0,1}  // Z
}; 

/*
 .-A1-.-A2-.
 |\   |   /|
 | H  I  J |
 F  \ | /  B
 |   \|/   |
 .-G1-.-G2-.
 |   /|\   |
 E  / | \  C
 | K  L  M |
 |/   |   \|
 .-D1-.-D2-.
 */

BOOL bvr20983::win::LEDWnd::m_fSixteenSegment[37][16] = 
{ 
// A1A2B C D2D1E F G1G2I L H J M K
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // blank
  {1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1}, // 0
  {0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0}, // 1
  {1,1,1,0,1,1,1,0,1,1,0,0,0,0,0,0}, // 2
  {1,1,1,1,1,1,0,0,0,1,0,0,0,0,0,0}, // 3
  {0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0}, // 4
  {1,1,0,1,1,1,0,1,1,1,0,0,0,0,0,0}, // 5
  {1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0}, // 6
  {0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0}, // 7
  {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0}, // 8
  {1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0}, // 9

  {1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0}, // A
  {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0}, // B
  {1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0}, // C
  {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0}, // D
  {1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0}, // E
  {1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0}, // F
  {1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0}, // G
  {0,0,1,1,0,0,1,1,1,1,0,0,0,0,0,0}, // H
  {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0}, // I
  {0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0}, // J
  {0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,0}, // K
  {0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0}, // L
  {0,0,1,1,0,0,1,1,0,0,0,0,1,1,0,0}, // M
  {0,0,1,1,0,0,1,1,0,0,0,0,1,0,1,0}, // N
  {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0}, // O
  {1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0}, // P
  {1,1,1,1,1,1,0,1,1,1,0,0,0,0,0,0}, // Q
  {1,1,1,0,0,0,1,1,1,1,0,0,0,0,1,0}, // R
  {1,1,0,1,1,1,0,1,1,1,0,0,0,0,0,0}, // S
  {1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0}, // T
  {0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0}, // U
  {0,0,1,1,0,0,0,0,0,0,0,0,1,0,1,0}, // V
  {0,0,1,1,0,0,1,1,0,0,0,0,0,0,1,1}, // W
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1}, // X
  {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1}, // Y
  {1,1,0,0,1,1,0,0,0,0,0,0,0,1,0,1}  // Z
}; 


POINT bvr20983::win::LEDWnd::m_ptSegment16[16][6] = 
{ { 5, 4, 9, 0,16, 0,20, 4,16, 8, 9, 8}, // A1
  {22, 4,26, 0,33, 0,37, 4,33, 8,26, 8}, // A2

  {38, 5,42, 9,42,28,38,32,34,28,34, 9}, // B
  {38,34,42,38,42,57,38,61,34,57,34,38}, // C

  {22,62,26,58,33,58,37,62,33,66,26,66}, // D2
  { 5,62, 9,58,16,58,20,62,16,66, 9,66}, // D1

  { 4,34, 8,38, 8,57, 4,61, 0,57, 0,38}, // E
  { 4, 5, 8, 9, 8,28, 4,32, 0,28, 0, 9}, // F

  { 5,33, 9,29,16,29,20,33,16,37, 9,37}, // G1
  {22,33,26,29,33,29,37,33,33,37,26,37}, // G2

  {21, 5,25, 9,25,28,21,32,17,28,17, 9}, // I
  {21,34,25,38,25,57,21,61,17,57,17,38}, // L

  {10,10,14,10,18,18,20,32,18,35,10,19}, // H
  {32,10,32,19,24,35,22,32,24,18,28,10}, // J
  {32,56,28,56,24,48,22,34,24,31,32,47}, // M
  {10,56,10,47,18,31,20,34,18,48,14,56}  // K
};

POINT bvr20983::win::LEDWnd::m_ptColonDecimalPoint[4][4] = 
{ {4,18, 8,22,4,26,0,22},
  {4,40, 8,44,4,48,0,44},
  {4,58, 8,62,4,66,0,62},
  {4,58, 8,62,0,66,2,62}
};

BOOL bvr20983::win::LEDWnd::m_fColon[2][2] =
{ { 0,0 },
  { 1,1 }
};

namespace bvr20983
{
  namespace win
  {
#pragma region Construction & Deconstruction

    /**
     *
     */
    Wnd* LEDWndFactory::Construct(HWND hWnd)
    { return new LEDWnd(hWnd); }

    /**
     *
     */
    void LEDWnd::RegisterClass(HINSTANCE hInst,WORD wIcon,HBRUSH backgroundBrush,UINT style)
    { WndClass::RegisterWindowClass(LEDWNDCLS,new LEDWndFactory(),hInst,wIcon,backgroundBrush,style,&WndProc); }

    /**
     *
     */
#pragma warning( disable:4355 )
    LEDWnd::LEDWnd(HWND hWnd) : Wnd(hWnd),m_thread(this,true)
    { m_suppress         = false;
      m_24hour           = false;
      m_use16segment     = false;
      m_clock            = false;
      m_hFgBrush         = NULL;
      m_hFg1Brush        = NULL;
      m_hBgBrush         = NULL;
      m_pMemoryDC        = NULL;
      m_hFgFocusPen      = NULL;
      m_hFgPen           = NULL;
      m_hFg1Pen          = NULL;
      m_charPos          = 0;
      m_hFgDisabledBrush = ::CreateSolidBrush(GetSysColor(COLOR_GRAYTEXT));
      m_hFgDisabledPen   = ::CreatePen(PS_SOLID, 1, GetSysColor(COLOR_GRAYTEXT));
      m_hPopupMenu       = NULL;
      m_hPopupMenu1      = NULL;
      m_fgColor          = 0;
      m_bgColor          = 0;
      m_textLen          = 0;
      m_textDisplayLen   = 0;
      m_pStatusCB        = NULL;
      m_useThread        = false;
      m_drawDirectInThread=false;
      m_isRunning         =false;
    } // of LEDWnd::LEDWnd()
#pragma warning( default:4355 )

    /**
     *
     */
    void LEDWnd::OnDestroy(bool destroyThis)
    { m_thread.StopThread();
      SetForegroundColor(WND_NULLCOLOR);
      SetForegroundColor1(WND_NULLCOLOR);
      SetBackgroundColor(WND_NULLCOLOR);
      DELETE_GDIOBJECT(m_hFgDisabledBrush);
      DELETE_GDIOBJECT(m_hFgDisabledPen);
      DELETE_POINTER(m_pMemoryDC);

      if( m_hPopupMenu!=NULL )
        ::DestroyMenu(m_hPopupMenu);

      Wnd::OnDestroy(destroyThis);
    } // of LEDWnd::OnDestroy()

    /**
     *
     */
    void LEDWnd::SetForegroundColor(COLORREF fgColor,bool isOle)
    { m_fgColor = isOle ? OleColor2ColorRef(fgColor) : fgColor;

      DELETE_GDIOBJECT(m_hFgBrush);
      DELETE_GDIOBJECT(m_hFgFocusPen);
      DELETE_GDIOBJECT(m_hFgPen);

      if( fgColor!=WND_NULLCOLOR )
      { int r = GetRValue(fgColor);
        int g = GetGValue(fgColor);
        int b = GetBValue(fgColor);

        m_hFgBrush      = ::CreateSolidBrush(fgColor);
        m_hFgFocusPen   = ::CreatePen(PS_SOLID, 1, RGB( (r + 32 ) % 256,(g + 32 ) % 256,(b + 32 ) % 256));
        m_hFgPen        = ::CreatePen(PS_SOLID, 1, fgColor);
      } // of if
    } // of LEDWnd::SetForegroundColor()

    /**
     *
     */
    void LEDWnd::SetForegroundColor1(COLORREF fgColor1,bool isOle)
    { m_fgColor1 = isOle ? OleColor2ColorRef(fgColor1) : fgColor1;

      DELETE_GDIOBJECT(m_hFg1Brush);
      DELETE_GDIOBJECT(m_hFg1Pen);

      if( fgColor1!=WND_NULLCOLOR )
      { m_hFg1Brush = ::CreateSolidBrush(fgColor1);
        m_hFg1Pen   = ::CreatePen(PS_SOLID, 1, fgColor1);
      } // of if
    } // of LEDWnd::SetForegroundColor1()

    /**
     *
     */
    void LEDWnd::SetBackgroundColor(COLORREF bgColor,bool isOle)
    { bgColor   = isOle ? OleColor2ColorRef(bgColor) : bgColor;
      m_bgColor = bgColor!=WND_NULLCOLOR ? bgColor : 0;

      HBRUSH oldBgBrush = m_hBgBrush;

      DELETE_GDIOBJECT(m_hBgBrush);

      m_hBgBrush = bgColor!=WND_NULLCOLOR ? ::CreateSolidBrush(bgColor) : NULL;

      if( NULL==oldBgBrush && NULL!=m_hBgBrush && m_size.cx>0 && m_size.cy>0 && NULL==m_pMemoryDC )
      { m_pMemoryDC = new MemoryDC(DC(m_hWnd),m_size.cx,m_size.cy);

        OnTimer();
      } // of if
      else if( NULL!=oldBgBrush && NULL==m_hBgBrush && NULL!=m_pMemoryDC )
      { DELETE_POINTER(m_pMemoryDC); }
    } // of LEDWnd::SetBackgroundColor()
#pragma endregion

#pragma region Implementation
    /**
     *
     */
    void LEDWnd::GetTextLen()
    { TString t;

      GetWindowText(t);

      m_textLen = 0;

      m_ledDisplay.clear();
      m_ledDisplayAdd.clear();

      int tLen = -1;

      for( int i=0;t[i]!=_T('\0');i++ )
      { TCHAR c = t[i];

        if( (_T('0')<=c && c<=_T('9')) ||
            (_T('A')<=c && c<=_T('Z')) ||
            _T(' ')==c
          )
        { tLen++;

          m_ledDisplay    += c;
          m_ledDisplayAdd += _T('\0');
        } // of if
        else if( _T('.')==c && tLen>=0 )
          m_ledDisplayAdd[tLen] |= LED_POINT;
        else if( _T(':')==c && tLen>=0 )
          m_ledDisplayAdd[tLen] |= LED_COLON;
      } // of for

      tLen++;

      m_textLen = tLen;
    } // of LEDWnd::GetTextLen()

    /**
     *
     */
    void LEDWnd::TransformPolygon(const POINT* lpPoint,int nCount,
                                  const SIZEL& windowOrig,const SIZEL& windowExt,const SIZEL& viewportOrig,const SIZEL& viewportExt,
                                  POINT* lpTrxPoint
                                 )
    { float v2w_x = (float)viewportExt.cx/(float)windowExt.cx;
      float v2w_y = (float)viewportExt.cy/(float)windowExt.cy;

      float v2w_iso = v2w_y<v2w_x ? v2w_y : v2w_x;

      for( int i=0;i<nCount;i++ )
      { lpTrxPoint[i].x = (LONG) ( (lpPoint[i].x + windowOrig.cx)*v2w_iso+viewportOrig.cx);
        lpTrxPoint[i].y = (LONG) ( (lpPoint[i].y + windowOrig.cy)*v2w_iso+viewportOrig.cy);
      } // of for
    } // of TransformPolygon()

    /**
     *
     */
    void LEDWnd::DisplayPolygon(DCBase& dc,
                                const SIZEL& windowOrig,const SIZEL& windowExt,const SIZEL& viewportOrig,const SIZEL& viewportExt,
                                POINT* p,int p0,int p1,BOOL segOn[]
                               )
    { POINT pTrx[10];
    
      //dc.SetWindowOrg(-xOff,-yOff);

      dc.SelectObject(m_hFg1Pen!=NULL   ? m_hFg1Pen   : (HPEN)::GetStockObject(BLACK_PEN));
      dc.SelectObject(m_hFg1Brush!=NULL ? m_hFg1Brush : (HBRUSH)::GetStockObject(BLACK_BRUSH));

      for( int iSeg=0;iSeg<p0;iSeg++ )
      { POINT* px = p+iSeg*p1;

        if( !segOn[iSeg] )
        { TransformPolygon(px,p1,windowOrig,windowExt,viewportOrig,viewportExt,pTrx);

          dc.Polygon(pTrx,p1);
        } // of if
      } // of for

      if( IsEnabled() )
      { dc.SelectObject(GetFocus() ? m_hFgFocusPen : GetStockObject(NULL_PEN));
        dc.SelectObject(m_hFgBrush);
        dc.SetBkMode(TRANSPARENT);
      } // of if
      else
      { dc.SelectObject(m_hFgDisabledPen);
        dc.SelectObject(m_hFgDisabledBrush);
      } // of else

      for( int iSeg=0;iSeg<p0;iSeg++ )
      { POINT* px = p+iSeg*p1;

        if( segOn[iSeg] )
        { TransformPolygon(px,p1,windowOrig,windowExt,viewportOrig,viewportExt,pTrx);

          dc.Polygon(pTrx,p1);
        } // of if
      } // of for
    } // of LEDWnd::DisplayPolygon()

    /**
     *
     */
    void LEDWnd::DisplayText(DCBase& dc,const SIZEL& windowExt,const SIZEL& viewportOrig,const SIZEL& viewportExt)
    { SIZEL windowOrig;

      windowOrig.cx = m_use16segment ? -52 : -48;
      windowOrig.cy = 0;

      for( size_t i=0;i<m_textDisplayLen;i++ )
      { TCHAR c  = m_ledDisplay[(i+m_charPos)%m_textLen];
        TCHAR c1 = m_ledDisplayAdd[(i+m_charPos)%m_textLen];

        if( c!=_T(':') && c!=_T('.') )
          windowOrig.cx += m_use16segment ? 52 : 48;

        int iNumber = -1;

        if( _T(' ')==c )
          iNumber = 0;
        else if( _T('0')<=c && c<=_T('9') )
          iNumber = c - _T('0') + 1;
        else if( _T('A')<=c && c<=_T('Z') )
          iNumber = c - _T('A') + 1 + 10;

        if( iNumber>=0 )
        { if( m_use16segment )
            DisplayPolygon(dc,windowOrig,windowExt,viewportOrig,viewportExt,(POINT*)m_ptSegment16,16,6,m_fSixteenSegment[iNumber]);
          else
            DisplayPolygon(dc,windowOrig,windowExt,viewportOrig,viewportExt,(POINT*)m_ptSegment,7,6,m_fSevenSegment[iNumber]);
        } // of if

        SIZEL windowOrig1 = windowOrig;

        windowOrig1.cx += (m_use16segment ? 43 : 39);

        DisplayPolygon(dc,windowOrig1,windowExt,viewportOrig,viewportExt,(POINT*) m_ptColonDecimalPoint,   2,4,m_fColon[ c1&LED_COLON ? 1 : 0 ]);
        DisplayPolygon(dc,windowOrig1,windowExt,viewportOrig,viewportExt,(POINT*)(m_ptColonDecimalPoint+2),1,4,m_fColon[ c1&LED_POINT ? 1 : 0 ]);
      } // of for
    } // of LEDWnd::DisplayText()

    /**
     *
     */
    void LEDWnd::Draw(DCBase& dc,LPCRECTL prcBounds)
    { SIZEL viewportOrig;
      SIZEL viewportExt;
      SIZEL windowExt;

      if( NULL==prcBounds )
      { viewportOrig.cx = viewportOrig.cy = 0;
        viewportExt = m_size;
      } // of if
      else
      { viewportOrig.cx = prcBounds->left;
        viewportOrig.cy = prcBounds->top;
        viewportExt.cx  = prcBounds->right - prcBounds->left;
        viewportExt.cy  = prcBounds->bottom - prcBounds->top;
      } // of else
      
      if( m_hBgBrush!=NULL )
        dc.FillRect(m_hBgBrush,viewportOrig.cx,viewportOrig.cy,viewportExt.cx,viewportExt.cy);

      if( m_textDisplayLen>0 )
      { int charW = m_use16segment ? 52 : 48;
        int charH = m_use16segment ? 68 : 70;

        windowExt.cx = m_textDisplayLen * charW;
        windowExt.cy = charH;

        int cxMax = viewportExt.cy * windowExt.cx / windowExt.cy;
        int cyMax = viewportExt.cx * windowExt.cy / windowExt.cx;

        int cxOff = (viewportExt.cx - cxMax)/2;
        int cyOff = (viewportExt.cy - cyMax)/2;

        dc.SaveDC();

        if( cxOff>=0 )
          viewportOrig.cx += cxOff;

        if( cyOff>=0 )
          viewportOrig.cy += cyOff;
/*
        dc.SetMapMode(MM_ISOTROPIC);
        dc.SetWindowExt( windowExt.cx, windowExt.cy );
        dc.SetViewportExt( viewportExt.cx,viewportExt.cy );
        dc.SetViewportOrg( viewportOrig.cx,viewportOrig.cy );
*/        
        DisplayText(dc,windowExt,viewportOrig,viewportExt);

        dc.RestoreDC();
      } // of if
    } // of LEDWnd::Draw()
#pragma endregion

#pragma region Events
    /**
     *
     */
    void LEDWnd::OnCreate(LPCREATESTRUCT lpCS)
    { Wnd::OnCreate(lpCS);

      LONG style = lpCS->style;

      if( style & LEDS_CLOCK )
        m_clock = true;

      if( style & LEDS_24HOUR )
        m_24hour = true;

      if( style & LEDS_SUPPRESS )
        m_suppress = true;

      if( style & LEDS_16SEGEGMENT )
        m_use16segment = true;

      SetForegroundColor( GetSysColor(COLOR_WINDOWTEXT) );
      SetForegroundColor1( WND_NULLCOLOR );

      m_hPopupMenu  = ::LoadMenu((HINSTANCE)::GetClassLongPtr(GetWindowHandle(),GCLP_HMODULE),MAKEINTRESOURCE(IDM_POPUP_LEDWND));
      m_hPopupMenu1 = ::GetSubMenu(m_hPopupMenu,0);

      LPLEDCREATESTRUCT ledCS = (LPLEDCREATESTRUCT)lpCS->lpCreateParams;

      if( NULL!=ledCS )
      { if( ledCS->magic!=LEDWNDMAGIC || ledCS->dwSize<sizeof(LEDCREATESTRUCT) )
        { WndCreateStruct* pWndCS = (WndCreateStruct*)lpCS->lpCreateParams;

          if( pWndCS->magic==WNDMAGIC && pWndCS->dwSize>=sizeof(WndCreateStruct) )
            ledCS = (LPLEDCREATESTRUCT)pWndCS->pCreateParams;

          if( ledCS->magic!=LEDWNDMAGIC || ledCS->dwSize<sizeof(LEDCREATESTRUCT) )
            ledCS = NULL;
        } // of if
      } // of if

      ChangeState(false,true);
      GetTextLen();
      m_textDisplayLen = m_textLen;

      if( ledCS!=NULL && ledCS->magic==LEDWNDMAGIC && ledCS->dwSize>=sizeof(LEDCREATESTRUCT) )
      { if( !m_clock && ledCS->displayLen>0 )
          m_textDisplayLen = ledCS->displayLen;

        SetForegroundColor(ledCS->fgColor);
        SetForegroundColor1(ledCS->fgColor1);
        SetBackgroundColor(ledCS->bgColor);

        m_useThread          = ledCS->useThread;
        m_drawDirectInThread = ledCS->drawDirectInThread;
        m_pStatusCB          = (ILEDStatus*)ledCS->pStatusCB;

        if( ledCS->initialStart )
          Start(1000);
      } // of if
    } // of LEDWnd::OnCreate()

    /**
     *
     */
    void LEDWnd::OnSize(WORD w,WORD h)
    { Wnd::OnSize(w,h);
      
      DELETE_POINTER(m_pMemoryDC);

      if( w>0 && h>0 && NULL!=m_hBgBrush )
      { m_pMemoryDC = new MemoryDC(DC(m_hWnd),w,h);

        OnTimer();
      } // of if
    } // of LEDWnd::OnSize()

    /**
     *
     */
    void LEDWnd::OnFocus(bool setFocus,HWND hWnd)
    { Wnd::OnFocus(setFocus,hWnd);

      if( NULL!=m_pMemoryDC )
        Draw(*m_pMemoryDC);

      InvalidateRect();
    } // of LEDWnd::OnFocus()


    /**
     *
     */
    void LEDWnd::OnMouse(MouseMsgT type,WORD x,WORD y)
    { Wnd::OnMouse(type,x,y);

      if( type==LBUTTONDOWN && (GetWindowStyle()& WS_TABSTOP) )
        SetFocus(true);
    } // of LEDWnd::OnMouse()

    /**
     *
     */
    void LEDWnd::OnTimer()
    { RepaintLED();

      Wnd::OnTimer();
    } // of LEDWnd::OnTimer()

    /**
     *
     */
    void LEDWnd::RepaintLED(bool calcUpdateRect)
    { ChangeState(calcUpdateRect);

      if( NULL!=m_pMemoryDC  )
        Draw(*m_pMemoryDC);

      if( m_useThread )
      { if( m_drawDirectInThread )
          Repaint();
        else if( NULL!=m_pStatusCB )
          m_pStatusCB->OnLEDChanged();
      } // of if
      else
        InvalidateRect();
    } // of LEDWnd::RepaintLED()

    /**
     *
     */
    void LEDWnd::ChangeState(bool calcUpdateRect,bool onCreate)
    { TString s0    = m_ledDisplay;
      int     tLen0 = m_textLen;

      TString s1;

      if( m_clock )
      { SYSTEMTIME tm;

        ::GetLocalTime(&tm);

        basic_ostringstream<TCHAR> os;

        if( m_24hour )
        { if( !m_suppress )
            os<<setfill(_T('0'))<<setw(2);
          else
            os<<setfill(_T(' '))<<setw(2);
          
          os<<tm.wHour<<_T(":");
        } // of if
        else
        { if( !m_suppress )
            os<<setfill(_T('0'))<<setw(2);
          else
            os<<setfill(_T(' '))<<setw(2);
          
          os<<(tm.wHour==12 ? 12 : tm.wHour%12)<<_T(":");
        } // of else

        os<<setfill(_T('0'))<<setw(2)<<tm.wMinute<<_T(":");
        os<<setfill(_T('0'))<<setw(2)<<tm.wSecond;

        SetWindowText(os.str());

        GetTextLen();
      } // of if
      else if( !onCreate && m_textLen>0 )
        m_charPos = (m_charPos+1)%m_textLen;

      if( m_textLen>0 )
      { int x0   = 0;
        int x1   = m_textLen;

        if( m_clock && tLen0==m_textLen )
        { 
          for( WORD x=0;x<s0.length();x++ )
          { TCHAR c0 = s0[x];
            TCHAR c1 = m_ledDisplay[x];

            if( c0==c1 )
              x0++;
            else
              break;
          } // of for
        } // of if

        if( calcUpdateRect && m_textDisplayLen>0 )
        { int charW = m_use16segment ? 52 : 48;
          int charH = m_use16segment ? 68 : 70;

          int maxX  = m_textDisplayLen * charW;
          int maxY  = charH;

          int cxMax = m_size.cy * maxX / maxY;
          int cyMax = m_size.cx * maxY / maxX;

          if( cxMax>m_size.cx )
            cxMax = m_size.cx;

          if( cyMax>m_size.cy )
            cyMax = m_size.cy;

          int cx    = cxMax/m_textDisplayLen;
          int cy    = cyMax;

          int cxOff = (m_size.cx - cxMax)/2;
          int cyOff = (m_size.cy - cyMax)/2;

          RECT invalidRect;

          invalidRect.left   = cxOff<0 ? 0 : cxOff;
          invalidRect.top    = cyOff<0 ? 0 : cyOff;

          invalidRect.left  += cx*x0;

          invalidRect.right  = invalidRect.left + cx*(x1-x0);
          invalidRect.bottom = invalidRect.top  + cy;

          int cxCorr = cx/20;
          int cyCorr = cy/20;

          invalidRect.left   -= cxCorr;
          invalidRect.right  += cxCorr;

          invalidRect.top    -= cyCorr;
          invalidRect.bottom += cyCorr;

          AddInvalidRect(&invalidRect);
        } // of if
        else
          AddInvalidRect(NULL);
      } // of if
    } // of LEDWnd::ChangeState()

    /**
     *
     */
    HRESULT LEDWnd::GetNaturalExtent(DWORD dwAspect,DVEXTENTINFO* pExtentInfo,LPSIZEL psizel)
    { HRESULT hr = E_FAIL;

      if( NULL==psizel || NULL==pExtentInfo )
        hr = E_POINTER;
      else
      { int charW = m_use16segment ? 52 : 48;
        int charH = m_use16segment ? 68 : 70;

        GetTextLen();
        m_textDisplayLen = m_textLen;

        int maxX  = m_textDisplayLen * charW;
        int maxY  = charH;

        if( pExtentInfo->dwExtentMode==DVEXTENT_CONTENT )
        { psizel->cx = maxX;
          psizel->cy = maxY;

          hr = S_OK;
        } // of if
        else if( pExtentInfo->dwExtentMode==DVEXTENT_INTEGRAL )
        { psizel->cx = -1;

          int cxMax = pExtentInfo->sizelProposed.cy * maxX / maxY;

          psizel->cy = pExtentInfo->sizelProposed.cx * maxY / maxX;

          hr = S_OK;
        } // of else if
      } // of else

      return hr;
    } // of LEDWnd::GetNaturalExtent()

    /**
     *
     */
    void LEDWnd::SetClock(bool isClock)
    { m_clock = isClock; 

      RepaintLED(false);

      if( m_clock )
      { GetTextLen();

        m_textDisplayLen = m_textLen;
        m_charPos        = 0;
      } // of if

      RepaintLED(false);
    } // of LEDWnd::SetClock()

    /**
     *
     */
    void LEDWnd::Set24Hour(bool is24hour)
    { m_24hour = is24hour; 

      RepaintLED(false);
    } // LEDWnd::Set24Hour()

    /**
     *
     */
    void LEDWnd::SetZeroSuppress(bool suppress)
    { m_suppress = suppress; 

      RepaintLED(false);
    } // of LEDWnd::Set16Segment()

    /**
     *
     */
    void LEDWnd::Set16Segment(bool use16segment)
    { m_use16segment = use16segment; 

      RepaintLED(false);
    } // of LEDWnd::Set16Segment()

    /**
     *
     */
    void LEDWnd::SetText(const TString& s)
    { if( !m_clock )
      { SetWindowText(s);

        GetTextLen();

        m_textDisplayLen = m_textLen;
        m_charPos        = -1;

        RepaintLED(false);
      } // of if
    } // of LEDWnd::SetText()

    /**
     *
     */
    void LEDWnd::OnKeyDown(WORD vKey)
    { switch( vKey )
      { case VK_SPACE:
          if( IsRunning() )
            Stop();
          else
            Start(1000);
          break;
        default:
          break;
      } // of switch
    } // of LEDWnd::OnKeyDown(WORD vKey)

    /**
     *
     */
    void LEDWnd::OnContextMenu(WORD xPos,WORD yPos)
    { POINT pt = { xPos,yPos };

      if( xPos==0xffff && yPos==0xffff )
      { pt.x = m_size.cx/2;
        pt.y = m_size.cy/2;

        ClientToScreen(pt); 
      } // of if

      ::TrackPopupMenu(m_hPopupMenu1, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, GetWindowHandle(), NULL); 
    } // of LEDWnd::OnContextMenu()

    /**
     *
     */
    void LEDWnd::OnInitMenu(HMENU hMenu)
    { Wnd::OnInitMenu(hMenu);

      ::EnableMenuItem(m_hPopupMenu1,IDM_LED_START,MF_BYCOMMAND | (!IsRunning() ? MF_ENABLED : MF_DISABLED|MF_GRAYED) );
      ::EnableMenuItem(m_hPopupMenu1,IDM_LED_STOP,MF_BYCOMMAND | (IsRunning() ? MF_ENABLED : MF_DISABLED|MF_GRAYED) );

      ::CheckMenuItem(m_hPopupMenu1,IDM_LED_16SEGMENT,MF_BYCOMMAND | (m_use16segment ? MF_CHECKED : MF_UNCHECKED) );
      ::CheckMenuItem(m_hPopupMenu1,IDM_LED_SUPPRESS,MF_BYCOMMAND | (m_suppress ? MF_CHECKED : MF_UNCHECKED) );
      ::CheckMenuItem(m_hPopupMenu1,IDM_LED_24HOUR,MF_BYCOMMAND | (m_24hour ? MF_CHECKED : MF_UNCHECKED) );
    }

    /**
     *
     */
    void LEDWnd::Start(UINT uMillis)
    { m_tickInMillis = uMillis;

      if( m_useThread )
        m_thread.StartThread();
      else
        StartTimer(m_tickInMillis);

      m_isRunning = true;

      if( NULL!=m_pStatusCB )
        m_pStatusCB->OnLEDStarted();
    }

    /**
     *
     */
    void LEDWnd::Stop()
    { if( m_useThread )
        m_thread.StopThread();
      else
        StopTimer();

      m_isRunning = false;

      if( NULL!=m_pStatusCB )
        m_pStatusCB->OnLEDStopped();
    }

    /**
     *
     */
    bool LEDWnd::OnCommand(WPARAM command)
    { bool result  = false;

      switch( command )
      { 
      case IDM_LED_START:
        Start(1000);
        break;
      case IDM_LED_STOP:
        Stop();
        break;
      case IDM_LED_16SEGMENT:
        m_use16segment = !m_use16segment;

        OnTimer();
        break;
      case IDM_LED_SUPPRESS:
        m_suppress = !m_suppress;

        OnTimer();
        break;
      case IDM_LED_24HOUR:
        m_24hour = !m_24hour;

        OnTimer();
        break;

      case IDM_LED_FGCOLOR:
      case IDM_LED_FG1COLOR:
      case IDM_LED_BGCOLOR:
        { ColorDialog ccDlg(GetWindowHandle(),command==IDM_LED_FGCOLOR ? m_fgColor : (command==IDM_LED_FG1COLOR ? m_fgColor1 : m_bgColor));

          if( ccDlg.Show() )
          { if( command==IDM_LED_FGCOLOR )
              SetForegroundColor(ccDlg.GetResult());
            else if( command==IDM_LED_FG1COLOR )
              SetForegroundColor1(ccDlg.GetResult());
            else
              SetBackgroundColor(ccDlg.GetResult());

            OnTimer();
          } // of if
        }
        break;
      case IDC_EDIT_COPY:
        { Clipboard clp(m_hWnd);

          clp.EmptyClipboard();

          TString text;

          GetWindowText(text);

          clp.AddData(text);

          if( m_pMemoryDC!=NULL )
          { MemoryDC memDC1(*m_pMemoryDC,false);

            clp.AddData(memDC1);

            DC screenDC(m_hWnd);

            int iWidthMM    = screenDC.GetDeviceCaps(HORZSIZE); 
            int iHeightMM   = screenDC.GetDeviceCaps(VERTSIZE); 
            int iWidthPels  = screenDC.GetDeviceCaps(HORZRES); 
            int iHeightPels = screenDC.GetDeviceCaps(VERTRES); 

            RECT rect;
             
            rect.left   = 0;
            rect.top    = 0;
            rect.right  = (m_size.cx * iWidthMM  * 100)/iWidthPels; 
            rect.bottom = (m_size.cy * iHeightMM * 100)/iHeightPels; 

            MetafileDC mfDC(*m_pMemoryDC,&rect,false);

            Draw(mfDC);

            mfDC.Close();

            clp.AddData(mfDC);
          } // of if
        }
        result = true;
        break;
      } // of switch

      return result;
    } // of LEDWnd::OnCommand()

    /**
     *
     */
    void LEDWnd::OnDraw(DCBase& dc,LPCRECTL prcBounds)
    { LPCRECT rr = dc.GetRepaintRect();

      if( NULL!=m_pMemoryDC  )
      { if( NULL!=prcBounds )
          dc.StretchBlt(prcBounds->left,prcBounds->top,
                        prcBounds->right-prcBounds->left,prcBounds->bottom-prcBounds->top,
                        *m_pMemoryDC,
                        0,0,
                        m_size.cx,m_size.cy
                       );
        else 
        { if( NULL!=rr )
            dc.BitBlt(rr->left,rr->top,rr->right,rr->bottom,*m_pMemoryDC,rr->left,rr->top,SRCCOPY);
          else
            dc.BitBlt(0,0,m_size.cx,m_size.cy,*m_pMemoryDC);
        } // of else
      } // of if
      else
        Draw(dc,prcBounds);

/*
      if( NULL!=rr )
      { dc.SaveDC();
        HPEN hPen  = ::CreatePen(PS_SOLID,1,RGB(255,0,0));

        dc.SelectObject(hPen);
        dc.SelectObject(::GetStockObject(NULL_BRUSH));

        dc.Rectangle(rr);

        dc.RestoreDC();

        DELETE_GDIOBJECT(hPen);
      } // of if
*/
    } // of LEDWnd::OnDraw()
#pragma endregion

    /**
     *
     */
    bool LEDWnd::InitThread()
    { HRESULT hr = ::CoInitializeEx(NULL,
                                    COINIT_APARTMENTTHREADED
                                    //COINIT_MULTITHREADED
                                   );

      return SUCCEEDED(hr) ? true : false; 
    } // of LEDWnd::InitThread()

    /**
     *
     */
    void LEDWnd::ExitThread(HRESULT hr)
    { ::CoUninitialize();
    } // of LEDWnd::ExitThread()

    /**
     *
     */
    void LEDWnd::RunThread(HANDLE hWaitEvent)
    { DWORD r = 0;
    
      do
      { r = ::WaitForSingleObject(hWaitEvent,m_tickInMillis);

        if( r==WAIT_TIMEOUT )
          RepaintLED();
      } while( r!=WAIT_OBJECT_0 );
    } // of LEDWnd::RunThread()

#pragma region Windows Event Handling
    /**
     *
     */
    LRESULT LEDWnd::WindowProcedure(UINT msg,WPARAM wParam,LPARAM lParam)
    { LRESULT result = 0;

      switch( msg )
      {
      case LEDM_STARTCLOCK:
        Start(1000);
        break;
      case LEDM_STOPCLOCK:
        Stop();
        break;
      case LEDM_ISRUNNING:
        result = IsRunning();
        break;
      default:
        result = Wnd::WindowProcedure(msg,wParam,lParam);
        break;
      } // of switch

      return result;
    } // of Wnd::WindowProcedure()
#pragma endregion
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
