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
    { virtual YAAllocatorResult* Allocate(size_t sizeInBytes,LPCTSTR filename, unsigned short lineno)=0;
      virtual void               Free(YAAllocatorResult* p   ) =0;
      virtual void               Free(unsigned int slot      ) =0;
      virtual unsigned int       GetSlot(YAAllocatorResult* p) =0;
      virtual YAAllocatorResult* GetPtr(unsigned int slot    ) =0;
    }; // of struct YAAllocatorBase

    struct YAAllocatorResult
    { YAAllocatorBase* m_pAllocator;
      unsigned short   m_allocated : 1;
      LPCTSTR          m_filename;
      unsigned short   m_lineno;
      void*            m_data;

      YAAllocatorResult() : m_pAllocator(NULL),m_data(NULL),m_allocated(0),m_filename(NULL),m_lineno(0)
      { }
    }; // of struct YAAllocatorResult

    template <class X>
    class YAAllocator : public YAAllocatorBase
    {
      public:
        YAAllocator(unsigned int startSize=0,unsigned int grow=10) throw() : m_pool(NULL),m_size(startSize),m_grow(grow)
        { if( m_size>0 )
          { m_pool = (YAAllocatorResult*)::calloc(m_size,sizeof(YAAllocatorResult)+sizeof(X));

            for( unsigned int i=0;i<m_size;i++ )
              new(Ptr(i))YAAllocatorResult();
          } // of if
        } // of YAAllocator()

        ~YAAllocator() throw() 
        { if( m_size>0 )
          { for( unsigned int i=0;i<m_size;i++ )
            { if( Ptr(i)->m_allocated==0 )
              { X* p = reinterpret_cast<X*>(Ptr(i)->m_data);
         
                p->~X();
              } // of if
            } // of if

            ::free(m_pool);
          } // of if

          m_pool = NULL;
          m_size = 0;
        } // of ~YAAllocator()

        YAAllocatorResult* Allocate(size_t sizeInBytes,LPCTSTR filename, unsigned short lineno)
        { assert( sizeInBytes==sizeof(X) );

          bool         slotFound = false;
          unsigned int slotIndex = 0;

          for( ;slotIndex<m_size;slotIndex++ )
            if( Ptr(slotIndex)->m_allocated==0 )
            { slotFound = true;

              break;
            } // of if

          if( !slotFound )
          { unsigned int newSize = m_size+m_grow;
            
            if( m_pool==NULL )
              m_pool = (YAAllocatorResult*)::calloc(newSize,sizeof(YAAllocatorResult)+sizeof(X));
            else
            { m_pool = (YAAllocatorResult*)::realloc(m_pool,newSize*(sizeof(YAAllocatorResult)+sizeof(X)));

              YAAllocatorResult* p = Ptr(m_size);
              
              ::memset(Ptr(m_size),'\0',m_grow*(sizeof(YAAllocatorResult)+sizeof(X)));
            } // of else

            for( unsigned int i=0;i<m_grow;i++ )
              new(Ptr(i)+1)YAAllocatorResult();

            slotFound  = true;
            slotIndex  = m_size;
            m_size    += m_grow;
          } // of if

          if( !slotFound )
            throw std::bad_alloc();

          YAAllocatorResult* result = Ptr(slotIndex);

          assert( result->m_allocated==0 );

          result->m_pAllocator = this;
          result->m_allocated  = 1;
          result->m_filename   = filename;
          result->m_lineno     = lineno;
          result->m_data       = result + 1;

          OutputDebugFmt(_T("%s[%d] YAAllocator::Allocate(size=%d):0x%lx\n"),filename,lineno,sizeInBytes,result);

          return result;
        } // of YAAllocator::Allocate()

        void Free(YAAllocatorResult* p)
        { if( NULL!=p )
          { OutputDebugFmt(_T("%s[%d] YAAllocator.Free(p=0x%lx)\n"),p->m_filename,p->m_lineno,p);

            Free(GetSlot(p));
          } // of if
        } // of YAAllocator::Free()

        void Free(unsigned int slot)
        { YAAllocatorResult* p0 = Ptr(slot);

          if( slot>m_size || p0->m_allocated==0 )
            throw "unknown slot";
          
          OutputDebugFmt(_T("%s[%d] YAAllocator.Free(slot=%d)\n"),p0->m_filename,p0->m_lineno,slot);

          GetPtr(slot)->m_allocated = 0;
        } // of YAAllocator::Free()

        unsigned int GetSlot(YAAllocatorResult* p)
        { bool         found  = false;
          unsigned int result = 0;

          for( ;result<m_size;result++ )
          { if( p==Ptr(result) )
            { found = true;
              break;
            } // of if
          } // of if

          if( !found )
            throw "unknown pointer";

          return result;
        } // of GetSlot()

        YAAllocatorResult* GetPtr(unsigned int slot)
        { YAAllocatorResult* p0 = Ptr(slot);

          if( slot>m_size || p0->m_allocated==0 )
            throw "unknown slot";

          return p0;
        } // of GetPtr()

      private:
        YAAllocatorResult* m_pool;
        unsigned int       m_size;
        unsigned int       m_grow;


        YAAllocatorResult* Ptr(unsigned int slot)
        { return reinterpret_cast<YAAllocatorResult*>(reinterpret_cast<char*>(m_pool) + (slot*(sizeof(YAAllocatorResult)+sizeof(X)))); }
    }; // of class YAAllocator

    template <class X>
    class YAPtr
    {
      public:
        YAPtr() throw() : m_pAllocator(NULL),m_slot(UINT_MAX),m_prev(NULL),m_next(NULL)
        { m_prev = m_next = this; }

        explicit YAPtr(X* r) throw() : m_pAllocator(NULL),m_slot(UINT_MAX),m_prev(NULL),m_next(NULL)
        { assert( NULL!=r );

          YAAllocatorResult* p = reinterpret_cast<YAAllocatorResult*>(r) - 1;
          
          m_pAllocator = p->m_pAllocator;
          
          assert( NULL!=m_pAllocator );

          m_slot = m_pAllocator->GetSlot(p);
          
          m_prev = m_next = this; 
        }

        YAPtr(const YAPtr<X>& r) throw(): m_pAllocator(NULL),m_slot(UINT_MAX),m_prev(NULL),m_next(NULL)
        { m_prev = m_next = this; 
          AddRef(r);
        }

        ~YAPtr() throw() 
        { Release(); }
        
        YAPtr<X>& operator=(const YAPtr<X>& r)
        { if( this!=&r ) 
          { Release();
            AddRef(r);
          } // of if

          return *this;
        }

        X&        operator*()  const
        { return *Get(); }

        X*        operator->() const
        { return Get(); }
        
        X*        Get()        const
        { return m_pAllocator!=NULL ? static_cast<X*>(m_pAllocator->GetPtr(m_slot)->m_data) : NULL; }
        
        bool      IsUnique()   const throw()  
        { return m_prev!=NULL ? m_prev==this : true; }

        YAPtr<X>     Clone(LPCTSTR filename, int lineno) const
        { YAPtr<X> result;

          if( NULL!=m_pAllocator )
          { X* pData = Get();

            result = YAPtr<X>(new(m_pAllocator,filename,lineno)X(*pData));
          } // of if

          return result;
        } // of Clone()

      private:
        YAAllocatorBase*        m_pAllocator;
        unsigned int            m_slot;
        mutable const YAPtr<X>* m_prev;
        mutable const YAPtr<X>* m_next;
    
        void AddRef(const YAPtr<X>& r) throw() 
        { m_pAllocator   = r.m_pAllocator;
          m_slot         = r.m_slot;
          m_next         = r.m_next;
          m_next->m_prev = this;
          m_prev         = &r;
          r.m_next       = this;
        } // of AddRef()

        void Release() throw() 
        { if( IsUnique() ) 
          { if( NULL!=m_pAllocator )
              m_pAllocator->Free(m_slot);
          } // of if
          else 
          { m_prev->m_next = m_next;
            m_next->m_prev = m_prev;
            m_prev         = m_next = NULL;
          } // of else
          
          m_pAllocator = NULL;
          m_slot       = UINT_MAX;
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

void* operator new   (size_t bytes,bvr20983::util::YAAllocatorBase* a,LPCTSTR filename,unsigned short lineno);
void  operator delete(void*  p    ,bvr20983::util::YAAllocatorBase* a,LPCTSTR filename,unsigned short lineno);
#endif // YANEW_H
/*==========================END-OF-FILE===================================*/
