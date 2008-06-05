/*
 * $Id$
 * 
 * Windows Application base class.
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
#include "bvr20983.h"
#include "win/app.h"
#include "win/wndclass.h"
#include "win/wnd.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace win
  {
#pragma region Construction & Deconstruction
    /*
     * App::App
     *
     * Purpose:
     *   Constructor.
     *
     * Parameters:
     *
     * Return: 
     *   void
     */
    App::App(HINSTANCE hInstance,LPSTR lpCmdLine,int nCmdShow,WORD wIcon,int acc,HBRUSH backgroundBrush) 
    { Wnd::RegisterClass(hInstance,wIcon,backgroundBrush);
      
      m_hInstance   = hInstance;
      m_hAccelTable = acc!=0 ? ::LoadAccelerators (m_hInstance, MAKEINTRESOURCE(acc)) : NULL;
      m_pWnd        = NULL;
    }

    /*
     * App::~App
     *
     * Purpose:
     *   Destructor.
     *
     * Parameters:
     *   void  
     *
     * Return: 
     *   void
     */
    App::~App()
    { }
#pragma endregion

#pragma region Implementation
    /**
     * main windows message loop
     */
    int App::Main()
    { MSG     msg;

      while( ::GetMessage(&msg, NULL, 0, 0) ) 
      { int acc = 0;

        if( m_hAccelTable!=NULL )
          acc = ::TranslateAccelerator(m_pWnd->GetWindowHandle(), m_hAccelTable, &msg);

        if( !acc ) 
        { ::TranslateMessage(&msg);
          ::DispatchMessage(&msg);
        } // of  if
      } // of while

      return (int) msg.wParam;
    } // of App::Main()
#pragma endregion
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
