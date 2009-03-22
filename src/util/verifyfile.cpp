/*
 * $Id$
 * 
 * A class for verify a signed file.
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
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>
#include "util/verifyfile.h"
#include "util/logstream.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace util
  {
    bool VerifyFile::Verify(LPCTSTR fileName)
    { bool               result = false;
      LONG               lStatus;
      DWORD              dwLastError;
      WINTRUST_FILE_INFO FileData;
      GUID               WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

      ::memset(&FileData, 0, sizeof(FileData));
      FileData.cbStruct       = sizeof(WINTRUST_FILE_INFO);
      FileData.pcwszFilePath  = fileName;
      FileData.hFile          = NULL;
      FileData.pgKnownSubject = NULL;

      WINTRUST_DATA WinTrustData;

      ::memset(&WinTrustData, 0, sizeof(WinTrustData));
      WinTrustData.cbStruct            = sizeof(WinTrustData);
      WinTrustData.pPolicyCallbackData = NULL;
      WinTrustData.pSIPClientData      = NULL;
      WinTrustData.dwUIChoice          = WTD_UI_NONE;
      WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE; 
      WinTrustData.dwUnionChoice       = WTD_CHOICE_FILE;
      WinTrustData.dwStateAction       = 0;
      WinTrustData.hWVTStateData       = NULL;
      WinTrustData.pwszURLReference    = NULL;
      WinTrustData.dwProvFlags         = WTD_SAFER_FLAG;
      WinTrustData.dwUIContext         = 0;
      WinTrustData.pFile               = &FileData;

      lStatus = WinVerifyTrust(NULL,&WVTPolicyGUID,&WinTrustData);

      switch( lStatus ) 
      {
      case ERROR_SUCCESS:
        result = true;
        break;
          
      case TRUST_E_NOSIGNATURE:
        // Get the reason for no signature.
        dwLastError = ::GetLastError();
        if( TRUST_E_NOSIGNATURE == dwLastError ||
            TRUST_E_SUBJECT_FORM_UNKNOWN == dwLastError ||
            TRUST_E_PROVIDER_UNKNOWN == dwLastError
          ) 
        { LOGGER_WARN<<_T("The file \"")<<fileName<<_T("\" is not signed.")<<endl; } 
        else 
        { LOGGER_WARN<<_T("An unknown error occurred trying to verify the signature of the \"")<<fileName<<_T("\" file.")<<endl; }
        break;

      case TRUST_E_EXPLICIT_DISTRUST:
        // The hash that represents the subject or the publisher 
        // is not allowed by the admin or user.
        LOGGER_WARN<<_T("The signature is present, but specifically disallowed.")<<endl;
        break;

      case TRUST_E_SUBJECT_NOT_TRUSTED:
        // The user clicked "No" when asked to install and run.
        LOGGER_WARN<<_T("The signature is present, but not trusted.")<<endl;
        break;

      case CRYPT_E_SECURITY_SETTINGS:
        /*
        The hash that represents the subject or the publisher 
        was not explicitly trusted by the admin and the 
        admin policy has disabled user trust. No signature, 
        publisher or time stamp errors.
        */
        LOGGER_WARN<<_T("CRYPT_E_SECURITY_SETTINGS - The hash ")<<
                     _T("representing the subject or the publisher wasn't ")<<
                     _T("explicitly trusted by the admin and admin policy ")<<
                     _T("has disabled user trust. No signature, publisher ")<<
                     _T("or timestamp errors.")<<endl;
        break;

      default:
        // The UI was disabled in dwUIChoice or the admin policy 
        // has disabled user trust. lStatus contains the 
        // publisher or time stamp chain error.
        LOGGER_WARN<<hex<<_T("Error is: 0x")<<lStatus<<_T(".")<<endl;
        break;
      }

      return result;
    } // of Verify()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
