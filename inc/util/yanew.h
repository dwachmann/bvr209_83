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

#define YACLONE(x) x.Clone(_T(__FILE__),__LINE__)
#define YANEW1(A)  &A,new(A,_T(__FILE__),__LINE__)
#define YANEW(A)   &A,new(_T(#A),_T(__FILE__),__LINE__)A

namespace bvr20983
{
  namespace util
  {
    struct YAAllocatorResult;

    struct YAAllocatorBase
    { virtual YAAllocatorResult* Allocate(size_t sizeInBytes,LPCTSTR filename, int lineno)=0;
      virtual void               Free(void* p) throw()=0;
    }; // of struct YAAllocatorBase

    struct YAAllocatorResult
    { YAAllocatorBase* m_pAllocator;
      void*            m_data;

      YAAllocatorResult() : m_pAllocator(NULL),m_data(NULL)
      { }
    }; // of struct YAAllocatorResult

    template <class X>
    class YAAllocator : public YAAllocatorBase
    {
      public:
        YAAllocator() throw()
        { }

        ~YAAllocator() throw() 
        { }

        YAAllocatorResult* Allocate(size_t sizeInBytes,LPCTSTR filename, int lineno)
        { assert( sizeInBytes==sizeof(X) );

          YAAllocatorResult* result = (YAAllocatorResult*)::calloc(sizeof(YAAllocatorResult) + sizeInBytes,1);

          if( NULL==result )
            throw std::bad_alloc();

          result->m_pAllocator = this;
          result->m_data       = result + 1;

          OutputDebugFmt(_T("%s[%d] YAAllocator::Allocate(size=%d):0x%lx\n"),filename,lineno,sizeInBytes,result);

          return result;
        } // of YAAllocator::Allocate()

        void Free(void* p) throw() 
        { OutputDebugFmt(_T("YAAllocator.Free(p=0x%lx)\n"),p);
          
          if( NULL!=p )
          { YAAllocatorResult* result = (YAAllocatorResult*)p - 1;
            ::free(p);  
          } // of if
        } // of YAAllocator::Free()
    }; // of class YAAllocator

    template <class X>
    class YAPtr
    {
      public:
        explicit YAPtr(void* p=NULL) throw() : m_ptr( (YAAllocatorResult*)p - 1) 
        { m_prev = m_next = this; }

        YAPtr(const YAPtr<X>& r) throw() : m_ptr(NULL) 
        { AddRef(r); }

        ~YAPtr() throw() 
        { Release(); }
        
        YAPtr<X>& operator=(const YAPtr<X>& r) throw() 
        { if( this!=&r ) 
          { Release();
            AddRef(r);
          } // of if

          return *this;
        }

        X&        operator*()  const throw()  
        { return *Get(); }

        X*        operator->() const throw()  
        { return Get(); }
        
        X*        Get()        const throw()  
        { return static_cast<X*>(m_ptr->m_data); }
        
        bool      IsUnique()   const throw()  
        { return m_prev!=NULL ? m_prev==this : true; }

        YAPtr<X>     Clone(LPCTSTR filename, int lineno) const
        { YAPtr<X> result;

          if( NULL!=m_ptr )
            result = YAPtr<X>(new(m_ptr->m_pAllocator,filename,lineno)X(*Get()));

          return result;
        } // of Clone()

      private:
        YAAllocatorResult*      m_ptr;
        mutable const YAPtr<X>* m_prev;
        mutable const YAPtr<X>* m_next;
    
        void AddRef(const YAPtr<X>& r) throw() 
        { m_ptr          = r.m_ptr;
          m_next         = r.m_next;
          m_next->m_prev = this;
          m_prev         = &r;
          r.m_next       = this;
        } // of AddRef()

        void Release() throw() 
        { if( IsUnique() ) 
          { if( NULL!=m_ptr && NULL!=m_ptr->m_pAllocator )
              m_ptr->m_pAllocator->Free(m_ptr);
          } // of if
          else 
          { m_prev->m_next = m_next;
            m_next->m_prev = m_prev;
            m_prev         = m_next = NULL;
          } // of else
          
          m_ptr = NULL;
        } // of Release()
    }; // of class YAPtr

    template<class charT, class Traits,class X>
    std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const YAPtr<X>& ptr)
    { os<< *ptr; 

      return os;
    }
  } // of namespace util
} // of namespace bvr20983

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
{ OutputDebugFmt(_T("%s[%d] delete(0x%lx)\n"),filename,lineno,p);

  if( NULL!=a )
    a->Free(p);
} // of operator delete()
#endif // YANEW_H
/*==========================END-OF-FILE===================================*/
