/*
 * $Id$
 * 
 * Exception class for C structured exception.
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
#include "exception/seexception.h"

namespace bvr20983
{
  SEException::SEException(unsigned int u,EXCEPTION_POINTERS* pExp) : BVR20983Exception((LONG)u)
  { m_pExp = pExp;
  
    switch( pExp->ExceptionRecord->ExceptionCode )
    { case EXCEPTION_ACCESS_VIOLATION: 
        m_errorMessage=_T("The thread tried to read from or write to a virtual address for which it does not have the appropriate access."); 
        break;
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: 
        m_errorMessage=_T("The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking."); 
        break;
      case EXCEPTION_BREAKPOINT: 
        m_errorMessage=_T("A breakpoint was encountered."); 
        break;
      case EXCEPTION_DATATYPE_MISALIGNMENT: 
        m_errorMessage=_T("The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on."); 
        break;
      case EXCEPTION_FLT_DENORMAL_OPERAND: 
        m_errorMessage=_T("One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value."); 
        break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO: 
        m_errorMessage=_T("The thread tried to divide a floating-point value by a floating-point divisor of zero."); 
        break;
      case EXCEPTION_FLT_INEXACT_RESULT: 
        m_errorMessage=_T("The result of a floating-point operation cannot be represented exactly as a decimal fraction."); 
        break;
      case EXCEPTION_FLT_INVALID_OPERATION: 
        m_errorMessage=_T("This exception represents any floating-point exception not included in this list."); 
        break;
      case EXCEPTION_FLT_OVERFLOW: 
        m_errorMessage=_T("The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type."); 
        break;
      case EXCEPTION_FLT_STACK_CHECK: 
        m_errorMessage=_T("The stack overflowed or underflowed as the result of a floating-point operation."); 
        break;
      case EXCEPTION_FLT_UNDERFLOW: 
        m_errorMessage=_T("The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type."); 
        break;
      case EXCEPTION_ILLEGAL_INSTRUCTION: 
        m_errorMessage=_T("The thread tried to execute an invalid instruction."); 
        break;
      case EXCEPTION_IN_PAGE_ERROR: 
        m_errorMessage=_T("The thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network."); 
        break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO: 
        m_errorMessage=_T("The thread tried to divide an integer value by an integer divisor of zero."); 
        break;
      case EXCEPTION_INT_OVERFLOW: 
        m_errorMessage=_T("The result of an integer operation caused a carry out of the most significant bit of the result."); 
        break;
      case EXCEPTION_INVALID_DISPOSITION: 
        m_errorMessage=_T("An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception."); 
        break;
      case EXCEPTION_NONCONTINUABLE_EXCEPTION: 
        m_errorMessage=_T("The thread tried to continue execution after a noncontinuable exception occurred."); 
        break;
      case EXCEPTION_PRIV_INSTRUCTION: 
        m_errorMessage=_T("The thread tried to execute an instruction whose operation is not allowed in the current machine mode."); 
        break;
      case EXCEPTION_SINGLE_STEP: 
        m_errorMessage=_T("A trace trap or other single-instruction mechanism signaled that one instruction has been executed."); 
        break;
      case EXCEPTION_STACK_OVERFLOW: 
        m_errorMessage=_T("The thread used up its stack."); 
        break;
      default: 
        m_errorMessage=_T("Unknown seh."); 
        break;
    } 
  }

  /**
   *
   */
  void SEException::throwException(unsigned int u, EXCEPTION_POINTERS* pExp)
  {
    throw SEException(u,pExp);
  }
}

/*==========================END-OF-FILE===================================*/
