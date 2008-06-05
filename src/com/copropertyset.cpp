/*
 * $Id$
 * 
 * COM PropertySet Helper.
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
#include "os.h"
#include "com/copropertyset.h"
#include "util/logstream.h"
#include "util/comlogstream.h"

#define CBIT_BYTE        8
#define CBIT_CHARMASK    5
#define CBIT_GUID        (sizeof(GUID)*8)
#define CCH_MAP          (1 << CBIT_CHARMASK)    // 32
#define CHARMASK         (CCH_MAP - 1)           // 0x1f
 
static TCHAR gAwcMap[CCH_MAP + 1] = _T("abcdefghijklmnopqrstuvwxyz012345");

using namespace bvr20983;

namespace bvr20983
{
  namespace COM
  {
    /**
     *
     */
    COPropertySet::COPropertySet(const FMTID& fmtid,LPCWSTR friendlySetName)
      : m_fmtid(fmtid),
        m_friendlySetName(friendlySetName),
        m_readFriendlySetName(NULL),
        m_firstPID(PID_FIRST_USABLE+0x1000)
    { }

    /**
     *
     */
    COPropertySet::COPropertySet(LPCTSTR fileName,const FMTID& fmtid,LPCWSTR friendlySetName)
      : m_fmtid(fmtid),
        m_friendlySetName(friendlySetName),
        m_readFriendlySetName(NULL),
        m_firstPID(PID_FIRST_USABLE+0x1000)
    { THROW_COMEXCEPTION(  ::StgOpenStorageEx( fileName,
                                               STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
                                               STGFMT_ANY,
                                               0,
                                               NULL,
                                               NULL,
                                               IID_IStorage,
                                               reinterpret_cast<void**>(&m_pStg) 
                                             ) 
                        );
      
      THROW_COMEXCEPTION( m_pStg->QueryInterface( IID_IPropertySetStorage,reinterpret_cast<void**>(&m_pPropSetStg) ) );

      Read();
    } // of COPropertySet::COPropertySet()

    /**
     *
     */
    COPropertySet::~COPropertySet()
    { m_pPropSetStg.Release();
      m_pStg.Release();
      
      if( NULL!=m_readFriendlySetName )
        ::CoTaskMemFree( m_readFriendlySetName );

      m_dictionary.clear();
    } // of COPropertySet::~COPropertySet()

    /**
     *
     */
    void COPropertySet::AddPropertyName(PROPID propId,LPOLESTR propName)
    { TString               dictName = propName;
      PROPDICTMAP::iterator dictIter = m_dictionary.find(dictName);

      if( m_dictionary.empty() || dictIter==m_dictionary.end() )
        m_dictionary.insert( PROPDICTPAIR(dictName,propId) );
      else
        dictIter->second = propId;    
    } // of COPropertySet::AddPropertyName()

    /**
     *
     */
    PROPID COPropertySet::AddPropertyName(LPOLESTR propName)
    { PROPID result = 0;

      if( !GetPropId(propName,result) )
      { result = m_firstPID;

        AddPropertyName(m_firstPID++,propName); 
      } // of if

      return result;
    } // of COPropertySet::AddPropertyName()

    /**
     *
     */
    bool COPropertySet::Exists(PROPID propId) const
    { PROPERTYMAP::const_iterator i = m_properties.find(propId);

      return !m_properties.empty() && i!=m_properties.end();
    } // of COPropertySet::Exists()

    /**
     *
     */
    bool COPropertySet::Exists(LPCOLESTR propName) const
    { bool   result = false;
      PROPID propId = 0;

      if( GetPropId(propName,propId) )
        result = Exists(propId);

      return result;
    } // of COPropertySet::Exists()

    /**
     *
     */
    LPCOLESTR COPropertySet::GetName(PROPID propId) const
    { PROPDICTMAP::const_iterator i      = m_dictionary.begin();
      LPCOLESTR                   result = NULL;

      if( !m_dictionary.empty() )
        for( ;i!=m_dictionary.end();i++ )
        { if( i->second==propId )
          { result = i->first.c_str();

            break;
          } // of if
        } // of for

      return result;
    }

    /**
     *
     */
    bool COPropertySet::GetPropId(LPCOLESTR propName,PROPID& propId) const
    { PROPDICTMAP::const_iterator i      = m_dictionary.find(propName);
      bool                        result = false;

      if( !m_dictionary.empty() && i!=m_dictionary.end() )
      { propId = i->second;
        result = true;
      } // of if

      return result;
    } // of COPropertySet::GetName()


    /**
     *
     */
    bool COPropertySet::Get(PROPID propId,PROPVARIANT& propValue) const
    { PROPERTYMAP::const_iterator i      = m_properties.find(propId);
      bool                        result = false;

      if( !m_properties.empty() && i!=m_properties.end() )
      { ::PropVariantClear(&propValue);
        ::PropVariantCopy(&propValue,i->second.m_property);

        result = true;
      } // of if

      return result;
    } // of COPropertySet::Get()

    /**
     *
     */
    bool COPropertySet::Get(LPCOLESTR propName,PROPVARIANT& propValue) const
    { PROPID propId;
      bool   result = false;

      if( GetPropId(propName,propId) )
        result = Get(propId,propValue);

      return result;
    } // of COPropertySet::Get()

    /**
     *
     */
    bool COPropertySet::Get(LPCOLESTR propName,COPropVariant& propValue) const
    { const PROPVARIANT& p0 = propValue;

      return Get(propName,const_cast<PROPVARIANT&>(p0)); 
    } // of COPropertySet::Get()

    /**
     *
     */
    bool COPropertySet::Get(PROPID propId,COPropVariant& propValue) const
    { const PROPVARIANT& p0 = propValue;
      
      return Get(propId,const_cast<PROPVARIANT&>(p0)); 
    } // of COPropertySet::Get()

    /**
     *
     */
    bool COPropertySet::Get(LPCOLESTR propName,bool& value) const
    { COPropVariant propValue;

      return Get(propName,propValue) && propValue.GetBool(value);
    } // of COPropertySet::Get()

    /**
     *
     */
    bool COPropertySet::Get(LPCOLESTR propName,VARIANT_BOOL* value) const
    { bool value0;
      bool result = Get(propName,value0);

      if( result && NULL!=value )
        *value = value0 ? VARIANT_TRUE : VARIANT_FALSE;

      return result;
    } // of COPropertySet::Get()

    /**
     *
     */
    bool COPropertySet::Get(LPCOLESTR propName,ULONG& value) const
    { COPropVariant propValue;

      return Get(propName,propValue) && propValue.GetULong(value);
    } // of COPropertySet::Get()

    /**
     *
     */
    bool COPropertySet::Get(LPCOLESTR propName,BSTR* value) const
    { COPropVariant propValue;

      return Get(propName,propValue) && propValue.GetString(value);
    } // of COPropertySet::Get()


    /**
     *
     */
    void COPropertySet::Add(PROPID propId,const COPropVariant& propValue)
    { Add(propId,propValue.GetConstValue()); }


    /**
     *
     */
    void COPropertySet::Add(LPCOLESTR propName,const COPropVariant& propValue)
    { Add(propName,propValue.GetConstValue()); }


    /**
     *
     */
    void COPropertySet::Add(LPCOLESTR propName,bool value)
    { PROPVARIANT propValue;

      ::PropVariantInit(&propValue);
      propValue.vt = VT_BOOL;

      propValue.boolVal = value ? VARIANT_TRUE : VARIANT_FALSE;
      Add(propName,propValue);
    }

    /**
     *
     */
    void COPropertySet::Add(LPCOLESTR propName,ULONG value)
    { PROPVARIANT propValue;

      ::PropVariantInit(&propValue);
      propValue.vt = VT_UI4;

      propValue.ulVal = value;
      Add(propName,propValue);
    }

    /**
     *
     */
    void COPropertySet::Add(LPCOLESTR propName,LPCTSTR value)
    { PROPVARIANT propValue;

      ::PropVariantInit(&propValue);
      propValue.vt = VT_BSTR;

      propValue.bstrVal = ::SysAllocString(value);
      Add(propName,propValue);
      //::SysFreeString(propValue.bstrVal);
    }

    /**
     *
     */
    void COPropertySet::Add(LPCOLESTR propName,const TString& value)
    { PROPVARIANT propValue;

      ::PropVariantInit(&propValue);
      propValue.vt = VT_BSTR;

      propValue.bstrVal = ::SysAllocString(value.c_str());
      Add(propName,propValue);
      ::SysFreeString(propValue.bstrVal);
    }

    /**
     *
     */
    void COPropertySet::Add(LPCOLESTR propName,const PROPVARIANT& propValue)
    { PROPID propId;

      if( GetPropId(propName,propId) )
        Add(propId,propValue);
    } // of COPropertySet::Add()

    /**
     *
     */
    void COPropertySet::Add(PROPID propId,const PROPVARIANT& propValue)
    { PROPERTYMAP::iterator i = m_properties.find(propId);

      if( m_properties.empty() || i==m_properties.end() )
        m_properties.insert( PROPERTYPAIR(propId,COProperty(propValue,NEW)) );
      else
      { i->second.m_property = propValue;
        i->second.m_status   = CHANGED;
      } // of else
    } // of COPropertySet::Add()

    /**
     *
     */
    void COPropertySet::Update(PROPID propId,const PROPVARIANT& propValue)
    { PROPERTYMAP::iterator i = m_properties.find(propId);

      if( !m_properties.empty() && i!=m_properties.end() )
      { i->second.m_property = propValue;
        i->second.m_status   = CHANGED;
      } // of else
    } // of COPropertySet::Update()

    /**
     *
     */
    void COPropertySet::Update(LPCOLESTR propName,const PROPVARIANT& propValue)
    { PROPID propId;

      if( GetPropId(propName,propId) )
        Update(propId,propValue);
    } // of COPropertySet::Update()

    /**
     *
     */
    void COPropertySet::Update(LPCOLESTR propName,const COPropVariant& propValue)
    { Update(propName,propValue.GetConstValue()); }

    /**
     *
     */
    void COPropertySet::Update(PROPID propId,const COPropVariant& propValue)
    { Update(propId,propValue.GetConstValue()); }

    /**
     *
     */
    void COPropertySet::Delete(LPCOLESTR propName)
    { PROPID propId;

      if( GetPropId(propName,propId) )
        Delete(propId);
    } // of COPropertySet::Delete()

    /**
     *
     */
    void COPropertySet::Delete(PROPID propId)
    { PROPERTYMAP::iterator i = m_properties.find(propId);

      if( !m_properties.empty() && i!=m_properties.end() )
        i->second.m_status = DELETED;
    } // of COPropertySet::Delete()

    /**
     *
     */
    void COPropertySet::Write(bool create)
    { Write(m_pPropSetStg,create); }

    /**
     *
     */
    void COPropertySet::Write(IPropertySetStorage* pPropSetStg,bool create)
    { if( NULL!=pPropSetStg )
      { COMPtr<IPropertyStorage> pPropStg;

        if( create )
        { THROW_COMEXCEPTION( pPropSetStg->Create( m_fmtid, NULL, PROPSETFLAG_DEFAULT, STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,&pPropStg ) ); }
        else
        { THROW_COMEXCEPTION( pPropSetStg->Open( m_fmtid, STGM_READWRITE|STGM_SHARE_EXCLUSIVE,&pPropStg ) ); }

        PROPERTYMAP::iterator iter;

        PROPSPEC propspec;

        propspec.ulKind = PRSPEC_PROPID;
        
        for( iter=m_properties.begin();iter!=m_properties.end();iter++ )
        { propspec.propid = iter->first;

          if( iter->second.m_status==CHANGED || iter->second.m_status==NEW )
          { const PROPVARIANT* p = iter->second.m_property;

            THROW_COMEXCEPTION( pPropStg->WriteMultiple( 1, &propspec, p, PID_FIRST_USABLE ) ); 
          } // of if
          else if( iter->second.m_status==DELETED )
          { THROW_COMEXCEPTION( pPropStg->DeleteMultiple( 1, &propspec) ); }
        } // of for

        PROPDICTMAP::const_iterator dictIter = m_dictionary.begin();

        if( !m_dictionary.empty() )
        { PROPID   propId      = 0;
          LPOLESTR propName[1] = {NULL};

          for( ;dictIter!=m_dictionary.end();dictIter++ )
          { propId      = dictIter->second;
            propName[0] = const_cast<LPOLESTR>(dictIter->first.c_str());

            THROW_COMEXCEPTION( pPropStg->WritePropertyNames( 1, &propId,propName) ); 
          } // of for
        } // of if

        if( NULL!=m_friendlySetName )
        { PROPID propidDictionary = PID_DICTIONARY;

          LPWSTR friendlySetName = const_cast<LPWSTR>(m_friendlySetName);

          THROW_COMEXCEPTION( pPropStg->WritePropertyNames( 1, &propidDictionary,&friendlySetName ) );
        } // of if

        THROW_COMEXCEPTION( pPropStg->Commit(STGC_DEFAULT) );

        PROPERTYMAP::iterator iter1;

        for( iter1=m_properties.begin();iter1!=m_properties.end(); )
        { switch( iter1->second.m_status )
          {
          case DELETED:
            m_properties.erase(iter1++);
            break;
          case CHANGED:
          case NEW:
            iter1->second.m_status = OK;
            // fallthrough
          default:
            iter1++;
            break;
          } // of switch
        } // of for
      } // of if
    } // of COPropertySet::Write()

    /**
     *
     */
    void COPropertySet::Read()
    { Read(m_pPropSetStg); }

    /**
     *
     */
    void COPropertySet::Read(IPropertySetStorage* pPropSetStg)
    { if( NULL!=pPropSetStg )
      { COMPtr<IPropertyStorage> pPropStg;
        COMPtr<IEnumSTATPROPSTG> pEnum;
        HRESULT                  hr = S_OK;
        STATPROPSTG              statpropstg;
        PROPSPEC                 propSpec;

        THROW_COMEXCEPTION( pPropSetStg->Open( m_fmtid, STGM_READ|STGM_SHARE_EXCLUSIVE,&pPropStg ) );
        THROW_COMEXCEPTION( pPropStg->Enum( &pEnum ) );

        statpropstg.lpwstrName = NULL;

        hr = pEnum->Next( 1, &statpropstg, NULL );
        THROW_COMEXCEPTION( hr );

        for( ;S_OK==hr; )
        { COPropVariant propValue;

          propSpec.ulKind = PRSPEC_PROPID;
          propSpec.propid = statpropstg.propid;

          const PROPVARIANT* p = propValue;

          THROW_COMEXCEPTION( pPropStg->ReadMultiple( 1, &propSpec, const_cast<PROPVARIANT*>(p) ) );

          PROPERTYMAP::iterator propIter = m_properties.find(propSpec.propid);

          if( m_properties.empty() || propIter==m_properties.end() )
            m_properties.insert( PROPERTYPAIR(propSpec.propid,propValue) );
          else
            propIter->second = propValue;

          if( NULL!=statpropstg.lpwstrName )
            AddPropertyName(propSpec.propid,statpropstg.lpwstrName);

          statpropstg.lpwstrName = NULL;

          hr = pEnum->Next( 1, &statpropstg, NULL );
          THROW_COMEXCEPTION( hr );
        } // of for

        PROPID propid = PID_DICTIONARY;

        if( NULL!=m_readFriendlySetName )
          ::CoTaskMemFree( m_readFriendlySetName );

        TCHAR* pwszFriendlyName = NULL;
        
        m_readFriendlySetName = NULL;

        if( SUCCEEDED(pPropStg->ReadPropertyNames( 1, &propid, &m_readFriendlySetName)) )
          m_friendlySetName = m_readFriendlySetName;
      } // of if
    } // of COPropertySet::Read()

    /**
     *
     */
    void COPropertySet::Write(IPropertyBag2* pPropertyBag2,bool create)
    { 
    } // of COPropertySet::Write()

    /**
     *
     */
    void COPropertySet::Read(IPropertyBag2* pPropertyBag2,IErrorLog* pErrorLog)
    { HRESULT hr          = S_OK;
      ULONG   cProperties = 0;

      if( NULL!=pPropertyBag2 )
      { hr = pPropertyBag2->CountProperties(&cProperties);

        if( SUCCEEDED(hr) )
        { PROPBAG2    p;
          HRESULT     valueHR=NOERROR;
          ULONG       cProp=0;

          for( ULONG i=0;i<cProperties;i++ )
          { ::memset(&p,'\0',sizeof(p));

            HRESULT hr1=pPropertyBag2->GetPropertyInfo(i,1,&p,&cProp);

            if( SUCCEEDED(hr1) )
            { COVariant value;

              PROPID         propId = AddPropertyName(p.pstrName);
              const VARIANT* v      = value;

              hr = pPropertyBag2->Read(1,&p,pErrorLog,const_cast<VARIANT*>(v),&valueHR);

              if( SUCCEEDED(hr) && SUCCEEDED(valueHR) )
              { PROPERTYMAP::iterator propIter = m_properties.find(propId);

                if( m_properties.empty() || propIter==m_properties.end() )
                  m_properties.insert( PROPERTYPAIR(propId,value) );
                else
                  propIter->second = value;
              } // of if

              ::CoTaskMemFree(p.pstrName);
            } // of if
          } // of for
        } // of if
      } // of for
    } // of COPropertySet::Read()


    /**
     *
     */
    void COPropertySet::DeleteSet()
    { DeleteSet(m_pPropSetStg);
    } // of COPropertySet::DeleteSet()

    /**
     *
     */
    void COPropertySet::DeleteSet(IPropertySetStorage* pPropSetStg)
    { if( NULL!=pPropSetStg )
        THROW_COMEXCEPTION( pPropSetStg->Delete(m_fmtid) );

      m_properties.clear();
      m_dictionary.clear();
    } // of COPropertySet::DeleteSet()

     
    /**
     *
     */
    void COPropertySet::GuidToPropertyStringName(GUID *pguid, TCHAR awcname[]) 
    { BYTE *pb         = (BYTE *) pguid;
      BYTE *pbEnd      = pb + sizeof(*pguid);
      ULONG cbitRemain = CBIT_BYTE;
      TCHAR *pwc       = awcname;
   
      *pwc++ = _T('\x05');
      while( pb<pbEnd ) 
      {
        ULONG i = *pb >> (CBIT_BYTE - cbitRemain);
        
        if (cbitRemain >= CBIT_CHARMASK) 
        {
          *pwc = gAwcMap[i & CHARMASK];

          if (cbitRemain == CBIT_BYTE && *pwc >= _T('a') && *pwc <= _T('z'))
            *pwc += (_T('A') - _T('a'));

          pwc++;
          cbitRemain -= CBIT_CHARMASK;
          
          if (cbitRemain == 0) 
          { pb++;
            cbitRemain = CBIT_BYTE;
          }
        }
        else 
        {
          if (++pb < pbEnd) 
            i |= *pb << cbitRemain;

          *pwc++ = gAwcMap[i & CHARMASK];
          cbitRemain += CBIT_BYTE - CBIT_CHARMASK;
        }
      } // of while
      
      *pwc = _T('\0');
    } // of COPropertySet::GuidToPropertyStringName()

    /**
     *
     */
    bool COPropertySet::PropertySetNameToGuid(ULONG cwcname,LPCTSTR awcname,GUID *pguid)
    { bool    result = false;
      LPCTSTR pwc    = awcname;
   
      if( pwc[0]==_T('\x05') )
      {
        //Note: cwcname includes the WC_PROPSET0, and
        //sizeof(wsz...) includes the trailing L'\0', but
        //the comparison excludes both the leading
        //WC_PROPSET0 and the trailing L'\0'.
 
        if( _tcsnicmp(&pwc[1], _T("SummaryInformation"), cwcname - 1) == 0)
        { *pguid = FMTID_SummaryInformation;

          return true;
        }
 
        if( cwcname==27 )
        { ULONG cbit;
          BYTE *pb = (BYTE *) pguid - 1;

          ::ZeroMemory(pguid, sizeof(*pguid));

          for( cbit = 0; cbit < CBIT_GUID; cbit += CBIT_CHARMASK )
          { ULONG cbitUsed = cbit % CBIT_BYTE;
            ULONG cbitStored;
            WCHAR wc;

            if (cbitUsed == 0)
              pb++;

            wc = *++pwc - _T('A');        //assume uppercase

            if( wc>(_T('Z')-_T('A')+1) )
            { wc += (_T('A') - _T('a')); //try lowercase

              if (wc > (_T('Z')-_T('A')+1) )
              { wc += _T('a') - _T('0') + (_T('Z')-_T('A')+1); //must be a digit

                if( wc > CHARMASK )
                  break;       //invalid character
              } // of if
            } // of if

            *pb |= (BYTE) (wc << cbitUsed);
            cbitStored = min(CBIT_BYTE - cbitUsed,CBIT_CHARMASK);

            //If the translated bits will not fit in the current byte

            if( cbitStored<CBIT_CHARMASK )
            { wc >>= CBIT_BYTE - cbitUsed;
              if (cbit + cbitStored == CBIT_GUID)
                 break;

              pb++;
              *pb |= (BYTE) wc;
            } // of if
          } // of for

          result = true;
        } // of if
      } // of if

      return result;
    } // of COPropertySet::PropertySetNameToGuid()
  } // of namespace COM

  /**
   *
   */
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const COM::COPropertySet& p)
  { const COM::PROPERTYMAP&          properties = p.GetProperties();
    COM::PROPERTYMAP::const_iterator iter;
    STATPROPSTG                      statpropstg;
    FMTID                            fmtid = p.GetFMTID();

    if( NULL!=p.GetFriendlySetName() )
      os<<_T("\"")<<p.GetFriendlySetName()<<_T("\"");

    if( FMTID_SummaryInformation == fmtid )
      os<<_T("{SummaryInformation}");
    else if( FMTID_DocSummaryInformation == fmtid )
      os<<_T("{DocumentSummaryInformation}");
    else if( FMTID_UserDefinedProperties == fmtid )
      os<<_T("{UserDefined}");
    else
      os<<((const TString&)CGUID(fmtid)).c_str();

    os<<endl;

    LogStream<TCHAR>* logger = dynamic_cast< LogStream<TCHAR>* >(&os);
    
    if( NULL!=logger )
      logger->SetIndent( logger->GetIndent()+1 );
    
    for( iter=properties.begin();iter!=properties.end();iter++ )
    { PROPID                  propId     = iter->first;
      const PROPVARIANT&      propValue  = iter->second.m_property;
      COM::PropertyStatusType propStatus = iter->second.m_status;
      TCHAR*                  pidName    = NULL;

      if( FMTID_SummaryInformation==fmtid )
        pidName = getPIDName(0,propId);
      else if( FMTID_DocSummaryInformation == fmtid )
        pidName = getPIDName(1,propId);
      else
        pidName = const_cast<LPOLESTR>(p.GetName(propId));

      if( NULL!=pidName )
        os<<pidName;

      statpropstg.vt     = propValue.vt;
      statpropstg.propid = propId;

      os<<statpropstg<<_T("=<")<<propValue<<_T(">")<<endl;
    } // of for

    if( NULL!=logger )
      logger->SetIndent( logger->GetIndent()-1 );

    return os;
  } // of operator <<()
} // of namespace bvr20983

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<< <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const bvr20983::COM::COPropertySet&);
/*==========================END-OF-FILE===================================*/
