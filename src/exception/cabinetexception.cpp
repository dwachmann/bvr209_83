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
#include "exception/cabinetexception.h"

/*
 * CabinetException::CabinetException
 *
 * Constructor Parameters:
 *  None
 */
bvr20983::cab::CabinetException::CabinetException(FCIERROR errorCode,LPCTSTR fileName,int lineNo) : BVR20983Exception(errorCode,NULL,fileName,lineNo)
{ m_errorCode = errorCode;

  switch( m_errorCode )
  {
    case FCIERR_NONE:
      m_errorMessage= _T("No error");
      break;

    case FCIERR_OPEN_SRC:
      m_errorMessage= _T("Failure opening file to be stored in cabinet");
      break;
    
    case FCIERR_READ_SRC:
      m_errorMessage= _T("Failure reading file to be stored in cabinet");
      break;
    
    case FCIERR_ALLOC_FAIL:
      m_errorMessage= _T("Insufficient memory in FCI");
      break;

    case FCIERR_TEMP_FILE:
      m_errorMessage= _T("Could not create a temporary file");
      break;

    case FCIERR_BAD_COMPR_TYPE:
      m_errorMessage= _T("Unknown compression type");
      break;

    case FCIERR_CAB_FILE:
      m_errorMessage= _T("Could not create cabinet file");
      break;

    case FCIERR_USER_ABORT:
      m_errorMessage= _T("Client requested abort");
      break;

    case FCIERR_MCI_FAIL:
      m_errorMessage= _T("Failure compressing data");
      break;

    default:
      m_errorMessage= "Unknown error";
      break;
  }
}

/*
 * CabinetException::CabinetException
 *
 * Constructor Parameters:
 *  None
 */
bvr20983::cab::CabinetException::CabinetException(FDIERROR errorCode,LPCTSTR fileName,int lineNo) : BVR20983Exception(errorCode,NULL,fileName,lineNo)
{ m_errorCode = errorCode;

  switch( m_errorCode )
  {
    case FDIERROR_NONE:
			m_errorMessage= _T("No error");
      break;
		case FDIERROR_CABINET_NOT_FOUND:
			m_errorMessage= _T("Cabinet not found");
      break;
		case FDIERROR_NOT_A_CABINET:
			m_errorMessage= _T("Not a cabinet");
      break;
		case FDIERROR_UNKNOWN_CABINET_VERSION:
			m_errorMessage= _T("Unknown cabinet version");
      break;
		case FDIERROR_CORRUPT_CABINET:
			m_errorMessage= _T("Corrupt cabinet");
      break;
		case FDIERROR_ALLOC_FAIL:
			m_errorMessage= _T("Memory allocation failed");
      break;
		case FDIERROR_BAD_COMPR_TYPE:
			m_errorMessage= _T("Unknown compression type");
      break;
		case FDIERROR_MDI_FAIL:
			m_errorMessage= _T("Failure decompressing data");
      break;
		case FDIERROR_TARGET_FILE:
			m_errorMessage= _T("Failure writing to target file");
      break;
		case FDIERROR_RESERVE_MISMATCH:
			m_errorMessage= _T("Cabinets in set have different RESERVE sizes");
      break;
		case FDIERROR_WRONG_CABINET:
			m_errorMessage= _T("Cabinet returned on fdintNEXT_CABINET is incorrect");
      break;
		case FDIERROR_USER_ABORT:
			m_errorMessage= _T("User aborted");
      break;
		default:
			m_errorMessage= _T("Unknown error");
      break;
  }
}
/*==========================END-OF-FILE===================================*/
