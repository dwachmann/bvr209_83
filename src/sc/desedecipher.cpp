/*
 * $Id$
 * 
 * Implementation of triple DES class.
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
#include "sc/desedecipher.h"
#include "util/logstream.h"

namespace bvr20983
{

  /**
   * standard constructor.
   */
  DESedeCipher::DESedeCipher()
  { m_workingKey1 = NULL;
    m_workingKey2 = NULL;
    m_workingKey3 = NULL;
  }

  /**
   * standard constructor.
   */
  DESedeCipher::~DESedeCipher()
  { delete m_workingKey1;
    delete m_workingKey2;
    delete m_workingKey3;
  }

  /**
   *
   */
  void DESedeCipher::Init(ByteBuffer key,bool encrypting)
  { LOGGER_DEBUG<<_T("DESedeCipher::Init(encrypting=")<<encrypting<<_T(") key:")<<key<<endl;
  
    m_encrypting = encrypting;

    BYTE k[8];
  
    memset(k,'\0',8);
        
    if( key.size()==16 )
    { for( int i=0;i<8;i++ )
        k[i] = key[0+i];
    
      m_workingKey1 = GenerateKey(ByteBuffer(k,8),encrypting);

      for( int i=0;i<8;i++ )
        k[i] = key[8+i];

      m_workingKey2 = GenerateKey(ByteBuffer(k,8),!encrypting);

      m_workingKey3 = new DWORD[32];
  
      for( int i=0;i<32;i++ )
        m_workingKey3[i] = m_workingKey1[i];
    } 
    else if( key.size()==24 )
    { for( int i=0;i<8;i++ )
        k[i] = key[0+i];
    
      m_workingKey1 = GenerateKey(ByteBuffer(k,8),encrypting);

      for( int i=0;i<8;i++ )
        k[i] = key[8+i];

      m_workingKey2 = GenerateKey(ByteBuffer(k,8),!encrypting);

      for( int i=0;i<8;i++ )
        k[i] = key[16+i];

      m_workingKey3 = GenerateKey(ByteBuffer(k,8),encrypting);
    } 
    else
      throw _T("invalid key size");
  }


  /**
   *
   */
  DWORD DESedeCipher::Process(const ByteBuffer& origIn,ByteBuffer& out,CipherMode mode)
  { if( m_workingKey1==NULL || m_workingKey2==NULL || m_workingKey3==NULL )
      throw _T("DESede Cipher not initialised");
      
    DWORD paddingBytes = BLOCK_SIZE - (origIn.size()%BLOCK_SIZE);
    
    ByteBuffer in = origIn;

    LOGGER_DEBUG<<_T("DESedeCipher::Process(): in.size()=")<<in.size()<<_T(",padding=")<<(paddingBytes%BLOCK_SIZE ? paddingBytes : 0)<<endl<<in<<endl;
    
    if( paddingBytes%BLOCK_SIZE )
    { for( DWORD k=0;k<paddingBytes;k++ )
        in.push_back( (BYTE)0x00 );
    } // of if
    
    DWORD i=0;
    
    for( ;i<in.size();i+=BLOCK_SIZE )
    { if( mode==CBC )
      { 
        LOGGER_DEBUG<<_T("DESedeCipher::Process(): m_icv():")<<m_icv<<endl;
        
        if( m_encrypting )
          for( DWORD k=0;k<8;k++ )
            in[i+k] ^= m_icv[k];
      } // of if

      BYTE outBuffer[8];

      if( m_encrypting )
      { Cipher(m_workingKey1,        in, i, outBuffer);
        Cipher(m_workingKey2, outBuffer, 0, outBuffer);
        Cipher(m_workingKey3, outBuffer, 0, outBuffer);
      }
      else
      { Cipher(m_workingKey3,        in, i, outBuffer);
        Cipher(m_workingKey2, outBuffer, 0, outBuffer);
        Cipher(m_workingKey1, outBuffer, 0, outBuffer);
      }

      if( mode==CBC )
      { if( m_encrypting )
        { for( BYTE k=0;k<8;k++ )
            m_icv[k] = outBuffer[k];
        } // of if
        else
        { for( BYTE k=0;k<8;k++ )
            outBuffer[k] ^= m_icv[k];
            
          for( BYTE k=0;k<8;k++ )
            m_icv[k] = in[i+k];
        } // of else
      } // of if

      out.append(outBuffer,8);
    } // of for

    if( paddingBytes%BLOCK_SIZE )
      out.resize( out.size()-paddingBytes );
    
    LOGGER_DEBUG<<_T("  result:")<<endl;
    LOGGER_DEBUG<<out<<endl;

    return i;
  }

} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
