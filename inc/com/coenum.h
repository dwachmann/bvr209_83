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
#if !defined(COENUM_H)
#define COENUM_H

#include <vector>
#include "com/counknown.h"

namespace bvr20983
{
  namespace COM
  {
    template <class A1>
    struct VoidAllocFtr 
    { A1& operator() (A1& data)
      { return data; }
    };

    template <class A1>
    struct AddrefAllocFtr 
    { A1& operator() (A1& data)
      { ADDREF_INTERFACE(data); 

        return data;
      }
    };

    template <class A1>
    struct VoidAddrefFtr 
    { void operator() (A1& data)
      { }
    };

    template <class A1>
    struct AddrefFtr 
    { void operator() (A1& data)
      { ADDREF_INTERFACE(data); }
    };

    struct StringAllocFtr 
    { LPTSTR operator() (LPTSTR& str)
      { size_t sLen   = ::_tcslen(str);
        LPTSTR result = NULL;

        if( sLen>0 )
        { result = new TCHAR[sLen+1];

          _tcscpy_s(result,sLen,str);
        } // of if

        return result;
      }
    };

    struct AddrefCPAllocFtr 
    { CONNECTDATA& operator() (CONNECTDATA& data)
      { ADDREF_INTERFACE(data.pUnk); 
    
        return data;
      }
    };

    struct AddrefCPFtr 
    { void operator() (CONNECTDATA& data)
      { ADDREF_INTERFACE(data.pUnk); }
    };

    template <class A1>
    struct VoidFreeFtr
    { void operator() (A1& data)
      { }
    };

    template <class A1>
    struct ReleaseFreeFtr
    { void operator() (A1& data)
      { RELEASE_INTERFACE(data); }
    };

    struct ReleaseCPFreeFtr
    { void operator() (CONNECTDATA& data)
      { RELEASE_INTERFACE(data.pUnk); }
    };

    struct StringFreeFtr 
    { void operator() (LPTSTR& str)
      { ::delete[] str; }
    };

    template <class IEnumIF,class DataT,
              class AllocF  = VoidAllocFtr<DataT>,
              class FreeF   = VoidFreeFtr<DataT>,
              class AddrefF = VoidAddrefFtr<DataT>
             >
    class COEnum : public COUnknown, public IEnumIF 
    {
      public:
        typedef std::vector<DataT> DataV;

        DECLARE_UNKNOWN
    
        // IEnumGeneric methods
        STDMETHODIMP Next(ULONG celt, DataT* rgelt, ULONG* pceltFetched); 
        STDMETHODIMP Skip(ULONG celt); 
        STDMETHODIMP Reset(); 
        STDMETHODIMP Clone(IEnumIF** ppenum); 
    
        COEnum(REFIID riid, int cElement=0, DataT* rgElements=NULL);
        COEnum(COEnum<IEnumIF,DataT,AllocF,FreeF,AddrefF>& coEnum);
        virtual ~COEnum();

        void Add(DataT& data)
        { m_data.push_back( m_allocF(data) ); }

        void Add(ULONG celt, DataT* rgelt)
        { if( NULL!=rgelt && celt>0 )
            for( ULONG i=0;i<celt;i++ )
              m_data.push_back( m_allocF(rgelt[i]) );
        }

        void Add(DataV& data)
        { DataV::iterator i;
       
          for( i=data.begin();i!=data.end();i++ )
            m_data.push_back( m_allocF(*i) );
        }

        ULONG size() const
        { return m_data.size(); }
    
      protected:
        virtual HRESULT InternalQueryInterface(REFIID riid,PPVOID ppv);
    
        REFIID     m_riid;
        DataV      m_data;
        ULONG      m_iCurrent;

        AllocF     m_allocF;
        FreeF      m_freeF;
        AddrefF    m_addrefF;
    }; // of class COEnum

    typedef COEnum<IEnumFORMATETC,FORMATETC> COEnumFORMATETC;
    typedef COEnum<IEnumOLEVERB,OLEVERB>     COEnumOLEVERB;

    typedef COEnum<IEnumConnectionPoints,IConnectionPoint*,
                   AddrefAllocFtr<IConnectionPoint*>,
                   ReleaseFreeFtr<IConnectionPoint*>,
                   AddrefFtr<IConnectionPoint*>
                  > COEnumConnectionPoints;

    typedef COEnum<IEnumConnections,CONNECTDATA,
                   AddrefCPAllocFtr,
                   ReleaseCPFreeFtr,
                   AddrefCPFtr
                  > COEnumConnections;

    typedef COEnum<IEnumString,LPOLESTR,StringAllocFtr,StringFreeFtr> COEnumString;
  } // of namespace COM
} // of namespace bvr20983
#endif // COENUM_H
/*==========================END-OF-FILE===================================*/
