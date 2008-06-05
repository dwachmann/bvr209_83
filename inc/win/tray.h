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
#if !defined(TRAY_H)
#define TRAY_H

#include "os.h"

namespace bvr20983
{
  namespace win
  { 
    class Tray
    {
      public:
        Tray(HWND hWnd);
        ~Tray();

        void Add   (UINT id,int iconId,int szTipId=0,int szInfoId=0,DWORD dwInfoFlag=NIIF_INFO);
        void Modify(UINT id,int iconId,int szTipId=0,int szInfoId=0,DWORD dwInfoFlag=NIIF_INFO);
        void Delete(UINT id,bool eraseFromList=true);
        void SetFocus(UINT id);
        
      protected:
        void Notify(DWORD dwMessage,UINT id,int iconId,int szTipId,int szInfoId,DWORD dwInfoFlag);

        HWND             m_hWnd;
        std::vector<int> m_Ids;
    }; // of class Tray
  } // of namespace win
} // of namespace bvr20983
#endif // TRAY_H
