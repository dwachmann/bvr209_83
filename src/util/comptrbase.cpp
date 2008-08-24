/*
 * $Id$
 * 
 * Implementation class for smart COM pointer.
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
#include "util/comptrbase.h"
#include "util/registry.h"
#include "util/guid.h"
#include "util/comptr.h"
#include "util/comstring.h"
#include "exception/comexception.h"

namespace bvr20983
{
  /**
   *
   */
  void COMPtrBase::GetInfo(LPCOLESTR lpszProgID,LPCOLESTR lpszIID,LPCLSID pClsId,LPIID pIID)
  { if( NULL!=pClsId )
    { THROW_COMEXCEPTION( ::CLSIDFromProgID(lpszProgID,pClsId) ); }
  
    if( NULL!=pIID )
    { CGUID clsIdGUID(*pClsId);
    
      // Create entries under CLSID.
      TString coclassRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID\\"));
      coclassRegKeyStr += clsIdGUID;
      
      RegistryValue tlibIDValue;
      RegistryValue tlibVersionValue;
      TString tlibID;
      TString tlibVersion;
      
      unsigned short wVerMajor = 1;  
      unsigned short wVerMinor = 0;
      
      Registry coclassRegKey(coclassRegKeyStr);
  
      if( coclassRegKey.QueryValue(_T("TypeLib"),NULL,tlibIDValue) &&
          coclassRegKey.QueryValue(_T("Version"),NULL,tlibVersionValue)
        )
      { tlibIDValue.GetValue(tlibID);
        tlibVersionValue.GetValue(tlibVersion);
        
        LPCTSTR period = _tcsstr(tlibVersion.c_str(),_T("."));
        TCHAR  majVer[10];
        TCHAR  minVer[10];
        
        if( NULL!=period )
        { _tcsncpy_s(majVer,ARRAYSIZE(majVer),tlibVersion.c_str(),period-tlibVersion.c_str());
          _tcscpy_s(minVer,ARRAYSIZE(minVer),period+1);
          
          wVerMajor = _tstoi(majVer);
          wVerMinor = _tstoi(minVer);
        } // of if
        
        COMPtr<ITypeLib> pTLib;
        CGUID tlibGUID(tlibID);
        
        THROW_COMEXCEPTION( ::LoadRegTypeLib(tlibGUID,wVerMajor,wVerMinor,LANG_SYSTEM_DEFAULT,&pTLib) );
        
        UINT maxTypeInfo = pTLib->GetTypeInfoCount();
    
        for( UINT i=0;i<maxTypeInfo;i++ )
        { COMPtr<ITypeInfo>  pTypeInfo;
          TYPEATTR*          pTypeAttr = NULL;
    
          THROW_COMEXCEPTION( pTLib->GetTypeInfo(i,&pTypeInfo) );
          THROW_COMEXCEPTION( pTypeInfo->GetTypeAttr(&pTypeAttr) );
    
          if( pTypeAttr->typekind==TKIND_INTERFACE || (pTypeAttr->typekind==TKIND_DISPATCH && pTypeAttr->wTypeFlags&TYPEFLAG_FDUAL) )
          { COMString typeName;
    
            THROW_COMEXCEPTION( pTypeInfo->GetDocumentation(MEMBERID_NIL,&typeName,NULL,NULL,NULL) );
            
            if( _tcscmp(typeName,lpszIID)==0 )
            { *pIID = pTypeAttr->guid;
              
              break;
            } // of if
          } // of if
        } // of for
      } // of if
    } // of if
  } // of COMPtrBase::GetInfo()


  /**
   *
   */
  IGlobalInterfaceTable* GCOMPtrBase::m_gpGIT=NULL;

  /**
   *
   */
  GCOMPtrBase::GCOMPtrBase()
  { }

  /**
   *
   */
  GCOMPtrBase::~GCOMPtrBase()
  { }

  /**
   *
   */
  void GCOMPtrBase::Init()
  { if( NULL==m_gpGIT )
      ::CoCreateInstance(CLSID_StdGlobalInterfaceTable,NULL,CLSCTX_INPROC_SERVER,IID_IGlobalInterfaceTable,(void **)&m_gpGIT);
  } // of COMPtrBase::Init()
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
