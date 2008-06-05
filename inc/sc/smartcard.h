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
#if !defined(SMARTCARD_H)
#define SMARTCARD_H

#include "sc/sctypes.h"
#include "sc/bytebuffer.h"
#include "sc/apdu.h"
#include "sc/atr.h"
#include "sc/tlv.h"
#include "sc/scardattribute.h"
#include "sc/scardeventstate.h"
#include "sc/blockcipher.h"

namespace bvr20983
{

  class Smartcard
  {
    public:
      enum Feature
      { FEATURE_VERIFY_PIN_START =0x01, /* OMNIKEY Proposal    */
        FEATURE_VERIFY_PIN_FINISH=0x02, /* OMNIKEY Proposal    */
        FEATURE_MODIFY_PIN_START =0x03, /* OMNIKEY Proposal    */
        FEATURE_MODIFY_PIN_FINISH=0x04, /* OMNIKEY Proposal    */
        FEATURE_GET_KEY_PRESSED  =0x05, /* OMNIKEY Proposal    */
        FEATURE_VERIFY_PIN_DIRECT=0x06, /* USB CCID PIN Verify */
        FEATURE_MODIFY_PIN_DIRECT=0x07, /* USB CCID PIN Modify */
        FEATURE_MCT_READERDIRECT =0x08, /* KOBIL Proposal      */
        FEATURE_MCT_UNIVERSAL    =0x09, /* KOBIL Proposal      */
        FEATURE_IFD_PIN_PROP     =0x0A, /* Gemplus Proposal    */
        FEATURE_ABORT            =0x0B  /* SCM Proposal        */
      };
      
      enum PinCoding
      { BCD,
        ASCII
      };
    
      Smartcard();
      ~Smartcard();
      
      void          ListCards(VTString&);
      void          ListReaders(VTString&);
      void          GetATR(ATR&);
      void          GetAttribute(DWORD dwAttrId);
      void          Connect(DWORD timeout,DWORD shareMode=SCARD_SHARE_SHARED);
      void          Disconnect(DWORD disposition=SCARD_LEAVE_CARD);
  
      void          OnInserted();
      void          OnRemoved();
      void          WaitForInsertEvent(DWORD timeout);
      void          WaitForRemoveEvent(DWORD timeout);
  
      bool          HasFeature(Feature f);
      
      bool          OutputText(const TString&,BYTE timeout=0);
      bool          InputText(const TString&,TString&,BYTE timeout=0);
      
      bool          IsCardPresent() const;
  
      void          Transmit(APDU& data,bool genMac=false);

      bool          PinpadVerification(const TString& msg,BYTE timeout,PinCoding pinCoding,BYTE pinLen,BYTE maxPinLen,BYTE CHVNo,bool isGSM);

      bool          IsConnected() const
      { return m_hCardConnected; }

      
    private:
      void                GetFeatureRequest();
      void                Init();
  
      SCARDCONTEXT        m_hSC;
      bool                m_hSCInited;
      LPCVOID             m_sendPci;
      
      SCARDHANDLE         m_hCard;
      bool                m_hCardConnected;
      bool                m_CardPresent;
  
      ATR                 m_ATR;
      TString             m_reader;
      
      BY_UL_Map           m_features;
      
      BlockCipher*        m_pEncrypt;
      BlockCipher*        m_pDecrypt;
  }; // of class Smartcard
  
  typedef auto_ptr<Smartcard> PSmartcard;

} // of namespace bvr20983

#endif // SMARTCARD_H
