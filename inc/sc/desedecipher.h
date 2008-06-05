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
#if !defined(DESEDECIPHER_H)
#define DESEDECIPHER_H

#include "os.h"
#include "sc/blockcipher.h"
#include "sc/descipher.h"

namespace bvr20983
{

  /**
   * a class that provides a basic Triple DES Cipher.
   */
  class DESedeCipher : public DESCipher
  {
    public:
      DESedeCipher();
      ~DESedeCipher();
  
      void Init(ByteBuffer key,bool encrypting=true);
    
      LPCTSTR GetAlgorithmName() const
      { return _T("DESede"); }
    
      DWORD Process(const ByteBuffer& in,ByteBuffer& out,CipherMode mode=ECB);
    
    private:
      DWORD*     m_workingKey1;
      DWORD*     m_workingKey2;
      DWORD*     m_workingKey3;
  }; // of class DESedeCipher

} // of namespace bvr20983

#endif // DESEDECIPHER_H


