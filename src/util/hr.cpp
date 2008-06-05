/*
 * $Id$
 * 
 * Wrapper class for COM HR errorcode.
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
#include "util/hr.h"
#include "util/apputil.h"

namespace bvr20983
{
  /**
   *
   */
  CHResult::CHResult(HRESULT hr) : m_hr(hr)
  { TCHAR szMsg[MAX_PATH];

    switch( hr )
    {
    case S_OK:
      m_hrStr = _T("ok");
      break;
    case S_FALSE:
      m_hrStr = _T("false");
      break;
    case E_FAIL:
      m_hrStr = _T("FAIL");
      break;
    case DISP_E_UNKNOWNNAME:
      m_hrStr = _T("DISP_E_UNKNOWNNAME");
      break;
    case DISP_E_MEMBERNOTFOUND:
      m_hrStr = _T("DISP_E_MEMBERNOTFOUND");
      break;
    default:
      if( ::GetErrorMsg(m_hr,szMsg,MAX_PATH) ) 
        m_hrStr = szMsg;
      break;
    } // of switch
  }
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
