/*
 * $Id$
 *
 * linked_ptr - simple reference linked pointer
 * (like reference counting, just using a linked list of the references
 * instead of their count.)
 *
 * The implementation stores three pointers for every linked_ptr, but
 * does not allocate anything on the free store.
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
#ifndef LINKED_PTR_H
#define LINKED_PTR_H

/* For ANSI-challenged compilers, you may want to #define
 * NO_MEMBER_TEMPLATES, explicit or mutable */

namespace bvr20983
{
  template <class X> 
  class linked_ptr
  {
    public:
      typedef X element_type;
  
      explicit linked_ptr(X* p = 0) throw() : itsPtr(p) 
      { itsPrev = itsNext = this; }
  
      ~linked_ptr()
      { release(); }
      
      linked_ptr(const linked_ptr<X>& r) throw()
      { acquire(r); }
      
      linked_ptr<X>& operator=(const linked_ptr<X>& r)
      { if (this != &r) 
        { release();
          acquire(r);
        }
  
        return *this;
      }
  
      X& operator*()  const throw()   
      { return *itsPtr; }
  
      X* operator->() const throw()   
      { return itsPtr; }
      
      X* get()        const throw()   
      { return itsPtr; }
  
      bool unique()   const throw()   
      { return itsPrev ? itsPrev==this : true; }
  
    private:
      X*                             itsPtr;
      mutable const linked_ptr<X>*   itsPrev;
      mutable const linked_ptr<X>*   itsNext;
  
      void acquire(const linked_ptr<X>& r) throw()
      { // insert this to the list
        itsPtr = r.itsPtr;
        itsNext = r.itsNext;
        itsNext->itsPrev = this;
        itsPrev = &r;
        r.itsNext = this;
      }
  
      void release()
      { // erase this from the list, delete if unique
        if( unique() ) 
          delete itsPtr;
        else 
        { itsPrev->itsNext = itsNext;
          itsNext->itsPrev = itsPrev;
          itsPrev = itsNext = 0;
        }
        
        itsPtr = 0;
      }
  };
} // of namespace bvr20983

#endif // LINKED_PTR_H
