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
#if !defined(APP_H)
#define APP_H

#include "os.h"

namespace bvr20983
{
  namespace win
  {
    class WndClass;
    class Wnd;

    /**
     *
     */
    class App
    {
      public:
        App(HINSTANCE hInstance,LPSTR lpCmdLine,int nCmdShow,WORD wIcon=0,int acc=0,
            HBRUSH backgroundBrush=::GetSysColorBrush(COLOR_WINDOW)
           );
        ~App();

        virtual int Main();

        HINSTANCE GetInstance() const
        { return m_hInstance; }

      protected:
        HACCEL    m_hAccelTable;
        HINSTANCE m_hInstance;
        WndClass* m_pWndClass;
        Wnd*      m_pWnd;
    }; // of class Wnd

  } // of namespace win
} // of namespace bvr20983
#endif // APP_H