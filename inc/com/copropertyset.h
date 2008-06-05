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
#include "util/guid.h"
#include "util/comptr.h"
#include "com/covariant.h"

#if !defined(COPROPERTYSET_H)
#define COPROPERTYSET_H

namespace bvr20983
{
  namespace COM
  {
    enum PropertyStatusType
    { OK,
      CHANGED,
      NEW,
      DELETED
    };

    struct COProperty
    { COPropVariant      m_property;
      PropertyStatusType m_status;

      COProperty(const PROPVARIANT& p,PropertyStatusType status=OK)
      { m_property = p;
        m_status   = status;
      }

      COProperty(const VARIANT& p,PropertyStatusType status=OK)
      { m_property = p;
        m_status   = status;
      }

      COProperty(const COPropVariant& p,PropertyStatusType status=OK)
      { m_property = p;
        m_status   = status;
      }

      COProperty(const COVariant& p,PropertyStatusType status=OK)
      { m_property = p;
        m_status   = status;
      }

      COProperty& operator=(const COVariant& value)
      { m_property = value;
        m_status   = CHANGED;

        return *this;
      }
    }; // of class COProperty

    typedef std::map<PROPID,COProperty>  PROPERTYMAP;
    typedef std::pair<PROPID,COProperty> PROPERTYPAIR;

    typedef std::map<TString,PROPID>   PROPDICTMAP;
    typedef std::pair<TString,PROPID>  PROPDICTPAIR;

    class COPropertySet
    {
      public:
        COPropertySet(const FMTID& fmtid,LPCWSTR friendlySetName=NULL);
        COPropertySet(LPCTSTR fileName,const FMTID& fmtid,LPCWSTR friendlySetName=NULL);
        virtual ~COPropertySet();

        void   AddPropertyName(PROPID propId,LPOLESTR propName);
        PROPID AddPropertyName(LPOLESTR propName);

        LPCOLESTR GetName(PROPID propId) const;
        bool      GetPropId(LPCOLESTR propName,PROPID& propId) const;

        bool Exists(PROPID propId) const;
        bool Exists(LPCOLESTR propName) const;

        bool Get(PROPID propId,PROPVARIANT& propValue) const;
        bool Get(PROPID propId,COPropVariant& propValue) const;
        bool Get(LPCOLESTR propName,PROPVARIANT& propValue) const;
        bool Get(LPCOLESTR propName,COPropVariant& propValue) const;
        bool Get(LPCOLESTR propName,bool& value) const;
        bool Get(LPCOLESTR propName,VARIANT_BOOL* value) const;
        bool Get(LPCOLESTR propName,ULONG& value) const;
        bool Get(LPCOLESTR propName,BSTR* value) const;

        void Add(PROPID propId,const COPropVariant& propValue);
        void Add(LPCOLESTR propName,bool value);
        void Add(LPCOLESTR propName,ULONG value);
        void Add(LPCOLESTR propName,LPCTSTR value);
        void Add(LPCOLESTR propName,const TString& value);
        void Add(LPCOLESTR propName,const COPropVariant& propValue);
        void Add(LPCOLESTR propName,const PROPVARIANT& propValue);
        void Add(PROPID propId,const PROPVARIANT& propValue);

        void Update(PROPID propId,const PROPVARIANT& propValue);
        void Update(PROPID propId,const COPropVariant& propValue);
        void Update(LPCOLESTR propName,const PROPVARIANT& propValue);
        void Update(LPCOLESTR propName,const COPropVariant& propValue);

        void Delete(PROPID propId);
        void Delete(LPCOLESTR propName);

        void Write(IPropertySetStorage* pPropSetStg,bool create=false);
        void Read(IPropertySetStorage* pPropSetStg);

        void Write(IPropertyBag2* pPropertyBag2,bool create=false);
        void Read(IPropertyBag2* pPropertyBag2,IErrorLog* pErrorLog);

        void Write(bool create=false);
        void Read();

        void DeleteSet(IPropertySetStorage* pPropSetStg);
        void DeleteSet();

        LPCWSTR GetFriendlySetName() const
        { return m_friendlySetName; }

        const PROPERTYMAP& GetProperties() const
        { return m_properties; }

        const FMTID& GetFMTID() const
        { return m_fmtid; }

        static void GuidToPropertyStringName(GUID* pguid, TCHAR awcname[]);
        static bool PropertySetNameToGuid(ULONG cwcname,TCHAR const awcname[],GUID *pguid);
    
      private:
        LPCWSTR     m_friendlySetName;
        TCHAR*      m_readFriendlySetName;
        FMTID       m_fmtid;
        PROPERTYMAP m_properties;
        PROPDICTMAP m_dictionary;
        PROPID      m_firstPID;

        COMPtr<IStorage>            m_pStg;
        COMPtr<IPropertySetStorage> m_pPropSetStg;
    }; // of class COPropertySet
  } // of namespace COM

  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const bvr20983::COM::COPropertySet& p);
} // of namespace bvr20983
#endif // COPROPERTYSET_H

