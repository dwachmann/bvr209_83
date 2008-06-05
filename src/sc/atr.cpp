/*
 * $Id$
 * 
 * ATR class.
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
#include "sc/atr.h"

namespace bvr20983
{

  /*
   * ATR::ATR
   * ATR::~ATR
   *
   * Constructor Parameters:
   *  None
   */
  ATR::ATR()
  { clear();
  }

  ATR::ATR(const ByteBuffer& buf)
  { clear();
  
    m_buffer = buf;
    
    if( m_buffer.size()>0 )
    { DWORD i = 0;
    
      m_ts    = m_buffer[i++];
      m_td[0] = m_buffer[i++];
      
      for( int m=0;m<=3;m++ )
      { if( m_td[m]&0x10 )
          m_ta[m+1] = m_buffer[i++];

        if( m_td[m]&0x20 )
          m_tb[m+1] = m_buffer[i++];

        if( m_td[m]&0x40 )
          m_tc[m+1] = m_buffer[i++];

        if( m_td[m]&0x80 )
          m_td[m+1] = m_buffer[i++];
      } // of for
      
      m_histLen = 0x0f&m_td[0];

      for( int m=1;m<=m_histLen;m++ )
        m_hist[m] = m_buffer[i++];
        
      switch( (m_ta[1]&0xf0)>>4 )
      { case 0x00:
          m_F=372; m_FMax = 4;
          break;
        case 0x01:
          m_F=372; m_FMax = 5;
          break;
        case 0x02:
          m_F=558; m_FMax = 6;
          break;
        case 0x03:
          m_F=744; m_FMax = 8;
          break;
        case 0x04:
          m_F=1116; m_FMax = 12;
          break;
        case 0x05:
          m_F=1488; m_FMax = 16;
          break;
        case 0x06:
          m_F=1860; m_FMax = 20;
          break;
        case 0x09:
          m_F=512; m_FMax = 5;
          break;
        case 0x0a:
          m_F=768; m_FMax = 7;
          break;
        case 0x0b:
          m_F=1024; m_FMax = 10;
          break;
        case 0x0c:
          m_F=1536; m_FMax = 15;
          break;
        case 0x0d:
          m_F=2048; m_FMax = 20;
          break;
      } // of switch

      switch( (m_ta[1]&0x0f) )
      { case 0x01:
          m_D=1;
          break;
        case 0x02:
          m_D=2;
          break;
        case 0x03:
          m_D=4;
          break;
        case 0x04:
          m_D=8;
          break;
        case 0x05:
          m_D=16;
          break;
        case 0x06:
          m_D=32;
          break;
        case 0x08:
          m_D=12;
          break;
        case 0x09:
          m_D=20;
          break;
      } // of switch
    } // of if
  }

  /*
   *
   */
  void ATR::clear()
  { m_ts      = '\0';
    m_histLen = 0;
    
    memset( m_ta, '\0', sizeof(m_ta) );
    memset( m_tb, '\0', sizeof(m_tb) );
    memset( m_tc, '\0', sizeof(m_tc) );
    memset( m_td, '\0', sizeof(m_td) );
  
    memset( m_hist, '\0', sizeof(m_hist) );
    
    m_F    = 0; 
    m_FMax = 0;
    m_D    = 0;
    
    m_buffer.clear();
  }
  
  /**
   *
   */
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const ATR& atr)
  { os<<_T("ATR:")<<endl;

    if( atr.m_buffer.size()>0 )
    {
      os<<atr.m_buffer;
  
      os<<_T("ts:[0x")<<hex<<setw(2)<<setfill(_T('0'))<<atr.m_ts<<_T("]");
      if( atr.m_ts==0x3b ) 
        os<<_T(" direct convention")<<endl;
      else
        os<<_T(" inverse convention")<<endl;
       
      os<<_T("t0:[0x")<<hex<<setw(2)<<setfill(_T('0'))<<atr.m_td[0]<<_T("] -->")<<_T("history bytes:")<<atr.m_histLen<<endl;
  
      for( int i=1;i<=4;i++ )
        os<<_T("ta[")<<i<<_T("]=0x")<<hex<<setw(2)<<setfill(_T('0'))<<atr.m_ta[i]<<_T(" ");
  
      os<<endl;
      
      for( int i=1;i<=4;i++ )
        os<<_T("tb[")<<i<<_T("]=0x")<<hex<<setw(2)<<setfill(_T('0'))<<atr.m_tb[i]<<_T(" ");
  
      os<<endl;
  
      for( int i=1;i<=4;i++ )
        os<<_T("tc[")<<i<<_T("]=0x")<<hex<<setw(2)<<setfill(_T('0'))<<atr.m_tc[i]<<_T(" ");
  
      os<<endl;
  
      for( int i=1;i<=4;i++ )
        os<<_T("td[")<<i<<_T("]=0x")<<hex<<setw(2)<<setfill(_T('0'))<<atr.m_td[i]<<_T(" ");
  
      os<<endl;
  
      os<<dec<<_T("ta[1] F=")<<atr.m_F<<_T(" FMax=")<<atr.m_FMax<<_T(" D=")<<atr.m_D;
      
      DWORD bpsMax = (1000000*atr.m_FMax * atr.m_D) / atr.m_F;
      
      if( bpsMax==0 )
        bpsMax = 3571200 / 372;
      
      os<<dec<<_T(" --> maximum bits per second=")<<bpsMax<<endl;
  
      os<<dec<<_T("tc[1] extra guardtime=")<<(atr.m_tc[1]==0xff ? 12 : atr.m_tc[1]) <<_T(" etu")<<endl;
      
      DWORD workWaitingTime = (atr.m_tc[2]==0 ? 10 : atr.m_tc[2])*960*atr.m_D;
  
      os<<dec<<_T("tc[2] work waiting guardtime=")<<workWaitingTime<<_T(" etu")<<endl;
  
      os<<dec<<_T("ta[3] IFSC=")<<(atr.m_ta[3]==0 ? 32 : atr.m_ta[3])<<_T(" byte")<<endl;
  
      os<<dec<<_T("Historical Bytes")<<endl;
      for( int i=1;i<=atr.m_histLen;i++ )
      { os<<_T("[")<<hex<<setw(2)<<setfill(_T('0'))<<i<<_T("]=")<<hex<<setw(2)<<setfill(_T('0'))<<atr.m_hist[i]<<_T(" ");
      
        if( i%4==0 )
          os<<endl;
      } // of for
  
      os<<endl;
      
      if( atr.m_histLen>0 )
      { switch( atr.m_hist[1] )
        { case 0x00: 
            os<<_T("Status info at the end of historical characters")<<endl;
            break;
          case 0x10:
            os<<_T("Reference to DIR-File")<<endl;
            break;
          case 0x65:
            if( atr.m_histLen>=15 && atr.m_hist[2]==0x63 )
            { os<<_T("Geldkarte")<<endl;
            
              BYTE  icManufacturerID = atr.m_hist[3];
              BYTE  icTypeID         = atr.m_hist[4];
              BYTE  romMaskID        = atr.m_hist[5];
              
              WORD  countryCode      = (atr.m_hist[6]<<8) | atr.m_hist[7];
              WORD  nationalRegId    = (atr.m_hist[8]<<8) | atr.m_hist[9];
  
              DWORD chipSeriesNo     = (atr.m_hist[10]<<24) | (atr.m_hist[11]<<16) | (atr.m_hist[12]<<8) | atr.m_hist[13];
  
              BYTE  osMajorVersion   = atr.m_hist[14];
              BYTE  osMinorVersion   = atr.m_hist[15];
              
              os<<_T("  icManufacturerID = 0x")<<hex<<icManufacturerID<<endl;
              os<<_T("  icTypeID         = 0x")<<hex<<icTypeID<<endl;
              os<<_T("  romMaskID        = 0x")<<hex<<romMaskID<<endl;
              os<<_T("  countryCode      = 0x")<<hex<<countryCode<<endl;
              os<<_T("  nationalRegId    = 0x")<<hex<<nationalRegId<<endl;
              os<<_T("  chipSeriesNo     = 0x")<<hex<<chipSeriesNo<<endl;
              os<<_T("  osMajorVersion   = 0x")<<hex<<osMajorVersion<<endl;
              os<<_T("  osMinorVersion   = 0x")<<hex<<osMinorVersion<<endl;
            } // of if
            break;
          case 0x80:
            os<<_T("Compact-TLV Status info")<<endl;
            
            { BYTE i=2;
              
              while( i<atr.m_histLen )
              { BYTE tag = atr.m_hist[i]>>4;
                BYTE len = atr.m_hist[i]&0x0f;
                
                i++;
  
                switch( tag )
                { case 0x1:
                    os<<_T("Country code and national date")<<endl;
                    break;
                  case 0x2:
                    os<<_T("Issuer identification number")<<endl;
                    break;
                  case 0x3:
                    os<<_T("Card Service data")<<endl;
                    
                    if( len==1 )
                    { BYTE cardService = atr.m_hist[i];
                    
                      if( cardService&0x80 )
                        os<<_T("  Direct application selection by full DF name")<<endl;
  
                      if( cardService&0x40 )
                        os<<_T("  Selection by partial DF name")<<endl;
  
                      if( cardService&0x20 )
                        os<<_T("  Data objects available in DIR-File")<<endl;
  
                      if( cardService&0x10 )
                        os<<_T("  Data objects available in ATR-File")<<endl;
  
                      if( (cardService&0x08)==0x08 )
                        os<<_T("  File I/O services by READ BINARY command")<<endl;
  
                      if( (cardService&0x08)==0x00 )
                        os<<_T("  File I/O services by READ RECORD command")<<endl;
                    } // of if
                    break;
                  case 0x4:
                    os<<_T("Initial Access Data")<<endl;
                    break;
                  case 0x5:
                    os<<_T("Card issuer Data")<<endl;
  
                    os<<_T("  ");
                    
                    for( BYTE k=0;k<len;k++ )
                      os<<hex<<setw(2)<<setfill(_T('0'))<<atr.m_hist[i+k]<<_T(' ');
                      
                    os<<endl;
                    break;
                  case 0x7:
                    os<<_T("Card Capabilities")<<endl;
                    
                    if( len>=1 )
                    { BYTE cardCap = atr.m_hist[i+0];
                    
                      if( cardCap&0x80 )
                        os<<_T("  DF selection by full DF name ")<<endl;
  
                      if( cardCap&0x40 )
                        os<<_T("  DF selection by partial DF name ")<<endl;
  
                      if( cardCap&0x20 )
                        os<<_T("  DF selection by path ")<<endl;
  
                      if( cardCap&0x10 )
                        os<<_T("  DF selection by file identifier ")<<endl;
                    
                      if( cardCap&0x08 )
                        os<<_T("  DF selection implicit ")<<endl;
  
                      if( cardCap&0x04 )
                        os<<_T("  EF management: Short EF identifier supported ")<<endl;
  
                      if( cardCap&0x02 )
                        os<<_T("  EF management: Record number supported ")<<endl;
  
                      if( cardCap&0x01 )
                        os<<_T("  EF management: Record identifier supported ")<<endl;
                    } // of if
  
                    os<<endl;
  
                    if( len>=2 )
                    { BYTE cardCap     = atr.m_hist[i+1];
                      BYTE dataNibbles = cardCap&0x7;
                    
                      if( (cardCap&0x60)==0x00 )
                        os<<_T("  Behavior of write functions: one-time write ")<<endl;
  
                      if( (cardCap&0x60)==0x20 )
                        os<<_T("  Behavior of write functions: proprietary ")<<endl;
  
                      if( (cardCap&0x60)==0x40 )
                        os<<_T("  Behavior of write functions: write OR  ")<<endl;
  
                      if( (cardCap&0x60)==0x06 )
                        os<<_T("  Behavior of write functions: write AND ")<<endl;
  
                      if( cardCap&0x08 )
                        os<<_T("  Implicit selection of DF")<<endl;
  
                      os<<_T("  Data unit size in nibbles: ")<<dataNibbles<<endl;
                    } // of if
  
                    os<<endl;
  
                    if( len>=3 )
                    { BYTE cardCap    = atr.m_hist[i+2];
                      BYTE noChannels = cardCap&0x03;
                    
                      if( cardCap&0x40 )
                        os<<_T("  Extended Lc and Le fields ")<<endl;
  
                      if( (cardCap&0x18)==0x08 )
                        os<<_T("  Logical channel assignment by the card ")<<endl;
  
                      if( (cardCap&0x18)==0x10 )
                        os<<_T("  Logical channel assignment by the interface device ")<<endl;
  
                      if( (cardCap&0x18)==0x00 )
                        os<<_T("  No logical channel ")<<endl;
  
                      os<<_T("  Maximum number of logical channels: ")<<noChannels<<endl;
                    } // of if
                    break;
                  case 0x8:
                    os<<_T("Status Information")<<endl;
                    break;
                  default:
                    os<<_T("Unknown tag ")<<tag<<endl;
                    break;
                } // of switch
                
                i += len;
              } // of while
            } 
            break;
        } // of switch
      } // of if
    } // of if
    
    return os;
  }
} // of namespace bvr20983

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const ATR&);
/*==========================END-OF-FILE===================================*/
