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
        CryptoContext(DWORD dwProvType=PROV_RSA_FULL,DWORD dwFlags=CRYPT_VERIFYCONTEXT,LPCTSTR pszProvider=MS_DEF_PROV);
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
        DWORD Get(auto_ptr<BYTE>& pBuffer) const;
        void  Get(TString& h) const;

        HCRYPTHASH operator*()  const
        { return m_hHash; }

#ifdef _UNICODE      
        virtual wostream& Dump(wostream& os) const;
#else
        virtual ostream& Dump(ostream& os) const;
#endif

      private:
        HCRYPTHASH m_hHash;
    }; // of CryptoHash

    template<class charT, class Traits>
    basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const CryptoHash& e);


    class MD5Sum
    {
      public:
        MD5Sum();
        ~MD5Sum();

        void CalcFileHash(LPCTSTR fileName,auto_ptr<CryptoHash>& hash);

      private:
        CryptoContext m_cryptoCtx;
    }; // of class MD5Sum
  } // of namespace util
} // of namespace bvr20983

#endif // MD5SUM_H
/*==========================END-OF-FILE===================================*/
