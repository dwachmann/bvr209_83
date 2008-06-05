/*
 * $Id$
 * 
 * Exception class for SCARD API Errors.
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
#include "exception/bvr20983exception.h"
#include "exception/scardexception.h"

/*
 * SCardException::SCardException
 *
 * Constructor Parameters:
 *  None
 */
bvr20983::SCardException::SCardException(LONG errorCode,LPCTSTR fileName,int lineNo) : BVR20983Exception(errorCode,NULL,fileName,lineNo)
{ m_errorCode = errorCode;

  switch( m_errorCode )
  { case SCARD_E_BAD_SEEK: m_errorMessage=_T("An error occurred in setting the smart card file object pointer. "); break;
    case SCARD_E_CANCELLED: m_errorMessage=_T("The action was canceled by an SCardCancel request. "); break;
    case SCARD_E_CANT_DISPOSE: m_errorMessage=_T("The system could not dispose of the media in the requested manner. "); break;
    case SCARD_E_CARD_UNSUPPORTED: m_errorMessage=_T("The smart card does not meet minimal requirements for support. "); break;
    case SCARD_E_CERTIFICATE_UNAVAILABLE: m_errorMessage=_T("The requested certificate could not be obtained. "); break;
    case SCARD_E_COMM_DATA_LOST: m_errorMessage=_T("A communications error with the smart card has been detected. "); break;
    case SCARD_E_DIR_NOT_FOUND: m_errorMessage=_T("The specified directory does not exist in the smart card. "); break;
    case SCARD_E_DUPLICATE_READER: m_errorMessage=_T("The reader driver did not produce a unique reader name. "); break;
    case SCARD_E_FILE_NOT_FOUND: m_errorMessage=_T("The specified file does not exist in the smart card. "); break;
    case SCARD_E_ICC_CREATEORDER: m_errorMessage=_T("The requested order of object creation is not supported. "); break;
    case SCARD_E_ICC_INSTALLATION: m_errorMessage=_T("No primary provider can be found for the smart card. "); break;
    case SCARD_E_INSUFFICIENT_BUFFER: m_errorMessage=_T("The data buffer for returned data is too small for the returned data. "); break;
    case SCARD_E_INVALID_ATR: m_errorMessage=_T("An ATR string obtained from the registry is not a valid ATR string. "); break;
    case SCARD_E_INVALID_CHV: m_errorMessage=_T("The supplied PIN is incorrect. "); break;
    case SCARD_E_INVALID_HANDLE: m_errorMessage=_T("The supplied handle was invalid. "); break;
    case SCARD_E_INVALID_PARAMETER: m_errorMessage=_T("One or more of the supplied parameters could not be properly interpreted. "); break;
    case SCARD_E_INVALID_TARGET: m_errorMessage=_T("Registry startup information is missing or invalid. "); break;
    case SCARD_E_INVALID_VALUE: m_errorMessage=_T("One or more of the supplied parameter values could not be properly interpreted. "); break;
    case SCARD_E_NO_ACCESS: m_errorMessage=_T("Access is denied to the file. "); break;
    case SCARD_E_NO_DIR: m_errorMessage=_T("The supplied path does not represent a smart card directory. "); break;
    case SCARD_E_NO_FILE: m_errorMessage=_T("The supplied path does not represent a smart card file. "); break;
    case SCARD_E_NO_MEMORY: m_errorMessage=_T("Not enough memory available to complete this command. "); break;
    case SCARD_E_NO_READERS_AVAILABLE: m_errorMessage=_T("No smart card reader is available. "); break;
    case SCARD_E_NO_SERVICE: m_errorMessage=_T("The smart card resource manager is not running. "); break;
    case SCARD_E_NO_SMARTCARD: m_errorMessage=_T("The operation requires a smart card, but no smart card is currently in the device. "); break;
    case SCARD_E_NO_SUCH_CERTIFICATE: m_errorMessage=_T("The requested certificate does not exist. "); break;
    case SCARD_E_NOT_READY: m_errorMessage=_T("The reader or card is not ready to accept commands. "); break;
    case SCARD_E_NOT_TRANSACTED: m_errorMessage=_T("An attempt was made to end a nonexistent transaction. "); break;
    case SCARD_E_PCI_TOO_SMALL: m_errorMessage=_T("The PCI receive buffer was too small. "); break;
    case SCARD_E_PROTO_MISMATCH: m_errorMessage=_T("The requested protocols are incompatible with the protocol currently in use with the card. "); break;
    case SCARD_E_READER_UNAVAILABLE: m_errorMessage=_T("The specified reader is not currently available for use. "); break;
    case SCARD_E_READER_UNSUPPORTED: m_errorMessage=_T("The reader driver does not meet minimal requirements for support. "); break;
    case SCARD_E_SERVICE_STOPPED: m_errorMessage=_T("The smart card resource manager has shut down. "); break;
    case SCARD_E_SHARING_VIOLATION: m_errorMessage=_T("The smart card cannot be accessed because of other outstanding connections. "); break;
    case SCARD_E_SYSTEM_CANCELLED: m_errorMessage=_T("The action was canceled by the system, presumably to log off or shut down. "); break;
    case SCARD_E_TIMEOUT: m_errorMessage=_T("The user-specified time-out value has expired. "); break;
    case SCARD_E_UNEXPECTED: m_errorMessage=_T("An unexpected card error has occurred. "); break;
    case SCARD_E_UNKNOWN_CARD: m_errorMessage=_T("The specified smart card name is not recognized. "); break;
    case SCARD_E_UNKNOWN_READER: m_errorMessage=_T("The specified reader name is not recognized. "); break;
    case SCARD_E_UNKNOWN_RES_MNG: m_errorMessage=_T("An unrecognized error code was returned from a layered component. "); break;
    case SCARD_E_UNSUPPORTED_FEATURE: m_errorMessage=_T("This smart card does not support the requested feature. "); break;
    case SCARD_E_WRITE_TOO_MANY: m_errorMessage=_T("An attempt was made to write more data than would fit in the target object. "); break;
    case SCARD_F_COMM_ERROR: m_errorMessage=_T("An internal communications error has been detected. "); break;
    case SCARD_F_INTERNAL_ERROR: m_errorMessage=_T("An internal consistency check failed. "); break;
    case SCARD_F_UNKNOWN_ERROR: m_errorMessage=_T("An internal error has been detected, but the source is unknown. "); break;
    case SCARD_F_WAITED_TOO_LONG: m_errorMessage=_T("An internal consistency timer has expired. "); break;
    case SCARD_P_SHUTDOWN: m_errorMessage=_T("The operation has been aborted to allow the server application to exit. "); break;
    case SCARD_S_SUCCESS: m_errorMessage=_T("No error was encountered. "); break;
    case SCARD_W_CANCELLED_BY_USER: m_errorMessage=_T("The action was canceled by the user. "); break;
    case SCARD_W_CHV_BLOCKED: m_errorMessage=_T("The card cannot be accessed because the maximum number of PIN entry attempts has been reached. "); break;
    case SCARD_W_EOF: m_errorMessage=_T("The end of the smart card file has been reached. "); break;
    case SCARD_W_REMOVED_CARD: m_errorMessage=_T("The smart card has been removed, so further communication is not possible. "); break;
    case SCARD_W_RESET_CARD: m_errorMessage=_T("The smart card has been reset, so any shared state information is invalid. "); break;
    case SCARD_W_SECURITY_VIOLATION: m_errorMessage=_T("Access was denied because of a security violation. "); break;
    case SCARD_W_UNPOWERED_CARD: m_errorMessage=_T("Power has been removed from the smart card, so that further communication is not possible. "); break;
    case SCARD_W_UNRESPONSIVE_CARD: m_errorMessage=_T("The smart card is not responding to a reset. "); break;
    case SCARD_W_UNSUPPORTED_CARD: m_errorMessage=_T("The reader cannot communicate with the card, due to ATR string configuration conflicts. "); break;
    case SCARD_W_WRONG_CHV: m_errorMessage=_T("The card cannot be accessed because the wrong PIN was presented. "); break;
    default: m_errorMessage=_T("unknown"); break;
  } // of switch
}
/*==========================END-OF-FILE===================================*/
