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
#if !defined(BLOCKCIPHER_H)
#define BLOCKCIPHER_H

#include "os.h"
#include "sc/bytebuffer.h"

namespace bvr20983
{

  /**
   * Block cipher engines are expected to conform to this interface.
   */
  class BlockCipher
  {
    public:
      enum CipherMode
      { ECB,
        CBC
      };

      static BlockCipher* GetInstance(LPCTSTR cipher);
    
      /**
       * Initialise the cipher.
       *
       * @param forEncryption if true the cipher is initialised for
       *  encryption, if false for decryption.
       * @param params the key and other data required by the cipher.
       * @exception IllegalArgumentException if the params argument is
       * inappropriate.
       */
      virtual void Init(ByteBuffer key,bool forEncryption) = 0;
  
      /**
       * Return the name of the algorithm the cipher implements.
       *
       * @return the name of the algorithm the cipher implements.
       */
      virtual LPCTSTR GetAlgorithmName() const = 0;
  
      /**
       * Return the block size for this cipher (in bytes).
       *
       * @return the block size for this cipher in bytes.
       */
      virtual int GetBlockSize() const = 0;
  
      /**
       * Cipher the input bytebuffer into the out bytebutter.
       *
       * @return the number of bytes processed and produced.
       */
      virtual DWORD Process(const ByteBuffer& in, ByteBuffer& out,CipherMode mode=ECB) = 0;
  
      /**
       * Reset the cipher. After resetting the cipher is in the same state
       * as it was after the last init (if there was one).
       */
      virtual void Reset() = 0;
  }; // of interface BlockCipher
} // of namespace bvr20983

#endif // BLOCKCIPHER_H
