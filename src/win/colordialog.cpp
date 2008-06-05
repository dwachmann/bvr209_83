/*
 * $Id$
 * 
 * Windows common color dialog class.
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
#include "win/colordialog.h"
#include "exception/windowsexception.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace win
  {
#pragma region Construction & Deconstruction
    /**
     *
     */
    ColorDialog::ColorDialog(HWND hWndOwner,COLORREF rgbInit)
    { memset(&m_cc,'\0',sizeof(m_cc));
    
      memset(&m_customColor,'\0',sizeof(m_customColor));
      
      m_cc.lStructSize  = sizeof(m_cc);
      m_cc.hwndOwner    = hWndOwner;
      m_cc.lpCustColors = (LPDWORD) m_customColor;
      m_cc.rgbResult    = rgbInit;
      m_cc.Flags        = CC_FULLOPEN | CC_RGBINIT;
    }
#pragma endregion

#pragma region Implementation
    /**
     *
     */
    bool ColorDialog::Show()
    { bool result = ::ChooseColor(&m_cc)==TRUE ;
    
      return result;
    }
#pragma endregion
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
