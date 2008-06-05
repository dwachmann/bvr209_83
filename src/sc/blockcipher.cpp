/*
 * $Id$
 * 
 * Implementation of blockcipher.
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
#include "sc/blockcipher.h"
#include "sc/descipher.h"
#include "sc/desedecipher.h"
#include "util/logstream.h"

namespace bvr20983
{


  /**
   *
   */
  BlockCipher* BlockCipher::GetInstance(LPCTSTR cipher)
  { BlockCipher* result = NULL;
  
    if( _tcscmp(_T("DES"),cipher)==0 )
      result = new DESCipher();
    else if( _tcscmp(_T("DESede"),cipher)==0 )
      result = new DESedeCipher();
    else
      throw _T("Unsupported Cipher");
    
    return result;
  }

} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
