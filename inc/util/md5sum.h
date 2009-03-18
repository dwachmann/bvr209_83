/*
 * $Id: $
 * 
 * Copyright (C) 2009 Dorothea Wachmann
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
#ifndef MD5SUM_H
#define MD5SUM_H

#include <Wincrypt.h>

namespace bvr20983
{
  namespace util
  {
    class CryptoContext
    { 
      public:
        CryptoContext(DWORD dwProvType=PROV_RSA_SIG,DWORD dwFlags=CRYPT_VERIFYCONTEXT);
        ~CryptoContext();

        HCRYPTPROV operator*()  const
        { return m_hCryptProv; }

      private:
        HCRYPTPROV m_hCryptProv;
    }; // of CryptoContext

    class CryptoHash
    { 
      public:
        CryptoHash(const CryptoContext& cryptoContext,ALG_ID algId=CALG_MD5);
        ~CryptoHash();

        void  Put(BYTE* pBuffer,DWORD bufferLen);
        DWORD Get(auto_ptr<BYTE>& pBuffer);

        HCRYPTHASH operator*()  const
        { return m_hHash; }

      private:
        HCRYPTHASH m_hHash;
    }; // of CryptoHash

    class MD5Sum
    {
      public:
        static MD5Sum* GetInstance();
        static void    DeleteInstance();

        void CalcFileHash(LPCTSTR fileName);

      private:
        MD5Sum();
        ~MD5Sum();

        static MD5Sum* m_pMe;

        CryptoContext m_cryptoCtx;

    }; // of class MD5Sum
  } // of namespace util
} // of namespace bvr20983

#endif // MD5SUM_H
/*==========================END-OF-FILE===================================*/
