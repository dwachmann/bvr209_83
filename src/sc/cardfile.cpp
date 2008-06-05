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
#include "sc/cardfile.h"
#include "sc/bytebuffer.h"
#include "sc/apdu.h"
#include "sc/smartcard.h"
#include "util/logstream.h"
#include "exception/scardapduexception.h"

namespace bvr20983
{

  /*
   * CardFile::CardFile
   * CardFile::~CardFile
   *
   * Constructor Parameters:
   *  None
   */
  CardFile::CardFile(bool isGsmCard)
  { m_class = isGsmCard ? 0xA0 : 0x00;
    m_fid   = 0;
  }
  
  
  /**
   *
   */
  void CardFile::Select(Smartcard& sc,WORD fi)
  { m_fid = fi;
  
    APDU send = APDU(m_class,(BYTE)0xa4,(BYTE)0x00,(BYTE)0x00);
    send.SetLe(0);
    send.SetResponseType(APDU::TLV_RESPONSE);
    
    BYTE fid[] = {(BYTE)(m_fid>>8),(BYTE)(0x00ff&m_fid)};
    
    send.AddData( ByteBuffer(fid,sizeof(fid)) );

    sc.Transmit(send);
    
    if( send.GetResponseCode().GetSW1()==0x9F )
    { APDU getResponse = APDU(m_class,(BYTE)0xc0,(BYTE)0x00,(BYTE)0x00);
      getResponse.SetLe(send.GetResponseCode().GetSW2());
  
      sc.Transmit(getResponse);
  
      LOGGER_INFO<<_T("CardFile::Select()")<<endl<<send<<endl<<getResponse;
    } // of if
    else  
      LOGGER_INFO<<_T("CardFile::Select()")<<endl<<send;
  }

  /**
   *
   */
  void CardFile::Verify(Smartcard& sc,BYTE no,LPCTSTR pin,BYTE pinLen)
  { APDU send = APDU(m_class,(BYTE)0x20,(BYTE)0x00,no);
    send.SetLe(0);
    
    ByteBuffer data;
    BYTE       i=0;
    
    for( ;i<pinLen && pin[i]!=_T('\0');i++ )
      data.push_back((BYTE)pin[i]);

    for( ;i<pinLen;i++ )
      data.push_back((BYTE)0xff);
      
    send.AddData(data);
    
    sc.Transmit(send);

    LOGGER_INFO<<_T("CardFile::Verify()")<<endl<<send<<endl;
  } // of CardFile::Verify()
  
  /**
   *
   */
  void CardFile::Read(Smartcard& sc,bool genMac)
  { APDU send = APDU(m_class,(BYTE)0xb2,(BYTE)0x00,(BYTE)0x00);
    send.SetLe(0);
    //send.SetResponseType(APDU::TLV);
    
    sc.Transmit(send,genMac);
    if( send.GetResponseCode().GetSW1()==0x9F )
    { APDU getResponse = APDU(m_class,(BYTE)0xc0,(BYTE)0x00,(BYTE)0x00);
  
      getResponse.SetLe(send.GetResponseCode().GetSW2());
  
      sc.Transmit(getResponse,genMac);
  
      LOGGER_INFO<<_T("CardFile::Read()")<<endl<<send<<endl<<getResponse;
    } // of if
    else  
      LOGGER_INFO<<_T("CardFile::Read()")<<endl<<send;
    
    send.SetP2((BYTE)0x02);
  
    for( ;; )
      try
      { sc.Transmit(send,genMac); 
  
        if( send.GetResponseCode().GetSW1()==0x9F )
        { APDU getResponse = APDU(m_class,(BYTE)0xc0,(BYTE)0x00,(BYTE)0x00);
      
          getResponse.SetLe(send.GetResponseCode().GetSW2());
      
          sc.Transmit(getResponse,genMac);
      
          LOGGER_INFO<<_T("CardFile::Read()")<<endl<<send<<endl<<getResponse;
        } // of if
        else  
          LOGGER_INFO<<_T("CardFile::Read()")<<endl<<send;
      }
      catch(SCardAPDUException e)
      { if( e.GetErrorCode()==0x9404 )
          break;
        else
          throw;
      }
  
  } // of CardFile::Read()

  /**
   *
   */
  void CardFile::ReadBinary(Smartcard& sc)
  { APDU send = APDU(m_class,(BYTE)0xb0,(BYTE)0x00,(BYTE)0x00);
    send.SetLe(0x08);
    
    sc.Transmit(send);
    if( send.GetResponseCode().GetSW1()==0x9F )
    { APDU getResponse = APDU(m_class,(BYTE)0xc0,(BYTE)0x00,(BYTE)0x00);
  
      getResponse.SetLe(send.GetResponseCode().GetSW2());
  
      sc.Transmit(getResponse);
  
      LOGGER_INFO<<_T("CardFile::ReadBinary()")<<endl<<send<<endl<<getResponse;
    } // of if
    else  
      LOGGER_INFO<<_T("CardFile::ReadBinary()")<<endl<<send;
    
  } // of CardFile::ReadBinary()
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
