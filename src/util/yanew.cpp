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

void* operator new(size_t bytes,bvr20983::util::YAAllocatorBase* a,LPCTSTR filename,unsigned short lineno)
{ void* result = NULL;

  if( NULL!=a )
  { void* alloc = a->Allocate(bytes,filename,lineno); 

    result = a->GetData(a->GetSlot(alloc));
  } // of if

  return result;
} // of operator new()

void  operator delete(void* p,bvr20983::util::YAAllocatorBase* a,LPCTSTR filename,unsigned short lineno)
{ if( NULL!=a )
    a->Free(a->GetDataSlot(p));
} // of operator delete()
/*==========================END-OF-FILE===================================*/
