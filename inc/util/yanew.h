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

#define YA_NEW ::new(_T(__FILE__),__LINE__)
#define YA_DEL ::delete

namespace bvr20983
{
  namespace util
  {
    class YAAllocator 
    {
      public:
        void*  Allocate(size_t bytes);
        void   Free(void* p);
        static YAAllocator& GetInstance();

      private:
        static YAAllocator m_me;

        YAAllocator();
        ~YAAllocator();
        YAAllocator(const YAAllocator &);           
        YAAllocator& operator=(const YAAllocator &);
    };

  } // of namespace util
} // of namespace bvr20983

void* operator new  (size_t bytes,LPCTSTR filename, int lineno);
void* operator new[](size_t bytes,LPCTSTR filename, int lineno);

void  operator delete  (void* p,LPCTSTR filename, int lineno);
void  operator delete[](void* p,LPCTSTR filename, int lineno);

void  operator delete  (void* p);
void  operator delete[](void* p);

#endif // YANEW_H
/*==========================END-OF-FILE===================================*/
