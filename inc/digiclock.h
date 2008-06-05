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
#if !defined(DIGICLOCKAPP_H)
#define DIGICLOCKAPP_H

#include "os.h"
#include "win/wnd.h"
#include "win/app.h"
#include "win/dc.h"

namespace bvr20983
{
  namespace win
  {
    class Tray;

    class DigiClockWnd;
    class DigiClockApp;

    /**
     *
     */
    struct DigiClockWndFactory : WndFactory
    { virtual Wnd* Construct(HWND hWnd); };

    /**
     *
     */
    class DigiClockWnd : public Wnd
    {
      public:
        ~DigiClockWnd();

        virtual void OnCreate(LPCREATESTRUCT lpCS);
        virtual void OnSize(WORD x,WORD y);
        virtual void OnFocus(bool setFocus,HWND hWnd);
        virtual void OnKeyDown(WORD vKey);
        virtual void OnHScroll(WORD request,WORD pos);
        virtual void OnVScroll(WORD request,WORD pos);
        virtual void OnInitMenu(HMENU hMenu);
        virtual void OnContextMenu(WORD xPos,WORD yPos);
        virtual void OnDestroy(bool destroyThis);
        virtual void OnShellNotify(WPARAM iconId,LPARAM lParam);
        virtual void OnMouse(MouseMsgT type,WORD x,WORD y);

        HWND GetFocusWnd() const
        { return m_focus!=-1 ? m_hLEDWnd[m_focus] : NULL; }

        void CreateChildren(int childrenCount);

        LRESULT CallWindowProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

        void SetApplication(DigiClockApp* pApplication)
        { m_pApplication = pApplication; }

        friend DigiClockWndFactory;

      private:
        DigiClockWnd(HWND hWnd);

        void CalcLEDPos(int num,int maxW,int maxH);
        void MoveChildren();

        void OnScroll(WORD request,int nBar);

        HWND    m_hLEDWnd[6];
        WNDPROC m_hLEDWndProc[6];
        RECT    m_LEDWndRect[6];
        int     m_focus;
        int     m_childrenCount;
        Tray*   m_pTray;
        
        DigiClockApp* m_pApplication;

        static LONG    m_hLEDWndStyle[6];
        static LPCTSTR m_hLEDText[6];
    }; // of class DigiClockWnd

    /**
     *
     */
    class DigiClockApp : public App, public ICommand
    {
      public:
        DigiClockApp(HINSTANCE hInstance,LPSTR lpCmdLine,int nCmdShow);

        void OnCommand(WPARAM cmd);

        LRESULT CallWindowProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

        bool ShouldBeTransparent() const
        { return m_transparent; }

        int GetNoWindows() const
        { return m_noWnd; }

      private:
        void Init(int nCmdShow);

        bool m_transparent;
        int  m_noWnd;
    }; // of class DigiClockApp

    LRESULT CALLBACK LEDWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
  } // of namespace win
} // of namespace bvr20983
#endif // DIGICLOCKAPP_H