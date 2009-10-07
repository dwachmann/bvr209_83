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

#define YAPTR(X) YAPtr<YAAllocator<X>,X>
#define YANEW(A) &A,new(A,_T(__FILE__),__LINE__)

namespace bvr20983
{
  namespace util
  {
    template <class X>
    class YAAllocator 
    {
      public:
        YAAllocator() throw() : m_classSize(sizeof(X))
        { }

        ~YAAllocator() throw() 
        { }

        void* Allocate(size_t sizeInBytes,LPCTSTR filename, int lineno)
        { assert( m_classSize==sizeInBytes );
          
          void* result = ::calloc(m_classSize,1);

          if( NULL==result )
            throw std::bad_alloc();

          OutputDebugFmt(_T("%s[%d] YAAllocator::Allocate(size=%d):0x%lx\n"),filename,lineno,sizeInBytes,result);

          return result;
        } // of YAAllocator::Allocate()

        void Free(void* p) throw() 
        { OutputDebugFmt(_T("YAAllocator.Free(p=0x%lx)\n"),p);
          
          ::free(p);  
        }

      private:
        unsigned int m_classSize;
    }; // of class YAAllocator

    template <class Allocator,class X>
    class YAPtr
    {
      public:
        explicit YAPtr(Allocator* pAlloc=NULL,X* p=NULL) throw() : m_pAllocator(pAlloc),m_ptr(p) 
        { m_prev = m_next = this; }

        YAPtr(const YAPtr<Allocator,X>& r) throw() : m_pAllocator(NULL),m_ptr(NULL) 
        { AddRef(r); }

        ~YAPtr() throw() 
        { Release(); }
        
        YAPtr<Allocator,X>& operator=(const YAPtr<Allocator,X>& r) throw() 
        { if( this!=&r ) 
          { Release();
            AddRef(r);
          } // of if

          return *this;
        }

        X&        operator*()  const throw()  
        { return *m_ptr; }

        X*        operator->() const throw()  
        { return m_ptr; }
        
        X*        Get()        const throw()  
        { return m_ptr; }
        
        bool      IsUnique()   const throw()  
        { return m_prev!=NULL ? m_prev==this : true; }

        YAPtr<Allocator,X>     Clone(LPCTSTR filename, int lineno) const
        { YAPtr<Allocator,X> result;

          if( NULL!=m_pAllocator && NULL!=m_ptr )
            result = YAPtr<Allocator,X>(m_pAllocator,new(*m_pAllocator,filename,lineno)X(*m_ptr));

          return result;
        } // of Clone()

      private:
        Allocator*                        m_pAllocator;
        X*                                m_ptr;
        mutable const YAPtr<Allocator,X>* m_prev;
        mutable const YAPtr<Allocator,X>* m_next;
    
        void AddRef(const YAPtr<Allocator,X>& r) throw() 
        { m_ptr          = r.m_ptr;
          m_next         = r.m_next;
          m_next->m_prev = this;
          m_prev         = &r;
          r.m_next       = this;
          m_pAllocator   = r.m_pAllocator;
        } // of AddRef()

        void Release() throw() 
        { if( IsUnique() ) 
          { if( NULL!=m_pAllocator )
              m_pAllocator->Free(m_ptr);
          } // of if
          else 
          { m_prev->m_next = m_next;
            m_next->m_prev = m_prev;
            m_prev         = m_next = NULL;
          } // of else
          
          m_ptr        = NULL;
          m_pAllocator = NULL;
        } // of Release()
    }; // of class YAPtr

    template<class charT, class Traits,class Allocator,class X>
    std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const YAPtr<Allocator,X>& ptr)
    { os<< *ptr; 

      return os;
    }
  } // of namespace util
} // of namespace bvr20983

/**
 *
 */
template <class X>
void* operator new(size_t bytes,X& a,LPCTSTR filename, int lineno)
{ void* result = a.Allocate(bytes,filename,lineno); 

  //OutputDebugFmt(_T("%s[%d] new(%d): 0x%lx\n"),filename,lineno,bytes,result);

  return result;
}

/**
 * is only called, if exception is thrown in constructor
 */
template <class X>
void operator delete(void* p,X& a,LPCTSTR filename, int lineno)
{ OutputDebugFmt(_T("%s[%d] delete(0x%lx)\n"),filename,lineno,p);

  a.Free(p);
} // of operator delete()
#endif // YANEW_H
/*==========================END-OF-FILE===================================*/
