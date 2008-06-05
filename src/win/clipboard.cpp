/*
 * $Id$
 * 
 * clipboard helper class.
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
#include "win/clipboard.h"

#ifdef UNICODE
#define CF_TCHAR CF_UNICODETEXT
#else
#define CF_TCHAR CF_TEXT
#endif

using namespace bvr20983;

namespace bvr20983
{
  namespace win
  {
#pragma region Construction & Deconstruction
    /*
     * Clipboard::Clipboard
     *
     * Purpose:
     *   Constructor.
     *
     * Parameters:
     *
     * Return: 
     *   void
     */
    Clipboard::Clipboard(HWND hWnd) 
    { ::OpenClipboard(hWnd);
    }

    
    /*
     * Clipboard::~Clipboard
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
    Clipboard::~Clipboard()
    { ::CloseClipboard();
    }
#pragma endregion

#pragma region Implementation
    /**
     *
     */
    void Clipboard::EmptyClipboard() 
    { ::EmptyClipboard();
    } // of Clipboard::EmptyClipboard()

    /**
     *
     */
    void Clipboard::AddData(const TString& s)
    { AddData(s.c_str()); }

    /**
     *
     */
    void Clipboard::AddData(LPCTSTR s)
    { if( s!=NULL )
      { int sLen = _tcslen(s);

        if( sLen>0 )
        { HANDLE hGlobal = ::GlobalAlloc(GHND | GMEM_SHARE,(sLen+1)*sizeof(TCHAR));
          LPVOID pGlobal = ::GlobalLock(hGlobal);

          _tcscpy_s((LPTSTR)pGlobal,sLen+1,s);

          ::GlobalUnlock(pGlobal);
          ::SetClipboardData(CF_TCHAR,hGlobal);
        } // of if
      } // of if
    } // of Clipboard::AddData()

    /**
     *
     */
    void Clipboard::AddData(HBITMAP bitmap)
    { if( NULL!=bitmap )
        ::SetClipboardData(CF_BITMAP,bitmap);
    } // of Clipboard::AddData()

    /**
     *
     */
    void Clipboard::AddData(HENHMETAFILE mf)
    { if( mf!=NULL )
        ::SetClipboardData(CF_ENHMETAFILE,mf);
    } // of Clipboard::AddData()
#pragma endregion
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
