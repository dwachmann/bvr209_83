/*
 * $Id$
 * 
 * Lists COM typeinfo.
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
#include "util/registry.h"
#include "util/comptr.h"
#include "util/comstring.h"
#include "util/guid.h"
#include "exception/bvr20983exception.h"
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"
#include "exception/seexception.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/versioninfo.h"

using namespace bvr20983::util;
using namespace bvr20983;
using namespace std;

/**
 *
 */
void printUsage(LPCTSTR progName)
{ LOGGER_INFO<<_T("Uaage:")<<endl;
  LOGGER_INFO<<_T("  ")<<progName<<_T(": [-c] <name of COM object>|<CLSID of COM object>")<<endl;
  LOGGER_INFO<<_T("  ")<<_T("         -c: only controls")<<endl;

  ::exit(0);
} // of printUsage()

/**
 *
 */
void listCOMObjects(bool onlyControls,LPCTSTR typeName)
{ TString typeRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID"));
  
  RegKeyEnum regKeyEnum(typeRegKeyStr,1);

  for( TString key;regKeyEnum.Next(key); )
  { RegKey        regKey(key);
    RegistryValue regValue;

    if( regKey.QueryValue(NULL,regValue) )
    { TString name;

      regValue.GetValue(name);

      if( NULL==typeName || _tcsstr(name.c_str(),typeName)!=NULL || _tcsstr(key.c_str(),typeName)!=NULL )
      { 

        LOGGER_INFO<<key<<_T(":")<<name<<endl;
      } // of if
    } // of if
  } // of for
} // of listCOMObjects()

/**
 *
 */
bool isCOMTypeRegistered(bool onlyControls,LPCTSTR typeGUID)
{ bool    result = false;
  TString typeRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID\\{"));

  typeRegKeyStr += typeGUID;
  typeRegKeyStr += _T("}");
  
  result = RegKey(typeRegKeyStr).Exists();

  return result;
} // of isCOMTypeRegistered()

/**
 *
 */
bool getRegistryTypeInfo(LPCTSTR typeGUID,LPCTSTR subKey,TString &typeInfo)
{ bool    result = false;
  TString typeRegKeyStr(_T("HKEY_CLASSES_ROOT\\CLSID\\{"));

  typeRegKeyStr += typeGUID;
  typeRegKeyStr += _T("}\\");
  typeRegKeyStr += subKey;

  RegKey        regKey(typeRegKeyStr);
  RegistryValue regValue;

  if( regKey.QueryValue(NULL,regValue) )
  { regValue.GetValue(typeInfo);

    result = true;
  } // of if

  return result;
} // of getRegistryTypeInfo()

/**
 *
 */
void loadTypeLibrary(LPCTSTR typeGUID,COMPtr<ITypeLib>& pTLib)
{ TString typeLibGUID;
  TString typeLibVersion;

  if( getRegistryTypeInfo(typeGUID,_T("TypeLib"),typeLibGUID) )
  { if( !getRegistryTypeInfo(typeGUID,_T("Version"),typeLibVersion) )
      typeLibVersion = _T("1.0");

    TString typeLibPath;
    TString typeLibRegKeyStr(_T("HKEY_CLASSES_ROOT\\TypeLib\\"));

    typeLibRegKeyStr += typeLibGUID;
    typeLibRegKeyStr += _T("\\");
    typeLibRegKeyStr += typeLibVersion;
    typeLibRegKeyStr += _T("\\0\\win32");

    RegKey        regKey(typeLibRegKeyStr);
    RegistryValue regValue;

    if( regKey.QueryValue(NULL,regValue) )
    { regValue.GetValue(typeLibPath);

      LOGGER_INFO<<_T("loading type library ")<<typeLibGUID<<_T(":")<<typeLibPath<<endl;

      THROW_COMEXCEPTION( ::LoadTypeLibEx(typeLibPath.c_str(),REGKIND_NONE,&pTLib) );
    } // of if
  } // of if
} // of loadTypeLibrary()

/**
 *
 */
static TYPEFLAGS gTypeFlags[] = 
{ TYPEFLAG_FAPPOBJECT,
  TYPEFLAG_FCANCREATE,
  TYPEFLAG_FLICENSED,
  TYPEFLAG_FPREDECLID,
  TYPEFLAG_FHIDDEN,
  TYPEFLAG_FCONTROL,
  TYPEFLAG_FDUAL,
  TYPEFLAG_FNONEXTENSIBLE,
  TYPEFLAG_FOLEAUTOMATION,
  TYPEFLAG_FRESTRICTED,
  TYPEFLAG_FAGGREGATABLE,
  TYPEFLAG_FREPLACEABLE,
  TYPEFLAG_FDISPATCHABLE,
  TYPEFLAG_FREVERSEBIND,
  TYPEFLAG_FPROXY
};

static LPCTSTR gTypeFlagsName[] =
{ _T("appobject"),
  _T("cancreate"),
  _T("licensed"),
  _T("PREDECLID"),
  _T("hidden"),
  _T("control"),
  _T("dual"),
  _T("nonextensible"),
  _T("oleautomation"),
  _T("restricted"),
  _T("aggregatable"),
  _T("replaceable"),
  _T("dispatchable"),
  _T("reversebind"),
  _T("proxy")
};

/**
 *
 */
void printTypeFlags(LPCTSTR prefix,WORD t)
{ bool hit = false;
  if( t!=0 )
    LOGGER_INFO<<prefix;

  for( UINT i=0;i<ARRAYSIZE(gTypeFlags);i++ )
    if( t&gTypeFlags[i] )
    { if( hit )
        LOGGER_INFO<<_T(",");

      hit = true;
      LOGGER_INFO<<gTypeFlagsName[i];
    } // of if

  if( t!=0 )
    LOGGER_INFO<<endl;
} // of printTypeFlags()

/**
 *
 */
static VARFLAGS gVarFlags[] = 
{ VARFLAG_FREADONLY,
  VARFLAG_FSOURCE,
  VARFLAG_FBINDABLE,
  VARFLAG_FREQUESTEDIT,
  VARFLAG_FDISPLAYBIND,
  VARFLAG_FDEFAULTBIND,
  VARFLAG_FHIDDEN,
  VARFLAG_FRESTRICTED,
  VARFLAG_FDEFAULTCOLLELEM,
  VARFLAG_FUIDEFAULT,
  VARFLAG_FNONBROWSABLE,
  VARFLAG_FREPLACEABLE,
  VARFLAG_FIMMEDIATEBIND
};

static LPCTSTR gVarFlagsName[] =
{ _T("READONLY"),
  _T("SOURCE"),
  _T("BINDABLE"),
  _T("REQUESTEDIT"),
  _T("DISPLAYBIND"),
  _T("DEFAULTBIND"),
  _T("HIDDEN"),
  _T("RESTRICTED"),
  _T("DEFAULTCOLLELEM"),
  _T("UIDEFAULT"),
  _T("NONBROWSABLE"),
  _T("REPLACEABLE"),
  _T("IMMEDIATEBIND")
};

/**
 *
 */
void printVarFlags(LPCTSTR prefix,WORD t)
{ if( t!=0 )
    LOGGER_INFO<<prefix;

  for( UINT i=0;i<ARRAYSIZE(gVarFlags);i++ )
    if( t&gVarFlags[i] )
      LOGGER_INFO<<gVarFlagsName[i]<<_T(" ");

  if( t!=0 )
    LOGGER_INFO<<endl;
} // of printVarFlags()

/**
 *
 */
static VARKIND gVarKind[] = 
{ VAR_PERINSTANCE,
  VAR_STATIC,
  VAR_CONST,
  VAR_DISPATCH
};

static LPCTSTR gVarKindName[] =
{ _T("PERINSTANCE"),
  _T("STATIC"),
  _T("CONST"),
  _T("DISPATCH")
};

/**
 *
 */
void printVarKind(WORD t)
{ for( UINT i=0;i<ARRAYSIZE(gVarKind);i++ )
    if( t==gVarKind[i] )
    { LOGGER_INFO<<gVarKindName[i];
      break;
    } // of if
} // of printVarKind()


static INVOKEKIND gInvokeKind[] =
{ INVOKE_FUNC,
  INVOKE_PROPERTYGET,
  INVOKE_PROPERTYPUT,
  INVOKE_PROPERTYPUTREF
};

static LPCTSTR gInvokeKindName[] =
{ _T(""),
  _T("propget"),
  _T("propput"),
  _T("propputref")
};

/**
 *
 */
void printInvokeKind(WORD invkind)
{ for( UINT i=0;i<ARRAYSIZE(gInvokeKind);i++ )
    if( gInvokeKind[i]==invkind )
    { if( invkind!=INVOKE_FUNC ) LOGGER_INFO<<_T(",")<<gInvokeKindName[i];
      break;
    } // of if
} // of printInvokeKind()

static int gImplTypeFlag[] =
{ IMPLTYPEFLAG_FDEFAULT,
  IMPLTYPEFLAG_FSOURCE,
  IMPLTYPEFLAG_FRESTRICTED,
  IMPLTYPEFLAG_FDEFAULTVTABLE
};

static LPCTSTR gImplTypeFlagName[] =
{ _T("default"),
  _T("source"),
  _T("restricted"),
  _T("defaultvtable")
};

/**
 *
 */
void printImplTypeFlag(int refTypeFlag)
{ bool hit = false;

  for( UINT i=0;i<ARRAYSIZE(gImplTypeFlag);i++ )
    if( gImplTypeFlag[i]&refTypeFlag )
    { if( hit )
        LOGGER_INFO<<_T(" ");

      hit = true;

      LOGGER_INFO<<gImplTypeFlagName[i];
    } // of if
} // of printImplTypeFlag()

static UINT gParamFlag[] =
{ PARAMFLAG_FIN,
  PARAMFLAG_FOUT,
  PARAMFLAG_FLCID,
  PARAMFLAG_FRETVAL,
  PARAMFLAG_FOPT,
  PARAMFLAG_FHASDEFAULT,
  PARAMFLAG_FHASCUSTDATA
};

static LPCTSTR gParamFlagName[] =
{ _T("in"),
  _T("out"),
  _T("lcid"),
  _T("retval"),
  _T("opt"),
  _T("hasdefault"),
  _T("hascustdata")
};

/**
 *
 */
void printParamFlags(WORD p)
{ bool hit=false;

  for( UINT i=0;i<ARRAYSIZE(gParamFlag);i++ )
    if( p&gParamFlag[i] )
    { if( hit )
      { LOGGER_INFO<<_T(","); }

      hit = true;

      LOGGER_INFO<<gParamFlagName[i];
    } // of if
} // of printParamFlags()

static TYPEKIND gType[] =
{ TKIND_ENUM,
  TKIND_RECORD,
  TKIND_MODULE,
  TKIND_INTERFACE,
  TKIND_DISPATCH,
  TKIND_COCLASS,
  TKIND_ALIAS,
  TKIND_UNION
};

static LPCTSTR gTypeName[] =
{ _T("enum"),
  _T("recode"),
  _T("module"),
  _T("interface"),
  _T("dispatch"),
  _T("coclass"),
  _T("typedef"),
  _T("union")
};

/**
 *
 */
void printTypeName(TYPEKIND p)
{ for( UINT i=0;i<ARRAYSIZE(gType);i++ )
    if( p==gType[i] )
      LOGGER_INFO<<gTypeName[i];
} // of printTypeName()

static UINT gVarType[] =
{ VT_NULL,
	VT_I2,
	VT_I4,
	VT_R4,
	VT_R8,
	VT_CY,
	VT_DATE,
	VT_BSTR,
	VT_DISPATCH,
	VT_ERROR,
	VT_BOOL,
	VT_VARIANT,
	VT_UNKNOWN,
	VT_DECIMAL,
	VT_I1,
	VT_UI1,
	VT_UI2,
	VT_UI4,
	VT_I8,
	VT_UI8,
	VT_INT,
	VT_UINT,
	VT_VOID,
	VT_HRESULT,
	VT_PTR,
	VT_SAFEARRAY,
	VT_CARRAY,
	VT_USERDEFINED,
	VT_LPSTR,
	VT_LPWSTR,
	VT_RECORD,
	VT_INT_PTR,
	VT_UINT_PTR,
	VT_FILETIME,
	VT_BLOB,
	VT_STREAM,
	VT_STORAGE,
	VT_STREAMED_OBJECT,
	VT_STORED_OBJECT,
	VT_BLOB_OBJECT,
	VT_CF,
	VT_CLSID,
	VT_VERSIONED_STREAM
};

static LPCTSTR gVarTypeName[] =
{ _T("NULL"),
	_T("int"),
	_T("long"),
	_T("float"),
	_T("double"),
	_T("CY"),
	_T("DATE"),
	_T("BSTR"),
	_T("DISPATCH"),
	_T("SCODE"),
	_T("VARIANT_BOOL"),
	_T("VARIANT"),
	_T("IUnknown"),
	_T("DECIMAL"),
	_T("small"),
	_T("byte"),
	_T("WORD"),
	_T("DWORD"),
	_T("__int64"),
	_T("unsigned __int64"),
	_T("int"),
	_T("UINT"),
	_T("void"),
	_T("HRESULT"),
	_T("PTR"),
	_T("SAFEARRAY"),
	_T("CARRAY"),
	_T("USERDEFINED"),
	_T("LPSTR"),
	_T("LPWSTR"),
	_T("RECORD"),
	_T("int*"),
	_T("UINT*"),
	_T("FILETIME"),
	_T("BLOB"),
	_T("STREAM"),
	_T("STORAGE"),
	_T("STREAMED_OBJECT"),
	_T("STORED_OBJECT"),
	_T("BLOB_OBJECT"),
	_T("CF"),
	_T("CLSID"),
	_T("VERSIONED_STREAM")
};

/**
 *
 */
void printVarType(ITypeInfo* pTypeInfo,const TYPEDESC& t)
{ for( UINT i=0;i<ARRAYSIZE(gVarType);i++ )
    if( t.vt==(gVarType[i]&VT_TYPEMASK) )
    { if( t.vt==VT_PTR && NULL!=t.lptdesc )
      { printVarType(pTypeInfo,*t.lptdesc);
        LOGGER_INFO<<_T("*");
      } // of if
      else if( t.vt==VT_USERDEFINED )
      { COMPtr<ITypeInfo> pRefTypeInfo;
        COMString refTypeName;

        pTypeInfo->GetRefTypeInfo(t.hreftype,&pRefTypeInfo);

        THROW_COMEXCEPTION( pRefTypeInfo->GetDocumentation(MEMBERID_NIL,&refTypeName,NULL,NULL,NULL) );

        LOGGER_INFO<<refTypeName;
      } // of else if
      else
      { if( t.vt&VT_VECTOR )
          LOGGER_INFO<<_T("VECTOR ");
      
        if( t.vt&VT_ARRAY )
          LOGGER_INFO<<_T("ARRAY ");

        if( t.vt&VT_BYREF )
          LOGGER_INFO<<_T("BYREF ");

        LOGGER_INFO<<gVarTypeName[i];
      } // of else
      break;
    } 
} // of printVarType()

/**
 *
 */
void printVarDescription(LPCTSTR prefix,UINT i,ITypeInfo* pTypeInfo,TYPEATTR* pTypeAttr)
{ VARDESC* pVarDesc;

  pTypeInfo->GetVarDesc(i,&pVarDesc);

  if( pTypeAttr->typekind!=TKIND_ENUM )
  { LOGGER_INFO<<prefix<<_T("/**")<<endl;
    LOGGER_INFO<<prefix<<_T(" * id=")<<dec<<pVarDesc->memid<<endl;
    LOGGER_INFO<<prefix<<_T(" * varkind=");
    printVarKind(pVarDesc->varkind);
    LOGGER_INFO<<endl;
    printVarFlags(prefix,pVarDesc->wVarFlags);
    LOGGER_INFO<<prefix<<_T(" */")<<endl;
  } // of if

  LOGGER_INFO<<prefix;
  printVarType(pTypeInfo,pVarDesc->elemdescVar.tdesc);

  BSTR names[40];
  UINT cNames=0;

  if( FAILED(pTypeInfo->GetNames(pVarDesc->memid,names,ARRAYSIZE(names),&cNames)) )
    LOGGER_INFO<<_T(" failed to call pTypeInfo->GetNames()")<<endl;

  if( cNames>0 )
    LOGGER_INFO<<_T(" ")<<names[0];

  if( VAR_CONST==pVarDesc->varkind )
  { LOGGER_INFO<<_T("=");

    switch(pVarDesc->elemdescVar.tdesc.vt )
    {
    case VT_I2:
      LOGGER_INFO<<_T("0x")<<hex<<V_I2(pVarDesc->lpvarValue);
      break;
    case VT_I4:
      LOGGER_INFO<<_T("0x")<<hex<<V_I4(pVarDesc->lpvarValue);
      break;
    case VT_I8:
      LOGGER_INFO<<_T("0x")<<hex<<V_I8(pVarDesc->lpvarValue);
      break;
    case VT_INT:
      LOGGER_INFO<<_T("0x")<<hex<<V_INT(pVarDesc->lpvarValue);
      break;
    default:
      LOGGER_INFO<<_T("???");
      break;
    } // of switch
  } // of if

  pTypeInfo->ReleaseVarDesc(pVarDesc);
} // of printVarDescription()


/**
 *
 */
void printFuncDescription(LPCTSTR prefix,UINT i,ITypeInfo* pTypeInfo)
{ FUNCDESC* pFuncDesc;

  pTypeInfo->GetFuncDesc(i,&pFuncDesc);

  LOGGER_INFO<<prefix<<_T("/**")<<endl;
  LOGGER_INFO<<prefix<<_T(" *    cParams=")<<dec<<pFuncDesc->cParams<<endl;
  LOGGER_INFO<<prefix<<_T(" * cParamsOpt=")<<dec<<pFuncDesc->cParamsOpt<<endl;
  LOGGER_INFO<<prefix<<_T(" *       oVft=")<<dec<<pFuncDesc->oVft<<endl;
  LOGGER_INFO<<prefix<<_T(" *    cScodes=")<<dec<<pFuncDesc->cScodes<<endl;
  LOGGER_INFO<<prefix<<_T(" * wFuncFlags=")<<dec<<pFuncDesc->wFuncFlags<<endl;
  LOGGER_INFO<<prefix<<_T(" */")<<endl;

  LOGGER_INFO<<prefix<<_T("[");
  LOGGER_INFO<<_T("id(0x")<<hex<<pFuncDesc->memid<<_T(")");
  printInvokeKind(pFuncDesc->invkind);
  LOGGER_INFO<<_T("]")<<endl;

  LOGGER_INFO<<prefix;
  printVarType(pTypeInfo,pFuncDesc->elemdescFunc.tdesc);

  BSTR names[40];
  UINT cNames=0;

  if( FAILED(pTypeInfo->GetNames(pFuncDesc->memid,names,ARRAYSIZE(names),&cNames)) )
    LOGGER_INFO<<_T(" failed to call pTypeInfo->GetNames()")<<endl;

  if( cNames>0 )
    LOGGER_INFO<<_T(" ")<<names[0];

  LOGGER_INFO<<_T("(");
  for( short k=0;k<pFuncDesc->cParams;k++ )
  { if( k>0 )
      LOGGER_INFO<<_T(",");

    if( pFuncDesc->lprgelemdescParam[k].paramdesc.wParamFlags!=0 )
    { LOGGER_INFO<<_T("[");
      printParamFlags(pFuncDesc->lprgelemdescParam[k].paramdesc.wParamFlags);
      LOGGER_INFO<<_T("]");
    } // of if

    printVarType(pTypeInfo,pFuncDesc->lprgelemdescParam[k].tdesc);

    if( (UINT)(k+1)<cNames )
      LOGGER_INFO<<_T(" ")<<names[k+1];
  } // of for

  LOGGER_INFO<<_T(")")<<endl<<endl;

  pTypeInfo->ReleaseFuncDesc(pFuncDesc);
} // of printFuncDescription()

void showTypeInfo(LPCTSTR prefix,ITypeInfo* pTypeInfo,ITypeInfo2* pTypeInfo2,bool showRefTypes);

/**
 *
 */
void printRefTypeInfo(LPCTSTR prefix,int i,ITypeInfo* pTypeInfo)
{ HREFTYPE          pRefType;
  COMPtr<ITypeInfo> pRefTypeInfo;
  TYPEATTR*         pRefTypeAttr = NULL;
  int               implTypeFlag;

  pTypeInfo->GetRefTypeOfImplType(i,&pRefType);
  pTypeInfo->GetImplTypeFlags(i,&implTypeFlag);
  pTypeInfo->GetRefTypeInfo(pRefType,&pRefTypeInfo);

  LOGGER_INFO<<prefix;
  if( implTypeFlag!=0 )
  { LOGGER_INFO<<_T("[");
    printImplTypeFlag(implTypeFlag);
    LOGGER_INFO<<_T("]");
  } // of if

  if( i==-1 )
  { TString prefix1str(prefix);

    prefix1str += _T("  ");

    showTypeInfo(prefix1str.c_str(),pRefTypeInfo,NULL,false);
  } // of if
  else
  { COMString refTypeName;

    THROW_COMEXCEPTION( pRefTypeInfo->GetDocumentation(MEMBERID_NIL,&refTypeName,NULL,NULL,NULL) );
    THROW_COMEXCEPTION( pRefTypeInfo->GetTypeAttr(&pRefTypeAttr) );

    LOGGER_INFO<<_T(" ");
    printTypeName(pRefTypeAttr->typekind);
    LOGGER_INFO<<_T(" ")<<refTypeName<<endl;

    pRefTypeInfo->ReleaseTypeAttr(pRefTypeAttr);
  } // of if
} // of printRefTypeInfo()

/**
 *
 */
void showTypeInfo(LPCTSTR prefix,ITypeInfo* pTypeInfo,ITypeInfo2* pTypeInfo2,bool showRefTypes)
{ TYPEATTR* pTypeAttr = NULL;
  TString   prefix1str(prefix);

  prefix1str += _T("  ");

  LPCTSTR   prefix1 = prefix1str.c_str();

  THROW_COMEXCEPTION( pTypeInfo->GetTypeAttr(&pTypeAttr) );

  COMString typeName;
  COMString typeDoc;

  THROW_COMEXCEPTION( pTypeInfo->GetDocumentation(MEMBERID_NIL,&typeName,&typeDoc,NULL,NULL) );

  LOGGER_INFO<<prefix<<_T("/**")<<endl;
  LOGGER_INFO<<prefix<<_T(" *     cFuncs:")<<dec<<pTypeAttr->cFuncs<<endl;
  LOGGER_INFO<<prefix<<_T(" *      cVars:")<<dec<<pTypeAttr->cVars<<endl;
  LOGGER_INFO<<prefix<<_T(" * cImplTypes:")<<dec<<pTypeAttr->cImplTypes<<endl;
  LOGGER_INFO<<prefix<<_T(" *  cbSizeVft:")<<dec<<pTypeAttr->cbSizeVft<<endl;
  LOGGER_INFO<<prefix<<_T(" */")<<endl;

  LOGGER_INFO<<prefix<<_T("[")<<endl;
  if( NULL!=typeDoc )               { LOGGER_INFO<<prefix<<_T("  helpstring(\"")<<typeDoc<<_T("\"),")<<endl; }
  if( IID_NULL!=pTypeAttr->guid )   { LOGGER_INFO<<prefix<<_T("  uuid(")<<setNoHR<<pTypeAttr->guid<<_T("),")<<endl; }
  if( pTypeAttr->lcid!=0 )          { LOGGER_INFO<<prefix<<_T("  lcid(")<<dec<<pTypeAttr->lcid<<_T("),")<<endl; }
  if( pTypeAttr->wMajorVerNum!=0 )  { LOGGER_INFO<<prefix<<_T("  version(")<<dec<<pTypeAttr->wMajorVerNum<<_T(".")<<pTypeAttr->wMinorVerNum<<_T("),")<<endl; }
  printTypeFlags(prefix1,pTypeAttr->wTypeFlags);
  LOGGER_INFO<<prefix<<_T("]")<<endl<<prefix;

  printTypeName(pTypeAttr->typekind);
  LOGGER_INFO<<_T(" ")<<typeName<<endl<<prefix<<_T("{")<<endl;

  for( UINT v=0;v<pTypeAttr->cVars;v++ )
  { if( v>0 && pTypeAttr->typekind==TKIND_ENUM )
      LOGGER_INFO<<_T(",")<<endl;

    printVarDescription(prefix1,v,pTypeInfo,pTypeAttr);
  } // of if

  for( UINT f=0;f<pTypeAttr->cFuncs;f++ )
    printFuncDescription(prefix1,f,pTypeInfo);

  for( UINT im=0;im<pTypeAttr->cImplTypes;im++ )
    printRefTypeInfo(prefix1,im,pTypeInfo);

  if( showRefTypes && pTypeAttr->wTypeFlags&TYPEFLAG_FDUAL )
    printRefTypeInfo(prefix1,-1,pTypeInfo);

  LOGGER_INFO<<endl<<prefix<<_T("}")<<endl;
  LOGGER_INFO<<endl;

  pTypeInfo->ReleaseTypeAttr(pTypeAttr);
} // of showTypeInfo()


/**
 *
 */
void showTypeLibrary(LPCTSTR typeGUID)
{ COMPtr<ITypeLib> pTLib;
  TLIBATTR*        pTLibAttr = NULL;

  loadTypeLibrary(typeGUID,pTLib);

  THROW_COMEXCEPTION( pTLib->GetLibAttr(&pTLibAttr) );

  COMString libName;
  COMString libDoc;

  THROW_COMEXCEPTION( pTLib->GetDocumentation(-1,&libName,&libDoc,NULL,NULL) );

  LOGGER_INFO<<_T("[")<<endl;
  if( NULL!=libDoc ) { LOGGER_INFO<<_T("  helpstring(\"")<<libDoc<<_T("\")")<<endl; }
  LOGGER_INFO<<_T("  uuid(")<<setNoHR<<pTLibAttr->guid<<_T(")")<<endl;
  if( pTLibAttr->lcid!=0 ) { LOGGER_INFO<<_T("  lcid(")<<pTLibAttr->lcid<<_T(")")<<endl; }
  if( pTLibAttr->wMajorVerNum!=0 ) { LOGGER_INFO<<_T("  version(")<<pTLibAttr->wMajorVerNum<<_T(".")<<pTLibAttr->wMinorVerNum<<_T(")")<<endl; }
  LOGGER_INFO<<_T("]")<<endl;
  LOGGER_INFO<<_T("library ")<<libName<<endl;
  LOGGER_INFO<<_T("{")<<endl;

  pTLib->ReleaseTLibAttr(pTLibAttr);

  UINT maxTypeInfo = pTLib->GetTypeInfoCount();

  for( UINT i=0;i<maxTypeInfo;i++ )
  { COMPtr<ITypeInfo>  pTypeInfo;
    COMPtr<ITypeInfo2> pTypeInfo2;
    TYPEATTR*          pTypeAttr = NULL;

    THROW_COMEXCEPTION( pTLib->GetTypeInfo(i,&pTypeInfo) );
    THROW_COMEXCEPTION( pTypeInfo->QueryInterface(IID_ITypeInfo2,(void**)&pTypeInfo2) );

    showTypeInfo(_T("  "),pTypeInfo,pTypeInfo2,true);
  } // of for

  LOGGER_INFO<<_T("};")<<endl;
} // of showTypeLibrary()

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

 Function : _tmain

 Synopsis : Entry point of SCQuery.

 Parameter: Standard ANSI C command line parameters.

 Return   : SCARD_S_SUCCESS or error code
------------------------------------------------------------------------------*/
extern "C" int __cdecl _tmain (int argc, _TCHAR  * argv[])
{ LogStreamT::ReadVersionInfo();

  LONG    exHr           = NOERROR;
  bool    onlyControls   = false;
  LPCTSTR comName        = NULL;

  try
  { _set_se_translator( SEException::throwException );

    { VersionInfo verInfo;
    
      verInfo.LogCopyright();
    }

    for( int i=1;i<argc;i++ )
    { if( _tcscmp(_T("-h"),argv[i])==0 || _tcscmp(_T("/?"),argv[i])==0 )
        printUsage(argv[0]);
      else if( _tcscmp(_T("-c"),argv[i])==0 )
        onlyControls = true;
      else
      { comName = argv[i];

        break;
      }
    } // of for

    if( NULL!=comName && isCOMTypeRegistered(onlyControls,comName) )
      showTypeLibrary(comName);
    else
      listCOMObjects(onlyControls,comName);
  }
  catch(BVR20983Exception& e)
  { LOGGER_ERROR<<e;
  
    exHr = e.GetErrorCode();
  }
  catch(exception& e) 
  { LOGGER_ERROR<<"Exception "<<typeid(e).name()<<":"<<e.what()<<endl;

    exHr = -4;
  }
  catch(LPCTSTR& e)
  { LOGGER_ERROR<<e<<endl;

    exHr = -3;
  }
  catch(...)
  { LOGGER_ERROR<<_T("Exception")<<endl;

    exHr = -2;
  }
  
  return (int)exHr;
} // of main
//======================================END-OF-FILE=========================
