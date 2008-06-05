/*
 * $Id$
 * 
 * Implementation of smartcard functionality.
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
#include "sc/sctypes.h"
#include "sc/bytebuffer.h"
#include "sc/scardattribute.h"
#include "util/logstream.h"


namespace bvr20983
{

  /*
   * SCardAttribute::SCardAttribute
   * SCardAttribute::~SCardAttribute
   *
   * Constructor Parameters:
   *  None
   */
  SCardAttribute::SCardAttribute(SCARDHANDLE hCard,DWORD dwAttrId)
  { m_dwAttribId = dwAttrId;
    m_pbAttr     = NULL;
    m_hCard      = hCard;
  
    DWORD  cByte     = SCARD_AUTOALLOCATE;
    LONG   lReturn   = ::SCardGetAttrib(m_hCard,m_dwAttribId,(LPBYTE)&m_pbAttr,&cByte);
    
    
    if( SCARD_S_SUCCESS==lReturn )
    { if( cByte>0 )
        m_attrib = ByteBuffer(m_pbAttr,cByte);
    } // of if
    else if( ERROR_NOT_SUPPORTED!=lReturn )
    { THROW_SCARDEXCEPTION(lReturn); }
  }
  
  /**
   *
   */
  SCardAttribute::~SCardAttribute()
  { if( NULL!=m_pbAttr ) 
      ::SCardFreeMemory(m_hCard,m_pbAttr); 
  }
  
  /**
   *
   */
  template<class charT, class Traits>
  std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const SCardAttribute& e)
  { os<<left;
  
    switch( e.m_dwAttribId )
    { case SCARD_ATTR_ATR_STRING:
        os<<setw(21)<<_T("ATR_STRING")<<endl;
        break;
      case SCARD_ATTR_CHANNEL_ID:
        os<<setw(21)<<_T("CHANNEL_ID")<<endl;
        break;
      case SCARD_ATTR_CHARACTERISTICS:
        os<<setw(21)<<_T("CHARACTERISTICS")<<endl;
        break;
      case SCARD_ATTR_CURRENT_BWT:
        os<<setw(21)<<_T("CURRENT_BWT")<<endl;
        break;
      case SCARD_ATTR_CURRENT_CLK:
        os<<setw(21)<<_T("CURRENT_CLK")<<endl;
        break;
      case SCARD_ATTR_CURRENT_CWT:
        os<<setw(21)<<_T("CURRENT_CWT")<<endl;
        break;
      case SCARD_ATTR_CURRENT_D:
        os<<setw(21)<<_T("CURRENT_D")<<endl;
        break;
      case SCARD_ATTR_CURRENT_EBC_ENCODING:
        os<<setw(21)<<_T("CURRENT_EBC_ENCODING")<<endl;
        break;
      case SCARD_ATTR_CURRENT_F:
        os<<setw(21)<<_T("CURRENT_F")<<endl;
        break;
      case SCARD_ATTR_CURRENT_IFSC:
        os<<setw(21)<<_T("CURRENT_IFSC")<<endl;
        break;
      case SCARD_ATTR_CURRENT_IFSD:
        os<<setw(21)<<_T("CURRENT_IFSD")<<endl;
        break;
      case SCARD_ATTR_CURRENT_N:
        os<<setw(21)<<_T("CURRENT_N")<<endl;
        break;
      case SCARD_ATTR_CURRENT_PROTOCOL_TYPE:
        os<<setw(21)<<_T("CURRENT_PROTOCOL_TYPE")<<endl;
        break;
      case SCARD_ATTR_CURRENT_W:
        os<<setw(21)<<_T("CURRENT_W")<<endl;
        break;
      case SCARD_ATTR_DEFAULT_CLK:
        os<<setw(21)<<_T("DEFAULT_CLK")<<endl;
        break;
      case SCARD_ATTR_DEFAULT_DATA_RATE:
        os<<setw(21)<<_T("DEFAULT_DATA_RATE")<<endl;
        break;
      case SCARD_ATTR_DEVICE_FRIENDLY_NAME:
        os<<setw(21)<<_T("DEVICE_FRIENDLY_NAME")<<endl;
        break;
      case SCARD_ATTR_DEVICE_IN_USE:
        os<<setw(21)<<_T("DEVICE_IN_USE")<<endl;
        break;
      case SCARD_ATTR_DEVICE_SYSTEM_NAME:
        os<<setw(21)<<_T("DEVICE_SYSTEM_NAME")<<endl;
        break;
      case SCARD_ATTR_DEVICE_UNIT:
        os<<setw(21)<<_T("DEVICE_UNIT")<<endl;
        break;
      case SCARD_ATTR_ICC_INTERFACE_STATUS:
        os<<setw(21)<<_T("ICC_INTERFACE_STATUS")<<endl;
        break;
      case SCARD_ATTR_ICC_PRESENCE:
        os<<setw(21)<<_T("ICC_PRESENCE")<<endl;
        break;
      case SCARD_ATTR_ICC_TYPE_PER_ATR:
        os<<setw(21)<<_T("ICC_TYPE_PER_ATR")<<endl;
        break;
      case SCARD_ATTR_MAX_CLK:
        os<<setw(21)<<_T("MAX_CLK")<<endl;
        break;
      case SCARD_ATTR_MAX_DATA_RATE:
        os<<setw(21)<<_T("MAX_DATA_RATE")<<endl;
        break;
      case SCARD_ATTR_MAX_IFSD:
        os<<setw(21)<<_T("MAX_IFSD")<<endl;
        break;
      case SCARD_ATTR_POWER_MGMT_SUPPORT:
        os<<setw(21)<<_T("POWER_MGMT_SUPPORT")<<endl;
        break;
      case SCARD_ATTR_PROTOCOL_TYPES:
        os<<setw(21)<<_T("PROTOCOL_TYPES")<<endl;
        break;
      case SCARD_ATTR_VENDOR_IFD_SERIAL_NO:
        os<<setw(21)<<_T("VENDOR_IFD_SERIAL_NO")<<endl;
        break;
      case SCARD_ATTR_VENDOR_IFD_TYPE:
        os<<setw(21)<<_T("VENDOR_IFD_TYPE")<<endl;
        break;
      case SCARD_ATTR_VENDOR_IFD_VERSION:
        os<<setw(21)<<_T("VENDOR_IFD_VERSION")<<endl;
        break;
      case SCARD_ATTR_VENDOR_NAME:
        os<<setw(21)<<_T("VENDOR_NAME")<<endl;
        break;
      default:
        os<<_T("unknown attrib[")<<hex<<e.m_dwAttribId<<_T("]")<<endl;
        break;
    } // of switch()
  
    os<<e.m_attrib<<endl;
    
    return os;
  }
} // of namespace bvr20983

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const SCardAttribute&);
/*==========================END-OF-FILE===================================*/
