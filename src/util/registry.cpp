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
  RegistryKey::RegistryKey(const RegistryKey& path,LPCTSTR subkey) : m_pDumpFile(NULL)
  { TString p = path;

    if( NULL!=subkey )
    { p += '\\';
      p += subkey;
    } // of if
    
    Init( p.c_str() ); 
  }

  /**
   *
   */
  RegistryKey::RegistryKey(const TString& path) : m_pDumpFile(NULL)
  { Init(path.c_str()); }

  /**
   *
   */
  RegistryKey::RegistryKey(LPCTSTR path) : m_pDumpFile(NULL)
  { Init(path); }

  /**
   *
   */
  void RegistryKey::Init(LPCTSTR path)
  { if( NULL==path )
      throw invalid_argument("path is NULL");

    m_keyOpened = false;
    
    int pathLen = _tcslen(path);
    int i       = 0;
    int k       = 0;
    
    for( ;i<=pathLen;i++ )
    { if( path[i]==_T('\\') || path[i]==_T('\0') )
      { TString s = TString(path,k,i-k);

        m_path.push_back( s );
        
        k = i+1;
      } // of if
    } // of for
      
    if( m_path.size()>0 )
    { const TString& first = m_path.front();
    
      if( first==_T("HKEY_CLASSES_ROOT") )
      { m_mainKey    = HKEY_CLASSES_ROOT;
        m_mainKeyStr = m_path.front();
        m_path.erase(m_path.begin());
      } // of if
      else if( first==_T("HKEY_CURRENT_USER") )
      { m_mainKey    = HKEY_CURRENT_USER;
        m_mainKeyStr = m_path.front();
        m_path.erase(m_path.begin());
      } // of else if
      else if( first==_T("HKEY_LOCAL_MACHINE") )
      { m_mainKey    = HKEY_LOCAL_MACHINE;
        m_mainKeyStr = m_path.front();
        m_path.erase(m_path.begin());
      } // of else if
      else 
      { m_mainKey    = HKEY_CURRENT_USER;
        m_mainKeyStr = _T("HKEY_CURRENT_USER");
      } // of else if
    } // of if
    else
      throw invalid_argument("path is empty");
      
    m_subpath = TString();
    
    VTString::iterator iter;
  
    for( iter=m_path.begin();iter!=m_path.end();iter++ )
    { m_subpath += *iter;
    
      if( iter<m_path.end()-1 )
        m_subpath += '\\';
    } // of for

    //LOGGER_DEBUG<<_T("RegistryKey::RegistryKey(")<<path<<_T(") <")<<m_subpath<<_T(">")<<endl;
  } // of RegistryKey::RegistryKey()

  /**
   *
   */
  RegistryKey::~RegistryKey()
  { Close(); 

    SetDumpFile(NULL);
  } // of RegistryKey::~RegistryKey()

  /**
   *
   */
  void RegistryKey::SetDumpFile(LPCTSTR dumpFilename)
  { if( NULL!=m_pDumpFile )
      m_pDumpFile->flush();

    delete m_pDumpFile;

    m_pDumpFile = NULL;

    if( NULL!=dumpFilename )
    {
#ifdef _UNICODE
      m_pDumpFile = new wofstream(dumpFilename,ios_base::app);
#else
      m_pDumpFile = new ofstream(dumpFilename,ios_base::app);
#endif
    } // of if
  } // of RegistryKey::SetDumpFile()


  /**
   *
   */
  void RegistryKey::Close()
  { if( m_keyOpened && NULL==m_pDumpFile )
      ::RegCloseKey(m_key);
      
    m_keyOpened = false;
  } // of RegistryKey::Close()

  /**
   *
   */
  bool RegistryKey::Create()
  { bool result = false;

    Close();

    if( NULL==m_pDumpFile )
    { DWORD lpdwDisposition = 0;

      THROW_LASTERROREXCEPTION( ::RegCreateKeyEx(m_mainKey,m_subpath.c_str(),0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&m_key,&lpdwDisposition) );

      m_keyOpened = true;

      if( lpdwDisposition==REG_CREATED_NEW_KEY )
        LOGGER_DEBUG<<_T("RegistryKey::Create() <")<<m_subpath<<_T(">")<<endl;
    
      result = lpdwDisposition==REG_CREATED_NEW_KEY;
    } // of if
    else
    { if( !m_keyOpened )
        result = true;

      m_keyOpened = true;
    } // of else

    return result;
  } // of RegistryKey::Create()

  /**
   *
   */
  bool RegistryKey::Exists()
  { bool result = false;

    if( NULL==m_pDumpFile )
    { HKEY key = NULL;
      
      result = ERROR_SUCCESS==::RegOpenKeyEx(m_mainKey,m_subpath.c_str(),0,KEY_READ,&key);

      if( result )
        ::RegCloseKey(key);
    } // of if
    else 
      result = true;

    return result;
  } // RegistryKey::Exists()

  /**
   *
   */
  void RegistryKey::Open()
  { Close();
    
    if( NULL==m_pDumpFile )
    { THROW_LASTERROREXCEPTION( ::RegOpenKeyEx(m_mainKey,m_subpath.c_str(),0,KEY_READ,&m_key) );
    } // of if

    m_keyOpened = true;
  } // RegistryKey::Open()

  /**
   *
   */
  void RegistryKey::Delete(bool deep)
  { if( NULL==m_pDumpFile )
    { LOGGER_DEBUG<<_T("RegistryKey::Delete(deep=")<<deep<<_T(") ")<<*this<<endl;

      if( !deep )
      { THROW_LASTERROREXCEPTION( ::RegDeleteKey(m_mainKey,m_subpath.c_str()) ); }
      else
      { TString  keyName;
        VTString keys;
      
        RegistryKeyEnum regEnum(*this,0);
      
        for( ;regEnum.Next(keyName); )
          keys.push_back( keyName );

        VTString::iterator iter;
      
        for( iter=keys.begin();iter!=keys.end();iter++ )
        { RegistryKey k(*iter);
        
          k.Delete(false);
        } // of for
        
        Delete(false);
      } // of else
    } // of if
  } // of RegistryKey::Delete()

  /**
   *
   */
  void RegistryKey::SetValue(LPCTSTR name,LPCTSTR value)
  { Create();

    if( NULL==m_pDumpFile )
    { if( NULL!=value )
      { LOGGER_DEBUG<<_T("RegistryKey::SetValue(name=")<<(name!=NULL?name:_T("NULL"))<<_T(",value=")<<(value!=NULL?value:_T("NULL"))<<_T(")")<<endl;
        
        THROW_LASTERROREXCEPTION( ::RegSetValueEx(*this,name,0,REG_SZ,(BYTE *)value,(_tcslen(value)+1)*sizeof(TCHAR)) ); 
      } // of if
    } // of if
    else
    { (*m_pDumpFile)<<_T("[")<<m_mainKeyStr<<_T("\\")<<m_subpath<<_T("]")<<endl;
      
      if( NULL!=value )
      { if( NULL==name )
          (*m_pDumpFile)<<_T("@=\"")<<value<<_T("\"")<<endl;
        else
          (*m_pDumpFile)<<_T("\"")<<name<<_T("\"=\"")<<value<<_T("\"")<<endl;
      } // of if

      (*m_pDumpFile)<<endl;
    } // of else
  } // of RegistryKey::SetValue()

  /**
   *
   */
  void RegistryKey::SetIntValue(LPCTSTR name,DWORD value)
  { Create();
    
    if( NULL==m_pDumpFile )
    { if( NULL!=value )
      { LOGGER_DEBUG<<_T("RegistryKey::SetIntValue(name=")<<(name!=NULL?name:_T("NULL"))<<_T(",value=")<<value<<_T(")")<<endl;

        THROW_LASTERROREXCEPTION( ::RegSetValueEx(*this,name,0,REG_DWORD,(BYTE *)&value,sizeof(value)) ); 
      }
    } // of if
    else
    { (*m_pDumpFile)<<_T("[")<<m_mainKeyStr<<_T("\\")<<m_subpath<<_T("]")<<endl;
      
      if( NULL==name )
        (*m_pDumpFile)<<_T("@=dword:")<<hex<<setfill(_T('0'))<<setw(8)<<value<<endl;
      else
        (*m_pDumpFile)<<_T("\"")<<name<<_T("\"=dword:")<<hex<<setfill(_T('0'))<<setw(8)<<value<<endl;

      (*m_pDumpFile)<<endl;
    } // of else
  } // of RegistryKey::SetIntValue()

  /**
   *
   */
  void RegistryKey::QueryValue(LPCTSTR name,TString &value)
  { Open();

    if( NULL==m_pDumpFile )
    { DWORD dataType = 0;
      DWORD dataSize = 0;
      
      THROW_LASTERROREXCEPTION( ::RegQueryValueEx(*this,name,0,&dataType,NULL,&dataSize) );

      if( REG_SZ==dataType )
      { LPTSTR dataBuffer = new TCHAR[dataSize];
        memset( dataBuffer, '\0', dataSize );
        
        THROW_LASTERROREXCEPTION( ::RegQueryValueEx(*this,name,0,&dataType,(LPBYTE) dataBuffer,&dataSize) );

        value.assign(dataBuffer,(dataSize-1)/sizeof(TCHAR));

        delete[] dataBuffer;
      } // of if
    } // of if
    else
      throw runtime_error("Could not query in dump mode");
  } // of RegistryKey::QueryValue()


  /**
   *
   */
  DWORD RegistryKey::QueryIntValue(LPCTSTR name)
  { DWORD queryResult = 0;
    
    Open();

    if( NULL==m_pDumpFile )
    { DWORD dataType = 0;
      DWORD dataSize = 0;
      
      LONG result = ::RegQueryValueEx(*this,name,0,&dataType,NULL,&dataSize);
      THROW_LASTERROREXCEPTION(result);

      if( REG_DWORD==dataType )
      { result = ::RegQueryValueEx(*this,name,0,&dataType,(LPBYTE) &queryResult,&dataSize);
        THROW_LASTERROREXCEPTION(result);
      } // of if
    } // of if
    else
      throw runtime_error("Could not query in dump mode");
      
    return queryResult;
  } // of RegistryKey::QueryIntValue()


  /**
   *
   */
  bool RegistryKey::HasSubKey()
  { TCHAR buffer[256];
    DWORD bufferSize = sizeof(buffer)/sizeof(buffer[0]);
    bool  result     = false;
  
    Open();
      
    if( NULL==m_pDumpFile )
    { LONG enumResult = ::RegEnumKeyEx(m_key,0,buffer,&bufferSize,NULL,NULL,NULL,NULL);

      if( enumResult!=ERROR_NO_MORE_ITEMS )
      { THROW_LASTERROREXCEPTION(enumResult); }
      
      result = enumResult==ERROR_SUCCESS;
    } // of if
    else
      throw runtime_error("Could not query in dump mode");

    return result;
  } // of RegistryKey::HasSubKey()

  /**
   *
   */
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const RegistryKey& rKey)
  { os<<"["<<(const TString)rKey<<"]"; 
  
    return os;
  }

  /**
   *
   */
  void Registry::SetKeyValue(LPCTSTR subkey,LPCTSTR name,LPCTSTR value)
  { if( subkey!=NULL )
    { RegistryKey key(m_key,subkey);

      if( !m_dumpFileName.empty() )
        key.SetDumpFile(m_dumpFileName.c_str());

      key.SetValue(name,value);
    } // of if
    else if( value!=NULL )
      m_key.SetValue(name,value);
    else
      m_key.Create();
  } // of Registry::SetKeyValue()

  /**
   *
   */
  void Registry::SetKeyValue(LPCTSTR subkey,LPCTSTR name,DWORD lValue)
  { TCHAR value[255];

    _ultot_s(lValue,value,ARRAYSIZE(value),10);

    if( subkey!=NULL )
    { RegistryKey key(m_key,subkey);

      if( !m_dumpFileName.empty() )
        key.SetDumpFile(m_dumpFileName.c_str());

      key.SetValue(name,value);
    } // of if
    else if( value!=NULL )
      m_key.SetValue(name,value);
    else
      m_key.Create();
  } // of Registry::SetKeyValue()

  /**
   *
   */
  void Registry::SetKeyIntValue(LPCTSTR subkey,LPCTSTR name,DWORD value)
  { if( subkey!=NULL )
    { RegistryKey key(m_key,subkey);

      if( !m_dumpFileName.empty() )
        key.SetDumpFile(m_dumpFileName.c_str());

      key.SetIntValue(name,value);
    }
    else if( value!=NULL )
      m_key.SetIntValue(name,value);
    else
      m_key.Create();
  } // of Registry::SetKeyIntValue()


  /**
   *
   */
  void Registry::QueryKeyValue(LPCTSTR subkey,LPCTSTR name,TString& value)
  { if( subkey!=NULL )
    { RegistryKey key(m_key,subkey);

      if( !m_dumpFileName.empty() )
        key.SetDumpFile(m_dumpFileName.c_str());

      key.QueryValue(name,value);
    }
    else
      m_key.QueryValue(name,value);
  } // of Registry::QueryKeyValue()


  /**
   *
   */
  template<class charT, class Traits>
  basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const Registry& reg)
  { os<<"Registry"<<reg.GetKey(); 
  
    return os;
  }
  
  /**
   *
   */
  RegistryKeyEnum::RegistryKeyEnum(const TString& path,UINT32 maxDepth,bool onlySubKey) 
    : m_maxDepth(maxDepth),
      m_onlySubKey(onlySubKey)
  { LOGGER_DEBUG<<_T("RegistryKeyEnum::RegistryKeyEnum(): path=<")<<path<<_T("> maxDepth=")<<m_maxDepth<<endl;
    m_stack.push(State(RegistryKey(path))); 
    
    m_stack.top().m_key.Open();
  }

  /**
   *
   */
  bool RegistryKeyEnum::Next(TString& keyName)
  { TCHAR buffer[256];
    DWORD bufferSize;
    LONG  result;
    bool  nextResult = false;
    
      for( ;; )
      { bufferSize = sizeof(buffer)/sizeof(buffer[0]);
        
        result = ::RegEnumKeyEx(m_stack.top().m_key,m_stack.top().m_index,buffer,&bufferSize,NULL,NULL,NULL,NULL);
        if( result!=ERROR_NO_MORE_ITEMS )
        { THROW_LASTERROREXCEPTION(result); }
        
        if( result==ERROR_SUCCESS )
        { TString topKey(m_stack.top().m_key);
        
          topKey += '\\';
          topKey += buffer;
          
          RegistryKey subkey( topKey );
          
          if( (m_maxDepth==0 || m_stack.size()<m_maxDepth) && subkey.HasSubKey() )
          { //LOGGER_DEBUG<<_T("RegistryKeyEnum::Next() push <")<<subkey<<_T("> ")<<endl;
            
            m_stack.push(State(subkey)); 
            m_stack.top().m_key.Open();
          } // of if
          else
          { InternalNext(keyName);
          
            m_stack.top().m_index++;

            //LOGGER_DEBUG<<_T("RegistryKeyEnum::Next() incr index=")<<m_stack.top().m_index<<endl;

            nextResult = true;         

            break;
          } // of else
        } // of if
        else if( m_stack.size()>1 )
        { m_stack.pop();

          InternalNext(keyName);
          
          //LOGGER_DEBUG<<_T("RegistryKeyEnum::Next() pop <")<<m_stack.top().m_key<<_T("> ")<<endl;

          m_stack.top().m_index++;
          
          nextResult = true;
          break;
        }
        else
          break;
      } // of for

    //LOGGER_DEBUG<<_T("RegistryKeyEnum::Next() <")<<m_stack.top().m_key<<_T("> ")<<m_stack.top().m_index<<_T(" result=")<<nextResult<<endl;
    
    return nextResult;
  } // of RegistryKeyEnum::Next()

  /**
   *
   */
  void RegistryKeyEnum::InternalNext(TString& keyName)
  { TCHAR buffer[256];
    DWORD bufferSize = sizeof(buffer)/sizeof(buffer[0]);
    
    LONG  result = ::RegEnumKeyEx(m_stack.top().m_key,m_stack.top().m_index,buffer,&bufferSize,NULL,NULL,NULL,NULL);
    if( result!=ERROR_NO_MORE_ITEMS )
    { THROW_LASTERROREXCEPTION(result); }

    keyName.clear();

    if( !m_onlySubKey )
    { keyName.assign(m_stack.top().m_key);
      keyName += '\\';
    } // of if

    keyName += buffer;
  } // of RegistryKeyEnum::InternalNext()
} // of namespace bvr20983

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const RegistryKey&);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const Registry&);
/*==========================END-OF-FILE===================================*/
