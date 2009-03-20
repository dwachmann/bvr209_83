/*
 * $Id$
 * 
 * windows dialog class.
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
#include <assert.h>
#include "win/dialog.h"
#include "exception/windowsexception.h"
#include "exception/lasterrorexception.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace win
  {
#pragma region Construction & Deconstruction
    /*
     * Dialog::Dialog
     *
     * Purpose:
     *   Constructor.
     *
     * Parameters:
     *
     * Return: 
     *   void
     */
    Dialog::Dialog() : Wnd(NULL)
    { m_hParentWnd = NULL;
      m_dlgName    = NULL;
    } // of Dialog::Dialog()
    
    /*
     * Dialog::~Dialog
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
    Dialog::~Dialog()
    { }
#pragma endregion

#pragma region Implementation
    /**
     *
     */
    int Dialog::Show(LPCTSTR dlgName,HWND hWnd,HINSTANCE hInstance)
    { m_hParentWnd = hWnd;
      m_dlgName    = dlgName;

      return ::DialogBoxParam(hInstance,m_dlgName,m_hParentWnd,&DlgProc,(LPARAM)this);
    } // of Show()

    /**
     *
     */
    HWND Dialog::Modeless(LPCTSTR dlgName,HWND hWnd,HINSTANCE hInstance)
    { m_hParentWnd = hWnd;
      m_dlgName    = dlgName;

      return ::CreateDialogParam(hInstance,m_dlgName,m_hParentWnd,&DlgProc,(LPARAM)this);
    } // of Modeless()
#pragma endregion

#pragma region Windows Event Handling
    /**
     *
     */
    INT_PTR Dialog::DialogProcedure(UINT message,WPARAM wParam,LPARAM lParam)
    { INT_PTR result = FALSE;
      
      switch( message )
      { 
      case WM_TIMER:
        OnTimer();
        break;
      case WM_COMMAND:
        switch(LOWORD(wParam))
        { 
        case IDOK:
          ::EndDialog(GetWindowHandle(),TRUE);
          result = TRUE;
          break;
        case IDCANCEL:
          ::EndDialog(GetWindowHandle(),FALSE);
          result = TRUE;
          break;
        default:
          if( OnCommand(wParam) )
            result = TRUE;

          break;
        } // of switch()
        break;
      } // of switch

      return result;
    } // of Dialog::DialogProcedure()

    /**
     *
     */
    BOOL Dialog::InitDialog()
    { return TRUE; }

    /**
     *
     */
    void Dialog::EndDialog(INT_PTR nResult)
    { ::EndDialog(m_hWnd,nResult); }

    /**
     *
     */
    INT_PTR CALLBACK DlgProc(HWND hDlg,UINT msg,WPARAM wParam,LPARAM lParam)
    { INT_PTR      result = FALSE;
      win::Dialog* dlg    = NULL;

      switch( msg )
      { 
      case WM_INITDIALOG:

        ::SetWindowLongPtr(hDlg,GWLP_USERDATA,(LONG_PTR)lParam);

        dlg = reinterpret_cast<win::Dialog*>(lParam);

        dlg->m_hWnd = hDlg;

        result = dlg->InitDialog();
        break;
      default:
        dlg = (win::Dialog*) ::GetWindowLongPtr(hDlg,GWLP_USERDATA);

        if( NULL!=dlg )
        { assert( dlg->GetWindowHandle()==hDlg );

          result = dlg->DialogProcedure(msg,wParam,lParam);
        } // of if
        break;
      } // of switch
      
      return result;
    } // of ::DlgProc()
#pragma endregion
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
