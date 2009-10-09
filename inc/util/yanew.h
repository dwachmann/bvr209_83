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

#define YACLONE(x)        x.Clone(_T(__FILE__),__LINE__)
#define YANEW1(A)         &A,new(A,_T(__FILE__),__LINE__)
#define YANEW(A)          new(YAAllocatorPool::GetAllocator(_T(#A)),_T(__FILE__),__LINE__)A

#define YAVPTR(Ty,x)      YAPtr<Ty> x(new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty ()   )
#define YAVPTR1(Ty,x,p1)  YAPtr<Ty> x(new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1) )
#define YAVPTR2(Ty,x,p1)  YAPtr<Ty> x(new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1,p2) )
#define YAVPTR3(Ty,x,p1)  YAPtr<Ty> x(new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1,p2,p3) )

#define YAPTR(Ty)         YAPtr<Ty>  (new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty ()   )
#define YAPTR1(Ty,p1)     YAPtr<Ty>  (new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1) )
#define YAPTR2(Ty,p1)     YAPtr<Ty>  (new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1,p2) )
#define YAPTR3(Ty,p1)     YAPtr<Ty>  (new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1,p2,p3) )

namespace bvr20983
{
  namespace util
  {
    struct YAAllocatorResult;

    struct YAAllocatorBase
    { virtual YAAllocatorResult* Allocate(size_t sizeInBytes,LPCTSTR filename, int lineno)=0;
      virtual void               Free(YAAllocatorResult* p) throw()=0;
    }; // of struct YAAllocatorBase

    struct YAAllocatorResult
    { YAAllocatorBase* m_pAllocator;
      LPCTSTR          m_filename;
      int              m_lineno;
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
          result->m_filename   = filename;
          result->m_lineno     = lineno;
          result->m_data       = result + 1;

          OutputDebugFmt(_T("%s[%d] YAAllocator::Allocate(size=%d):0x%lx\n"),filename,lineno,sizeInBytes,result);

          return result;
        } // of YAAllocator::Allocate()

        void Free(YAAllocatorResult* p) throw() 
        { if( NULL!=p )
          { OutputDebugFmt(_T("%s[%d] YAAllocator.Free(p=0x%lx)\n"),p->m_filename,p->m_lineno,p);

            ::free(p);  
          } // of if
        } // of YAAllocator::Free()
    }; // of class YAAllocator

    template <class X>
    class YAPtr
    {
      public:
        YAPtr() throw() : m_ptr(NULL),m_prev(NULL),m_next(NULL)
        { m_prev = m_next = this; }

        explicit YAPtr(X* r) throw() : m_ptr( reinterpret_cast<YAAllocatorResult*>(r) - 1),m_prev(NULL),m_next(NULL)
        { m_prev = m_next = this; }

        YAPtr(const YAPtr<X>& r) throw() : m_ptr(NULL),m_prev(NULL),m_next(NULL)
        { m_prev = m_next = this; 
          AddRef(r);
        }

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
        { return m_ptr!=NULL ? static_cast<X*>(m_ptr->m_data) : NULL; }
        
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
    { if( ptr.Get()==NULL )
        os<<_T("NULL");
      else
        os<< *ptr; 

      return os;
    }
  } // of namespace util
} // of namespace bvr20983

void* operator new   (size_t bytes,bvr20983::util::YAAllocatorBase* a,LPCTSTR filename, int lineno);
void  operator delete(void*  p    ,bvr20983::util::YAAllocatorBase* a,LPCTSTR filename, int lineno);
#endif // YANEW_H
/*==========================END-OF-FILE===================================*/
