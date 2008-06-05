/*
 * $Id$
 * 
 * Exception class for GetLastError.
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
#include "exception/memoryexception.h"

/*
 * MemoryException::MemoryException
 * MemoryException::~MemoryException
 *
 * Constructor Parameters:
 *  None
 */
bvr20983::MemoryException::MemoryException(LPCTSTR fileName,int lineNo) : BVR20983Exception(0,NULL,fileName,lineNo)
{ m_errorMessage = _T("Memory Exception");
}
/*==========================END-OF-FILE===================================*/
