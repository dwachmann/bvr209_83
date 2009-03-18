/*
 * $Id: $
 * 
 * A class for download files using the windows BITS service.
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
#include "os.h"
#include "util/md5sum.h"
#include "util/logstream.h"
#include "util/handle.h"
#include "exception/lasterrorexception.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace util
  {
    MD5Sum* MD5Sum::m_pMe = NULL;

    /**
     *
     */
    CryptoContext::CryptoContext(DWORD dwProvType,DWORD dwFlags) :
      m_hCryptProv(NULL)
    { THROW_LASTERROREXCEPTION1( ::CryptAcquireContext(&m_hCryptProv,NULL,NULL,dwProvType,dwFlags) );
    }

    /**
     *
     */
    CryptoContext::~CryptoContext()
    { if( NULL!=m_hCryptProv) 
        ::CryptReleaseContext(m_hCryptProv, 0);

      m_hCryptProv = NULL;
    }

    /**
     *
     */
    CryptoHash::CryptoHash(const CryptoContext& cryptoContext,ALG_ID algId) : 
      m_hHash(NULL)
    { THROW_LASTERROREXCEPTION1( ::CryptCreateHash(*cryptoContext, algId, 0, 0, &m_hHash) );
    }

    /**
     *
     */
    CryptoHash::~CryptoHash()
    { if( NULL!=m_hHash )
        ::CryptDestroyHash(m_hHash);

      m_hHash = NULL;
    }

    /**
     *
     */
    void CryptoHash::Put(BYTE* pBuffer,DWORD bufferLen)
    { if( NULL!=m_hHash && NULL!=pBuffer )
      { THROW_LASTERROREXCEPTION1( ::CryptHashData(m_hHash, pBuffer, bufferLen, 0) ); }
    } // of CryptoHash::Put()

    /**
     *
     */
    DWORD CryptoHash::Get(auto_ptr<BYTE>& pBuffer)
    { DWORD dwHashLen = 0;

      if( NULL!=m_hHash )
      { THROW_LASTERROREXCEPTION1( ::CryptGetHashParam(m_hHash,HP_HASHVAL,NULL,&dwHashLen,0) );

        if( dwHashLen>0 )
        { BYTE* hashBuffer = new BYTE[dwHashLen];

          THROW_LASTERROREXCEPTION1( ::CryptGetHashParam(m_hHash,HP_HASHVAL,hashBuffer,&dwHashLen,0) );

          pBuffer.reset(hashBuffer);
        } // of if
      } // of if

      return dwHashLen;
    } // of CryptoHash::Get()

    /**
     *
     */
    MD5Sum* MD5Sum::GetInstance()
    { if( m_pMe==NULL )
        m_pMe = new MD5Sum();
    
      return m_pMe; 
    }

    /**
     *
     */
    void MD5Sum::DeleteInstance()
    { if( NULL!=m_pMe )
      { if( NULL!=m_pMe )
          delete m_pMe;
          
        m_pMe = NULL;
      } // of if
    }

    /**
     *
     */
    MD5Sum::MD5Sum() :
      m_cryptoCtx(PROV_RSA_SIG,CRYPT_VERIFYCONTEXT)
    { 
    }

    /**
     *
     */
    MD5Sum::~MD5Sum()
    { }

 
    /**
     *
     */
    void MD5Sum::CalcFileHash(LPCTSTR fileName)
    { FileHandle dataFile(fileName);
      CryptoHash hash(m_cryptoCtx,CALG_MD5);
      BYTE       pbBuffer[1024];
      DWORD      dwBytesRead;
      
      do
      { dwBytesRead = 0;

        THROW_LASTERROREXCEPTION1( ::ReadFile(*dataFile, 
                                              pbBuffer, sizeof(pbBuffer), &dwBytesRead, NULL
                                             ) 
                                 );

        if( dwBytesRead==0 ) 
          break;

        hash.Put(pbBuffer, dwBytesRead);
      } while( dwBytesRead>=sizeof(pbBuffer) );

      auto_ptr<BYTE> hashValue;

      DWORD hashLen = hash.Get(hashValue);

      LOGGER_INFO<<_T("MD5:")<<setw(2)<<hex;

      BYTE* pBuffer = hashValue.get();

      if( NULL!=pBuffer )
        for( DWORD i=0;i<hashLen;i++ )
          LOGGER_INFO<<pBuffer[i];

      LOGGER_INFO<<endl;
    } // of MD5Sum::CalcFileHash()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
