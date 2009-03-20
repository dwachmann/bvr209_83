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
#if !defined(DIALOG_H)
#define DIALOG_H

#include "os.h"
#include "win/wnd.h"

namespace bvr20983
{
  namespace win
  {
    /**
     *
     */
    class Dialog : public Wnd
    {
      public:
        Dialog();
        virtual ~Dialog();

        int  Show(LPCTSTR dlgName,HWND hWnd,HINSTANCE hInstance=NULL);
        HWND Modeless(LPCTSTR dlgName,HWND hWnd,HINSTANCE hInstance=NULL);

        void EndDialog(INT_PTR nResult);

        friend INT_PTR CALLBACK DlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);

      protected:
        LPCTSTR m_dlgName;
        HWND    m_hParentWnd;

        virtual INT_PTR DialogProcedure(UINT message,WPARAM wParam,LPARAM lParam);
        virtual BOOL    InitDialog();
    }; // of class Dialog

    INT_PTR CALLBACK DlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam);
  } // of namespace win
} // of namespace bvr20983
#endif // DIALOG_H