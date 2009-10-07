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
#if !defined(YANEW_H)
#define YANEW_H

namespace bvr20983
{
  namespace util
  {
    template <class X>
    class YAAllocator 
    {
      public:
        YAAllocator() : m_classSize(sizeof(X))
        { }

        ~YAAllocator()
        { }

        void* Allocate(size_t bytes)
        { assert( m_classSize==bytes );
          
          void* result = ::calloc(m_classSize,1);

          if( NULL==result )
            throw std::bad_alloc();

          return result;
        } // of YAAllocator::Allocate()

        void Free(void* p)
        { ::free(p);  }

      private:
        unsigned int m_classSize;
    }; // of class YAAllocator

    template <class X>
    class YAPtr
    {
      public:
        explicit YAPtr(X* p=NULL) : m_ptr(p) 
        { m_prev = m_next = this; }

        YAPtr(const YAPtr<X>& r)
        { AddRef(r); }

        ~YAPtr()
        { Release(); }
        
        YAPtr<X>& operator=(const YAPtr<X>& r)
        { if( this!=&r ) 
          { Release();
            AddRef(r);
          } // of if

          return *this;
        }

        X&        operator*()  const
        { return *m_ptr; }

        X*        operator->() const
        { return m_ptr; }
        
        X*        Get()        const
        { return m_ptr; }
        
        bool      IsUnique()   const
        { return m_prev!=NULL ? m_prev==this : true; }

      private:
        X*                      m_ptr;
        mutable const YAPtr<X>* m_prev;
        mutable const YAPtr<X>* m_next;
    
        void AddRef(const YAPtr<X>& r)
        { m_ptr          = r.m_ptr;
          m_next         = r.m_next;
          m_next->m_prev = this;
          m_prev         = &r;
          r.m_next       = this;
        }
        void Release()
        { if( IsUnique() ) 
            delete m_ptr;
          else 
          { m_prev->m_next = m_next;
            m_next->m_prev = m_prev;
            m_prev         = m_next = NULL;
          } // of else
          
          m_ptr = NULL;
        }
    }; // of class YAPtr

  } // of namespace util
} // of namespace bvr20983

/**
 *
 */
template <class X>
void* operator new(size_t bytes,X& a,LPCTSTR filename, int lineno)
{ void* result = a.Allocate(bytes);

  OutputDebugFmt(_T("%s[%d] new(%ld): 0x%lx"),filename,lineno,bytes,result);

  return result;
} // of operator new()

/**
 *
 */
template <class X>
void operator delete(void* p,X& a,LPCTSTR filename, int lineno)
{ OutputDebugFmt(_T("%s[%d] delete(0x%lx)"),filename,lineno,p);

  a.Free(p);
} // of operator delete()
#endif // YANEW_H
/*==========================END-OF-FILE===================================*/
