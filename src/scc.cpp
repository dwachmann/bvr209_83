/*
 * $Id$
 * 
 * Test program using pcsc interface.
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
#include "scc.h"
#include "sc/bytebuffer.h"
#include "sc/apdu.h"
#include "sc/atr.h"
#include "sc/smartcard.h"
#include "sc/cardfile.h"
#include "exception/bvr20983exception.h"
#include "exception/seexception.h"
#include "util/logstream.h"

using namespace bvr20983;
using namespace std;

static bool gGsmCard = false;
static bool gGenMac  = false;


/**
 *
 */
void printUsage(LPCTSTR progName)
{ LOGGER_INFO<<progName<<_T(": [-r] [-c] [-a] [-b] [-atr] [-o <text>] [-i <text>] [-pin <pin>] [-verify <pinno>] [-readfile fid] [-readbinary fid] ")<<endl;
  LOGGER_INFO<<_T("         -r: List Readers")<<endl;
  LOGGER_INFO<<_T("         -c: List Cards")<<endl;
  LOGGER_INFO<<_T("         -a: Get Attributes")<<endl;
  LOGGER_INFO<<_T("         -f: Get Features")<<endl;
  LOGGER_INFO<<_T("         -o: output text on terminal")<<endl;
  LOGGER_INFO<<_T("         -i: input text on terminal")<<endl;
  LOGGER_INFO<<_T("         -b: balance for sample java wallet")<<endl;
  LOGGER_INFO<<_T("       -pin: verify PIN")<<endl;
  LOGGER_INFO<<_T("       -atr: Show ATR of card")<<endl;
  LOGGER_INFO<<_T("       -gsm: a gsm card")<<endl;
  LOGGER_INFO<<_T("       -mac: generate a secure channel mac")<<endl;
  LOGGER_INFO<<_T("    -verify: verify PIN no")<<endl;
  LOGGER_INFO<<_T("  -readfile: Read a record file")<<endl;
  LOGGER_INFO<<_T("-readbinary: Read a binary file")<<endl;
}

/**
 *
 */
void ListReaders(Smartcard& pSmartcard)
{ VTString readers = VTString();
  
  pSmartcard.ListReaders(readers);

  VTString::iterator iter;
  int                i=0;

  for( iter=readers.begin();iter!=readers.end();iter++,i++ )
    LOGGER_INFO<<_T("reader[")<<i<<_T("]=")<<iter->c_str()<<endl;
}

/**
 *
 */
void ListCards(Smartcard& pSmartcard)
{ VTString cards = VTString();

  pSmartcard.ListCards(cards);

  VTString::iterator iter;
  int                i=0;
  
  for( iter=cards.begin();iter!=cards.end();iter++,i++ )
    LOGGER_INFO<<_T("card[")<<i<<_T("]=")<<iter->c_str()<<endl;
}

/**
 *
 */
void ShowATR(Smartcard& pSmartcard)
{ ATR atr;

  if( !pSmartcard.IsCardPresent() )
    LOGGER_INFO<<_T("Insert card...")<<endl;
  
  pSmartcard.Connect(10000);
  
  pSmartcard.GetATR(atr);
        
  LOGGER_INFO<<atr;

  if( pSmartcard.IsCardPresent() && !pSmartcard.OutputText(TString(_T("Remove Card")),10) )
    LOGGER_INFO<<_T("Remove card...");

  pSmartcard.Disconnect(SCARD_EJECT_CARD);
}

/**
 *
 */
void OutputText(Smartcard& pSmartcard,LPCTSTR text)
{ pSmartcard.Connect(10000);
  
  pSmartcard.OutputText(TString(text),4);
        
  pSmartcard.WaitForRemoveEvent(10000);
}

/**
 *
 */
void InputText(Smartcard& pSmartcard,LPCTSTR text)
{ pSmartcard.Connect(10000);
  
  TString in;
  
  pSmartcard.InputText(TString(text),in,4);
  
  LOGGER_INFO<<_T("InputText:<")<<in.c_str()<<_T(")")<<endl;
        
  pSmartcard.WaitForRemoveEvent(10000);
}

/**
 *
 */
void GetBalance(Smartcard& pSmartcard)
{ pSmartcard.Connect(10000);

  BYTE       aid[]     = {(BYTE)0xA0,(BYTE)0x00,(BYTE)0x00,(BYTE)0x00,(BYTE)0x99,(BYTE)0x01};
  APDU       selectAID = APDU(0x00,0xA4,0x04,0x00);
  
  selectAID.AddData( ByteBuffer(aid,sizeof(aid)) );

  pSmartcard.Transmit(selectAID);
  
  LOGGER_INFO<<_T("GetBalance select AID")<<endl<<selectAID;
  
  APDU       send = APDU(0xB0,0x50,0x00,0x00);
  send.SetLe(2);
  
  pSmartcard.Transmit(send);

  LOGGER_INFO<<_T("GetBalance")<<endl<<send;
}

/**
 *
 */
void ReadFile(Smartcard& pSmartcard,WORD fid)
{ pSmartcard.Connect(10000);

  CardFile cf(gGsmCard);
  
  cf.Select(pSmartcard,fid);
  
  cf.Read(pSmartcard,gGenMac);
}

/**
 *
 */
void ReadBinary(Smartcard& pSmartcard,WORD fid)
{ pSmartcard.Connect(10000);

  CardFile cf(gGsmCard);
  
  cf.Select(pSmartcard,fid);
  
  cf.ReadBinary(pSmartcard);
}

/**
 *
 */
void VerifyPIN(Smartcard& pSmartcard,LPCTSTR pin)
{ pSmartcard.Connect(10000);

  CardFile cf(gGsmCard);
  
  cf.Verify(pSmartcard,(BYTE)1,pin,(BYTE)8);
}

/**
 *
 */
void Verify(Smartcard& pSmartcard,BYTE pinNo)
{ pSmartcard.Connect(10000);

  TCHAR msg[256];
  
  _stprintf_s(msg,sizeof(msg)/sizeof(msg[0]),_T("Input PIN %d:"),pinNo);

  pSmartcard.PinpadVerification(TString(msg),30,Smartcard::ASCII,4,8,pinNo,true);
}

/**
 *
 */
void ShowAttributes(Smartcard& pSmartcard)
{ pSmartcard.Connect(10000);

  pSmartcard.GetAttribute(SCARD_ATTR_ATR_STRING);
  pSmartcard.GetAttribute(SCARD_ATTR_CHARACTERISTICS);
  pSmartcard.GetAttribute(SCARD_ATTR_CURRENT_IFSC);
  pSmartcard.GetAttribute(SCARD_ATTR_CURRENT_IFSD);
  pSmartcard.GetAttribute(SCARD_ATTR_CURRENT_PROTOCOL_TYPE);
  pSmartcard.GetAttribute(SCARD_ATTR_DEFAULT_CLK);
  pSmartcard.GetAttribute(SCARD_ATTR_DEFAULT_DATA_RATE);
//  pSmartcard.GetAttribute(SCARD_ATTR_DEVICE_FRIENDLY_NAME);
  pSmartcard.GetAttribute(SCARD_ATTR_DEVICE_IN_USE);
  pSmartcard.GetAttribute(SCARD_ATTR_DEVICE_SYSTEM_NAME);
  pSmartcard.GetAttribute(SCARD_ATTR_DEVICE_UNIT);
  pSmartcard.GetAttribute(SCARD_ATTR_ICC_INTERFACE_STATUS);
  pSmartcard.GetAttribute(SCARD_ATTR_ICC_PRESENCE);

  pSmartcard.GetAttribute(SCARD_ATTR_ICC_TYPE_PER_ATR);
  pSmartcard.GetAttribute(SCARD_ATTR_MAX_CLK);
  pSmartcard.GetAttribute(SCARD_ATTR_MAX_DATA_RATE);
  pSmartcard.GetAttribute(SCARD_ATTR_MAX_IFSD);
  pSmartcard.GetAttribute(SCARD_ATTR_POWER_MGMT_SUPPORT);
  pSmartcard.GetAttribute(SCARD_ATTR_PROTOCOL_TYPES);

  pSmartcard.GetAttribute(SCARD_ATTR_CHANNEL_ID);
  pSmartcard.GetAttribute(SCARD_ATTR_VENDOR_IFD_SERIAL_NO);
  pSmartcard.GetAttribute(SCARD_ATTR_VENDOR_IFD_TYPE);
  pSmartcard.GetAttribute(SCARD_ATTR_VENDOR_IFD_VERSION);

  pSmartcard.GetAttribute(SCARD_ATTR_VENDOR_NAME);
  
  pSmartcard.WaitForRemoveEvent(10000);
}



/**
 *
 */
void GetFeatures(Smartcard& pSmartcard)
{ pSmartcard.Connect(10000);

  if( pSmartcard.HasFeature(Smartcard::FEATURE_VERIFY_PIN_START) )
    LOGGER_INFO<<_T("FEATURE_VERIFY_PIN_START : true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_VERIFY_PIN_START : false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_VERIFY_PIN_FINISH) )
    LOGGER_INFO<<_T("FEATURE_VERIFY_PIN_FINISH: true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_VERIFY_PIN_FINISH: false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_MODIFY_PIN_START) )
    LOGGER_INFO<<_T("FEATURE_MODIFY_PIN_START : true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_MODIFY_PIN_START : false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_MODIFY_PIN_FINISH) )
    LOGGER_INFO<<_T("FEATURE_MODIFY_PIN_FINISH: true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_MODIFY_PIN_FINISH: false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_GET_KEY_PRESSED) )
    LOGGER_INFO<<_T("FEATURE_GET_KEY_PRESSED  : true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_GET_KEY_PRESSED  : false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_VERIFY_PIN_DIRECT) )
    LOGGER_INFO<<_T("FEATURE_VERIFY_PIN_DIRECT: true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_VERIFY_PIN_DIRECT: false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_MODIFY_PIN_DIRECT) )
    LOGGER_INFO<<_T("FEATURE_MODIFY_PIN_DIRECT: true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_MODIFY_PIN_DIRECT: false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_MCT_READERDIRECT) )
    LOGGER_INFO<<_T("FEATURE_MCT_READERDIRECT : true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_MCT_READERDIRECT : false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_MCT_UNIVERSAL) )
    LOGGER_INFO<<_T("FEATURE_MCT_UNIVERSAL    : true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_MCT_UNIVERSAL    : false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_IFD_PIN_PROP) )
    LOGGER_INFO<<_T("FEATURE_IFD_PIN_PROP     : true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_IFD_PIN_PROP     : false")<<endl;

  if( pSmartcard.HasFeature(Smartcard::FEATURE_ABORT) )
    LOGGER_INFO<<_T("FEATURE_ABORT            : true")<<endl;
  else
    LOGGER_INFO<<_T("FEATURE_ABORT            : false")<<endl;
  
//  pSmartcard.WaitForRemoveEvent(10000);
} // of GetFeatures()



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

 Function : _tmain

 Synopsis : Entry point of SCQuery.

 Parameter: Standard ANSI C command line parameters.

 Return   : SCARD_S_SUCCESS or error code
------------------------------------------------------------------------------*/
extern "C" int __cdecl _tmain (int argc, _TCHAR  * argv[])
{ LONG   exHr           = 0;
  bool   someOption     = false;
  bool   listReaders    = false;
  bool   listCards      = false;
  bool   showATR        = false;
  bool   showAttributes = false;
  bool   getFeatures    = false;
  bool   getBalance     = false;
  LPTSTR outText        = NULL;
  LPTSTR inText         = NULL;
  LPTSTR inPIN          = NULL;
  bool   readFile       = false;
  bool   readBinary     = false;
  int    fileFid        = 0;
  int    pinNo          = 255;

  if( argc<=1 )
    printUsage(argv[0]);
  else
  { for( int i=1;i<argc;i++ )
    { if( _tcscmp(_T("-r"),argv[i])==0 )
        someOption = listReaders = true;
      
      else if( _tcscmp(_T("-c"),argv[i])==0 )
        someOption = listCards = true;
      else if( _tcscmp(_T("-atr"),argv[i])==0 )
        someOption = showATR = true;
      else if( _tcscmp(_T("-gsm"),argv[i])==0 )
        gGsmCard = true;
      else if( _tcscmp(_T("-mac"),argv[i])==0 )
        gGenMac = true;
      else if( _tcscmp(_T("-a"),argv[i])==0 )
        someOption = showAttributes = true;
      else if( _tcscmp(_T("-f"),argv[i])==0 )
        someOption = getFeatures = true;
      else if( _tcscmp(_T("-b"),argv[i])==0 )
        someOption = getBalance = true;
      else if( _tcscmp(_T("-o"),argv[i])==0 )
      { someOption = true;
        outText    = argv[++i];
      }
      else if( _tcscmp(_T("-pin"),argv[i])==0 )
      { someOption = true;
        inPIN      = argv[++i];
      }
      else if( _tcscmp(_T("-i"),argv[i])==0 )
      { someOption = true;
        inText    = argv[++i];
      }
      else if( _tcscmp(_T("-readfile"),argv[i])==0 )
      { someOption = readFile = true;
        _stscanf_s(argv[++i],_T("%04X"),&fileFid);
      }
      else if( _tcscmp(_T("-readbinary"),argv[i])==0 )
      { someOption = readBinary = true;
        _stscanf_s(argv[++i],_T("%04X"),&fileFid);
      }
      else if( _tcscmp(_T("-verify"),argv[i])==0 )
      { someOption = true;
        _stscanf_s(argv[++i],_T("%d"),&pinNo);
      }
    } // of for
  
    if( someOption )
    { exHr = SCARD_S_SUCCESS;

      try
      { _set_se_translator( SEException::throwException );

        PSmartcard pSmartcard(new Smartcard());
        
        if( listReaders )
          ListReaders(*pSmartcard);
          
        if( listCards )
          ListCards(*pSmartcard);

        if( showATR )
          ShowATR(*pSmartcard);
          
        if( showAttributes )
          ShowAttributes(*pSmartcard);

        if( getFeatures )
          GetFeatures(*pSmartcard);

        if( getBalance )
          GetBalance(*pSmartcard);

        if( NULL!=outText )
          OutputText(*pSmartcard,outText);

        if( NULL!=inText )
          InputText(*pSmartcard,inText);
          
        if( readFile && fileFid!=0 )
          ReadFile(*pSmartcard,fileFid);

        if( readBinary && fileFid!=0 )
          ReadBinary(*pSmartcard,fileFid);

        if( inPIN!=NULL )
          VerifyPIN(*pSmartcard,inPIN);

        if( pinNo!=255 )
          Verify(*pSmartcard,pinNo);
   
      }
      catch(BVR20983Exception& e)
      { LOGGER_ERROR<<e;
      
        exHr = e.GetErrorCode();
      }
      catch(exception& e) 
      { LOGGER_ERROR<<"Exception "<<typeid(e).name()<<":"<<e.what()<<endl;

        exHr = -4;
      }
      catch(LPCTSTR& e)
      { LOGGER_ERROR<<e<<endl;

        exHr = -3;
      }
      catch(...)
      { LOGGER_ERROR<<_T("Exception")<<endl;

        exHr = -2;
      }
    } // of if
  } // of else
  
  return (int)exHr;
}
//======================================END-OF-FILE=========================
