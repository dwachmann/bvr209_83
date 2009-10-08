 /*
 * $Id$
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
#if !defined(YAALLOCATORPOOL_H)
#define YAALLOCATORPOOL_H

#include "util/yanew.h"
#include "util/linked_ptr.h"

namespace bvr20983
{
  namespace util
  {
    typedef std::map <LPCTSTR, linked_ptr<YAAllocatorBase>,strless> AllocatorMap;
    typedef std::pair<LPCTSTR, linked_ptr<YAAllocatorBase>>         AllocatorPair;

    class YAAllocatorPool
    {
      public:
        static void             AddAllocator(YAAllocatorBase* allocator,LPCTSTR allocatorName=NULL);
        static YAAllocatorBase* GetAllocator(LPCTSTR allocatorName);

      private: 
        YAAllocatorPool();
        ~YAAllocatorPool();

        static YAAllocatorPool* GetInstance();

        static YAAllocatorPool* m_pMe;
        AllocatorMap            m_allocators;
    }; // of class YAAllocatorPool

  } // of namespace util
} // of namespace bvr20983
#endif // YAALLOCATORPOOL_H
/*==========================END-OF-FILE===================================*/
