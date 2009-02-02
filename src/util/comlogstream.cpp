/*
 * $Id$
 * 
 * iostream stream.
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
#include "util/comlogstream.h"
#include "util/apputil.h"
#include "util/guid.h"
#include "util/comptr.h"
#include "com/comutil.h"
#include "com/copropertyset.h"

namespace bvr20983
{
  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, REFIID refiid)
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);

    TCHAR szID[GUID_SIZE+1];

    ::StringFromGUID2(refiid, szID, GUID_SIZE);
    
    os<<szID;

    if( NULL!=logger && logger->IsGUID() )
    {
      os<<_T(" [");
      switch( refiid.Data1 )
      {
      case Data1_IAdviseSink              : os<<_T("IAdviseSink"); break;
      case Data1_IAdviseSink2             : os<<_T("IAdviseSink2"); break;
      case Data1_IAdviseSinkEx            : os<<_T("IAdviseSinkEx"); break;
      case Data1_IActiveScript            : os<<_T("IActiveScript"); break;
      case Data1_IBindCtx                 : os<<_T("IBindCtx"); break;
      case Data1_ICDataDoc                : os<<_T("ICDataDoc"); break;
      case Data1_IClassFactory            : os<<_T("IClassFactory"); break;
      case Data1_IClassFactory2           : os<<_T("IClassFactory2"); break;
      case Data1_IClientSecurity          : os<<_T("IClientSecurity"); break;
      case Data1_IConnectionPoint         : os<<_T("IConnectionPoint"); break;
      case Data1_IConnectionPointContainer: os<<_T("IConnectionPointContainer"); break;
      case Data1_IControl_95              : os<<_T("IControl_95"); break;
      case Data1_IControl                 : os<<_T("IControl"); break;
      case Data1_ICreateErrorInfo         : os<<_T("ICreateErrorInfo"); break;
      case Data1_ICreateTypeInfo          : os<<_T("ICreateTypeInfo"); break;
      case Data1_ICreateTypeLib           : os<<_T("ICreateTypeLib"); break;
      case Data1_IDataAdviseHolder        : os<<_T("IDataAdviseHolder"); break;
      case Data1_IDataFrame               : os<<_T("IDataFrame"); break;
      case Data1_IDataFrameExpert         : os<<_T("IDataFrameExpert"); break;
      case Data1_IDataObject              : os<<_T("IDataObject"); break;
      case Data1_IDispatch                : os<<_T("IDispatch"); break;
      case Data1_IDispatchEx              : os<<_T("IDispatchEx"); break;
      case Data1_IDropSource              : os<<_T("IDropSource"); break;
      case Data1_IDropTarget              : os<<_T("IDropTarget"); break;
      case Data1_IEnumCallback            : os<<_T("IEnumCallback"); break;
      case Data1_IEnumConnectionPoints    : os<<_T("IEnumConnectionPoints"); break;
      case Data1_IEnumConnections         : os<<_T("IEnumConnections"); break;
      case Data1_IEnumFORMATETC           : os<<_T("IEnumFORMATETC"); break;
      case Data1_IEnumGeneric             : os<<_T("IEnumGeneric"); break;
      case Data1_IEnumHolder              : os<<_T("IEnumHolder"); break;
      case Data1_IEnumMoniker             : os<<_T("IEnumMoniker"); break;
      case Data1_IEnumOLEVERB             : os<<_T("IEnumOLEVERB"); break;
      case Data1_IEnumSTATDATA            : os<<_T("IEnumSTATDATA"); break;
      case Data1_IEnumSTATSTG             : os<<_T("IEnumSTATSTG"); break;
      case Data1_IEnumString              : os<<_T("IEnumString"); break;
      case Data1_IEnumOleUndoActions      : os<<_T("IEnumOleUndoActions"); break;
      case Data1_IEnumUnknown             : os<<_T("IEnumUnknown"); break;
      case Data1_IEnumVARIANT             : os<<_T("IEnumVARIANT"); break;
      case Data1_IErrorInfo               : os<<_T("IErrorInfo"); break;
      case Data1_IExternalConnection      : os<<_T("IExternalConnection"); break;
      case Data1_IFont                    : os<<_T("IFont"); break;
      case Data1_IFontDisp                : os<<_T("IFontDisp"); break;
      case Data1_IFormExpert              : os<<_T("IFormExpert"); break;
      case Data1_IHTMLInputTextElement    : os<<_T("IHTMLInputTextElement"); break;
      case Data1_IGangConnectWithDefault  : os<<_T("IGangConnectWithDefault"); break;
      case Data1_IInternalMoniker         : os<<_T("IInternalMoniker"); break;
      case Data1_ILockBytes               : os<<_T("ILockBytes"); break;
      case Data1_IMalloc                  : os<<_T("IMalloc"); break;
      case Data1_IMarshal                 : os<<_T("IMarshal"); break;
      case Data1_IMessageFilter           : os<<_T("IMessageFilter"); break;
      case Data1_IMoniker                 : os<<_T("IMoniker"); break;
      case Data1_IMsoCommandTarget        : os<<_T("IMsoCommandTarget"); break;
      case Data1_IMsoDocument             : os<<_T("IMsoDocument"); break;
      case Data1_IMsoView                 : os<<_T("IMsoView"); break;
      case Data1_IObjectSafety            : os<<_T("IObjectSafety"); break;
      case Data1_IOleInPlaceComponent     : os<<_T("IOleInPlaceComponent"); break;
      case Data1_IOleAdviseHolder         : os<<_T("IOleAdviseHolder"); break;
      case Data1_IOleCache                : os<<_T("IOleCache"); break;
      case Data1_IOleCache2               : os<<_T("IOleCache2"); break;
      case Data1_IOleCacheControl         : os<<_T("IOleCacheControl"); break;
      case Data1_IOleClientSite           : os<<_T("IOleClientSite"); break;
      case Data1_IOleCompoundUndoAction   : os<<_T("IOleCompoundUndoAction"); break;
      case Data1_IOleContainer            : os<<_T("IOleContainer"); break;
      case Data1_IOleControl              : os<<_T("IOleControl"); break;
      case Data1_IOleControlSite          : os<<_T("IOleControlSite"); break;
      case Data1_IOleInPlaceActiveObject  : os<<_T("IOleInPlaceActiveObject"); break;
      case Data1_IOleInPlaceFrame         : os<<_T("IOleInPlaceFrame"); break;
      case Data1_IOleInPlaceObject        : os<<_T("IOleInPlaceObject"); break;
      case Data1_IOleInPlaceObjectWindowless : os<<_T("IOleInPlaceObjectWindowless"); break;
      case Data1_IOleInPlaceSite          : os<<_T("IOleInPlaceSite"); break;
      case Data1_IOleInPlaceSiteEx        : os<<_T("IOleInPlaceSiteEx"); break;
      case Data1_IOleInPlaceSiteWindowless: os<<_T("IOleInPlaceSiteWindowless"); break;
      case Data1_IOleInPlaceUIWindow      : os<<_T("IOleInPlaceUIWindow"); break;
      case Data1_IOleItemContainer        : os<<_T("IOleItemContainer"); break;
      case Data1_IOleLink                 : os<<_T("IOleLink"); break;
      case Data1_IOleManager              : os<<_T("IOleManager"); break;
      case Data1_IOleObject               : os<<_T("IOleObject"); break;
      case Data1_IOlePresObj              : os<<_T("IOlePresObj"); break;
      case Data1_IOlePropertyFrame        : os<<_T("IOlePropertyFrame"); break;
      case Data1_IOleStandardTool         : os<<_T("IOleStandardTool"); break;
      case Data1_IOleUndoAction           : os<<_T("IOleUndoAction"); break;
      case Data1_IOleUndoActionManager    : os<<_T("IOleUndoActionManager"); break;
      case Data1_IOleWindow               : os<<_T("IOleWindow"); break;
      case Data1_IPSFactory               : os<<_T("IPSFactory"); break;
      case Data1_IPSFactoryBuffer         : os<<_T("IPSFactoryBuffer"); break;
      case Data1_IParseDisplayName        : os<<_T("IParseDisplayName"); break;
      case Data1_IPerPropertyBrowsing     : os<<_T("IPerPropertyBrowsing"); break;
      case Data1_IPersist                 : os<<_T("IPersist"); break;
      case Data1_IPersistFile             : os<<_T("IPersistFile"); break;
      case Data1_IPersistPropertyBag      : os<<_T("IPersistPropertyBag"); break;
      case Data1_IPersistPropertyBag2     : os<<_T("IPersistPropertyBag2"); break;
      case Data1_IPersistStorage          : os<<_T("IPersistStorage"); break;
      case Data1_IPersistStream           : os<<_T("IPersistStream"); break;
      case Data1_IPersistStreamInit       : os<<_T("IPersistStreamInit"); break;
      case Data1_IPicture                 : os<<_T("IPicture"); break;
      case Data1_IPictureDisp             : os<<_T("IPictureDisp"); break;
      case Data1_IPointerInactive         : os<<_T("IPointerInactive"); break;
      case Data1_IPropertyNotifySink      : os<<_T("IPropertyNotifySink"); break;
      case Data1_IPropertyBag             : os<<_T("IPropertyBag"); break;
      case Data1_IPropertyBag2            : os<<_T("IPropertyBag2"); break;
      case Data1_IPropertyPage            : os<<_T("IPropertyPage"); break;
      case Data1_IPropertyPage2           : os<<_T("IPropertyPage2"); break;
      case Data1_IPropertyPage3           : os<<_T("IPropertyPage3"); break;
      case Data1_IPropertyPageInPlace     : os<<_T("IPropertyPageInPlace"); break;
      case Data1_IPropertyPageSite        : os<<_T("IPropertyPageSite"); break;
      case Data1_IPropertyPageSite2       : os<<_T("IPropertyPageSit2"); break;
      case Data1_IProvideClassInfo        : os<<_T("IProvideClassInfo"); break;
      case Data1_IProvideDynamicClassInfo : os<<_T("IProvideDynamicClassInfo"); break;
      case Data1_IProxyManager            : os<<_T("IProxyManager"); break;
      case Data1_IQuickActivate           : os<<_T("IQuickActivate"); break;
      case Data1_IRequireClasses          : os<<_T("IRequireClasses"); break;
      case Data1_IRootStorage             : os<<_T("IRootStorage"); break;
      case Data1_IRunnableObject          : os<<_T("IRunnableObject"); break;
      case Data1_IRunningObjectTable      : os<<_T("IRunningObjectTable"); break;
      case Data1_ISelectionContainer      : os<<_T("ISelectionContainer"); break;
      case Data1_IServiceProvider         : os<<_T("IServiceProvider"); break;
      case Data1_ISimpleFrameSite         : os<<_T("ISimpleFrameSite"); break;
      case Data1_ISpecifyPropertyPages    : os<<_T("ISpecifyPropertyPages"); break;
      case Data1_IStdMarshalInfo          : os<<_T("IStdMarshalInfo"); break;
      case Data1_IStorage                 : os<<_T("IStorage"); break;
      case Data1_IStream                  : os<<_T("IStream"); break;
      case Data1_ISupportErrorInfo        : os<<_T("ISupportErrorInfo"); break;
      case Data1_ITypeComp                : os<<_T("ITypeComp"); break;
      case Data1_ITypeInfo                : os<<_T("ITypeInfo"); break;
      case Data1_ITypeLib                 : os<<_T("ITypeLib"); break;
      case Data1_IUnknown                 : os<<_T("IUnknown"); break;
      case Data1_IViewObject              : os<<_T("IViewObject"); break;
      case Data1_IViewObject2             : os<<_T("IViewObject2"); break;
      case Data1_IViewObjectEx            : os<<_T("IViewObjectEx"); break;
      case Data1_IWeakRef                 : os<<_T("IWeakRef"); break;
      case Data1_ICategorizeProperties    : os<<_T("ICategorizeProperties"); break;
      default: 
        os<<_T("???"); 
        break;
      } // of switch

      os<<_T("]");

      logger->SetGUID(false);
    } // of if

    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, CLSID* pclsid)
  { if( NULL==pclsid )
      os<<_T("NULL");
    else
    { TCHAR szID[GUID_SIZE+1];

      ::StringFromGUID2(*pclsid, szID, GUID_SIZE);
      
      os<<szID;
    } // of else

    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, const COMString& s)
  { const BSTR& bs = (const BSTR&)s;
    
    os<<(bs!=NULL ? bs : _T("NULL"));

    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, const CHResult& chr)
  { LogStream<charT,traits>* logger = dynamic_cast< LogStream<charT,traits>* >(&os);

    HRESULT        hr    = chr;
    const TString& hrMsg = chr;

    os<<_T("0x")<<hex<<setw(8)<<setfill(_T('0'))<<hr;
    
    if( NULL!=logger && logger->IsHR() )
    { os<<_T("{")<<hrMsg<<_T("}");

      logger->SetHR(false);
    } // of if

    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, SIZEL* pSizeL)
  { if( NULL==pSizeL )
      os<<_T("NULL");
    else
      os<<dec<<_T("{cx=")<<pSizeL->cx<<_T(",cy=")<<pSizeL->cy<<_T("}");

    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, POINT* pPoint)
  { if( NULL==pPoint )
      os<<_T("NULL");
    else
      os<<dec<<_T("{x=")<<pPoint->x<<_T(",y=")<<pPoint->y<<_T("}");

    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, LPCRECT lpcRect)
  { if( NULL==lpcRect )
      os<<_T("NULL");
    else
      os<<dec<<_T("{left=")<<lpcRect->left<<_T(",bottom=")<<lpcRect->bottom<<_T(",right=")<<lpcRect->right<<_T(",top=")<<lpcRect->top<<_T("}");

    return os;
  }

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, DWORD* pdword)
  { if( NULL==pdword )
      os<<_T("NULL");
    else
      os<<(*pdword);

    return os;
  }

  /**
   *
   */
  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os,const VARIANT& var)
  { switch( var.vt )
    {
    case VT_EMPTY:
      os<<_T("EMPTY");
      break;
    case VT_NULL:
      os<<_T("NULL");
      break;
    case VT_I2:
      os<<_T("I2:")<<var.iVal;
      break;
    case VT_I4:
      os<<_T("I4:")<<var.lVal;
      break;
    case VT_BOOL:
      os<<_T("BOOL:")<<(var.boolVal==-1 ? _T("True") : _T("False"));
      break;
    case VT_BSTR:
      os<<_T("BSTR:")<<var.bstrVal;
      break;
    default:
      os<<_T("VARIANT.vt=")<<var.vt;
      break;
    } // of switch

    return os;
  } // of basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os,const VARIANT& var)

  /**
   *
   */
  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os,const STATPROPSTG& statpropstg)
  { os<<_T("[")<<statpropstg.propid<<_T(",");

    switch( statpropstg.vt & VT_TYPEMASK )
    {
    case VT_EMPTY:
      os<<_T("VT_EMPTY");
      break;
    case VT_NULL:
      os<<_T("VT_NULL");
      break;
    case VT_I2:
      os<<_T("VT_I2");
      break;
    case VT_I4:
      os<<_T("VT_I4");
      break;
    case VT_I8:
      os<<_T("VT_I8");
      break;
    case VT_UI2:
      os<<_T("VT_UI2");
      break;
    case VT_UI4:
      os<<_T("VT_UI4");
      break;
    case VT_UI8:
      os<<_T("VT_UI8");
      break;
    case VT_R4:
      os<<_T("VT_R4");
      break;
    case VT_R8:
      os<<_T("VT_R8");
      break;
    case VT_CY:
      os<<_T("VT_CY");
      break;
    case VT_DATE:
      os<<_T("VT_DATE");
      break;
    case VT_BSTR:
      os<<_T("VT_BSTR");
      break;
    case VT_ERROR:
      os<<_T("VT_ERROR");
      break;
    case VT_BOOL:
      os<<_T("VT_BOOL");
      break;
    case VT_VARIANT:
      os<<_T("VT_VARIANT");
      break;
    case VT_DECIMAL:
      os<<_T("VT_DECIMAL");
      break;
    case VT_I1:
      os<<_T("VT_I1");
      break;
    case VT_UI1:
      os<<_T("VT_UI1");
      break;
    case VT_INT:
      os<<_T("VT_INT");
      break;
    case VT_UINT:
      os<<_T("VT_UINT");
      break;
    case VT_VOID:
      os<<_T("VT_VOID");
      break;
    case VT_SAFEARRAY:
      os<<_T("VT_SAFEARRAY");
      break;
    case VT_USERDEFINED:
      os<<_T("VT_USERDEFINED");
      break;
    case VT_LPSTR:
      os<<_T("VT_LPSTR");
      break;
    case VT_LPWSTR:
      os<<_T("VT_LPWSTR");
      break;
    case VT_RECORD:
      os<<_T("VT_RECORD");
      break;
    case VT_FILETIME:
      os<<_T("VT_FILETIME");
      break;
    case VT_BLOB:
      os<<_T("VT_BLOB");
      break;
    case VT_STREAM:
      os<<_T("VT_STREAM");
      break;
    case VT_STORAGE:
      os<<_T("VT_STORAGE");
      break;
    case VT_STREAMED_OBJECT:
      os<<_T("VT_STREAMED_OBJECT");
      break;
    case VT_STORED_OBJECT:
      os<<_T("VT_BLOB_OBJECT");
      break;
    case VT_CF:
      os<<_T("VT_CF");
      break;
    case VT_CLSID:
      os<<_T("VT_CLSID");
      break;
    default:
      os<<_T("Unknown (")<< (statpropstg.vt & VT_TYPEMASK) <<_T(")");
      break;
    } // of switch

    if( statpropstg.vt & VT_VECTOR )
     os<<_T(" | VT_VECTOR");        

    if( statpropstg.vt & VT_ARRAY )
      os<<_T(" | VT_ARRAY");        

    if( statpropstg.vt & VT_RESERVED )
      os<<_T(" | VT_RESERVED");        

    os<<_T("]");

    return os;
  } // of basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& osconst STATPROPSTG& statpropstg)

  /**
   *
   */
  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, const PROPVARIANT& propvar)
  { switch( propvar.vt )
    {
    case VT_EMPTY:
      os<<_T("\"\"");
      break;
    case VT_NULL:
      os<<_T("NULL");
      break;
    case VT_I2:
      os<<propvar.iVal;
      break;
    case VT_I4:
    case VT_INT:
      os<<propvar.lVal;
      break;
    case VT_I8:
      os<<propvar.hVal.HighPart<<propvar.hVal.LowPart;
      break;
    case VT_UI2:
      os<<propvar.uiVal;
      break;
    case VT_UI4:
    case VT_UINT:
      os<<propvar.ulVal;
      break;
    case VT_UI8:
      os<<propvar.uhVal.HighPart<<propvar.uhVal.LowPart;
      break;
    case VT_R4:
      os<<propvar.fltVal;
      break;
    case VT_R8:
      os<<propvar.dblVal;
      break;
    case VT_BSTR:
      os<<propvar.bstrVal;
      break;
    case VT_ERROR:
      os<<hex<<_T("0x")<<setw(8)<<setfill(_T('0'))<<propvar.scode;
      break;
    case VT_BOOL:
      os<< (VARIANT_TRUE == propvar.boolVal ? _T("True") : _T("False") );
      break;
    case VT_I1:
      os<<propvar.cVal;
      break;
    case VT_UI1:
      os<<propvar.bVal;
      break;
    case VT_VOID:
      os<<_T("void");
      break;
    case VT_LPSTR:
      os<<propvar.pszVal;
      break;
    case VT_LPWSTR:
      os<<propvar.pwszVal;
      break;
    case VT_FILETIME:
      os<<propvar.filetime.dwHighDateTime<<_T(":")<<propvar.filetime.dwLowDateTime;
      break;
    case VT_CLSID:
      os<<((const TString&)CGUID(*propvar.puuid)).c_str();
      break;
    default:
      if( propvar.vt & VT_VECTOR )
      { ULONG count = 0;

        switch( propvar.vt & VT_TYPEMASK )
        {
        case VT_LPSTR:
          count = propvar.calpstr.cElems;
          for( ULONG i=0;i<count;i++ )
            os<<propvar.calpstr.pElems[i]<<_T(",");

          break;
        case VT_LPWSTR:
          count = propvar.calpwstr.cElems;
          for( ULONG i=0;i<count;i++ )
            os<<propvar.calpwstr.pElems[i]<<_T(",");

          break;
        default:
          os<<_T("VT_VECTOR");
          break;
        } // of switch
      } // of if
      else
        os<<_T("...");
      break;
    } // of switch
    
    return os;
  } // of basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& osconst STATPROPSTG& statpropstg)

  /**
   *
   */
  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, IPropertySetStorage *pPropSetStg)
  { COMPtr<IEnumSTATPROPSETSTG> penum;
    HRESULT                     hr = S_OK;
    STATPROPSETSTG              statpropsetstg;

    THROW_COMEXCEPTION( pPropSetStg->Enum( &penum ) );

    ::memset( &statpropsetstg, 0, sizeof(statpropsetstg) );

    hr = penum->Next( 1, &statpropsetstg, NULL );
    THROW_COMEXCEPTION( hr );

    while( S_OK==hr && statpropsetstg.fmtid!=GUID_NULL )
    { { auto_ptr<COM::COPropertySet> pPropertySet(new COM::COPropertySet(statpropsetstg.fmtid));

        pPropertySet->Read(pPropSetStg);

        os<<*pPropertySet;
      }

      hr = penum->Next( 1, &statpropsetstg, NULL );
      THROW_COMEXCEPTION( hr );
    } // of while

    try
    { auto_ptr<COM::COPropertySet> pPropertySet(new COM::COPropertySet(FMTID_UserDefinedProperties));

      pPropertySet->Read(pPropSetStg);

      os<<*pPropertySet;
    }
    catch(...)
    { }

    return os;
  } // of basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, IPropertySetStorage *pPropSetStg)

  /**
   *
   */
  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, IStorage *pStg)
  { COMPtr<IPropertySetStorage> pPropSetStg;
    COMPtr<IEnumSTATSTG>        penum;
    HRESULT                     hr = S_OK;
    STATSTG                     statstg;
    int                         indent;

    LogStream<TCHAR>* logger = dynamic_cast< LogStream<TCHAR>* >(&os);
    
    if( NULL!=logger )
      indent = logger->GetIndent();

    ::memset( &statstg, 0, sizeof(statstg) );

    if( SUCCEEDED(pStg->QueryInterface( IID_IPropertySetStorage,reinterpret_cast<void**>(&pPropSetStg) ) ) )
    { os<<pPropSetStg;
    } // of if
    
    THROW_COMEXCEPTION( pStg->EnumElements( NULL, NULL, NULL, &penum ) );

    hr = penum->Next( 1, &statstg, 0 );
    THROW_COMEXCEPTION( hr );

    while( S_OK == hr )
    { if( NULL!=logger )
        logger->SetIndent(indent);

      { if( STGTY_STORAGE==statstg.type )
          os<<_T("[");

        if( NULL!=statstg.pwcsName )
        { for( int i=0;statstg.pwcsName[i]!=_T('\00');i++ )
          { if( statstg.pwcsName[i]>_T('\x20') && statstg.pwcsName[i]<_T('\x7F') )
		          os<<statstg.pwcsName[i];
		        else
		          os<<_T("\\x")<<hex<<setw(2)<<setfill(_T('0'))<<(UINT)(statstg.pwcsName[i]);
		      } // of if
	      } // of if
	      
	      os<<_T(" {0x")<<hex<<statstg.cbSize.HighPart<<statstg.cbSize.LowPart<<_T('}');

        if( STGTY_STORAGE==statstg.type )
          os<<_T("]");

        if( _T('\005')==statstg.pwcsName[0] )
        { GUID fmtid;

          if( COM::COPropertySet::PropertySetNameToGuid(_tcslen(statstg.pwcsName),statstg.pwcsName,&fmtid) )
            os<<_T(" --> FMTID=")<<((const TString&)CGUID(fmtid)).c_str();
        } // of if

        os<<endl;
      } // of if

      if( STGTY_STORAGE==statstg.type && _T('\005')!=statstg.pwcsName[0] )
      { COMPtr<IStorage> pStgChild;

        THROW_COMEXCEPTION( pStg->OpenStorage( statstg.pwcsName,NULL,STGM_READ | STGM_SHARE_EXCLUSIVE,NULL, 0,&pStgChild ) );

        if( logger!=NULL )
          logger->SetIndent(indent+1);

        os<<pStgChild;
      } // of if
      
      ::CoTaskMemFree( statstg.pwcsName );
      statstg.pwcsName = NULL;
      
      hr = penum->Next( 1, &statstg, 0 );
      THROW_COMEXCEPTION( hr );
    } // of while

    os.flush();
    
    return os;
  } // of basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, IStorage *pStg)

  /**
   * Array of PIDSI's you are interested in.
   */
  struct pidsiStruct 
  { TCHAR* name;
    ULONG  pidsi;
  };

  static pidsiStruct gSummaryInfoPIDNames[] = 
  { {_T("Title"),            PIDSI_TITLE}, // VT_LPSTR
    {_T("Subject"),          PIDSI_SUBJECT}, // ...
    {_T("Author"),           PIDSI_AUTHOR},
    {_T("Keywords"),         PIDSI_KEYWORDS},
    {_T("Comments"),         PIDSI_COMMENTS},
    {_T("Template"),         PIDSI_TEMPLATE},
    {_T("LastAuthor"),       PIDSI_LASTAUTHOR},
    {_T("Revision Number"),  PIDSI_REVNUMBER},
    {_T("Edit Time"),        PIDSI_EDITTIME}, // VT_FILENAME (UTC)
    {_T("Last printed"),     PIDSI_LASTPRINTED}, // ...
    {_T("Created"),          PIDSI_CREATE_DTM},
    {_T("Last Saved"),       PIDSI_LASTSAVE_DTM},
    {_T("Page Count"),       PIDSI_PAGECOUNT}, // VT_I4
    {_T("Word Count"),       PIDSI_WORDCOUNT}, // ...
    {_T("Char Count"),       PIDSI_CHARCOUNT},
    {_T("Thumpnail"),        PIDSI_THUMBNAIL}, // VT_CF
    {_T("AppName"),          PIDSI_APPNAME}, // VT_LPSTR
    {_T("Doc Security"),     PIDSI_DOC_SECURITY}, // VT_I4
    {0, 0}
  };

  static pidsiStruct gDocSummaryInfoPIDNames[] = 
  { {_T("Category"),      PIDDSI_CATEGORY},
    {_T("Presformat"),    PIDDSI_PRESFORMAT},
    {_T("Bytes"),         PIDDSI_BYTECOUNT},
    {_T("Lines"),         PIDDSI_LINECOUNT},
    {_T("Paragraphs"),    PIDDSI_PARCOUNT},
    {_T("Slides"),        PIDDSI_SLIDECOUNT},
    {_T("Notes"),         PIDDSI_NOTECOUNT},
    {_T("Hidden"),        PIDDSI_HIDDENCOUNT},
    {_T("MMClip"),        PIDDSI_MMCLIPCOUNT},
    {_T("Scale"),         PIDDSI_SCALE},
    {_T("Heading Pair"),  PIDDSI_HEADINGPAIR},
    {_T("Doc Parts"),     PIDDSI_DOCPARTS},
    {_T("Manager"),       PIDDSI_MANAGER},
    {_T("Company"),       PIDDSI_COMPANY},
    {_T("Links Dirty"),   PIDDSI_LINKSDIRTY},
    {0, 0}
  };

  static pidsiStruct* gPIDNames[] = 
  { gSummaryInfoPIDNames,gDocSummaryInfoPIDNames };

  /**
   *
   */
  TCHAR* getPIDName(UINT index,ULONG pid)
  { for( UINT i=0;gPIDNames[index][i].name!=NULL;i++ )
      if( gPIDNames[index][i].pidsi==pid )
        return gPIDNames[index][i].name;

    return NULL;
  } // of getPIDName()
} // of namespace bvr20983

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, REFIID refiid);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, CLSID* pclsid);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, const CHResult& hr);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, DWORD* pdword);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, SIZEL* pSizeL);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, POINT* pPoint);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, LPCRECT lpcRect);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, const VARIANT& var);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, const COMString& s);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, const STATPROPSTG& statpropstg);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, const PROPVARIANT& propvar);

template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, IPropertySetStorage *);
template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator<<(basic_ostream<TCHAR,char_traits<TCHAR>>& os, IStorage *pStg);
/*==========================END-OF-FILE===================================*/

