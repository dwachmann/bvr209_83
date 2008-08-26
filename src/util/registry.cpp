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

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

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
    { THROW_LASTERROREXCEPTION( ::RegDeleteKey(m_mainKey,m_subKey.c_str()) ); }
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
  { if( Create() )
    { THROW_LASTERROREXCEPTION( ::RegSetValueEx(m_key,value.GetName(),0,value.GetType(),value.GetBuffer(),value.GetSize()) ); }
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

      if( state.m_key.EnumKey(key,state.m_index) )
      { m_stack.push(State(RegKey(key))); 
        
        if( m_stack.size()>=m_maxDepth )
          break;
      } // of if
      else
      { m_stack.pop();
        break;
      } // of else
    } // of for

    if( m_stack.size()>1 )
    { State& state = m_stack.top();

      keyName = state.m_key;

      state.m_index++;
        
      nextResult = true;
    } // of if

    return nextResult;
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
    m_name(name),
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
  { this->m_name     = val.m_name; 
    this->m_pValue   = NULL!=val.m_pValue ? new TString(*val.m_pValue) : NULL; 
    this->m_intValue = val.m_intValue; 
    this->m_type     = val.m_type; 
  
    return *this; 
  }


  /**
   *
   */
  void RegistryValue::GetValue(TString &value)
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
  void RegistryValue::GetValue(DWORD& value)
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
  { if( m_values.empty() || m_values.find(name)==m_values.end() )
      m_values.insert( RegistryValueP(name,RegistryValue(name,value)) );
    else
      m_values.find(name)->second = RegistryValue(name,value);
  } // of RegistryKey::SetValue()

  /**
   *
   */
  void RegistryKey::SetValue(LPCTSTR name,DWORD value)
  { if( m_values.empty() || m_values.find(name)==m_values.end() )
      m_values.insert( RegistryValueP(name,RegistryValue(name,value)) );
    else
      m_values.find(name)->second = RegistryValue(name,value);
  } // of RegistryKey::SetValue()

  /**
   *
   */
  bool RegistryKey::QueryValue(LPCTSTR name,RegistryValue& value) const
  { bool                           result = false;
    RegistryValueM::const_iterator iter   = m_values.find(name);

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
  bool RegistryKey::Prepare() const
  { bool result = true;

    return result;
  } // of RegistryKey::Prepare()

  /**
   *
   */
  bool RegistryKey::Commit()
  { bool result = false;

    RegKey regKey(m_key.c_str());

    if( regKey.Create() )
    {
      for( RegistryValueM::const_iterator valueIter=m_values.begin();valueIter!=m_values.end();valueIter++ )
        regKey.SetValue(valueIter->second);

      result = true;
    } // of if

    return result;
  } // of RegistryKey::Commit()

  /**
   *
   */
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const RegistryKey& rKey)
  { os<<"["<<rKey.GetKey()<<"]"; 
  
    return os;
  }

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
  void Registry::SetValue(LPCTSTR subkey,LPCTSTR name,const TString& value,DWORD type)
  { TString keyPath;

    GetKeyPath(subkey,keyPath);

    if( !keyPath.empty() )
    { RegistryKey                  key;
      RegistryKeyM::const_iterator keyIter = m_keys.find(keyPath.c_str());

      if( m_keys.empty() || keyIter==m_keys.end() )
      { key = RegistryKey(keyPath.c_str());

        m_keys.insert( RegistryKeyP(keyPath.c_str(),key) );
      } // of if
      else
        key = keyIter->second;

      key.SetValue(name,value.c_str());
    } // of if
  } // of Registry::SetValue()

  /**
   *
   */
  void Registry::SetValue(LPCTSTR subkey,LPCTSTR name,DWORD value,DWORD type)
  { TString keyPath;

    GetKeyPath(subkey,keyPath);

    if( !keyPath.empty() )
    { RegistryKey                  key;
      RegistryKeyM::const_iterator keyIter = m_keys.find(keyPath.c_str());

      if( m_keys.empty() || keyIter==m_keys.end() )
      { key = RegistryKey(keyPath.c_str());

        m_keys.insert( RegistryKeyP(keyPath.c_str(),key) );
      } // of if
      else
        key = keyIter->second;

      key.SetValue(name,value);
    } // of if
  } // of Registry::SetValue()

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
    { if( !keyIter->second.Prepare() )
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
    { if( !keyIter->second.Commit() )
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

    return result;
  } // of Registry::Rollback()
} // of namespace bvr20983

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const RegistryKey&);
/*==========================END-OF-FILE===================================*/
