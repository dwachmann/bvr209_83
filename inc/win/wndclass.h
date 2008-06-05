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
#if !defined(WNDCLASS_H)
#define WNDCLASS_H

#include "os.h"
#include "win/wnd.h"

namespace bvr20983
{
  namespace win
  {
    class WndClass;

    typedef std::pair<LPCTSTR, WndClass>        WndClassP;
    typedef std::map<LPCTSTR, WndClass,strless> WndClassM;

    class WndClass
    {
      public:
        static void RegisterWindowClass(LPCTSTR clsName,
                                        WndFactory* pWndFactory,
                                        HINSTANCE hInst,
                                        WORD wIcon=0,
                                        HBRUSH backgroundBrush=(HBRUSH)::GetStockObject(WHITE_BRUSH),
                                        UINT style=CS_HREDRAW | CS_VREDRAW,
                                        WNDPROC lpfnWndProc=&WndProc
                                       );

        static void        UnregisterWindowClasses();
        static WndFactory* GetWindowFactory(LPCTSTR clsName);

        ~WndClass();

      private:
        WndClass(LPCTSTR clsName,
                 WndFactory* pWndFactory,
                 HINSTANCE hInst,
                 WORD wIcon,
                 HBRUSH backgroundBrush,
                 UINT style,
                 WNDPROC lpfnWndProc
                );

        void Register();

        bool        m_inited;
        WNDCLASSEX  m_wndClsEx;
        ATOM        m_wndCls;
        HINSTANCE   m_hInst;
        WndFactory* m_pWndFactory;

        static WndClassM m_windowClass;
    }; // of class Wnd
  } // of namespace win
} // of namespace bvr20983
#endif // WNDCLASS_H