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
#include "sc/smartcard.h"
#include "exception/scardapduexception.h"
#include "sc/smartcardioctls.h"
#include "util/logstream.h"

namespace bvr20983
{

  /*
   * Smartcard::Smartcard
   * Smartcard::~Smartcard
   *
   * Constructor Parameters:
   *  None
   */
  Smartcard::Smartcard()
  { LOGGER_DEBUG<<_T("Smartcard::Smartcard()")<<endl;
  
    Init();
    
    LONG lReturn = ::SCardEstablishContext(SCARD_SCOPE_USER,NULL,NULL,&m_hSC);
    THROW_SCARDEXCEPTION(lReturn);
    
    m_hSCInited = true;

    BYTE keyData[] = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f };
      
    ByteBuffer key(keyData,sizeof(keyData));

    m_pEncrypt = BlockCipher::GetInstance(_T("DESede"));
    m_pDecrypt = BlockCipher::GetInstance(_T("DESede"));

    m_pEncrypt->Init(key,true);
    m_pDecrypt->Init(key,false);
        
    VTString readers = VTString();
    
    ListReaders(readers);
    
    if( !readers.empty() )
      m_reader.append(readers.at(0));
  }
  
  /**
   *
   */
  Smartcard::~Smartcard()
  { Disconnect();
  
    if( m_hSCInited )
      ::SCardReleaseContext(m_hSC);
      
    delete m_pEncrypt;
    delete m_pDecrypt;
  }

  /*
   *
   */
  void Smartcard::Init()
  { m_hCardConnected = false;
    m_CardPresent    = false;
    m_sendPci        = SCARD_PCI_T0;
  }

  /**
   *
   */
  void Smartcard::Disconnect(DWORD disposition)
  { if( m_hSCInited && m_hCardConnected )
      ::SCardDisconnect(m_hCard,disposition);
      
    Init();
  }
  
  /**
   * 
   */
  void Smartcard::Connect(DWORD timeout,DWORD shareMode)
  { LONG lReturn = SCARD_E_READER_UNAVAILABLE;
  
    if( !m_hCardConnected )
    { DWORD dwAP = 0;
      
      WaitForInsertEvent(timeout);
      
      lReturn = ::SCardConnect( m_hSC, 
                                m_reader.c_str(),
                                shareMode,
                                SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                                &m_hCard,
                                &dwAP 
                              );
      THROW_SCARDEXCEPTION(lReturn);
      
      m_hCardConnected = true;
      
      switch ( dwAP )
      { case SCARD_PROTOCOL_T0:
          LOGGER_INFO<<_T("Active protocol T0")<<endl; 
          m_sendPci        = SCARD_PCI_T0;
          break;
        case SCARD_PROTOCOL_T1:
          m_sendPci        = SCARD_PCI_T1;
          LOGGER_INFO<<_T("Active protocol T1")<<endl; 
          break;
        case SCARD_PROTOCOL_UNDEFINED:
        default:
          LOGGER_WARN<<_T("Active protocol unnegotiated or unknown")<<endl; 
          break;
      } // of switch
      
      GetFeatureRequest();
    } // of if
  } // of Smartcard::Connect()
  
  /**
   * 
   */
  void Smartcard::WaitForInsertEvent(DWORD timeout)
  { LONG lReturn = SCARD_E_TIMEOUT;
  
    if( !m_reader.empty() )
    { SCARD_READERSTATE rgscState[1];
      
      memset( rgscState, '\0', sizeof(rgscState) );
    
      rgscState[0].szReader       = m_reader.c_str();
      rgscState[0].dwCurrentState = SCARD_STATE_UNAWARE;
  
      LOGGER_DEBUG<<_T("SCardGetStatusChange(): szReader       = ")<<rgscState[0].szReader<<endl;
      LOGGER_DEBUG<<_T("SCardGetStatusChange(): dwCurrentState = 0x")<<hex<<rgscState[0].dwCurrentState<<endl;
      
      lReturn = ::SCardGetStatusChange(m_hSC,0,rgscState,1 );
      THROW_SCARDEXCEPTION(lReturn);
  
      m_ATR.clear();
      
      if( rgscState[0].cbAtr>0 )
        m_ATR = ATR(ByteBuffer((const BYTE*)rgscState[0].rgbAtr,rgscState[0].cbAtr));
      
      if( rgscState[0].dwEventState&SCARD_STATE_PRESENT )
      { if( !m_CardPresent )
          OnInserted();
      } // of if
      else if( timeout>0 )
      { rgscState[0].dwCurrentState = rgscState[0].dwEventState;
    
        lReturn = ::SCardGetStatusChange(m_hSC,timeout,rgscState,1 );
        THROW_SCARDEXCEPTION(lReturn);
    
        SCardEventState eventState(rgscState[0].dwEventState);
        LOGGER_DEBUG<<_T("SCardGetStatusChange(): dwEventState   = ")<<eventState<<endl;
  
        if( rgscState[0].cbAtr>0 )
          m_ATR = ATR(ByteBuffer((const BYTE*)rgscState[0].rgbAtr,rgscState[0].cbAtr));
      
        if( (rgscState[0].dwEventState&SCARD_STATE_PRESENT) && !m_CardPresent ) 
          OnInserted();
      } // of if
    } // of if
  }

  /**
   * 
   */
  bool Smartcard::IsCardPresent() const
  { LONG lReturn = SCARD_E_TIMEOUT;
    bool result  = false;
  
    if( !m_reader.empty() )
    { SCARD_READERSTATE rgscState[1];
      
      memset( rgscState, '\0', sizeof(rgscState) );
    
      rgscState[0].szReader       = m_reader.c_str();
      rgscState[0].dwCurrentState = SCARD_STATE_UNAWARE;
  
      lReturn = ::SCardGetStatusChange(m_hSC,0,rgscState,1 );
      THROW_SCARDEXCEPTION(lReturn);
  
      result = (rgscState[0].dwEventState&SCARD_STATE_PRESENT)!=0;
    } // of if
    
    return result;
  }
  
  /**
   * 
   */
  void Smartcard::WaitForRemoveEvent(DWORD timeout)
  { LONG lReturn = SCARD_E_TIMEOUT;
  
    if( !m_reader.empty() )
    { SCARD_READERSTATE rgscState[1];
      
      memset( rgscState, '\0', sizeof(rgscState) );
    
      rgscState[0].szReader       = m_reader.c_str();
      rgscState[0].dwCurrentState = SCARD_STATE_UNAWARE;
  
      lReturn = ::SCardGetStatusChange(m_hSC,0,rgscState,1 );
      THROW_SCARDEXCEPTION(lReturn);
  
      LOGGER_DEBUG<<_T("SCardGetStatusChange(): dwEventState   = ")<<SCardEventState(rgscState[0].dwEventState);
  
      if( (rgscState[0].dwEventState&SCARD_STATE_PRESENT)==0 )
      { if( m_CardPresent ) 
          OnRemoved();
      } // of if
      else if( timeout>0 )
      { rgscState[0].dwCurrentState = rgscState[0].dwEventState;
      
        lReturn = ::SCardGetStatusChange(m_hSC,timeout,rgscState,1 );
        THROW_SCARDEXCEPTION(lReturn);
    
        LOGGER_DEBUG<<_T("SCardGetStatusChange(): dwEventState   = ")<<SCardEventState(rgscState[0].dwEventState);
  
        if( (rgscState[0].dwEventState&SCARD_STATE_PRESENT)==0 && m_CardPresent ) 
          OnRemoved();
      } // of if
    } // of if
  }
  
  /**
   *
   */
  void Smartcard::GetATR(ATR& result)
  { result = m_ATR;
  } // of Smartcard:GetATR()
  
  /**
   *
   */
  void Smartcard::GetAttribute(DWORD dwAttrId)
  { if( m_hCardConnected ) 
    { SCardAttribute attrib = SCardAttribute(m_hCard,dwAttrId);
    
      LOGGER_INFO<<attrib;
    } // of if
  } // of Smartcard:GetAttribute()
  
  
  /**
   *
   */
  void Smartcard::ListCards(VTString& result)
  { result.clear();
    
    LPTSTR    buffer  = NULL;
    DWORD     dwCards = 0;
    
    if( m_hSCInited && SCARD_S_SUCCESS==::SCardListCards(m_hSC,NULL,NULL,0,NULL,&dwCards) )
    { auto_ptr<TCHAR> buffer =  auto_ptr<TCHAR>(new TCHAR[dwCards]);

      ::memset(buffer.get(),dwCards,sizeof(TCHAR));

      if( buffer.get()!=NULL )
      { if( SCARD_S_SUCCESS==::SCardListCards(m_hSC,NULL,NULL,0,buffer.get(),&dwCards) )
        { LPCTSTR str = buffer.get();
        
          for( ;*str != _T('\0'); )
          { int len = _tcslen(str) + 1;
          
            result.push_back( TString(str) );
          
            str += len;
          } // of for
        } // of if
        
      } // of if
    } // of if
  } // of Smartcard::ListCards()
  
  /**
   *
   */
  void Smartcard::ListReaders(VTString& result)
  { result.clear();
    
    LPTSTR    buffer    = NULL;
    DWORD     dwReaders = 0;
    
    if( m_hSCInited && SCARD_S_SUCCESS==SCardListReaders(m_hSC,NULL,NULL,&dwReaders) )
    { auto_ptr<TCHAR> buffer = auto_ptr<TCHAR>(new TCHAR[dwReaders]);

      ::memset(buffer.get(),dwReaders,sizeof(TCHAR));
      
      if( buffer.get()!=NULL )
      { if( SCARD_S_SUCCESS== ::SCardListReaders(m_hSC,NULL,buffer.get(),&dwReaders) )
        { LPCTSTR str = buffer.get();
  
          for( ;*str != _T('\0'); )
          { int len = _tcslen(str) + 1;
          
            result.push_back(str);
            
            str += len;
          } // of for
        } // of if
      } // of if
    } // of if
  }
  
  /**
   *
   */
  void Smartcard::OnInserted()
  { LOGGER_INFO<<_T("Inserted")<<endl;
  
    m_CardPresent = true;
  }
  
  /**
   *
   */
  void Smartcard::OnRemoved()
  { LOGGER_INFO<<_T("Removed")<<endl;

    m_CardPresent = false;
  }
  
  /**
   *
   */
  void Smartcard::GetFeatureRequest()
  { BYTE   recvBuffer[1024];
    DWORD  recvLen = 0;

    memset( recvBuffer, '\0', sizeof(recvBuffer) );

    LONG   lReturn = SCARD_S_SUCCESS;
  
    if( m_hCardConnected )
    { lReturn = ::SCardControl(m_hCard, CM_IOCTL_GET_FEATURE_REQUEST, NULL, 0,(LPVOID)recvBuffer, sizeof(recvBuffer), &recvLen);
      THROW_SCARDEXCEPTION(lReturn);
      
      int length=0;
      
      if( (recvLen % sizeof(PCSC_TLV))==0 )
      { int maxRecords = recvLen / sizeof(PCSC_TLV);
      
        PCSC_TLV* pcsc_tlv = (PCSC_TLV *)recvBuffer;
        for( int i=0;i<maxRecords;i++ )
        { BYTE  tag   = pcsc_tlv[i].tag;
          ULONG ioctl = ntohl(pcsc_tlv[i].value);
          
          LOGGER_DEBUG<<_T("Smartcard::GetFeatureRequest(): [tag=0x")<<hex<<tag<<_T("]: ioctl=")<<ioctl;
        
          m_features.insert( BY_UL_Pair(tag,ioctl) );
        } // of for
      } //
      else
      { LOGGER_DEBUG<<_T("Smartcard::GetFeatureRequest(): Bad TLV values!"); }
    } // of if
  } // of Smartcard:GetFeatureRequest()
  
  /**
   *
   */
  bool Smartcard::HasFeature(Smartcard::Feature f)
  { bool result = m_features.find( (BYTE)f )!=m_features.end();
  
    return result;
  } // of Smartcard::HasFeature
  
  
  /**
   *
   */
  bool Smartcard::OutputText(const TString& s,BYTE timeout)
  { bool result = false;
  
    if( m_hCardConnected && HasFeature(FEATURE_MCT_READERDIRECT) )
    { BYTE  recvBuffer[1024];
      DWORD recvLen   = 0;

      memset( recvBuffer, '\0', sizeof(recvBuffer) );

      ULONG ioctl     = m_features.find( (BYTE)FEATURE_MCT_READERDIRECT )->second;
      
      /*
       * CLA=0x20 MCT TERMINAL
       * INS=0x17 OUTPUT
       * P1 =0x40 DISPLAY
       * P2 =0x00
       */
      APDU sendApdu = APDU(0x20,0x17,0x40,0x00);
  
      sendApdu.AddData( TLV(0x50,ByteBuffer(s)) );
  
      if( timeout>0 )
      { TLV timeoutTLV = TLV(0x80);
      
        timeoutTLV.push_back((BYTE)timeout);
  
        sendApdu.AddData( timeoutTLV );
      } // of if
  
      ByteBuffer sendBuffer = sendApdu;
  
      LOGGER_DEBUG<<_T("Smartcard::OutputText(")<<s.c_str()<<_T(",")<<ioctl<<_T(")")<<endl;
      LOGGER_DEBUG<<sendBuffer<<endl;
  
      LONG lReturn = ::SCardControl(m_hCard, ioctl, sendBuffer.data(), sendBuffer.size(),(LPVOID)recvBuffer, sizeof(recvBuffer), &recvLen);
      THROW_SCARDEXCEPTION(lReturn);
  
      LOGGER_DEBUG<<_T("Smartcard::OutputText(")<<s.c_str()<<_T(",")<<ioctl<<_T(")")<<endl;
      ByteBuffer revc(recvBuffer,recvLen);
  
      LOGGER_DEBUG<<revc<<endl;
      
      result = true;
    } // of if
    
    return result;
  } // of Smartcard::OutputText()
  
  /**
   *
   */
  bool Smartcard::InputText(const TString& msg,TString& s,BYTE timeout)
  { bool result = false;
  
    if( m_hCardConnected && HasFeature(FEATURE_MCT_READERDIRECT) )
    { BYTE  recvBuffer[1024];
      DWORD recvLen   = 0;
      
      memset( recvBuffer, '\0', sizeof(recvBuffer) );

      ULONG ioctl     = m_features.find( (BYTE)FEATURE_MCT_READERDIRECT )->second;
      
      /*
       * CLA=0x20 MCT TERMINAL
       * INS=0x16 OUTPUT
       * P1 =0x50 KEYBOARD
       * P2 =0x01 Input indication
       */
      APDU sendApdu = APDU(0x20,0x16,0x50,0x02);
  
      if( msg.size()>0 )
        sendApdu.AddData( TLV(0x50,ByteBuffer(msg)) );
  
      if( timeout>0 )
      { TLV timeoutTLV = TLV(0x80);
      
        timeoutTLV.push_back((BYTE)timeout);
  
        sendApdu.AddData( timeoutTLV );
      } // of if
      
      sendApdu.SetLe(0);
  
      ByteBuffer sendBuffer = sendApdu;

      LOGGER_INFO<<_T("Smartcard::InputText(")<<s.c_str()<<_T(",")<<ioctl<<_T(")")<<endl;
      LOGGER_INFO<<sendBuffer<<endl;
  
      LONG lReturn = ::SCardControl(m_hCard, ioctl, sendBuffer.data(), sendBuffer.size(),(LPVOID)recvBuffer, sizeof(recvBuffer), &recvLen);
      THROW_SCARDEXCEPTION(lReturn);
  
      LOGGER_INFO<<_T("Smartcard::InputText(")<<s.c_str()<<_T(",")<<ioctl<<_T(")")<<endl;
      ByteBuffer recv(recvBuffer,recvLen);
  
      LOGGER_INFO<<recv<<endl;
      
      s = recv;
    } // of if
  
    return result;
  } // of Smartcard::InputText()
  
  
  /**
   *
   */
  bool Smartcard::PinpadVerification(const TString& msg,BYTE timeout,
                                     PinCoding pinCoding,BYTE pinLen,BYTE maxPinLen,
                                     BYTE CHVNo,bool isGSM
                                    )
  { bool result = false;
  
    if( m_hCardConnected && HasFeature(FEATURE_MCT_READERDIRECT) )
    { BYTE  recvBuffer[1024];
      DWORD recvLen   = 0;

      memset( recvBuffer, '\0', sizeof(recvBuffer) );

      ULONG ioctl     = m_features.find( (BYTE)FEATURE_MCT_READERDIRECT )->second;
      
      /*
       * CLA=0x20 MCT TERMINAL
       * INS=0x18 PERFORM VERIFICATION
       * P1 =0x01 CT Interface
       * P2 =0x00 PINPAD (0x01=biometric)
       */
      APDU sendApdu = APDU(0x20,0x18,0x01,0x00);
      
      ByteBuffer cmd;
      
      BYTE controlByte = (BYTE)0x00;
      
      if( pinLen>0 && pinLen<16 )
        controlByte |= 0xf0&(pinLen<<4);
      
      /**
       * 00 = BCD
       * 01 = ASCII
       * 10 = Format 2 PIN Block for HBCI
       */
      switch( pinCoding )
      { case ASCII:
          controlByte |= 0x01;
          break;
        case BCD:
        default:
          break;
      } // of switch
      
      cmd.push_back(controlByte);

      // insertion position
      cmd.push_back((BYTE)0x06);
      
      cmd.push_back(isGSM ? (BYTE)0xA0 : (BYTE)0x00); // CLA 0xA0=GSM 
      cmd.push_back((BYTE)0x20);                      // INS 0x20=VERIFY
      cmd.push_back((BYTE)0x00);                      // P1  0x00
      cmd.push_back(CHVNo);                           // P2  no of PIN
      
      /*
       * create verify command with padding
       */
      if( maxPinLen>0 && maxPinLen>=pinLen )
      { cmd.push_back(maxPinLen);
      
        for( BYTE i=0;i<maxPinLen;i++ )
          cmd.push_back((BYTE)0xff);
      } // of if

      sendApdu.AddData( TLV(0x52,cmd) );

      if( msg.size()>0 )
        sendApdu.AddData( TLV(0x50,ByteBuffer(msg)) );
  
      if( timeout>0 )
      { TLV timeoutTLV = TLV(0x80);
      
        timeoutTLV.push_back((BYTE)timeout);
  
        sendApdu.AddData( timeoutTLV );
      } // of if
      
      sendApdu.SetLe(0);
  
      ByteBuffer sendBuffer = sendApdu;

      LOGGER_DEBUG<<_T("Smartcard::PinpadVerification(")<<msg.c_str()<<_T(",")<<ioctl<<_T(")")<<endl;
      LOGGER_DEBUG<<sendBuffer<<endl;
  
      LONG lReturn = ::SCardControl(m_hCard, ioctl, sendBuffer.data(), sendBuffer.size(),(LPVOID)recvBuffer, sizeof(recvBuffer), &recvLen);
      THROW_SCARDEXCEPTION(lReturn);
  
      LOGGER_DEBUG<<_T("Smartcard::PinpadVerification(")<<msg.c_str()<<_T(",")<<ioctl<<_T(")")<<endl;
      ByteBuffer recv(recvBuffer,recvLen);
  
      LOGGER_DEBUG<<recv<<endl;

      SCardAPDUException ex;
      
      if( recvLen>=2 )
      { BYTE sw1 = recvBuffer[recvLen-2];
        BYTE sw2 = recvBuffer[recvLen-1];
        
        ex = SCardAPDUException((sw1<<8) | sw2);
      } // of if
      
      if( ex.IsError() )
        throw ex;
      
      result = true;
    } // of if
  
    return result;
  } // of Smartcard::PinpadVerification()
  
  /**
   *
   */
  void Smartcard::Transmit(APDU& send,bool genMac)
  { if( m_hCardConnected )
    { BYTE  recvBuffer[1024];
      DWORD recvLen   = sizeof(recvBuffer)/sizeof(recvBuffer[0]);

      memset( recvBuffer, '\0', sizeof(recvBuffer) );
      
      ByteBuffer sendBuffer = (ByteBuffer)send;
      
      LOGGER_DEBUG<<_T("Smartcard::Transmit(): send")<<endl;
      LOGGER_DEBUG<<sendBuffer<<endl;

      if( genMac )
      { ByteBuffer out;
  
        m_pEncrypt->Reset();
        m_pEncrypt->Process(sendBuffer,out);
      } // of if
      
      LONG lReturn = ::SCardTransmit(m_hCard,(LPCSCARD_IO_REQUEST)m_sendPci,sendBuffer.data(),sendBuffer.size(),NULL,recvBuffer,&recvLen);
      THROW_SCARDEXCEPTION(lReturn);
      
      SCardAPDUException ex;
      
      if( recvLen>=2 )
      { BYTE sw1 = recvBuffer[recvLen-2];
        BYTE sw2 = recvBuffer[recvLen-1];
        
        ex = SCardAPDUException((sw1<<8) | sw2);
      } // of if
      
      send.SetResponse(recvBuffer,recvLen-2,ex);

      LOGGER_DEBUG<<_T("Smartcard::Transmit(): received")<<endl;
      LOGGER_DEBUG<<send<<endl;

      if( genMac && recvLen>2 )
      { ByteBuffer out;
      
        m_pEncrypt->Reset();
        m_pEncrypt->Process(ByteBuffer(recvBuffer,recvLen-2),out,BlockCipher::CBC);

        ByteBuffer in1;
        
        m_pDecrypt->Reset();
        m_pDecrypt->Process(out,in1,BlockCipher::CBC);
      } // of if

      if( ex.IsError() )
        throw ex;
    } // of if
  } // of Smartcard::Send()
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
