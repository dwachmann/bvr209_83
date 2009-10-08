/*
 * $Id:$
 * 
 * An Allocator Factory.
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
#include "util/yaallocatorpool.h"
#include "util/yastring.h"
#include "util/logstream.h"

namespace bvr20983
{
  namespace util
  {

    YAAllocatorPool* YAAllocatorPool::m_pMe = NULL;

    /**
     *
     */
    YAAllocatorPool::YAAllocatorPool()
    { }

    /**
     *
     */
    YAAllocatorPool::~YAAllocatorPool()
    { 
    } // of YAAllocatorPool::~YAAllocatorPool()

    /**
     *
     */
    YAAllocatorPool* YAAllocatorPool::GetInstance()
    { if( NULL==m_pMe )
        m_pMe = new YAAllocatorPool;

      return m_pMe;
    } // of YAAllocatorPool::GetInstance()

    /**
     *
     */
    void YAAllocatorPool::AddAllocator(YAAllocatorBase* allocator,LPCTSTR allocatorName)
    { if( NULL!=allocator )
      { YAString         rawName;
        YAAllocatorPool* pool = GetInstance();

        if( NULL==allocatorName )
        { rawName       = typeid( *allocator ).raw_name();
          allocatorName = rawName.w_str();
        } // of if

        pool->m_allocators.erase( allocatorName );
        pool->m_allocators.insert( AllocatorPair(allocatorName,linked_ptr<YAAllocatorBase>(allocator)) );
      } // of if
    } // of YAAllocatorPool::AddAllocator()

    /**
     *
     */
    YAAllocatorBase* YAAllocatorPool::GetAllocator(LPCTSTR allocatorName)
    { YAAllocatorBase* result = NULL;
  
      if( NULL!=allocatorName )
      { YAAllocatorPool*       pool   = GetInstance();
        AllocatorMap::iterator i      = pool->m_allocators.find(allocatorName);

        if( i!=pool->m_allocators.end() )
          result = i->second.get();
      } // of if

      return result;
    } // of YAAllocatorPool::GetAllocator()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
