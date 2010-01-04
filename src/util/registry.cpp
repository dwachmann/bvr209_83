/*
 * $Id$
 * 
 * access to windows registry.
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
#include <comcat.h>
#include <shlguid.h>
#include "util/registry.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/comptr.h"
#include "util/comstring.h"
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"
#include "util/guid.h"
#include <sstream>


using namespace bvr20983;

namespace bvr20983
{
  /**
   *
   */
  RegKey::RegKey(const TString& path) : m_mainKey(NULL),m_key(NULL)
  { Init(path.c_str()); }

  /**
   *
   */
  RegKey::RegKey(LPCTSTR path) : m_mainKey(NULL),m_key(NULL)
  { Init(path); }

  /**
   *
   */
  void RegKey::Init(LPCTSTR path)
  { if( NULL==path || _tcslen(path)==0 )
      throw runtime_error("path is empty");

    int     i     = 0;
    int     k     = 0;
    LPCTSTR slash = _tcschr(path,_T('\\'));

    if( slash!=_T('\0') )
    { TCHAR  firstPart[MAX_PATH];

      _tcsncpy_s(firstPart,MAX_PATH,path,slash-path);

      if( _tcscmp(firstPart,_T("HKEY_CLASSES_ROOT"))==0 )
        m_mainKey    = HKEY_CLASSES_ROOT;
      else if( _tcscmp(firstPart,_T("HKEY_CURRENT_USER"))==0  )
        m_mainKey    = HKEY_CURRENT_USER;
      else if( _tcscmp(firstPart,_T("HKEY_LOCAL_MACHINE"))==0  )
        m_mainKey    = HKEY_LOCAL_MACHINE;
    } // of if

    if( NULL!=m_mainKey )
      m_subKey  = slash+1;
    else
    { m_subKey  = path;
      m_mainKey = HKEY_CURRENT_USER;
    } // of else
  } // of RegKey::Init()

  /**
   *
   */
  RegKey::~RegKey()
  { Close(); }

  /**
   *
   */
  RegKey::operator TString() const
  { TString result;

    if( NULL!=m_mainKey )
    { if( m_mainKey==HKEY_CLASSES_ROOT )
        result += _T("HKEY_CLASSES_ROOT");
      else if( m_mainKey==HKEY_LOCAL_MACHINE )
        result += _T("HKEY_LOCAL_MACHINE");
      else if( m_mainKey==HKEY_CURRENT_USER )
        result += _T("HKEY_CURRENT_USER");
      else
        result += _T("HKEY_KNOWN");

      if( !m_subKey.empty() )
      { result += '\\';
        result += m_subKey;
      } // of if
    } // of if

    return result;
  }

  /**
   *
   */
  bool RegKey::Create()
  { bool result = false;
    
    if( NULL==m_key )
    { DWORD   disposition = 0;
      LPCTSTR subkey      = m_subKey.empty() ? NULL : m_subKey.c_str();
      LONG    hr          = ::RegCreateKeyEx(m_mainKey,subkey,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&m_key,&disposition);

      if( hr==ERROR_SUCCESS && disposition==REG_CREATED_NEW_KEY )
      { LOGGER_DEBUG<<_T("RegKey::Create() <")<<subkey<<_T(">")<<endl;
    
        result = true;
      } // of if
      
      THROW_LASTERROREXCEPTION(hr);
    } // of if

    return result;
  } // of RegKey::Create()

  /**
   *
   */
  bool RegKey::Open() const
  { bool result = false;

    if( NULL==m_key )
    { LPCTSTR subkey = m_subKey.empty() ? NULL : m_subKey.c_str();
      LONG    hr     = ::RegOpenKeyEx(m_mainKey,subkey,0,KEY_READ,const_cast<HKEY*>(&m_key));

      if( hr==ERROR_SUCCESS )
        result = true;
      else if( hr!=ERROR_FILE_NOT_FOUND )
      { THROW_LASTERROREXCEPTION(hr); }
    } // of if
    else
      result = true;

    return result;
  } // RegKey::Open()

  /**
   *
   */
  void RegKey::Close()
  { if( NULL!=m_key )
    { THROW_LASTERROREXCEPTION( ::RegCloseKey(m_key) ); }

    m_key = NULL;
  } // RegKey::Close()

  /**
   *
   */
  bool RegKey::Exists() const
  { bool    result = false;
    HKEY    key    = NULL;
    LPCTSTR subkey = m_subKey.empty() ? NULL : m_subKey.c_str();
    
    result = ERROR_SUCCESS==::RegOpenKeyEx(m_mainKey,subkey,0,KEY_READ,&key);

    if( result )
      ::RegCloseKey(key);

    return result;
  } // RegKey::Exists()

  /**
   *
   */
  void RegKey::Delete(bool deep)
  { Open();

    TString key = (TString)(*this);

    LOGGER_DEBUG<<_T("RegKey::Delete(deep=")<<deep<<_T(") ")<<key.c_str()<<endl;

    if( deep )
    { TCHAR keyName[MAX_KEY_LENGTH];
      DWORD keyNameSize = MAX_KEY_LENGTH;
      DWORD keyIndex    = 0;
      LONG  result      = ::RegEnumKeyEx(m_key,keyIndex,keyName,&keyNameSize,NULL,NULL,NULL,NULL);

      for( ;
           result!=ERROR_NO_MORE_ITEMS;
           keyNameSize=MAX_KEY_LENGTH,result=::RegEnumKeyEx(m_key,keyIndex++,keyName,&keyNameSize,NULL,NULL,NULL,NULL) 
         )
      { 
        THROW_LASTERROREXCEPTION(result);

        TString subkey = key+_T("\\")+keyName;

        RegKey(subkey.c_str());
      } // of for
    } // of if

    if( !m_subKey.empty() )
    { LONG hr = ::RegDeleteKey(m_mainKey,m_subKey.c_str());

      if( hr!=ERROR_FILE_NOT_FOUND )
        THROW_LASTERROREXCEPTION(hr); 
    }
  } // of RegKey::Delete()

  /**
   *
   */
  bool RegKey::HasSubKey() const
  { TString key;

    return EnumKey(key,0);
  } // of RegKey::HasSubKey()

  /**
   *
   */
  bool RegKey::EnumKey(TString& keyName,DWORD index) const
  { TCHAR buffer[MAX_KEY_LENGTH];
    DWORD bufferSize = MAX_KEY_LENGTH;
    bool  result     = false;

    Open();
  
    LONG enumResult = ::RegEnumKeyEx(m_key,index,buffer,&bufferSize,NULL,NULL,NULL,NULL);

    if( enumResult!=ERROR_NO_MORE_ITEMS )
    { THROW_LASTERROREXCEPTION(enumResult); }
    
    result = enumResult==ERROR_SUCCESS;

    keyName.clear();

    if( result )
    { keyName = *this;
      keyName += _T("\\");
      keyName += buffer;
    } // of if

    return result;
  } // of RegKey::EnumKey()

  /**
   *
   */
  bool RegKey::QueryValue(LPCTSTR name,RegistryValue &value) const
  { bool  result   = false;
    DWORD dataType = 0;
    DWORD dataSize = 0;

    if( Open() )
    { LONG hr = ::RegQueryValueEx(m_key,name,0,&dataType,NULL,&dataSize);
   
      if( hr==ERROR_SUCCESS )
      { if( REG_SZ==dataType )
        { LPTSTR dataBuffer = new TCHAR[dataSize];
          ::memset( dataBuffer, '\0', dataSize );
          
          THROW_LASTERROREXCEPTION( ::RegQueryValueEx(m_key,name,0,&dataType,(LPBYTE) dataBuffer,&dataSize) );

          value = RegistryValue(name,dataBuffer,dataType);

          delete[] dataBuffer;
        } // of if
        else if( REG_DWORD==dataType )
        { DWORD dataValue;

          dataSize = sizeof(dataValue);

          THROW_LASTERROREXCEPTION( ::RegQueryValueEx(m_key,name,0,&dataType,(LPBYTE) &dataValue,&dataSize) );

          value = RegistryValue(name,dataValue,dataType);
        } // of else if

        result = true;
      } // of if
      else if( hr!=ERROR_FILE_NOT_FOUND )
        THROW_LASTERROREXCEPTION( hr );
    } // of if

    return result;
  } // of RegKey::QueryValue()

  /**
   *
   */
  void RegKey::SetValue(const RegistryValue& value)
  { Create();

    LPCTSTR valueName = value.IsDefaultValue() ? NULL : value.GetName();

    THROW_LASTERROREXCEPTION( ::RegSetValueEx(m_key,valueName,0,value.GetType(),value.GetBuffer(),value.GetSize()) ); 
  } // of RegistryValue::SetValue()

  /**
   *
   */
  RegKeyEnum::RegKeyEnum(LPCTSTR path,UINT32 maxDepth) : m_maxDepth(maxDepth)
  { m_stack.push(State(RegKey(path))); 
  }
  /**
   *
   */
  RegKeyEnum::RegKeyEnum(const TString& path,UINT32 maxDepth) : m_maxDepth(maxDepth)
  { m_stack.push(State(RegKey(path))); 
  }

  /**
   *
   */
  bool RegKeyEnum::Next(TString& keyName)
  { bool  nextResult = false;
    
    for( ;; )
    { const State& state = m_stack.top();
      TString      key;

      bool hasKey = state.m_key.EnumKey(key,state.m_index);

      if( hasKey )
        m_stack.push(State(RegKey(key))); 

      if( !hasKey || m_stack.size()>=m_maxDepth )
      { keyName = m_stack.top().m_key;

        m_stack.pop();
        m_stack.top().m_index++;

        return true;
      } // of if
    } // of for

    return false;
  } // of RegKeyEnum::Next()

  /**
   *
   */
  RegistryValue::RegistryValue() 
    : m_type(REG_SZ),
      m_pValue(NULL),
      m_intValue(0)
  { }

  /**
   *
   */
  RegistryValue::RegistryValue(LPCTSTR name,LPCTSTR value,DWORD type) :
    m_pValue(NULL),
    m_intValue(0),
    m_type(type)
  { if( NULL!=name )
      m_name = name;

    m_pValue = new TString(NULL!=value ? value : _T("")); 
  }

  /**
   *
   */
  RegistryValue::RegistryValue(LPCTSTR name,DWORD value,DWORD type) :
    m_pValue(NULL),
    m_intValue(value),
    m_type(type)
  { if( NULL!=name )
      m_name = name;
  }

  /**
   *
   */
  RegistryValue::RegistryValue(const RegistryValue& val) :
    m_pValue(NULL),
    m_intValue(0),
    m_type(REG_SZ)
  { *this = val; }

  /**
   *
   */
  RegistryValue::~RegistryValue()
  { delete m_pValue; }


  RegistryValue& RegistryValue::operator=(const RegistryValue& val)
  { this->m_name                = val.m_name; 
    this->m_pValue              = NULL!=val.m_pValue ? new TString(*val.m_pValue) : NULL; 
    this->m_intValue            = val.m_intValue; 
    this->m_type                = val.m_type; 
  
    return *this; 
  }


  /**
   *
   */
  void RegistryValue::GetValue(TString &value) const
  { if( NULL!=m_pValue )
      value = *m_pValue;
    else
    { TCHAR intValue[32];

      _ltot_s(m_intValue,intValue,ARRAYSIZE(intValue),10);

      value = intValue;
    } // of else
  } // of RegistryValue::GetValue()

  /**
   *
   */
  void RegistryValue::GetValue(DWORD& value) const
  { if( NULL!=m_pValue )
      value = _ttol(m_pValue->c_str());
    else
      value = m_intValue;
  } // of RegistryValue::GetValue()

  /**
   *
   */
  RegistryKey::RegistryKey(LPCTSTR key)
  { if( NULL!=key )
      m_key = key;
  }

  /**
   *
   */
  RegistryKey::RegistryKey(const TString& key)
  { m_key = key; }

  /**
   *
   */
  RegistryKey::RegistryKey(const RegistryKey& key)
  { *this = key; }

  /**
   *
   */
  RegistryKey::~RegistryKey()
  { }

  /**
   *
   */
  RegistryKey& RegistryKey::operator=(const RegistryKey& val)
  { this->m_key    = val.m_key;
    this->m_values = val.m_values;

    return *this;
  }

  /**
   *
   */
  void RegistryKey::SetValue(LPCTSTR name,LPCTSTR value)
  { LPCTSTR intName = name==NULL ? _T("@") : name;
    
    if( m_values.empty() || m_values.find(intName)==m_values.end() )
      m_values.insert( RegistryValueP(intName,RegistryValue(intName,value)) );
    else
      m_values.find(intName)->second = RegistryValue(intName,value);
  } // of RegistryKey::SetValue()

  /**
   *
   */
  void RegistryKey::SetValue(LPCTSTR name,DWORD value)
  { LPCTSTR intName = name==NULL ? _T("@") : name;

    if( m_values.empty() || m_values.find(intName)==m_values.end() )
      m_values.insert( RegistryValueP(intName,RegistryValue(intName,value)) );
    else
      m_values.find(intName)->second = RegistryValue(intName,value);
  } // of RegistryKey::SetValue()

  /**
   *
   */
  bool RegistryKey::QueryValue(LPCTSTR name,RegistryValue& value) const
  { LPCTSTR                        intName = name==NULL ? _T("@") : name;
    bool                           result  = false;
    RegistryValueM::const_iterator iter    = m_values.find(intName);

    if( !m_values.empty() && iter!=m_values.end() )
    { value = iter->second;

      result = true;
    } // of if
    else
    { RegKey regKey(m_key);

      result = regKey.QueryValue(name,value);
    } // of else

    return result;
  } // of RegistryKey::QueryValue()

  /**
   *
   */
  bool RegistryKey::Prepare(bool deleteKey) const
  { bool result = true;

    return result;
  } // of RegistryKey::Prepare()

  /**
   *
   */
  bool RegistryKey::Commit(bool deleteKey)
  { bool result = false;

    RegKey regKey(m_key.c_str());

    if( deleteKey )
    { regKey.Delete();

      result = true;
    } // of if
    else
    { regKey.Create();

      for( RegistryValueM::const_iterator valueIter=m_values.begin();valueIter!=m_values.end();valueIter++ )
        regKey.SetValue(valueIter->second);

      result = true;
    } // of else

    return result;
  } // of RegistryKey::Commit()


  /**
   *
   */
  void Registry::GetKeyPath(LPCTSTR subkey,TString& keyPath) const
  { keyPath.clear();

    if( !m_keyPrefix.empty() )
      keyPath += m_keyPrefix;
    else
      keyPath += _T("HKEY_CURRENT_USER");

    if( NULL!=subkey )
    { keyPath += _T("\\");
      keyPath += subkey;
    } // of if
  } // of Registry::GetKeyPath()


  /**
   *
   */
  void Registry::SetKey(LPCTSTR subkey)
  { TString keyPath;

    GetKeyPath(subkey,keyPath);

    if( !keyPath.empty() )
    { RegistryKey                  key;
      RegistryKeyM::const_iterator keyIter = m_keys.find(keyPath.c_str());

      if( m_keys.empty() || keyIter==m_keys.end() )
      { key = RegistryKey(keyPath.c_str());

        m_keys.insert( RegistryKeyP(keyPath.c_str(),key) );
      } // of if
    } // of if
  } // of Registry::SetKey() 

  /**
   *
   */
  void Registry::SetValue(LPCTSTR subkey,LPCTSTR name,const TString& value,DWORD type)
  { TString keyPath;

    GetKeyPath(subkey,keyPath);

    if( !keyPath.empty() )
    { RegistryKey            key;
      RegistryKeyM::iterator keyIter = m_keys.find(keyPath.c_str());

      if( m_keys.empty() || keyIter==m_keys.end() )
      { key = RegistryKey(keyPath.c_str());

        key.SetValue(name,value.c_str());

        m_keys.insert( RegistryKeyP(keyPath.c_str(),key) );
      } // of if
      else
        keyIter->second.SetValue(name,value.c_str());;
    } // of if
  } // of Registry::SetValue()

  /**
   *
   */
  void Registry::SetValue(LPCTSTR subkey,LPCTSTR name,DWORD value,DWORD type)
  { TString keyPath;

    GetKeyPath(subkey,keyPath);

    if( !keyPath.empty() )
    { RegistryKey            key;
      RegistryKeyM::iterator keyIter = m_keys.find(keyPath.c_str());

      if( m_keys.empty() || keyIter==m_keys.end() )
      { key = RegistryKey(keyPath.c_str());

        key.SetValue(name,value);

        m_keys.insert( RegistryKeyP(keyPath.c_str(),key) );
      } // of if
      else
        keyIter->second.SetValue(name,value);
    } // of if
  } // of Registry::SetValue()

  /**
   *
   */
  void Registry::DeleteKey(LPCTSTR subkey,bool deep)
  { TString keyPath;

    GetKeyPath(subkey,keyPath);

    if( !keyPath.empty() )
    { RegistryKey                   key;
      RegistryKeyM1::const_iterator keyIter = m_deletedKeys.find(keyPath.c_str());

      if( m_deletedKeys.empty() || keyIter==m_deletedKeys.end() )
      { key = RegistryKey(keyPath.c_str());

        m_deletedKeys.insert( RegistryKeyP(keyPath.c_str(),key) );
      } // of if
    } // of if

    if( deep )
    { TString    oldKeyPrefix;
      RegKeyEnum regKeyEnum(keyPath.c_str(),10);

      GetKeyPrefix(oldKeyPrefix);
      SetKeyPrefix(NULL);

      for( TString key;regKeyEnum.Next(key); )
        DeleteKey(key.c_str(),false);

      SetKeyPrefix(oldKeyPrefix);
    } // of if
  } // of Registry::DeleteKey()

  /**
   *
   */

  bool Registry::QueryValue(LPCTSTR subkey,LPCTSTR name,RegistryValue& value) const
  { bool    result = false;
    TString keyPath;

    GetKeyPath(subkey,keyPath);

    if( !keyPath.empty() )
    { if(  !m_keys.empty() )
      { RegistryKeyM::const_iterator keyIter = m_keys.find(keyPath.c_str());

        if( keyIter!=m_keys.end() )
          keyIter->second.QueryValue(name,value);
      } // of if
      else
      { RegistryKey key(keyPath.c_str());

        result = key.QueryValue(name,value);
      } // of else
    } // of if

    return result;
  } // of Registry::QueryValue()

  /**
   *
   */
  bool Registry::Prepare() const
  { bool result = true;

    for( RegistryKeyM::const_iterator keyIter=m_keys.begin();keyIter!=m_keys.end();keyIter++ )
    { if( !keyIter->second.Prepare(false) )
      { result = false;
        break;
      } // of if
    } // of for

    for( RegistryKeyM1::const_iterator keyIter=m_deletedKeys.begin();keyIter!=m_deletedKeys.end();keyIter++ )
    { if( !keyIter->second.Prepare(true) )
      { result = false;
        break;
      } // of if
    } // of for

    return result;
  } // of Registry::Prepare()

  /**
   *
   */
  bool Registry::Commit()
  { bool result = false;

    for( RegistryKeyM::iterator keyIter=m_keys.begin();keyIter!=m_keys.end();keyIter++ )
    { if( !keyIter->second.Commit(false) )
      { result = false;
        break;
      } // of if
    } // of for

    for( RegistryKeyM1::iterator keyIter=m_deletedKeys.begin();keyIter!=m_deletedKeys.end();keyIter++ )
    { 
      if( !keyIter->second.Commit(true) )
      { result = false;
        break;
      } // of if
    } // of for

    return result;
  } // of Registry::Commit()

  /**
   *
   */
  bool Registry::Rollback()
  { bool result = true;

    m_keys.clear();
    m_deletedKeys.clear();

    return result;
  } // of Registry::Rollback()

  /**
   *
   */
  void Registry::DeleteKeys()
  { 
    for( RegistryKeyM::const_iterator keyIter=m_keys.begin();keyIter!=m_keys.end();keyIter++ )
      m_deletedKeys.insert( *keyIter );

    m_keys.clear();
  } // of Registry::DeleteKeys()

  /**
   *
   */
  void Registry::EnumRegistry(REGISTRYINFOPROC pEnumProc, LPARAM lParam)
  { if( NULL!=pEnumProc )
    { DWORD                          i           = 0;
      const TString&                 compId      = GetComponentId();
      const Registry::RegistryKeyM&  keys        = GetKeys();
      const Registry::RegistryKeyM1& deletedKeys = GetDeletedKeys();

      for( Registry::RegistryKeyM::const_iterator it=keys.begin();it!=keys.end();it++ )
      { RegKey                            regKey(it->second.GetKey());
        const HKEY                        mainKey = regKey.GetMainKey();
        const TString&                    subKey  = regKey.GetSubKey();
        const RegistryKey::RegistryValueM values  = it->second.GetValues();

        TString key;

        if( mainKey==HKEY_CLASSES_ROOT )
          key.append(_T("HKEY_CLASSES_ROOT"));
        else if( mainKey==HKEY_CURRENT_USER )
          key.append(_T("HKEY_CURRENT_USER"));
        else if( mainKey==HKEY_LOCAL_MACHINE )
          key.append(_T("HKEY_LOCAL_MACHINE"));
        else if( mainKey==HKEY_USERS )
          key.append(_T("HKEY_USERS"));

        key.append(_T("\\"));
        key.append(subKey);

        if( !values.empty() )
        { 
          for( RegistryKey::RegistryValueM::const_iterator it1=values.begin();it1!=values.end();it1++ )
          { const RegistryValue& rVal = it1->second;

            TString valueName;
            TString value;

            if( rVal.IsDefaultValue() )
              valueName.append(_T("@"));
            else
              valueName.append(rVal.GetName());

            DWORD type=rVal.GetType();

            if( REG_SZ==type )
            { TString val;

              rVal.GetValue(val);

              //value.append(_T("\""));
              value.append(val);
              //value.append(_T("\""));
            } // of if
            else if( REG_DWORD==type )
            { DWORD val;
              TCHAR val1[20];

              rVal.GetValue(val);

              value.append(_T("dword:"));

              _itot_s(val,val1,20,10);
              value.append(val1);
            } // of else if

            if( !(*pEnumProc)(lParam,false,key.c_str(),valueName.c_str(),value.c_str()) )
              return;
          } // of for
        } // of if
      } // of if
    } // of if
  } // of Registry::EnumRegistry()

  /**
   *
   */
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const RegistryValue& rVal)
  { os<<_T("\"");

    if( rVal.IsDefaultValue() )
      os<<_T("@");
    else
      os<<rVal.GetName();
    
    os<<_T("\"")<<_T("="); 

    DWORD type=rVal.GetType();

    if( REG_SZ==type )
    { TString value;

      rVal.GetValue(value);

      for( DWORD i=0;i<value.length();i++ )
      { if( value[i]==_T('\\') )
        { value.insert(i+1,1,'\\');
          i++;
        } // of if
      } // of for

      os<<_T("\"")<<value.c_str()<<_T("\"");
    } // of if
    else if( REG_DWORD==type )
    { DWORD value;

      rVal.GetValue(value);

      os<<value;
    } // of else if

    return os;
  }

  /**
   *
   */
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const RegKey& rKey)
  { TString path = rKey;

    os<<_T("[")<<path.c_str()<<_T("]"); 
  
    return os;
  }

  /**
   *
   */
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const RegistryKey& rKey)
  { os<<_T("[")<<rKey.GetKey()<<_T("]")<<endl; 

    const RegistryKey::RegistryValueM values = rKey.GetValues();

    for( RegistryKey::RegistryValueM::const_iterator it=values.begin();it!=values.end();it++ )
      os<<it->second<<endl;

    return os;
  }

  /**
   *
   */
  template<class charT, class Traits> 
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const Registry& reg)
  { const Registry::RegistryKeyM&  keys        = reg.GetKeys();
    const Registry::RegistryKeyM1& deletedKeys = reg.GetDeletedKeys();
    Registry::DumpT                dumpType    = reg.GetDumpType();

    if( dumpType==Registry::REGEDIT )
    { os<<_T("Windows Registry Editor Version 5.00")<<endl<<endl;
      
      for( Registry::RegistryKeyM::const_iterator it=keys.begin();it!=keys.end();it++ )
        os<<it->second<<endl;

      for( Registry::RegistryKeyM1::const_iterator it=deletedKeys.begin();it!=deletedKeys.end();it++ )
        os<<_T("D:")<<it->second<<endl;
    } // of if
    else if( dumpType==Registry::MSI )
    { DWORD i               = 0;
      const TString& compId = reg.GetComponentId();

      for( Registry::RegistryKeyM::const_iterator it=keys.begin();it!=keys.end();it++ )
      { 
        RegKey                            regKey(it->second.GetKey());
        const HKEY                        mainKey = regKey.GetMainKey();
        const TString&                    subKey  = regKey.GetSubKey();
        const RegistryKey::RegistryValueM values  = it->second.GetValues();
        if( !values.empty() )
        { for( RegistryKey::RegistryValueM::const_iterator it1=values.begin();it1!=values.end();it1++ )
          { const RegistryValue& rVal = it1->second;

            os<<compId<<_T('.')<<(i++)<<_T('\t');

            if( mainKey==HKEY_CLASSES_ROOT )
              os<<0;
            else if( mainKey==HKEY_CURRENT_USER )
              os<<1;
            else if( mainKey==HKEY_LOCAL_MACHINE )
              os<<2;
            else if( mainKey==HKEY_USERS )
              os<<3;

            os<<_T('\t');
            os<<subKey;
            os<<_T('\t');

            if( !rVal.IsDefaultValue() )
              os<<rVal.GetName();

            os<<_T('\t');

            DWORD type=rVal.GetType();

            if( REG_SZ==type )
            { TString value;

              rVal.GetValue(value);

              os<<value;
            } // of if
            else if( REG_DWORD==type )
            { DWORD value;

              rVal.GetValue(value);

              os<<_T('#')<<value;
            } // of else if

            os<<_T('\t')<<compId<<endl;
          } // of for
        } // of if
        else
        { os<<compId<<_T('.')<<(i++)<<_T('\t');

          RegKey                            regKey(it->second.GetKey());
          const HKEY                        mainKey = regKey.GetMainKey();
          const TString&                    subKey  = regKey.GetSubKey();
          const RegistryKey::RegistryValueM values  = it->second.GetValues();

          if( mainKey==HKEY_CLASSES_ROOT )
            os<<0;
          else if( mainKey==HKEY_CURRENT_USER )
            os<<1;
          else if( mainKey==HKEY_LOCAL_MACHINE )
            os<<2;
          else if( mainKey==HKEY_USERS )
            os<<3;

          os<<_T('\t');
          os<<subKey;
          os<<_T('\t');
          os<<_T('\t');
          os<<_T('\t')<<compId<<endl;
        } // of else
      } // of for
    } // of else
    else if( dumpType==Registry::XML )
    { 
      os<<_T("<?xml version='1.0' encoding='UTF-8' ?>")<<endl<<endl;

      os<<_T("<registry>")<<endl;
      
      DWORD i               = 0;
      const TString& compId = reg.GetComponentId();

      for( Registry::RegistryKeyM::const_iterator it=keys.begin();it!=keys.end();it++ )
      { 
        RegKey                            regKey(it->second.GetKey());
        const HKEY                        mainKey = regKey.GetMainKey();
        const TString&                    subKey  = regKey.GetSubKey();
        const RegistryKey::RegistryValueM values  = it->second.GetValues();

        os<<_T("  <reg id='")<<compId<<_T('.')<<(i++)<<_T("' ");

        if( mainKey==HKEY_CLASSES_ROOT )
          os<<_T(" root='0' ");
        else if( mainKey==HKEY_CURRENT_USER )
          os<<_T(" root='1' ");
        else if( mainKey==HKEY_LOCAL_MACHINE )
          os<<_T(" root='2' ");
        else if( mainKey==HKEY_USERS )
          os<<_T(" root='3' ");

        os<<_T(" compid='")<<compId<<_T("'>")<<endl;

        os<<_T("    <key>")<<subKey<<_T("</key>")<<endl;

        if( !values.empty() )
        { 
          os<<_T("    <values>")<<endl;

          for( RegistryKey::RegistryValueM::const_iterator it1=values.begin();it1!=values.end();it1++ )
          { const RegistryValue& rVal = it1->second;

            if( rVal.IsDefaultValue() )
              os<<_T("      <default>");
            else
              os<<_T("      <value id='")<<rVal.GetName()<<_T("'>");

            DWORD type=rVal.GetType();

            if( REG_SZ==type )
            { TString value;

              rVal.GetValue(value);

              os<<value;
            } // of if
            else if( REG_DWORD==type )
            { DWORD value;

              rVal.GetValue(value);

              os<<_T('#')<<value;
            } // of else if

            if( rVal.IsDefaultValue() )
              os<<_T("</default>")<<endl;
            else
              os<<_T("</value>")<<endl;
          } // of for

          os<<_T("    </values>")<<endl;
        } // of if

        os<<_T("  </reg>")<<endl;
      } // of for

      os<<_T("</registry>")<<endl;
    } // of else

    return os;
  }

  template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const RegistryValue&);
  template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const RegistryKey&);
  template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const RegKey&);
  template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const Registry&);
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
