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
#include <sstream>
#include "util/registryutil.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/comptr.h"
#include "util/comstring.h"
#include "util/apputil.h"
#include "util/guid.h"
#include "util/versioninfo.h"
#include "com/comserver.h"
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"

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
  EnumRegistration* RegistryParameter::m_pEnumRegistration = NULL;

  /**
   *
   */
  EnumRegistration* RegistryParameter::GetEnumRegistration()
  { if( NULL==m_pEnumRegistration )
      m_pEnumRegistration = new EnumRegistration(NULL,(LPARAM)new RegistryParameter(),GetValueCB);

    return m_pEnumRegistration;
  } // of RegistryParameter::GetEnumRegistration()

  /**
   *
   */
  RegistryParameter* RegistryParameter::GetInstance()
  { return (RegistryParameter*) (GetEnumRegistration()->lParam); }


  /**
   *
   */
  RegistryParameter::RegistryParameter()
  { COM::COMServer::GetModuleFileName(m_szModulePath,ARRAYSIZE(m_szModulePath));

    util::VersionInfo verInfo(m_szModulePath);

    m_prodPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ProductPrefix"));
    m_compPrefix = (LPCTSTR)verInfo.GetStringInfo(_T("ComponentPrefix"));
  } // of RegistryParameter::RegistryParameter()

  /**
   *
   */
  DWORD RegistryParameter::GetValue(HINSTANCE hDllInst,LPCTSTR key, LPTSTR value, DWORD maxValueLen)
  { DWORD result   = ULONG_MAX;
    bool  knownKey = false;
    
    if( NULL!=key && NULL!=value )
    { value[0] = _T('\0');

      if( _tcscmp(key,_T("WINDOWSDIRECTORY"))==0 || _tcscmp(key,_T("HELPDIRECTORY"))==0 )
      { ::GetSystemWindowsDirectory(value,maxValueLen);

        knownKey = true;
      } // of if
      else if( _tcscmp(key,_T("MODULEPATH"))==0 )
      { if( _tcscpy_s(value,maxValueLen,m_szModulePath)==0 )
          knownKey = true;
      } // of else if
      else if( _tcscmp(key,_T("PRODUCTPREFIX"))==0 || _tcscmp(key,_T("SERVICENAME"))==0 )
      { if( _tcscpy_s(value,maxValueLen,m_prodPrefix.c_str())==0 )
          knownKey = true;
      } // of else if
      else if( _tcscmp(key,_T("COMPONENTPREFIX"))==0 )
      { if( _tcscpy_s(value,maxValueLen,m_compPrefix.c_str())==0 )
          knownKey = true;
      } // of else if
      else if( _tcscmp(key,_T("THREADINGMODEL"))==0 )
      { if( _tcscpy_s(value,maxValueLen,_T("Apartment"))==0 )
          knownKey = true;
      } // of else if

      if( knownKey )
        result = _tcslen(value);
    } // of if

    return result;
  } // of RegistryParameter::GetValue()

  /**
   *
   */
  DWORD RegistryParameter::GetValueCB(LPARAM lParam, HINSTANCE hDllInst,LPCTSTR key, LPTSTR value, DWORD maxValueLen)
  { DWORD result = ULONG_MAX;

    if( NULL!=lParam )
      result = ((RegistryParameter*)lParam)->GetValue(hDllInst,key,value,maxValueLen);

    return result;
  } // of RegistryParameter::GetValueCB()

  /**
   *
   */
  void RegistryUtil::GetKeyPrefix(COMRegistrationType registrationType,TString& keyPrefix)
  { switch(registrationType)
    {
    case CLASSES_ROOT:
      keyPrefix = _T("HKEY_CLASSES_ROOT");
      break;
    case PER_MACHINE:
      keyPrefix = _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes");
      break;
    case PER_USER:
      keyPrefix = _T("HKEY_CURRENT_USER\\Software\\Classes");
      break;
    case AUTO:
      if( IsUserInAdministrationGroup() )
        keyPrefix = _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Classes");
      else
        keyPrefix = _T("HKEY_CURRENT_USER\\Software\\Classes");
      break;
    default:
      keyPrefix.clear();
      break;
    } // of switch
  } // of RegistryUtil::GetKeyPrefix()

  /**
   *
   */
  void RegistryUtil::RegisterComObjectsInTypeLibrary(Registry& registry,
                                                     COMRegistrationType registrationType,
                                                     EnumRegistration* pEnumRegistration
                                                    )
  { TLIBATTR*        pTLibAttr = NULL;
    COMPtr<ITypeLib> pTLib;
    ITypeLib*        pTLib0=NULL;         
    TCHAR            szModulePath[MAX_PATH];

    COM::COMServer::GetModuleFileName(szModulePath,ARRAYSIZE(szModulePath));
    THROW_COMEXCEPTION( ::LoadTypeLibEx(szModulePath,REGKIND_NONE,&pTLib0) );

    pTLib = pTLib0;

    RELEASE_INTERFACE(pTLib0);

    THROW_COMEXCEPTION( pTLib->GetLibAttr(&pTLibAttr) );

    COMString libName;
    COMString libDoc;

    THROW_COMEXCEPTION( pTLib->GetDocumentation(-1,&libName,&libDoc,NULL,NULL) );

/*
    LOGGER_INFO<<_T("Registry::RegisterComObjectsInTypeLibrary()")<<endl;
    LOGGER_INFO<<_T("libname=")<<libName<<endl;
    LOGGER_INFO<<_T("libdoc =")<<libDoc<<endl;
    LOGGER_INFO<<_T("guid   =")<<pTLibAttr->guid<<endl;
    LOGGER_INFO<<_T("lcid   =")<<pTLibAttr->lcid<<endl;
    LOGGER_INFO<<_T("version=")<<pTLibAttr->wMajorVerNum<<_T(".")<<pTLibAttr->wMinorVerNum<<endl;
*/

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

        RegisterCoClass(registry,pTLibAttr,libName,
                        pTypeAttr->guid,typeName,typeDoc,pTypeAttr->wMajorVerNum,
                        *pTypeInfo2,
                        (pTypeAttr->wTypeFlags & TYPEFLAG_FCONTROL)!=0 ? true : false,registrationType,
                        pEnumRegistration
                       );
      } // of if

      for( UINT impl=0;impl<pTypeAttr->cImplTypes;impl++ )
      { COMPtr<ITypeInfo> pRefTypeInfo;
        HREFTYPE          pRefType=NULL;
        int               refTypeAttr = 0;
        
        THROW_COMEXCEPTION( pTypeInfo->GetRefTypeOfImplType(impl,&pRefType) );
        THROW_COMEXCEPTION( pTypeInfo->GetRefTypeInfo(pRefType,&pRefTypeInfo) );
        THROW_COMEXCEPTION( pTypeInfo->GetImplTypeFlags(impl,&refTypeAttr) );

        { TYPEATTR* pRefTypeAttr = NULL;
          COMString refTypeName;
          COMString refTypeDoc;

          THROW_COMEXCEPTION( pRefTypeInfo->GetTypeAttr(&pRefTypeAttr) );
          THROW_COMEXCEPTION( pRefTypeInfo->GetDocumentation(MEMBERID_NIL,&refTypeName,&refTypeDoc,NULL,NULL) )

          if( (refTypeAttr&IMPLTYPEFLAG_FSOURCE)!=0 || (pRefTypeAttr->typekind&TKIND_DISPATCH)!=0 )
          { RegisterInterface(registry,
                              pTLibAttr->guid,pTLibAttr->wMajorVerNum,pTLibAttr->wMinorVerNum,
                              pRefTypeAttr->guid,refTypeName,refTypeDoc,registrationType,
                              pEnumRegistration
                             );
          } // of if
        }
      } // of for

      pTypeInfo->ReleaseTypeAttr(pTypeAttr);
    } // of for

    RegisterTypeLib(registry,
                    _T("TypeLib BVR20983"),
                    pTLibAttr->guid,
                    pTLibAttr->lcid,pTLibAttr->wMajorVerNum,pTLibAttr->wMinorVerNum,
                    registrationType,
                    pEnumRegistration
                   );

    pTLib->ReleaseTLibAttr(pTLibAttr);
  } // of RegistryUtil::RegisterComObjectsInTypeLibrary()


  /**
   *
   */
  void RegistryUtil::RegisterCoClass(Registry& registry,TLIBATTR* pTypeLib,LPCTSTR typelibName,
                                     REFGUID typeGUID,LPCTSTR typeName,LPCTSTR typeDesc,WORD typeVersion,
                                     ITypeInfo2& rTypeInfo2,
                                     bool isControl,
                                     COMRegistrationType registrationType,
                                     EnumRegistration* pEnumRegistration
                                    )
  { LOGGER_DEBUG<<_T("RegistryUtil::RegisterCoClass()")<<endl;
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
    TString verIndepProgIdRegKeyStr;
    GetKeyPrefix(registrationType,verIndepProgIdRegKeyStr);
    verIndepProgIdRegKeyStr += _T("\\");
    verIndepProgIdRegKeyStr += verIndepProgID;

    registry.SetKeyPrefix(verIndepProgIdRegKeyStr);
    registry.SetValue(NULL,NULL,typeDesc);
    registry.SetValue(_T("CurVer"),NULL,progID);
    registry.SetValue(_T("CLSID"),NULL,typeID);

    // Create ProgID keys.
    TString progIdRegKeyStr;
    GetKeyPrefix(registrationType,progIdRegKeyStr);
    progIdRegKeyStr += _T("\\");
    progIdRegKeyStr += progID;

    registry.SetKeyPrefix(progIdRegKeyStr);
    registry.SetValue(NULL,NULL,typeDesc);
    registry.SetValue(_T("CLSID"),NULL,typeID);

    // Create entries under CLSID.
    TString typeRegKeyStr;
    GetKeyPrefix(registrationType,typeRegKeyStr);
    typeRegKeyStr += _T("\\CLSID\\");
    typeRegKeyStr += typeID;
    
    registry.SetKeyPrefix(typeRegKeyStr);
    registry.SetValue(NULL,NULL,typeDesc);
    registry.SetValue(_T("ProgID"),NULL,progID);
    registry.SetValue(_T("VersionIndependentProgID"),NULL,verIndepProgID);
    registry.SetKey(_T("NotInsertable"));
    registry.SetKey(_T("Programmable"));
    registry.SetValue(_T("TypeLib"),NULL,tlibID);
    registry.SetValue(_T("Version"),NULL,tlibVersion.str());

    TCHAR szModulePath[MAX_PATH];
    TCHAR threadingModel[64];

    REG_QUERY_PARAMETER(pEnumRegistration,_T("MODULEPATH"),szModulePath,ARRAYSIZE(szModulePath),COM::COMServer::GetInstanceHandle());
    REG_QUERY_PARAMETER(pEnumRegistration,_T("THREADINGMODEL"),threadingModel,ARRAYSIZE(threadingModel),COM::COMServer::GetInstanceHandle());

    registry.SetValue(_T("InprocServer32"),NULL,szModulePath);
    registry.SetValue(_T("InprocServer32"),_T("ThreadingModel"),threadingModel);

    TString defaultIcon(szModulePath);
    defaultIcon += _T(",1");

    registry.SetValue(_T("DefaultIcon"),NULL,defaultIcon);

    if( isControl ) 
    { registry.SetKey(_T("Control"));

      if( NULL!=toolboxBitmap )
      { TString tBitmap(szModulePath);

        if( toolboxBitmap[0]==_T('#') )
        { tBitmap += _T(", ");
          tBitmap += (toolboxBitmap+1);
        } // of if
        else
          tBitmap += toolboxBitmap;

        registry.SetValue(_T("ToolboxBitmap32"),NULL,tBitmap);
      } // of if

      if( miscStatus!=0 )
        registry.SetValue(_T("MiscStatus\\1"),NULL,miscStatus,REG_DWORD);
    } // of if

    /**
     * use direct registry access to get information for msi
     */
    for( int c=0;c<ARRAYSIZE(g_KnownCATIDs);c++ )
    { GUID catid = CUST_CATID;

      catid.Data4[1] = c;

      if( rTypeInfo2.GetCustData(catid,&var)==S_OK && VT_I4==V_VT(&var) && V_I4(&var)==1 )
      { TString category(_T("Implemented Categories\\"));
        CGUID   catid(g_KnownCATIDs[c]);
        category += catid;

        registry.SetKey(category.c_str());
      } // of if
    } // of for

/**
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
*/
  } // of RegistryUtil::RegisterCoClass()

  /**
   *
   */
  void RegistryUtil::RegisterInterface(Registry& regIf,REFGUID typelibGUID,
                                       WORD majorVersion,WORD minorVersion,
                                       REFGUID typeGUID,LPCTSTR typeName,LPCTSTR typeDesc,
                                       COMRegistrationType registrationType,
                                       EnumRegistration* pEnumRegistration
                                      )
  { LOGGER_DEBUG<<_T("RegistryUtil::RegisterInterface()")<<endl;
    LOGGER_DEBUG<<_T("typelibGUID=")<<typelibGUID<<endl;
    LOGGER_DEBUG<<_T("typelibVer =")<<majorVersion<<_T(".")<<minorVersion<<endl;
    LOGGER_DEBUG<<_T("typeGUID   =")<<typeGUID<<endl;
    LOGGER_DEBUG<<_T("typeName   =")<<typeName<<endl;
    LOGGER_DEBUG<<_T("typeDesc   =")<<typeDesc<<endl;
    
    CGUID tlibID(typelibGUID);
    CGUID typeID(typeGUID);

    TString ifStr;
    GetKeyPrefix(registrationType,ifStr);
    ifStr += _T("\\Interface\\");
    ifStr += typeID;

    regIf.SetKeyPrefix(ifStr);

    regIf.SetValue(NULL,NULL,typeName);
    regIf.SetValue(_T("ProxyStubClsid"),NULL,_T("{00020420-0000-0000-C000-000000000046}"));
    regIf.SetValue(_T("ProxyStubClsid32"),NULL,_T("{00020420-0000-0000-C000-000000000046}"));
    regIf.SetValue(_T("TypeLib"),NULL,tlibID);

    basic_ostringstream<TCHAR> os;

    os<<majorVersion<<_T(".")<<minorVersion;

    regIf.SetValue(_T("TypeLib"),_T("Version"),os.str());
  } // of RegistryUtil::RegisterInterface()
  
  /**
   *
   */
  void RegistryUtil::RegisterTypeLib(Registry& registry,
                                     LPCTSTR typelibDesc,
                                     REFGUID typelibGUID,LCID lcid,USHORT majorVersion,USHORT minorVersion,
                                     COMRegistrationType registrationType,
                                     EnumRegistration* pEnumRegistration
                                    )
  { LOGGER_DEBUG<<_T("RegistryUtil::RegisterTypeLib()")<<endl;
    LOGGER_DEBUG<<_T("typelibGUID=")<<typelibGUID<<endl;
    LOGGER_DEBUG<<_T("typelibVer =")<<majorVersion<<_T(".")<<minorVersion<<endl;

    basic_ostringstream<TCHAR> os;
    os<<majorVersion<<_T(".")<<minorVersion;

    CGUID tlibID(typelibGUID);

    TString typelibPrefix;
    GetKeyPrefix(registrationType,typelibPrefix);
    typelibPrefix += _T("\\TypeLib\\");
    typelibPrefix += tlibID;

    registry.SetKeyPrefix(typelibPrefix);
    registry.SetValue(NULL,NULL,typelibDesc);
    registry.SetKey(TString(os.str()).c_str());

    typelibPrefix += _T("\\");
    typelibPrefix += os.str();

    registry.SetKeyPrefix(typelibPrefix);

    basic_ostringstream<TCHAR> os1;
    os1<<lcid;

    registry.SetKey(TString(os1.str()).c_str());
    registry.SetKeyPrefix(typelibPrefix);

    os1<<_T("\\win32");

    TCHAR szModulePath[MAX_PATH];
    TCHAR szHelpDirectory[MAX_PATH];

    REG_QUERY_PARAMETER(pEnumRegistration,_T("MODULEPATH"),szModulePath,ARRAYSIZE(szModulePath),COM::COMServer::GetInstanceHandle());
    REG_QUERY_PARAMETER(pEnumRegistration,_T("HELPDIRECTORY"),szHelpDirectory,ARRAYSIZE(szHelpDirectory),COM::COMServer::GetInstanceHandle());

    registry.SetValue(TString(os1.str()).c_str(),NULL,szModulePath);
    registry.SetValue(_T("FLAGS"),NULL,_T("0"));

    registry.SetValue(_T("HELPDIR"),NULL,szHelpDirectory);
  } // of RegistryUtil::RegisterTypeLib()

  /**
   *
   */
  void RegistryUtil::RegisterTypeLib(bool register4User,
                                     REFGUID typelibGUID,LCID lcid,USHORT majorVersion,USHORT minorVersion,
                                     EnumRegistration* pEnumRegistration
                                    )
  { ITypeLib*        pITypeLib       = NULL;
    ICreateTypeLib2* pICreateTypeLib = NULL;
    
    TCHAR szModulePath[MAX_PATH];
    TCHAR szHelpDirectory[MAX_PATH];

    REG_QUERY_PARAMETER(pEnumRegistration,_T("MODULEPATH"),szModulePath,ARRAYSIZE(szModulePath),COM::COMServer::GetInstanceHandle());
    REG_QUERY_PARAMETER(pEnumRegistration,_T("HELPDIRECTORY"),szHelpDirectory,ARRAYSIZE(szHelpDirectory),COM::COMServer::GetInstanceHandle());

    THROW_COMEXCEPTION( ::CreateTypeLib2(SYS_WIN32,(LPCOLESTR)szModulePath,&pICreateTypeLib) );

    THROW_COMEXCEPTION( pICreateTypeLib->SetVersion(majorVersion,minorVersion) );
    THROW_COMEXCEPTION( pICreateTypeLib->SetGuid(typelibGUID) );
    THROW_COMEXCEPTION( pICreateTypeLib->SetLcid(lcid) );
    THROW_COMEXCEPTION( pICreateTypeLib->QueryInterface(IID_ITypeLib,(PPVOID)&pITypeLib) );
      
    //if( register4User )
    //{ THROW_COMEXCEPTION( ::RegisterTypeLibForUser (pITypeLib,(LPOLESTR)szModulePath,(LPOLESTR)szHelpDirectory) ); }
    //else
    { THROW_COMEXCEPTION( ::RegisterTypeLib(pITypeLib,(LPOLESTR)szModulePath,(LPOLESTR)szHelpDirectory) ); }

    pITypeLib->Release();
  } // of RegistryUtil::RegisterTypeLib()
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
