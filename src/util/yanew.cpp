/*
 * $Id$
 * 
 * Yet Another Allocator Class.
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
#include "util/yanew.h"
#include "util/logstream.h"

namespace bvr20983
{
  namespace util
  {
    
    YAAllocator YAAllocator::m_me;

    /**
     *
     */
    YAAllocator::YAAllocator()
    { }

    /**
     *
     */
    YAAllocator::~YAAllocator()
    { }

    /**
     *
     */
    YAAllocator::YAAllocator(const YAAllocator &)
    { }

    /**
     *
     */
    YAAllocator& YAAllocator::operator=(const YAAllocator &)
    { return *this; }

    /**
     *
     */
    void* YAAllocator::Allocate(size_t bytes)
    { void* result = ::calloc(bytes,1);

      if( NULL==result )
        throw std::bad_alloc();

      return result;
    } // of YAAllocator::Allocate()

    /**
     *
     */
    void YAAllocator::Free(void* p)
    { ::free(p);
    } // of YAAllocator::Free()

    /**
     *
     */
    YAAllocator& YAAllocator::GetInstance()
    { return m_me; }

  } // of namespace util
} // of namespace bvr20983

/**
 *
 */
void* operator new(size_t bytes,LPCTSTR filename, int lineno)
{ void* result = bvr20983::util::YAAllocator::GetInstance().Allocate(bytes);

  OutputDebugFmt(_T("%s[%d] new(%ld): 0x%lx"),filename,lineno,bytes,result);

  return result;
}

/**
 *
 */
void* operator new[](size_t bytes,LPCTSTR filename, int lineno)
{ void* result = bvr20983::util::YAAllocator::GetInstance().Allocate(bytes);

  OutputDebugFmt(_T("%s[%d] new[](%ld): 0x%lx"),filename,lineno,bytes,result);

  return result;
}

/**
 *
 */
void operator delete(void* p,LPCTSTR filename, int lineno)
{ OutputDebugFmt(_T("%s[%d] delete[0x%lx"),filename,lineno,p);

  bvr20983::util::YAAllocator::GetInstance().Free(p);
} 

/**
 *
 */
void operator delete[](void* p,LPCTSTR filename, int lineno)
{ OutputDebugFmt(_T("%s[%d] delete[] 0x%lx"),filename,lineno,p);

  bvr20983::util::YAAllocator::GetInstance().Free(p);
} 

/**
 *
 */
void operator delete(void* p)
{ OutputDebugFmt(_T("delete 0x%lx"),p);

  bvr20983::util::YAAllocator::GetInstance().Free(p);
} 

/**
 *
 */
void operator delete[](void* p)
{ OutputDebugFmt(_T("delete[] 0x%lx"),p);

  bvr20983::util::YAAllocator::GetInstance().Free(p);
} 
/*==========================END-OF-FILE===================================*/
