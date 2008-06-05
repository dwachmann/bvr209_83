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
#if !defined(DESCIPHER_H)
#define DESCIPHER_H

#include "os.h"
#include "sc/blockcipher.h"

namespace bvr20983
{

  /**
   * a class that provides a basic DES Cipher.
   */
  class DESCipher : public BlockCipher
  {
    public:
      DESCipher();
      ~DESCipher();
  
      void Init(ByteBuffer key,bool encrypting=true);
    
      LPCTSTR GetAlgorithmName() const
      { return _T("DES"); }
    
      int GetBlockSize() const
      { return BLOCK_SIZE; }
      
      void SetICV(const ByteBuffer& icv)
      { m_icv=icv; }
    
      DWORD Process(const ByteBuffer& in,ByteBuffer& out,CipherMode mode=ECB);
    
      void Reset();

    protected:
      DWORD* GenerateKey(ByteBuffer& key,bool encrypting);
      void   Cipher(DWORD* workingKey,ByteBuffer& in,int inOff,BYTE* out);
      void   Cipher(DWORD* workingKey,BYTE*       in,int inOff,BYTE* out);

      void   CipherCore(DWORD* workingKey,DWORD left,DWORD right,BYTE* out);

      bool       m_encrypting;
      ByteBuffer m_icv;
  
      static int  BLOCK_SIZE;
  
    private:
      static WORD  bytebit[];
      static DWORD bigbyte[];
      static BYTE  pc1[];
      static BYTE  totrot[];
      static BYTE  pc2[];
      static DWORD SP1[];
      static DWORD SP2[];
      static DWORD SP3[];
      static DWORD SP4[];
      static DWORD SP5[];
      static DWORD SP6[];
      static DWORD SP7[];
      static DWORD SP8[];
      static BYTE  icvDefault[];

      DWORD* m_workingKey;
  }; // of class DESCipher

} // of namespace bvr20983

#endif // DESCIPHER_H


