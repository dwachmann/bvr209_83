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

const GUID CUST_MISCSTATUS    = {0x7dc6e86d,0x0000,0x1000,{0x00,0x01,0x42,0x56,0x52,0x20,0x09,0x83}};
const GUID CUST_TOOLBOXBITMAP = {0x7dc6e86d,0x0000,0x1000,{0x00,0x02,0x42,0x56,0x52,0x20,0x09,0x83}};
const GUID CUST_CATID         = {0x7dc6e86d,0x0000,0x1000,{0x10,0x00,0x42,0x56,0x52,0x20,0x09,0x83}};

const CATID CATID_SafeForScripting =
{0x7dd95801,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};

const CATID CATID_SafeForInitializing =
{0x7dd95802,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};

const CATID CATID_WindowlessObject =
{0x1D06B600,0x3AE3,0x11cf,{0x87,0xB9,0x00,0xAA,0x00,0x6C,0x81,0x66}};

const CATID CATID_PropertyNotifyControl =
{0x157083E1,0x2368,0x11cf,{0x87,0xB9,0x00,0xAA,0x00,0x6C,0x81,0x66}};

const CATID CATID_SimpleFrameControl =
{0x157083E0,0x2368,0x11cf,{0x87,0xB9,0x00,0xAA,0x00,0x6C,0x81,0x66}};


CATID g_KnownCATIDs[] =
{ CATID_Insertable,                           // 0x00
  CATID_Control,                              // 0x01
  CATID_Programmable,                         // 0x02
  CATID_IsShortcut,                           // 0x03
  CATID_NeverShowExt,                         // 0x04
  CATID_DocObject,                            // 0x05
  CATID_Printable,                            // 0x06
  CATID_RequiresDataPathHost,                 // 0x07
  CATID_PersistsToMoniker,                    // 0x08
  CATID_PersistsToStorage,                    // 0x09
  CATID_PersistsToStreamInit,                 // 0x0a
  CATID_PersistsToStream,                     // 0x0b
  CATID_PersistsToMemory,                     // 0x0c
  CATID_PersistsToFile,                       // 0x0d
  CATID_PersistsToPropertyBag,                // 0x0e
  CATID_InternetAware,                        // 0x0f
  CATID_DesignTimeUIActivatableControl,       // 0x10
  CATID_SafeForInitializing,                  // 0x11
  CATID_SafeForScripting,                     // 0x12
  CATID_PropertyNotifyControl,                // 0x13
  CATID_SimpleFrameControl,                   // 0x14
  CATID_InfoBand,                             // 0x15
  CATID_CommBand,                             // 0x16
  CATID_DeskBand,                             // 0x17
  CATID_BrowsableShellExt,                    // 0x18
  CATID_BrowseInPlace                         // 0x19
};

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
  void Registry::RegisterCoClass(TLIBATTR* pTypeLib,LPCTSTR typelibName,
                                 REFGUID typeGUID,LPCTSTR typeName,LPCTSTR typeDesc,WORD typeVersion,
                                 LPCTSTR modulePath,
                                 ITypeInfo2& rTypeInfo2,
                                 bool isControl,
                                 LPCTSTR threadingModel,
                                 LPCTSTR dumpFileName
                                )
  { LOGGER_DEBUG<<_T("Registry::RegisterCoClass()")<<endl;
    LOGGER_DEBUG<<_T("typelibGUID=")<<pTypeLib->guid<<endl;
    LOGGER_DEBUG<<_T("typelibName=")<<typelibName<<endl;
    LOGGER_DEBUG<<_T("typeGUID   =")<<typeGUID<<endl;
    LOGGER_DEBUG<<_T("typeName   =")<<typeName<<endl;
    LOGGER_DEBUG<<_T("typeDesc   =")<<typeDesc<<endl;
    LOGGER_DEBUG<<_T("typeVersion=")<<typeVersion<<endl;
    LOGGER_DEBUG<<_T("isControl  =")<<isControl<<endl;

    VARIANT var;
    BSTR    toolboxBitmap=NULL;
    DWORD   miscStatus=0;

    if( rTypeInfo2.GetCustData(CUST_TOOLBOXBITMAP,&var)==S_OK && VT_BSTR==V_VT(&var) )
    { LOGGER_DEBUG<<_T("toolboxbitmap=")<< V_BSTR(&var)<<endl;

      toolboxBitmap = V_BSTR(&var);
    } // of if

    if( rTypeInfo2.GetCustData(CUST_MISCSTATUS,&var)==S_OK && VT_I4==V_VT(&var) )
    { LOGGER_DEBUG<<_T("miscstatus=0x")<<hex<<V_I4(&var)<<endl;

      miscStatus = V_I4(&var);
    } // of if

    basic_ostringstream<TCHAR> typeVersionStr;
    typeVersionStr<<typeVersion;

    CGUID tlibID(pTypeLib->guid);

    basic_ostringstream<TCHAR> tlibVersion;
    tlibVersion<<pTypeLib->wMajorVerNum<<_T(".")<<pTypeLib->wMinorVerNum;

    CGUID typeID(typeGUID);
    
    TString verIndepProgID(typelibName);
    verIndepProgID += _T(".");
    verIndepProgID += typeName;

    TString progID(verIndepProgID);
    progID += _T(".");
    progID += typeVersionStr.str();

    // Create VersionIndependentProgID keys.
    TString verIndepProgIdRegKeyStr(_T("HKEY_CLASSES_ROOT\\"));
    verIndepProgIdRegKeyStr += verIndepProgID;

    Registry verIndepProgIdRegKey(verIndepProgIdRegKeyStr,dumpFileName);
    verIndepProgIdRegKey.SetKeyValue(NULL,NULL,typeDesc);
    verIndepProgIdRegKey.SetKeyValue(_T("CurVer"),NULL,progID);
    verIndepProgIdRegKey.SetKeyValue(_T("CLSID"),NULL,typeID);

    // Create ProgID keys.
    TString progIdRegKeyStr(_T("HKEY_CLASSES_ROOT\\"));
    progIdRegKeyStr += progID;

    Registry progIdRegKey(progIdRegKeyStr,dumpFileName);
    progIdRegKey.SetKeyValue(NULL,NULL,typeDesc);
    progIdRegKey.SetKeyValue(_T("CLSID"),NULL,typeID);

    // Create entries under CLSID.
    TString typeRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID\\"));
    typeRegKeyStr += typeID;
    
    Registry typeRegKey(typeRegKeyStr,dumpFileName);

    typeRegKey.SetKeyValue(NULL,NULL,typeDesc);
    typeRegKey.SetKeyValue(_T("ProgID"),NULL,progID);
    typeRegKey.SetKeyValue(_T("VersionIndependentProgID"),NULL,verIndepProgID);
    typeRegKey.SetKeyValue(_T("NotInsertable"),NULL,(LPCTSTR)NULL);
    typeRegKey.SetKeyValue(_T("Programmable"),NULL,(LPCTSTR)NULL);
    typeRegKey.SetKeyValue(_T("TypeLib"),NULL,tlibID);
    typeRegKey.SetKeyValue(_T("Version"),NULL,tlibVersion.str());
    typeRegKey.SetKeyValue(_T("InprocServer32"),NULL,modulePath);
    typeRegKey.SetKeyValue(_T("InprocServer32"),_T("ThreadingModel"),threadingModel);

    TString defaultIcon(modulePath);
    defaultIcon += _T(",1");

    typeRegKey.SetKeyValue(_T("DefaultIcon"),NULL,defaultIcon);

    if( isControl ) 
    { typeRegKey.SetKeyValue(_T("Control"),NULL,(LPCTSTR)NULL);

      if( NULL!=toolboxBitmap )
      { TString tBitmap;

        if( toolboxBitmap[0]==_T('#') )
        { tBitmap += modulePath;
          tBitmap += _T(", ");
          tBitmap += (toolboxBitmap+1);
        } // of if
        else
          tBitmap += toolboxBitmap;

        typeRegKey.SetKeyValue(_T("ToolboxBitmap32"),NULL,tBitmap);
      } // of if

      if( miscStatus!=0 )
      { TString typeRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID\\"));
        typeRegKeyStr += typeID;
        typeRegKeyStr += _T("\\MiscStatus");
    
        Registry miscStatusKey(typeRegKeyStr,dumpFileName);

        miscStatusKey.SetKeyValue(_T("1"),NULL,miscStatus);
      } // of if
    } // of if

    if( NULL==dumpFileName )
    { CATID catIds[ARRAYSIZE(g_KnownCATIDs)];
      int   catIdsCount=0;

      for( int c=0;c<ARRAYSIZE(g_KnownCATIDs);c++ )
      { GUID catid = CUST_CATID;

        catid.Data4[1] = c;

        if( rTypeInfo2.GetCustData(catid,&var)==S_OK && VT_I4==V_VT(&var) && V_I4(&var)==1 )
          catIds[catIdsCount++] = g_KnownCATIDs[c];

        if( catIdsCount>0 )
        { COMPtr<ICatRegister> pCatRegister(CLSID_StdComponentCategoriesMgr,IID_ICatRegister);

          pCatRegister->RegisterClassImplCategories(typeGUID,catIdsCount,catIds);
        } // of if
      } // of for
    } // of if
  } // of Registry::RegisterCoClass()

 /**
   *
   */
  void Registry::UnregisterCoClass(TLIBATTR* pTypeLib,LPCTSTR typelibName,
                                   REFGUID typeGUID,LPCTSTR typeName,WORD typeVersion,
                                   LPCTSTR dumpFileName
                                  )
  { LOGGER_DEBUG<<_T("Registry::UnregisterCoClass()")<<endl;
    LOGGER_DEBUG<<_T("typeGUID   =")<<typeGUID<<endl;
    LOGGER_DEBUG<<_T("typeName   =")<<typeName<<endl;
    LOGGER_DEBUG<<_T("typeVersion=")<<typeVersion<<endl;

    basic_ostringstream<TCHAR> typeVersionStr;
    typeVersionStr<<typeVersion;
    
    CGUID typeID(typeGUID);

    TString verIndepProgID(typelibName);
    verIndepProgID += _T(".");
    verIndepProgID += typeName;

    TString progID(verIndepProgID);
    progID += _T(".");
    progID += typeVersionStr.str();

    TString verIndepProgIdRegKeyStr(_T("HKEY_CLASSES_ROOT\\"));
    verIndepProgIdRegKeyStr += verIndepProgID;

    RegistryKey verIndepProgIdRegKey(verIndepProgIdRegKeyStr,dumpFileName);
    verIndepProgIdRegKey.SetDumpFile(dumpFileName);
    verIndepProgIdRegKey.Delete(true);

    // Create ProgID keys.
    TString progIdRegKeyStr(_T("HKEY_CLASSES_ROOT\\"));
    progIdRegKeyStr += progID;

    RegistryKey progIdRegKey(progIdRegKeyStr);
    progIdRegKey.SetDumpFile(dumpFileName);
    progIdRegKey.Delete(true);

    // Create entries under CLSID.
    TString typeRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID\\"));
    typeRegKeyStr += typeID;

    RegistryKey typeRegKey(typeRegKeyStr);
    typeRegKey.SetDumpFile(dumpFileName);
    typeRegKey.Delete(true);
  } // of Registry::UnregisterCoClass()

  /**
   *
   */
  void Registry::RegisterInterface(REFGUID typelibGUID,WORD majorVersion,WORD minorVersion,REFGUID typeGUID,LPCTSTR typeDesc)
  { LOGGER_DEBUG<<_T("Registry::RegisterInterface()")<<endl;
    LOGGER_DEBUG<<_T("typelibGUID=")<<typelibGUID<<endl;
    LOGGER_DEBUG<<_T("typelibVer =")<<majorVersion<<_T(".")<<minorVersion<<endl;
    LOGGER_DEBUG<<_T("typeGUID   =")<<typeGUID<<endl;
    LOGGER_DEBUG<<_T("typeDesc   =")<<typeDesc<<endl;
    
    CGUID tlibID(typelibGUID);
    CGUID typeID(typeGUID);

    TString ifStr(_T("HKEY_CLASSES_ROOT\\Interface\\"));
    ifStr += typeID;

    Registry regIf(ifStr);
    regIf.SetKeyValue(NULL,NULL,typeDesc);
    regIf.SetKeyValue(_T("ProxyStubClsid32"),NULL,_T("{00020420-0000-0000-C000-000000000046}"));
    regIf.SetKeyValue(_T("TypeLib"),NULL,tlibID);

    basic_ostringstream<TCHAR> os;

    os<<majorVersion<<_T(".")<<minorVersion;

    regIf.SetKeyValue(_T("TypeLib"),_T("Version"),os.str());
  } // of Registry::RegisterInterface()
  
  /**
   *
   */
  void Registry::UnregisterInterface(REFGUID typeGUID)
  { LOGGER_DEBUG<<_T("Registry::UnregisterInterface()")<<endl;
    LOGGER_DEBUG<<_T("typeGUID   =")<<typeGUID<<endl;
    
    CGUID typeID(typeGUID);

    TString ifStr(_T("HKEY_CLASSES_ROOT\\Interface\\"));
    ifStr += typeID;

    RegistryKey regIf(ifStr);

    regIf.Delete(true);
  } // of Registry::UnregisterInterface()
  
  /**
   *
   */
  void Registry::RegisterTypeLib(REFGUID typelibGUID,LCID lcid,LPCTSTR resId,USHORT majorVersion,USHORT minorVersion,LPCTSTR modulePath,LPCTSTR helpPath)
  { ITypeLib*        pITypeLib       = NULL;
    ICreateTypeLib2* pICreateTypeLib = NULL;
    
    THROW_COMEXCEPTION( ::CreateTypeLib2(SYS_WIN32,(LPCOLESTR)modulePath,&pICreateTypeLib) );

    THROW_COMEXCEPTION( pICreateTypeLib->SetVersion(majorVersion,minorVersion) );
    THROW_COMEXCEPTION( pICreateTypeLib->SetGuid(typelibGUID) );
    THROW_COMEXCEPTION( pICreateTypeLib->SetLcid(lcid) );
    THROW_COMEXCEPTION( pICreateTypeLib->QueryInterface(IID_ITypeLib,(PPVOID)&pITypeLib) );
      
    THROW_COMEXCEPTION( ::RegisterTypeLib(pITypeLib,(LPOLESTR)modulePath,(LPOLESTR)helpPath) );

    pITypeLib->Release();
  } // of Registry::RegisterTypeLib()

  /**
   *
   */
  void Registry::RegisterComObjectsInTypeLibrary(LPCTSTR szModulePath,bool registerTypes,LPCTSTR dumpFileName)
  { TCHAR            szWindowsDir[MAX_PATH];
    TLIBATTR*        pTLibAttr = NULL;
    COMPtr<ITypeLib> pTLib;
    ITypeLib*        pTLib0=NULL;         

    ::GetSystemWindowsDirectory(szWindowsDir,sizeof(szWindowsDir)/sizeof(szWindowsDir[0]));

    THROW_COMEXCEPTION( ::LoadTypeLibEx(szModulePath,REGKIND_NONE,&pTLib0) );

    pTLib = pTLib0;

    RELEASE_INTERFACE(pTLib0);

    THROW_COMEXCEPTION( pTLib->GetLibAttr(&pTLibAttr) );

    COMString libName;
    COMString libDoc;

    THROW_COMEXCEPTION( pTLib->GetDocumentation(-1,&libName,&libDoc,NULL,NULL) );

    LOGGER_INFO<<_T("Registry::RegisterComObjectsInTypeLibrary(registerTypes=")<<registerTypes<<_T(")")<<endl;
    LOGGER_INFO<<_T("libname=")<<libName<<endl;
    LOGGER_INFO<<_T("libdoc =")<<libDoc<<endl;
    LOGGER_INFO<<_T("guid   =")<<pTLibAttr->guid<<endl;
    LOGGER_INFO<<_T("lcid   =")<<pTLibAttr->lcid<<endl;
    LOGGER_INFO<<_T("version=")<<pTLibAttr->wMajorVerNum<<_T(".")<<pTLibAttr->wMinorVerNum<<endl;

    UINT maxTypeInfo = pTLib->GetTypeInfoCount();

    for( UINT i=0;i<maxTypeInfo;i++ )
    { COMPtr<ITypeInfo>  pTypeInfo;
      COMPtr<ITypeInfo2> pTypeInfo2;
      TYPEATTR*          pTypeAttr = NULL;

      THROW_COMEXCEPTION( pTLib->GetTypeInfo(i,&pTypeInfo) );
      THROW_COMEXCEPTION( pTypeInfo->QueryInterface(IID_ITypeInfo2,(void**)&pTypeInfo2) );
      THROW_COMEXCEPTION( pTypeInfo->GetTypeAttr(&pTypeAttr) );

      if( (pTypeAttr->typekind==TKIND_COCLASS && (pTypeAttr->wTypeFlags&TYPEFLAG_FRESTRICTED)==0 ) )
      { COMString typeName;
        COMString typeDoc;

        THROW_COMEXCEPTION( pTypeInfo->GetDocumentation(MEMBERID_NIL,&typeName,&typeDoc,NULL,NULL) );

        if( registerTypes )
          RegisterCoClass(pTLibAttr,libName,
                          pTypeAttr->guid,typeName,typeDoc,pTypeAttr->wMajorVerNum,
                          szModulePath,
                          *pTypeInfo2,
                          (pTypeAttr->wTypeFlags & TYPEFLAG_FCONTROL)!=0 ? true : false,
                          _T("Apartment"),
                          dumpFileName
                         );
        else
          UnregisterCoClass(pTLibAttr,libName,pTypeAttr->guid,typeName,pTypeAttr->wMajorVerNum,dumpFileName);
      } // of if

      pTypeInfo->ReleaseTypeAttr(pTypeAttr);
    } // of for

/*
 * not necessary because ::RegisterTypeLib will register all dispinterfaces
 *
      for( UINT impl=0;impl<pTypeAttr->cImplTypes;impl++ )
      { COMPtr<ITypeInfo> pRefTypeInfo;
        HREFTYPE          pRefType=NULL;
        int               refTypeAttr = 0;
        
        THROW_COMEXCEPTION( pTypeInfo->GetRefTypeOfImplType(impl,&pRefType) );
        THROW_COMEXCEPTION( pTypeInfo->GetRefTypeInfo(pRefType,&pRefTypeInfo) );
        THROW_COMEXCEPTION( pTypeInfo->GetImplTypeFlags(impl,&refTypeAttr) );

        if( refTypeAttr & IMPLTYPEFLAG_FSOURCE )
        { TYPEATTR* pRefTypeAttr = NULL;
          COMString refTypeDoc;

          THROW_COMEXCEPTION( pRefTypeInfo->GetTypeAttr(&pRefTypeAttr) );
          THROW_COMEXCEPTION( pRefTypeInfo->GetDocumentation(MEMBERID_NIL,NULL,&refTypeDoc,NULL,NULL) )

          if( registerTypes )
            RegisterInterface(pTLibAttr->guid,pTLibAttr->wMajorVerNum,pTLibAttr->wMinorVerNum,
                              pRefTypeAttr->guid,refTypeDoc
                             );
          else
            Registry::UnregisterInterface(pRefTypeAttr->guid);
        } // of if
      } // of for
*/

    pTLib->ReleaseTLibAttr(pTLibAttr);

    if( NULL==dumpFileName )
    { if( registerTypes )
      { THROW_COMEXCEPTION( ::RegisterTypeLib(pTLib,(OLECHAR *)szModulePath,szWindowsDir) ); }
      else
      { THROW_COMEXCEPTION( ::UnRegisterTypeLib(pTLibAttr->guid,pTLibAttr->wMajorVerNum,pTLibAttr->wMinorVerNum,pTLibAttr->lcid,SYS_WIN32) ); }
    } // of if
  } // of Registry::RegisterComObjectsInTypeLibrary()


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
