/*
 * $Id$
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
#if !defined(COLLECTION_H)
#define COLLECTION_H

#include "sc/sctypes.h"

// number of elements that are added per reallod
#define COLLECTION_REDIM 10

namespace bvr20983
{
  namespace COM
  {
    class Collection
    {
      public:
        Collection(ULONG initialSize=COLLECTION_REDIM,ULONG redim=COLLECTION_REDIM);
        ~Collection();
    
       bool    Add(IDispatch*);
       bool    Add(LPCTSTR);
    
       bool    Remove(IDispatch*);
       HRESULT Enumeration(IUnknown**);
    
       bool    Get(LONG index,LPVARIANTARG pVar);
    
       ULONG   Size()
       { return m_size; }
    
      private:
        void Alloc(ULONG newSize);
        LONG GetNextFreeIndex();
    
        ULONG      m_size;
        LONG       m_index;
        LONG       m_redim;
        SAFEARRAY* m_pSA;
    };
  } // of namespace COM
} // of namespace bvr20983

#endif // COLLECTION_H
