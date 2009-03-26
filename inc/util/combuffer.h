/*
 * $Id$
 * 
 * Copyright (C) 2008-2009 Dorothea Wachmann
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
#if !defined(COMBUFFER_H)
#define COMBUFFER_H

#include "os.h"

namespace bvr20983
{
  template <class BufferType>
  class COMBuffer 
  {
    public:
      /**
       *
       */
      COMBuffer(BufferType* pBuffer=NULL) : m_pBuffer(NULL)
      { Init(pBuffer); }

      /**
       *
       */
      COMBuffer(const COMBuffer<BufferType>& pBuffer) : m_pBuffer(NULL)
      { Init(pBuffer.m_pBuffer); }

      /**
       *
       */
      ~COMBuffer()
      { Release(); }

      /**
       *
       */
      void Release()
      { if( NULL!=m_pBuffer )
          ::CoTaskMemFree(m_pBuffer);

        m_pBuffer = NULL;
      } // of Release()

      /**
       *
       */
      COMBuffer<BufferType>& operator=(const BufferType* pBuffer)
      { Init(pBuffer);

        return *this;
      }

      /**
       *
       */
      COMBuffer<BufferType>& operator=(const COMBuffer<BufferType>& pBuffer)
      { Init(pBuffer.m_pBuffer);
  
        return *this;
      }

      /**
       *
       */
      bool IsNULL() const
      { return m_pBuffer==NULL; }

      /**
       *
       */
      operator BufferType*()
      { return GetBuffer(); }

      /**
       *
       */
      BufferType** operator&() 
      { Release();

        return &m_pBuffer; 
      }

      /**
       *
       */
      BufferType* GetBuffer() const
      { return m_pBuffer; }


    private:
      /**
       *
       */
      void Init(BufferType* pBuffer)
      { Release();

        m_pBuffer = pBuffer;
      } // of Init()

      BufferType* m_pBuffer;
  }; // of class COMBuffer
} // of namespace bvr20983
#endif // COMBUFFER_H
//=================================END-OF-FILE==============================
