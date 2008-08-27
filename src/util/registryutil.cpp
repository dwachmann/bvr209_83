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
#include "util/registryutil.h"
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
  void RegistryUtil::RegisterCoClass(Registry& registry,TLIBATTR* pTypeLib,LPCTSTR typelibName,
                                     REFGUID typeGUID,LPCTSTR typeName,LPCTSTR typeDesc,WORD typeVersion,
                                     LPCTSTR modulePath,
                                     ITypeInfo2& rTypeInfo2,
                                     bool isControl,
                                     LPCTSTR threadingModel
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
    TString verIndepProgIdRegKeyStr(_T("HKEY_CLASSES_ROOT\\"));
    verIndepProgIdRegKeyStr += verIndepProgID;

    registry.SetKeyPrefix(verIndepProgIdRegKeyStr);
    registry.SetValue(NULL,NULL,typeDesc);
    registry.SetValue(_T("CurVer"),NULL,progID);
    registry.SetValue(_T("CLSID"),NULL,typeID);

    // Create ProgID keys.
    TString progIdRegKeyStr(_T("HKEY_CLASSES_ROOT\\"));
    progIdRegKeyStr += progID;

    registry.SetKeyPrefix(progIdRegKeyStr);
    registry.SetValue(NULL,NULL,typeDesc);
    registry.SetValue(_T("CLSID"),NULL,typeID);

    // Create entries under CLSID.
    TString typeRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID\\"));
    typeRegKeyStr += typeID;
    
    registry.SetKeyPrefix(typeRegKeyStr);
    registry.SetValue(NULL,NULL,typeDesc);
    registry.SetValue(_T("ProgID"),NULL,progID);
    registry.SetValue(_T("VersionIndependentProgID"),NULL,verIndepProgID);
    registry.SetValue(_T("NotInsertable"),NULL,(LPCTSTR)NULL);
    registry.SetValue(_T("Programmable"),NULL,(LPCTSTR)NULL);
    registry.SetValue(_T("TypeLib"),NULL,tlibID);
    registry.SetValue(_T("Version"),NULL,tlibVersion.str());
    registry.SetValue(_T("InprocServer32"),NULL,modulePath);
    registry.SetValue(_T("InprocServer32"),_T("ThreadingModel"),threadingModel);

    TString defaultIcon(modulePath);
    defaultIcon += _T(",1");

    registry.SetValue(_T("DefaultIcon"),NULL,defaultIcon);

    if( isControl ) 
    { registry.SetValue(_T("Control"),NULL,(LPCTSTR)NULL);

      if( NULL!=toolboxBitmap )
      { TString tBitmap;

        if( toolboxBitmap[0]==_T('#') )
        { tBitmap += modulePath;
          tBitmap += _T(", ");
          tBitmap += (toolboxBitmap+1);
        } // of if
        else
          tBitmap += toolboxBitmap;

        registry.SetValue(_T("ToolboxBitmap32"),NULL,tBitmap);
      } // of if

      if( miscStatus!=0 )
        registry.SetValue(_T("MiscStatus\\1"),NULL,miscStatus,REG_DWORD);
    } // of if

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
  } // of RegistryUtil::RegisterCoClass()

 /**
   *
   */
  void RegistryUtil::UnregisterCoClass(Registry& registry,TLIBATTR* pTypeLib,LPCTSTR typelibName,
                                       REFGUID typeGUID,LPCTSTR typeName,WORD typeVersion
                                      )
  { LOGGER_DEBUG<<_T("RegistryUtil::UnregisterCoClass()")<<endl;
    LOGGER_DEBUG<<_T("typeGUID   =")<<typeGUID<<endl;
    LOGGER_DEBUG<<_T("typeName   =")<<typeName<<endl;
    LOGGER_DEBUG<<_T("typeVersion=")<<typeVersion<<endl;

    registry.SetKeyPrefix(NULL);

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

    registry.DeleteKey(verIndepProgIdRegKeyStr,true);

    // Create ProgID keys.
    TString progIdRegKeyStr(_T("HKEY_CLASSES_ROOT\\"));
    progIdRegKeyStr += progID;

    registry.DeleteKey(progIdRegKeyStr,true);

    // Create entries under CLSID.
    TString typeRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID\\"));
    typeRegKeyStr += typeID;

    registry.DeleteKey(typeRegKeyStr,true);
  } // of RegistryUtil::UnregisterCoClass()

  /**
   *
   */
  void RegistryUtil::RegisterInterface(Registry& regIf,REFGUID typelibGUID,WORD majorVersion,WORD minorVersion,REFGUID typeGUID,LPCTSTR typeDesc)
  { LOGGER_DEBUG<<_T("Registry::RegisterInterface()")<<endl;
    LOGGER_DEBUG<<_T("typelibGUID=")<<typelibGUID<<endl;
    LOGGER_DEBUG<<_T("typelibVer =")<<majorVersion<<_T(".")<<minorVersion<<endl;
    LOGGER_DEBUG<<_T("typeGUID   =")<<typeGUID<<endl;
    LOGGER_DEBUG<<_T("typeDesc   =")<<typeDesc<<endl;
    
    CGUID tlibID(typelibGUID);
    CGUID typeID(typeGUID);

    TString ifStr(_T("HKEY_CLASSES_ROOT\\Interface\\"));
    ifStr += typeID;

    regIf.SetValue(NULL,NULL,typeDesc);
    regIf.SetValue(_T("ProxyStubClsid32"),NULL,_T("{00020420-0000-0000-C000-000000000046}"));
    regIf.SetValue(_T("TypeLib"),NULL,tlibID);

    basic_ostringstream<TCHAR> os;

    os<<majorVersion<<_T(".")<<minorVersion;

    regIf.SetValue(_T("TypeLib"),_T("Version"),os.str());
  } // of RegistryUtil::RegisterInterface()
  
  /**
   *
   */
  void RegistryUtil::UnregisterInterface(Registry& registry,REFGUID typeGUID)
  { LOGGER_DEBUG<<_T("Registry::UnregisterInterface()")<<endl;
    LOGGER_DEBUG<<_T("typeGUID   =")<<typeGUID<<endl;
    
    CGUID typeID(typeGUID);

    registry.SetKeyPrefix(_T("HKEY_CLASSES_ROOT\\Interface"));

    registry.DeleteKey(typeID,true);
  } // of RegistryUtil::UnregisterInterface()
  
  /**
   *
   */
  void RegistryUtil::RegisterTypeLib(Registry& regIf,REFGUID typelibGUID,LCID lcid,LPCTSTR resId,USHORT majorVersion,USHORT minorVersion,LPCTSTR modulePath,LPCTSTR helpPath)
  { ITypeLib*        pITypeLib       = NULL;
    ICreateTypeLib2* pICreateTypeLib = NULL;
    
    THROW_COMEXCEPTION( ::CreateTypeLib2(SYS_WIN32,(LPCOLESTR)modulePath,&pICreateTypeLib) );

    THROW_COMEXCEPTION( pICreateTypeLib->SetVersion(majorVersion,minorVersion) );
    THROW_COMEXCEPTION( pICreateTypeLib->SetGuid(typelibGUID) );
    THROW_COMEXCEPTION( pICreateTypeLib->SetLcid(lcid) );
    THROW_COMEXCEPTION( pICreateTypeLib->QueryInterface(IID_ITypeLib,(PPVOID)&pITypeLib) );
      
    THROW_COMEXCEPTION( ::RegisterTypeLib(pITypeLib,(LPOLESTR)modulePath,(LPOLESTR)helpPath) );

    pITypeLib->Release();
  } // of RegistryUtil::RegisterTypeLib()

  /**
   *
   */
  void RegistryUtil::RegisterComObjectsInTypeLibrary(Registry& registry,LPCTSTR szModulePath,bool registerTypes)
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
          RegisterCoClass(registry,pTLibAttr,libName,
                          pTypeAttr->guid,typeName,typeDoc,pTypeAttr->wMajorVerNum,
                          szModulePath,
                          *pTypeInfo2,
                          (pTypeAttr->wTypeFlags & TYPEFLAG_FCONTROL)!=0 ? true : false
                         );
        else
          UnregisterCoClass(registry,pTLibAttr,libName,pTypeAttr->guid,typeName,pTypeAttr->wMajorVerNum);
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

    if( registerTypes )
    { THROW_COMEXCEPTION( ::RegisterTypeLib(pTLib,(OLECHAR *)szModulePath,szWindowsDir) ); }
    else
    { THROW_COMEXCEPTION( ::UnRegisterTypeLib(pTLibAttr->guid,pTLibAttr->wMajorVerNum,pTLibAttr->wMinorVerNum,pTLibAttr->lcid,SYS_WIN32) ); }
  } // of RegistryUtil::RegisterComObjectsInTypeLibrary()
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
