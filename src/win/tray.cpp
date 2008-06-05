/*
 * $Id$
 * 
 * shell tray notification icon class.
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
#include "win/tray.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace win
  {
#pragma region Construction & Deconstruction
    /**
     *
     */
    Tray::Tray(HWND hWnd)
    { m_hWnd = hWnd;

      NOTIFYICONDATA nid;

	    nid.cbSize  = sizeof(nid);
	    nid.hWnd		= m_hWnd;
      nid.uVersion= NOTIFYICON_VERSION;

      ::Shell_NotifyIcon(NIM_SETVERSION, &nid);
    }

    /**
     *
     */
    Tray::~Tray()
    { std::vector<int>::iterator iter;
    
      for( iter=m_Ids.begin();iter!=m_Ids.end();iter++ )
      { int id = *iter;

        Delete(id,false);
      } // of for
    } // of Tray::~Tray()

#pragma endregion

#pragma region Implementation
    /**
     *
     */
    void Tray::Notify(DWORD dwMessage,UINT id,int iconId,int szTipId,int szInfoId,DWORD dwInfoFlag)
    { NOTIFYICONDATA nid;

      HINSTANCE hInst = (HINSTANCE) ::GetWindowLongPtr(m_hWnd,GWLP_HINSTANCE) ;
      HICON     hIcon     = ::LoadIcon(hInst,MAKEINTRESOURCE(iconId));

	    nid.cbSize  = sizeof(nid);
	    nid.hWnd		= m_hWnd;
	    nid.uID			= id;
      nid.uFlags  = NIF_ICON;
      nid.hIcon   = hIcon;

      if( dwMessage==NIM_ADD )
      { nid.uFlags           |= NIF_MESSAGE;
        nid.uCallbackMessage  = WM_BVR_SHELLNOTIFY;
      } // of if

      if( szInfoId!=0 )
      { nid.uFlags |= NIF_INFO;

        ::LoadString(hInst,szInfoId,nid.szInfo,sizeof(nid.szInfo)/sizeof(nid.szInfo[0]));
        
        nid.szInfoTitle[0] = _T('\0');

        nid.uTimeout = 30000;

        if( szTipId!=NULL )
          ::LoadString(hInst,szTipId,nid.szInfoTitle,sizeof(nid.szInfoTitle)/sizeof(nid.szInfoTitle[0]));

        nid.dwInfoFlags = dwInfoFlag;
      } // of if
      else if( szTipId!=NULL )
      { nid.uFlags |= NIF_TIP;

        ::LoadString(hInst,szTipId,nid.szTip,sizeof(nid.szTip)/sizeof(nid.szTip[0]));
      } // of if

      ::Shell_NotifyIcon(dwMessage, &nid);

      ::DestroyIcon(hIcon);
    }

    /**
     *
     */
    void Tray::Add(UINT id,int iconId,int szTipId,int szInfoId,DWORD dwInfoFlag)
    { Notify(NIM_ADD,id,iconId,szTipId,szInfoId,dwInfoFlag); 

      m_Ids.push_back( id );
    }

    void Tray::Modify(UINT id,int iconId,int szTipId,int szInfoId,DWORD dwInfoFlag)
    { Notify(NIM_MODIFY,id,iconId,szTipId,szInfoId,dwInfoFlag); }

    /**
     *
     */
    void Tray::Delete(UINT id,bool eraseFromList)
    { NOTIFYICONDATA nid;

	    nid.cbSize  = sizeof(nid);
	    nid.hWnd		= m_hWnd;
	    nid.uID			= id;

      ::Shell_NotifyIcon(NIM_DELETE, &nid);

      if( eraseFromList )
      { std::vector<int>::iterator iter;
    
        for( iter=m_Ids.begin();iter!=m_Ids.end();iter++ )
        { int id0 = *iter;
        
          if( id0==id )
          { m_Ids.erase(iter);
            break;
          } // of if
        } // of for
      } // of if
    } // of void Tray::Delete()

    /**
     *
     */
    void Tray::SetFocus(UINT id)
    { NOTIFYICONDATA nid;

	    nid.cbSize  = sizeof(nid);
	    nid.hWnd		= m_hWnd;
	    nid.uID			= id;

      ::Shell_NotifyIcon(NIM_SETFOCUS, &nid);
    } // of void Tray::SetFocus()
#pragma endregion
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
