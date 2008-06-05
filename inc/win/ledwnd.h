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
#if !defined(LEDWND_H)
#define LEDWND_H

#include "os.h"
#include "bvr20983cc.h"
#include "win/wnd.h"
#include "win/dc.h"

#define LED_COLON 0x01
#define LED_POINT 0x02

namespace bvr20983
{
  namespace win
  {
    class LEDWnd;

    /**
     *
     */
    struct LEDWndFactory : WndFactory
    { virtual Wnd* Construct(HWND hWnd); };

    /**
     *
     */
    struct ILEDStatus
    { virtual void OnLEDStarted()=0;
      virtual void OnLEDStopped()=0;
      virtual void OnLEDChanged()=0;
    }; // of struct ILEDStatus

    /**
     *
     */
    class LEDWnd : public Wnd, public ThreadIF
    {
      public:
        static void RegisterClass(HINSTANCE hInst,
                                  WORD wIcon=0,
                                  HBRUSH backgroundBrush=(HBRUSH)::GetStockObject(WHITE_BRUSH),
                                  UINT style=CS_HREDRAW | CS_VREDRAW
                                 );

        void SetForegroundColor(COLORREF fgColor,bool isOle=false);
        void SetForegroundColor1(COLORREF fgColor1,bool isOle=false);
        void SetBackgroundColor(COLORREF bgColor,bool isOle=false);
        void SetUseThread(bool useThread)
        { m_useThread = useThread; }

        void SetClock(bool isClock);
        void Set24Hour(bool is24hour);
        void SetZeroSuppress(bool suppress);
        void Set16Segment(bool use16segment);
        void SetText(const TString& s);

        bool GetClock() const
        { return m_clock; }

        bool Get24Hour() const
        { return m_24hour; }

        bool GetZeroSuppress() const
        { return m_suppress; }

        bool Get16Segment() const
        { return m_use16segment; }

        bool IsRunning() const
        { return m_isRunning; }

        void GetText(TString& s)
        { GetWindowText(s); } 

        void SetStatusCallback(ILEDStatus* pStatusCB)
        { m_pStatusCB = pStatusCB; }
        
        virtual void    OnCreate(LPCREATESTRUCT lpCS);
        virtual void    OnSize(WORD w,WORD h);
        virtual void    OnDraw(DCBase& dc,LPCRECTL prcBounds=NULL);
        virtual void    OnDestroy(bool destroyThis);
        virtual void    OnTimer();
        virtual void    OnFocus(bool setFocus,HWND hWnd);
        virtual void    OnMouse(MouseMsgT type,WORD x,WORD y);
        virtual void    OnKeyDown(WORD vKey);
        virtual bool    OnCommand(WPARAM command);
        virtual void    OnContextMenu(WORD xPos,WORD yPos);
        virtual void    OnInitMenu(HMENU hMenu);


        virtual HRESULT GetNaturalExtent(DWORD dwAspect,DVEXTENTINFO* pExtentInfo,LPSIZEL psizel);

        void    Start(UINT uMillis);
        void    RepaintLED(bool calcUpdateRect=true);
        void    Stop();

        virtual bool    InitThread();
        virtual void    ExitThread(HRESULT hr);
        virtual void    RunThread(HANDLE hWaitEvent);

        virtual LRESULT WindowProcedure(UINT msg,WPARAM wParam,LPARAM lParam);

        friend struct LEDWndFactory;

      protected:
        LEDWnd(HWND hWnd);

      private:
        void DisplayText(DCBase& dc,const SIZEL& windowExt,const SIZEL& viewportOrig,const SIZEL& viewportExt);
        void DisplayPolygon(DCBase& dc,const SIZEL& windowOrig,const SIZEL& windowExt,const SIZEL& viewportOrig,const SIZEL& viewportExt,POINT* p,int p0,int p1,BOOL segOn[]);
        void Draw(DCBase& dc,LPCRECTL prcBounds=NULL);
        void ChangeState(bool calcUpdateRect=true,bool onCreate=false);
        void GetTextLen();
        void TransformPolygon(const POINT* lpPoint,int nCount,const SIZEL& windowOrig,const SIZEL& windowExt,const SIZEL& viewportOrig,const SIZEL& viewportExt,POINT* lpTrxPoint);

        static BOOL  m_fSevenSegment[37][7];
        static POINT m_ptSegment[7][6];

        static BOOL  m_fSixteenSegment[37][16];
        static POINT m_ptSegment16[16][6];
        static POINT m_ptColonDecimalPoint[4][4];

        static BOOL  m_fColon[2][2];

        HPEN        m_hFgFocusPen;
        HPEN        m_hFgPen;
        HBRUSH      m_hFgBrush;
        HBRUSH      m_hFg1Brush;
        HPEN        m_hFg1Pen;
        HBRUSH      m_hFgDisabledBrush;
        HPEN        m_hFgDisabledPen;
        HBRUSH      m_hBgBrush;
        COLORREF    m_fgColor;
        COLORREF    m_fgColor1;
        COLORREF    m_bgColor;
        MemoryDC*   m_pMemoryDC;
        HMENU       m_hPopupMenu;
        HMENU       m_hPopupMenu1;

        bool        m_24hour;
        bool        m_suppress;
        bool        m_use16segment;
        bool        m_clock;
        UINT        m_charPos;
        UINT        m_textLen;
        UINT        m_textDisplayLen;
        TString     m_ledDisplay;
        TString     m_ledDisplayAdd;
        UINT        m_tickInMillis;
        bool        m_useThread;
        bool        m_drawDirectInThread;
        Thread      m_thread;
        bool        m_isRunning;

        ILEDStatus* m_pStatusCB;
    }; // of class LEDWnd
  } // of namespace win
} // of namespace bvr20983
#endif // LEDWND_H