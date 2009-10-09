 /*
 * $Id$
 * 
 * new and delete for YAAllocator.
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
#include "util/logstream.h"
#include "util/yanew.h"

/**
 *
 */
void* operator new(size_t bytes,bvr20983::util::YAAllocatorBase* a,LPCTSTR filename, int lineno)
{ bvr20983::util::YAAllocatorResult* result;

  if( NULL!=a )
    result = a->Allocate(bytes,filename,lineno); 

  //OutputDebugFmt(_T("%s[%d] new(%d): 0x%lx\n"),filename,lineno,bytes,result);

  return result->m_data;
} // of operator new()

/**
 * is only called, if exception is thrown in constructor
 */
void operator delete(void* p,bvr20983::util::YAAllocatorBase* a,LPCTSTR filename, int lineno)
{ //OutputDebugFmt(_T("%s[%d] delete(0x%lx)\n"),filename,lineno,p);

  if( NULL!=a )
    a->Free(reinterpret_cast<bvr20983::util::YAAllocatorResult*>(p)-1);
} // of operator delete()
/*==========================END-OF-FILE===================================*/
