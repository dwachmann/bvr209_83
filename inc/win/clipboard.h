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
#if !defined(CLIPBOARD_H)
#define CLIPBOARD_H

#include "os.h"

namespace bvr20983
{
  namespace win
  {
    /**
     *
     */
    class Clipboard
    {
      public:
        Clipboard(HWND hWnd);
        ~Clipboard();
        
        void EmptyClipboard();
        void AddData(LPCTSTR s);
        void AddData(const TString& s);
        void AddData(HBITMAP bitmap);
        void AddData(HENHMETAFILE mf);
  
      protected:
    }; // of class Clipboard
  } // of namespace win
} // of namespace bvr20983
#endif // CLIPBOARD_H
