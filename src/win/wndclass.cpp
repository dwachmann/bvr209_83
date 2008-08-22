/*
 * $Id$
 * 
 * Windows class class.
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
#include "win/wndclass.h"
#include "exception/lasterrorexception.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace win
  {
     WndClassM WndClass::m_windowClass;

#pragma region Construction & Deconstruction
    /*
     * WndClass::WndClass
     *
     * Purpose:
     *   Constructor.
     *
     * Parameters:
     *
     * Return: 
     *   void
     */
    WndClass::WndClass(LPCTSTR clsName,WndFactory* pWndFactory,HINSTANCE hInst,WORD wIcon,HBRUSH backgroundBrush,UINT style,WNDPROC lpfnWndProc) 
      : m_inited(false),m_hInst(hInst),m_wndCls(0),m_pWndFactory(pWndFactory)
    { ::memset(&m_wndClsEx,0,sizeof(m_wndClsEx));

      m_wndClsEx.cbSize         = sizeof(m_wndClsEx);
      m_wndClsEx.style          = style;
      m_wndClsEx.lpfnWndProc    = lpfnWndProc;
      m_wndClsEx.cbClsExtra     = 0;
      m_wndClsEx.cbWndExtra     = sizeof(void*);
      m_wndClsEx.hInstance      = m_hInst;
      m_wndClsEx.hIcon          = wIcon!=0 ? ::LoadIcon(m_hInst,MAKEINTRESOURCE(wIcon)) : NULL;
      m_wndClsEx.hCursor        = ::LoadCursor(NULL,IDC_ARROW);
      m_wndClsEx.hbrBackground  = backgroundBrush;
      m_wndClsEx.lpszMenuName   = NULL;
      m_wndClsEx.lpszClassName  = (LPTSTR)clsName;
      m_wndClsEx.hIconSm        = NULL;
    }

    /*
     *
     */
    WndClass::~WndClass()
    { if( m_inited )
      { ::UnregisterClass(MAKEINTRESOURCE(m_wndCls),m_hInst);

        m_inited = false;
        
        //delete m_pWndFactory;
      } // of if
    } // of WndClass::~WndClass()
#pragma endregion

#pragma region RegisterControlClass
    /**
     *
     */
    void WndClass::RegisterWindowClass(LPCTSTR     clsName,
                                       WndFactory* pWndFactory,
                                       HINSTANCE   hInst,
                                       WORD        wIcon,
                                       HBRUSH      backgroundBrush,
                                       UINT        style,
                                       WNDPROC     lpfnWndProc
                                      )
    { if( m_windowClass.empty() || m_windowClass.find(clsName)==m_windowClass.end() )
      { m_windowClass.insert( WndClassP(clsName,WndClass(clsName,pWndFactory,hInst,wIcon,backgroundBrush,style,lpfnWndProc)) );

        m_windowClass.find(clsName)->second.Register();
      } // of if
    } // of WndClass::RegisterWindowClass()

    /**
     *
     */
    void WndClass::UnregisterWindowClasses()
    { m_windowClass.clear(); }

    /**
     *
     */
    WndFactory* WndClass::GetWindowFactory(LPCTSTR clsName)
    { WndFactory* result = NULL;

      if( !m_windowClass.empty() && m_windowClass.find(clsName)!=m_windowClass.end() )
        result = m_windowClass.find(clsName)->second.m_pWndFactory;

      return result;
    } // of WndClass::GetWindowFactory()

    /**
     *
     */
    void WndClass::Register()
    { if( !m_inited )
      { m_wndCls =  ::RegisterClassEx(&m_wndClsEx);
        THROW_LASTERROREXCEPTION1( m_wndCls );

        m_inited = true;
      } // of if
    } // of WndClass::Register()
#pragma endregion
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
