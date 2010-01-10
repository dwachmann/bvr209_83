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
#define YANEW(A)          new(YAAllocatorPool::GetAllocator(_T(#A)),_T(__FILE__),__LINE__)A

#define YAVPTR(Ty,x)           YAPtr<Ty> x(YAAllocatorPool::GetAllocator(_T(#Ty)),new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty ()   )
#define YAVPTR1(Ty,x,p1)       YAPtr<Ty> x(YAAllocatorPool::GetAllocator(_T(#Ty)),new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1) )
#define YAVPTR2(Ty,x,p1,p2)    YAPtr<Ty> x(YAAllocatorPool::GetAllocator(_T(#Ty)),new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1,p2) )
#define YAVPTR3(Ty,x,p1,p2,p3) YAPtr<Ty> x(YAAllocatorPool::GetAllocator(_T(#Ty)),new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1,p2,p3) )

#define YAPTR(Ty)              YAPtr<Ty>  (YAAllocatorPool::GetAllocator(_T(#Ty)),new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty ()   )
#define YAPTR1(Ty,p1)          YAPtr<Ty>  (YAAllocatorPool::GetAllocator(_T(#Ty)),new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1) )
#define YAPTR2(Ty,p1,p2)       YAPtr<Ty>  (YAAllocatorPool::GetAllocator(_T(#Ty)),new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1,p2) )
#define YAPTR3(Ty,p1,p2,p3)    YAPtr<Ty>  (YAAllocatorPool::GetAllocator(_T(#Ty)),new(YAAllocatorPool::GetAllocator(_T(#Ty)),_T(__FILE__),__LINE__)Ty (p1,p2,p3) )

namespace bvr20983
{
  namespace util
  {
    struct YAAllocatorBase
    { virtual void*                 Allocate(size_t sizeInBytes,LPCTSTR filename, unsigned short lineno)=0;
      virtual void                  Free(void* p                   ) =0;
      virtual void                  Free(unsigned int slot         ) =0;
      virtual unsigned int          GetSlot(void* p                ) =0;
      virtual void*                 GetPtr(unsigned int slot       ) =0;
      virtual void*                 GetData(unsigned int slot      ) =0;
      virtual unsigned int          GetDataSlot(void* p            ) =0;
    }; // of struct YAAllocatorBase

    struct YAAllocatorResult
    { YAAllocatorBase*    m_pAllocator;
      LPCTSTR             m_filename;
      unsigned short      m_lineno;
      unsigned short      m_allocated : 1;

      YAAllocatorResult() : m_pAllocator(NULL),m_allocated(0),m_filename(NULL),m_lineno(0)
      { }
    }; // of struct YAAllocatorResult

    template <class X>
    class YAAllocator : public YAAllocatorBase
    {
      public:
        YAAllocator(unsigned int startSize=0,unsigned int grow=10) throw() : m_poolAdmin(NULL),m_poolData(NULL),m_size(0),m_grow(grow)
        { ReallocatePool(m_size,startSize);
        } // of YAAllocator()

        ~YAAllocator() throw() 
        { if( m_size>0 && NULL!=m_poolAdmin && NULL!=m_poolData )
          { for( unsigned int i=0;i<m_size;i++ )
              if( m_poolAdmin[i].m_allocated==0 )
                (m_poolData+i)->~X();

            ::free(m_poolAdmin);
            ::free(m_poolData);
          } // of if

          m_poolAdmin = NULL;
          m_poolData  = NULL;
          m_size      = 0;
        } // of ~YAAllocator()

        void* Allocate(size_t sizeInBytes,LPCTSTR filename, unsigned short lineno)
        { assert( sizeInBytes==sizeof(X) );

          unsigned int slotIndex = FindFreeSlot();

          if( slotIndex==UINT_MAX )
          { slotIndex  = m_size;

            ReallocatePool(m_size,m_grow);
          } // of if

          if( slotIndex>=m_size )
            throw std::bad_alloc();

          YAAllocatorResult* result = m_poolAdmin+slotIndex;

          assert( result->m_allocated==0 );

          result->m_pAllocator = this;
          result->m_allocated  = 1;
          result->m_filename   = filename;
          result->m_lineno     = lineno;

          OutputDebugFmt(_T("%s[%d] YAAllocator::Allocate(size=%d): 0x%lx slot=%d\n"),filename,lineno,sizeInBytes,result,slotIndex);

          return result;
        } // of YAAllocator::Allocate()

        void Free(void* p)
        { if( NULL!=p )
          { YAAllocatorResult* r = reinterpret_cast<YAAllocatorResult*>(p);

            unsigned int slot = GetSlot(p);
            
            OutputDebugFmt(_T("%s[%d] YAAllocator.Free(p=0x%lx): slot=%d\n"),r->m_filename,r->m_lineno,p,slot);

            Free(slot);
          } // of if
        } // of YAAllocator::Free()

        void Free(unsigned int slot)
        { if( NULL!=m_poolAdmin )
          { YAAllocatorResult* p0 = m_poolAdmin+slot;

            if( slot>=m_size || p0->m_allocated==0 )
              throw "unknown slot";
            
            OutputDebugFmt(_T("%s[%d] YAAllocator.Free(slot=%d): 0x%lx\n"),p0->m_filename,p0->m_lineno,slot,p0);

            p0->m_allocated = 0;
          } // of if
        } // of YAAllocator::Free()

        unsigned int GetSlot(void* p)
        { unsigned int result = UINT_MAX;

          if( NULL!=m_poolAdmin )
          { for( unsigned i=0;i<m_size;i++ )
            { if( p==m_poolAdmin+i )
              { result = i;
                break;
              } // of if
            } // of if

            if( result==UINT_MAX )
              throw "unknown pointer";
          } // of if

          return result;
        } // of GetSlot()

        void* GetPtr(unsigned int slot)
        { YAAllocatorResult* p0 = NULL;
        
          if( NULL!=m_poolAdmin )
          { p0 = m_poolAdmin+slot;

            if( slot>=m_size || p0->m_allocated==0 )
              throw "unknown slot";
          } // of if

          return p0;
        } // of GetPtr()

        void* GetData(unsigned int slot)
        { X* p0 = NULL;
        
          if( NULL!=m_poolData && NULL!=m_poolAdmin )
          { p0 = m_poolData+slot;

            if( slot>=m_size || m_poolAdmin[slot].m_allocated==0 )
              throw "unknown slot";
          } // of if

          return p0;
        } // of GetData()

        unsigned int GetDataSlot(void* p)
        { unsigned int result = UINT_MAX;

          if( NULL!=m_poolData && NULL!=m_poolAdmin )
          { for( unsigned i=0;i<m_size;i++ )
            { if( p==m_poolData+i )
              { result = i;
                break;
              } // of if
            } // of if

            if( result==UINT_MAX || m_poolAdmin[result].m_allocated==0 || result>=m_size )
              throw "unknown pointer";
          } // of if

          return result;
        } // of GetDataSlot()

      private:
        YAAllocatorResult*    m_poolAdmin;
        X*                    m_poolData;
        unsigned int          m_size;
        unsigned int          m_grow;

        void ReallocatePool(unsigned int oldSize,unsigned int grow)
        { unsigned int newSize = oldSize + grow;

          if( newSize>0 )
          { if( m_poolAdmin==NULL )
            { m_poolAdmin = (YAAllocatorResult*)::calloc(newSize,sizeof(YAAllocatorResult));
              m_poolData  = (X*)                ::calloc(newSize,sizeof(X));
            }
            else
            { m_poolAdmin = (YAAllocatorResult*)::realloc(m_poolAdmin,newSize*sizeof(YAAllocatorResult));
              m_poolData  = (X*)                ::realloc(m_poolData ,newSize*sizeof(X));

              ::memset(m_poolAdmin + oldSize,'\0',grow*sizeof(YAAllocatorResult));
              ::memset(m_poolData  + oldSize,'\0',grow*sizeof(X));
            } // of else

            for( unsigned int i=0;i<grow;i++ )
              new(m_poolAdmin + oldSize + i)YAAllocatorResult();
          } // of if

          m_size = newSize;
        } // of AllocatePool()

        unsigned int FindFreeSlot()
        { unsigned int result = UINT_MAX;
          
          if( NULL!=m_poolAdmin )
            for( unsigned int i=0;i<m_size;i++ )
              if( m_poolAdmin[i].m_allocated==0 )
              { result = i;

                break;
              } // of if

          return result;
        } // of FindFreeSlot()
    }; // of class YAAllocator

    template <class X>
    class YAPtr
    {
      public:
        YAPtr() throw() : m_pAllocator(NULL),m_slot(UINT_MAX),m_prev(NULL),m_next(NULL)
        { m_prev = m_next = this; }

        explicit YAPtr(YAAllocatorBase* pAllocator,X* r) throw() : m_pAllocator(pAllocator),m_slot(UINT_MAX),m_prev(NULL),m_next(NULL)
        { assert( NULL!=r );
          assert( NULL!=m_pAllocator );

          m_slot = m_pAllocator->GetDataSlot(r);
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
        { return IsNull() ? NULL : reinterpret_cast<X*>(m_pAllocator->GetData(m_slot)); }

        bool      IsNull()   const throw()  
        { return m_pAllocator==NULL; }

        bool      IsUnique()   const throw()  
        { return m_prev!=NULL ? m_prev==this : true; }

        YAPtr<X>     Clone(LPCTSTR filename, int lineno) const
        { YAPtr<X> result;

          if( NULL!=m_pAllocator )
          { X* pData = Get();

            result = YAPtr<X>(m_pAllocator,new(m_pAllocator,filename,lineno)X(*pData));
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
    { if( ptr.IsNull() )
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
