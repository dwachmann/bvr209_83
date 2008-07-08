/*
 * $Id$
 * 
 * OCX Base class.
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

#include "com/cocontrol.h"
#include "com/comutil.h"
#include "com/comserver.h"
#include "com/coenum.h"
#include "exception/comexception.h"
#include "win/dc.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/comptr.h"

// this is the name of the stream we'll save our ole controls to.
//
const TCHAR szCtlSaveStream[] = _T("BVR20983COCONTROLSAVESTREAM");


#define STREAMHDR_SIGNATURE 0x12344321  // Signature to identify our format (avoid crashes!)


namespace bvr20983
{
  namespace COM
  {


#pragma region COVerbInfo
    /**
     *
     */
    void COVerbInfo::toOLEVERB(OLEVERB& v) const
    { LPOLESTR pVerbNameStr = NULL;

      if( m_verbName!=0 )
      { LPTSTR s = NULL;
      
        if( COMServer::GetInstance()->LoadString(m_verbName,&s)>0 )
          pVerbNameStr = s;
      } // of if

      v.lVerb        = m_verbID;
      v.lpszVerbName = pVerbNameStr;
      v.fuFlags      = m_flags;
      v.grfAttribs   = m_attribs;
    } // of COVerbInfo::toOLEVERB()
#pragma endregion

#pragma region Construction & Deconstruction
    /**
     *
     */
    COVerbInfo COControl::rgInPlaceVerbs[] = 
    { { OLEIVERB_SHOW,            0, 0, 0},
      { OLEIVERB_HIDE,            0, 0, 0},
      { OLEIVERB_INPLACEACTIVATE, 0, 0, 0},
      { OLEIVERB_PRIMARY,         0, 0, 0}
    };

    // NOTE: Resource ID for Properties string must be 1000
    //
    COVerbInfo COControl::ovProperties =
    { CTLIVERB_PROPERTIES, 1000, 0, OLEVERBATTRIB_ONCONTAINERMENU };

    COVerbInfo COControl::ovUIActivate =
    { OLEIVERB_UIACTIVATE, 0, 0, 0 };

    COControlClassM COControl::m_controlClass;

    const FMTID COControl::propertySetFMTID = 
    { /* 7dc6e86d-0000-1000-1xxx-425652200983 */
        0x7dc6e86d,
        0x0f0d,
        0x0000,
        {0x00,0x00,0x42, 0x56, 0x52, 0x20, 0x09, 0x83}
    };

    /**
     *
     */
    typedef struct tagSTREAMHDR 
    { DWORD  dwSignature;     // Signature.
      size_t cbWritten;       // Number of bytes written
    } STREAMHDR;

    /**
     *
     */
    COControl::COControl(LPCTSTR        controlClassName,
                         LPUNKNOWN      pUnkOuter,
                         LPUNKNOWN      pInstance,
                         REFGUID        typelibguid,
                         REFIID         dispiid,
                         REFIID         coclassiid,
                         REFIID         outgoing
                        )
    : CODispatch(pUnkOuter,pInstance,typelibguid,dispiid,coclassiid,outgoing),
      //m_cpEvents(SINK_TYPE_EVENT),
      //m_cpPropNotify(SINK_TYPE_PROPNOTIFY),
      m_myControlClass(NULL),
      m_pWnd(NULL),
      m_controlClassName(controlClassName), 
      m_propertySet(propertySetFMTID,_T("BVR20983 COM Control Propery Set"))
    { COControlClassM::const_iterator i = m_controlClass.find(controlClassName);

      if( m_controlClass.empty() || i==m_controlClass.end() )
        throw _T("unknown control class");

      m_myControlClass        = &(i->second);
      m_nFreezeEvents         = 0;
      m_pClientSite           = NULL;
      m_pControlSite          = NULL;
      m_pInPlaceSite          = NULL;
      m_pInPlaceSiteEx        = NULL;
      m_pInPlaceSiteWndless   = NULL;
      m_pInPlaceFrame         = NULL;
      m_pInPlaceUIWindow      = NULL;
  
      m_nFreezeEvents         = 0;
      m_pSimpleFrameSite      = NULL;
      m_pOleAdviseHolder      = NULL;
      m_pViewAdviseSink       = NULL;
      m_pDispAmbient          = NULL;
      m_size.cx               = 0;
      m_size.cy               = 0;
  
      m_fDirty                = false;
      m_fModeFlagValid        = false;
      m_fSaveSucceeded        = false;
      m_fViewAdvisePrimeFirst = false;
      m_fViewAdviseOnlyOnce   = false;
      m_fRunMode              = false;

      m_propertySet.AddPropertyName(_T("extendx"));
      m_propertySet.AddPropertyName(_T("extendy"));
    } // of COControl::COControl()
    
    /**
     *
     */
    COControl::~COControl()
    { DestroyWindow();
     
      RELEASE_INTERFACE(m_pClientSite);
      RELEASE_INTERFACE(m_pControlSite);
      RELEASE_INTERFACE(m_pInPlaceSite);
      RELEASE_INTERFACE(m_pInPlaceSiteEx);
      RELEASE_INTERFACE(m_pInPlaceSiteWndless);
      RELEASE_INTERFACE(m_pInPlaceFrame);
      RELEASE_INTERFACE(m_pInPlaceUIWindow);
      RELEASE_INTERFACE(m_pSimpleFrameSite);
      RELEASE_INTERFACE(m_pOleAdviseHolder);
      RELEASE_INTERFACE(m_pViewAdviseSink);
      RELEASE_INTERFACE(m_pDispAmbient);
    } // of COControl::~COControl()
#pragma endregion

#pragma region InternalQueryInterface
    /**
     *
     */
    HRESULT COControl::InternalQueryInterface(REFIID riid,PPVOID ppv)
    { HRESULT hr = S_OK;

      if( NULL==ppv )
        hr = E_POINTER;
      else
      { switch( riid.Data1 ) 
        { QI_INHERITS(this, IOleControl);
          QI_INHERITS(this, IPointerInactive);
          QI_INHERITS(this, IQuickActivate);
          QI_INHERITS(this, IOleObject);
          QI_INHERITS((IPersistStorage *)this, IPersist);
          //QI_INHERITS(this, IPersistStreamInit);
          QI_INHERITS(this, IOleInPlaceObject);
          QI_INHERITS(this, IOleInPlaceObjectWindowless);
          QI_INHERITS((IOleInPlaceActiveObject *)this, IOleWindow);
          QI_INHERITS(this, IOleInPlaceActiveObject);
          QI_INHERITS(this, IViewObject);
          QI_INHERITS(this, IViewObject2);
          QI_INHERITS(this, IViewObjectEx);
          QI_INHERITS(this, ISpecifyPropertyPages);
          QI_INHERITS(this, IPersistStorage);
          QI_INHERITS(this, IPersistPropertyBag2);
          QI_INHERITS(this, IObjectSafety);
          default:
            break;
        } // of switch
    
        if( NULL!=*ppv )
          ((LPUNKNOWN )(*ppv))->AddRef();
        else
          hr = CODispatch::InternalQueryInterface(riid,ppv);
      } // of else

      LOGGER_DEBUG<<_T("COControl::InternalQueryInterface riid=")<<setGUID<<riid<<_T(" hr=")<<setHR<<CHResult(hr)<<endl;

      return hr;
    }
#pragma endregion    

#pragma region IObjectSafety
    /**
     *
     */
    STDMETHODIMP COControl::GetInterfaceSafetyOptions(REFIID riid,DWORD* pdwSupportedOptions,DWORD* pdwEnabledOptions)
    { HRESULT hr = S_OK;

      if( NULL==pdwSupportedOptions || NULL==pdwEnabledOptions )
        hr = E_POINTER;
      else
      { *pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
        *pdwEnabledOptions   = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
      } // of else

      LOGGER_DEBUG<<_T("COControl::IObjectSafety::GetInterfaceSafetyOptions(riid=")<<riid<<_T(") hr=")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetInterfaceSafetyOptions()

    /**
     *
     */
    STDMETHODIMP COControl::SetInterfaceSafetyOptions(REFIID riid,DWORD dwOptionSetMask,DWORD dwEnabledOptions)
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IObjectSafety::SetInterfaceSafetyOptions(riid=")<<riid<<_T(",dwOptionSetMask=")<<dwOptionSetMask<<_T(",dwEnabledOptions=")<<dwEnabledOptions<<_T(") hr=")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::SetInterfaceSafetyOptions()
#pragma endregion
    
#pragma region ISpecifyPropertyPages
    /**
     *
     */
    STDMETHODIMP COControl::GetPages(CAUUID* pPages)
    { HRESULT hr = S_OK;

      if( NULL==pPages )
        hr = E_POINTER;
      else
      { pPages->pElems = NULL;
        pPages->cElems = m_myControlClass->m_propPageGuid.size();

        if( pPages->cElems>0 )
        { pPages->pElems = (GUID *)::CoTaskMemAlloc(sizeof(GUID) * (pPages->cElems));

          if( NULL==pPages->pElems ) 
            hr = E_OUTOFMEMORY;
          else
          { GUIDV::const_iterator i = m_myControlClass->m_propPageGuid.begin();
            
            for( ULONG x=0;i!=m_myControlClass->m_propPageGuid.end();i++,x++ )
              pPages->pElems[x] = *i;
          } // of else
        } // of else
      } // of else

      LOGGER_DEBUG<<_T("COControl::ISpecifyPropertyPages::GetPages():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    }
#pragma endregion    

#pragma region IOleControl
    /**
     *
     */
    STDMETHODIMP COControl::GetControlInfo(CONTROLINFO* pControlInfo)
    { HRESULT hr = S_OK;

      if( NULL==pControlInfo )
        hr = E_POINTER;
      else
      { assert(pControlInfo->cb==sizeof(CONTROLINFO));
    
        pControlInfo->hAccel = NULL;
        pControlInfo->cAccel = NULL;
      } // of else

      LOGGER_DEBUG<<_T("COControl::IOleControl::GetControlInfo():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetControlInfo()
    
    /**
     *
     */
    STDMETHODIMP COControl::OnMnemonic(LPMSG pMsg)
    { HRESULT hr = InPlaceActivate(OLEIVERB_UIACTIVATE);

      LOGGER_DEBUG<<_T("COControl::IOleControl::OnMnemonic():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::OnMnemonic()
    
    /**
     *
     */
    STDMETHODIMP COControl::OnAmbientPropertyChange(DISPID dispid)
    { HRESULT hr = S_OK;

      // if we're being told about a change in mode [design/run] then
      // remember that so our stashing of mode will update itself
      // correctly
      //
      if( dispid==DISPID_AMBIENT_USERMODE || dispid==DISPID_UNKNOWN )
        m_fModeFlagValid = false;
  
      // just pass this on to the derived control and see if they want
      // to do anything with it.
      //
      AmbientPropertyChanged(dispid);

      LOGGER_DEBUG<<_T("COControl::IOleControl::OnAmbientPropertyChange(dispid=");
      
      switch( dispid )
      {
      case DISPID_AMBIENT_BACKCOLOR:
        LOGGER_DEBUG<<_T("BACKCOLOR");
        break;
      case DISPID_AMBIENT_DISPLAYNAME:
        LOGGER_DEBUG<<_T("DISPLAYNAME");
        break;
      case DISPID_AMBIENT_FONT:
        LOGGER_DEBUG<<_T("FONT");
        break;
      case DISPID_AMBIENT_FORECOLOR:
        LOGGER_DEBUG<<_T("FORECOLOR");
        break;
      case DISPID_AMBIENT_LOCALEID:
        LOGGER_DEBUG<<_T("LOCALEID");
        break;
      case DISPID_AMBIENT_MESSAGEREFLECT:
        LOGGER_DEBUG<<_T("MESSAGEREFLECT");
        break;
      case DISPID_AMBIENT_SCALEUNITS:
        LOGGER_DEBUG<<_T("SCALEUNITS");
        break;
      case DISPID_AMBIENT_TEXTALIGN:
        LOGGER_DEBUG<<_T("TEXTALIGN");
        break;
      case DISPID_AMBIENT_USERMODE:
        LOGGER_DEBUG<<_T("USERMODE");
        break;
      case DISPID_AMBIENT_UIDEAD:
        LOGGER_DEBUG<<_T("UIDEAD");
        break;
      case DISPID_AMBIENT_SHOWGRABHANDLES:
        LOGGER_DEBUG<<_T("SHOWGRABHANDLES");
        break;
      case DISPID_AMBIENT_SHOWHATCHING:
        LOGGER_DEBUG<<_T("SHOWHATCHING");
        break;
      case DISPID_AMBIENT_DISPLAYASDEFAULT:
        LOGGER_DEBUG<<_T("DISPLAYASDEFAULT");
        break;
      case DISPID_AMBIENT_SUPPORTSMNEMONICS:
        LOGGER_DEBUG<<_T("SUPPORTSMNEMONICS");
        break;
      case DISPID_AMBIENT_AUTOCLIP:
        LOGGER_DEBUG<<_T("AUTOCLIP");
        break;
      case DISPID_AMBIENT_APPEARANCE:
        LOGGER_DEBUG<<_T("APPEARANCE");
        break;
      case DISPID_AMBIENT_CODEPAGE:
        LOGGER_DEBUG<<_T("CODEPAGE");
        break;
      case DISPID_AMBIENT_PALETTE:
        LOGGER_DEBUG<<_T("PALETTE");
        break;
      case DISPID_AMBIENT_CHARSET:
        LOGGER_DEBUG<<_T("CHARSET");
        break;
      case DISPID_AMBIENT_TRANSFERPRIORITY:
        LOGGER_DEBUG<<_T("TRANSFERPRIORITY");
        break;
      case DISPID_AMBIENT_RIGHTTOLEFT:
        LOGGER_DEBUG<<_T("RIGHTTOLEFT");
        break;
      case DISPID_AMBIENT_TOPTOBOTTOM:
        LOGGER_DEBUG<<_T("TOPTOBOTTOM");
        break;
      case DISPID_UNKNOWN:
        LOGGER_DEBUG<<_T("UNKNOWN");
        break;
      default:
        LOGGER_DEBUG<<_T("0x")<<hex<<dispid;
        break;
      } // of switch
        
      LOGGER_DEBUG<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::OnAmbientPropertyChange()
    
    /**
     *
     */
    STDMETHODIMP COControl::FreezeEvents(BOOL fFreeze)
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IOleControl::FreezeEvents(fFreeze=")<<fFreeze<<_T("):")<<setHR<<CHResult(hr)<<endl;
      
      return hr; 
    } // of COControl::FreezeEvents()
#pragma endregion

#pragma region IOleObject
    /**
     *
     */
    STDMETHODIMP COControl::SetClientSite(IOleClientSite* pClientSite)
    { HRESULT hr = S_OK;

      RELEASE_INTERFACE(m_pClientSite);
      RELEASE_INTERFACE(m_pControlSite);
      RELEASE_INTERFACE(m_pSimpleFrameSite);
  
      // store away the new client site
      //
      m_pClientSite = pClientSite;
  
      // if we've actually got one, then get some other interfaces we want to keep
      // around, and keep a handle on it
      //
      if( m_pClientSite ) 
      { m_pClientSite->AddRef();
        m_pClientSite->QueryInterface(IID_IOleControlSite, (void **)&m_pControlSite);

        if( m_myControlClass->m_oleMiscFlags & OLEMISC_SIMPLEFRAME )
          m_pClientSite->QueryInterface(IID_ISimpleFrameSite, (void **)&m_pSimpleFrameSite);
      } // of if

      LOGGER_DEBUG<<_T("COControl::IOleObject::SetClientSite():")<<setHR<<CHResult(hr)<<endl;
  
      return hr;
    } // of COControl::SetClientSite()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetClientSite(IOleClientSite** ppClientSite)
    { HRESULT hr = S_OK;

      if( NULL==ppClientSite )
        hr = E_POINTER;
      else
      { *ppClientSite = m_pClientSite;

        ADDREF_INTERFACE(*ppClientSite);
      } // of else

      LOGGER_DEBUG<<_T("COControl::IOleObject::GetClientSite():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetClientSite()
    
    /**
     *
     */
    STDMETHODIMP COControl::SetHostNames(LPCOLESTR szContainerApp,LPCOLESTR szContainerObject)
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IOleObject::SetHostNames(szContainerApp=")<<szContainerApp<<_T(",szContainerObject=")<<szContainerObject<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::SetHostNames()
    
    /**
     *
     */
    STDMETHODIMP COControl::Close(DWORD dwSaveOption)
    { HRESULT hr = S_OK;
  
      if( m_fInPlaceActive() ) 
        hr = InPlaceDeactivate();

      if( SUCCEEDED(hr) && ((dwSaveOption==OLECLOSE_SAVEIFDIRTY || dwSaveOption==OLECLOSE_PROMPTSAVE) && m_fDirty) )
      { if( m_pClientSite ) 
          m_pClientSite->SaveObject();
        
        if( m_pOleAdviseHolder ) 
          m_pOleAdviseHolder->SendOnSave();
      } // of if

      LOGGER_DEBUG<<_T("COControl::IOleObject::Close(dwSaveOption=")<<dwSaveOption<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::Close()
    
    /**
     *
     */
    STDMETHODIMP COControl::SetMoniker(DWORD dwWhichMoniker,IMoniker* pMoniker)
    { HRESULT hr = E_NOTIMPL;

      LOGGER_DEBUG<<_T("COControl::IOleObject::SetMoniker(dwWhichMoniker=")<<dwWhichMoniker<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::SetMoniker()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetMoniker(DWORD dwAssign,DWORD dwWhichMoniker,IMoniker** ppMonikerOut)
    { HRESULT hr = E_NOTIMPL;

      LOGGER_DEBUG<<_T("COControl::IOleObject::GetMoniker(dwAssign=")<<dwAssign<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::GetMoniker()
    
    /**
     *
     */
    STDMETHODIMP COControl::InitFromData(IDataObject* pDataObject,BOOL fCreation,DWORD dwReserved)
    { HRESULT hr = E_NOTIMPL;

      LOGGER_DEBUG<<_T("COControl::IOleObject::InitFromData(fCreation=")<<fCreation<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::InitFromData()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetClipboardData(DWORD dwReserved,IDataObject** ppDataObject)
    { HRESULT hr = S_OK;

      if( NULL==ppDataObject )
        hr = E_POINTER;
      else
      { *ppDataObject = NULL;
      
        hr = E_NOTIMPL;
      } // of else

      LOGGER_DEBUG<<_T("COControl::IOleObject::GetClipboardData():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetClipboardData()
    
    /**
     *
     */
    STDMETHODIMP COControl::DoVerb(LONG lVerb,LPMSG pMsg,IOleClientSite* pActiveSite,LONG lIndex,HWND hwndParent,LPCRECT prcPosRect)
    { HRESULT hr=S_OK;
    
      switch (lVerb) 
      { case OLEIVERB_SHOW:
        case OLEIVERB_INPLACEACTIVATE:
        case OLEIVERB_UIACTIVATE:
          hr = InPlaceActivate(lVerb);
          break;
        case OLEIVERB_HIDE:
          UIDeactivate();
          SetInPlaceVisible(false);
          break;
        // we used to have OLEIVERB_PRIMARY InPlaceActivate Ourselves, but it
        // turns out that the CDK and certain hosts expect this to show the
        // properties instead.  Users can change what this verb does at will.
        //
        case OLEIVERB_PRIMARY:
        case CTLIVERB_PROPERTIES:
        case OLEIVERB_PROPERTIES:
          ShowPropertyPages();
          break;
        default:
          if( lVerb>0 ) 
          { hr = DoCustomVerb(lVerb);

            if( hr==OLEOBJ_S_INVALIDVERB ) 
            { hr = InPlaceActivate(OLEIVERB_PRIMARY);

              hr = (FAILED(hr)) ? hr : OLEOBJ_S_INVALIDVERB;
            } // of if
          } // of if
          else 
            hr = E_NOTIMPL;
          break;
      } // of switch

      LOGGER_DEBUG<<_T("COControl::IOleObject::DoVerb(lVerb=");
      
      switch(lVerb)
      {
      case OLEIVERB_PRIMARY:
        LOGGER_DEBUG<<_T("PRIMARY");
        break;
      case OLEIVERB_SHOW:
        LOGGER_DEBUG<<_T("SHOW");
        break;
      case OLEIVERB_HIDE:
        LOGGER_DEBUG<<_T("HIDE");
        break;
      case OLEIVERB_INPLACEACTIVATE:
        LOGGER_DEBUG<<_T("INPLACEACTIVATE");
        break;
      case OLEIVERB_UIACTIVATE:
        LOGGER_DEBUG<<_T("UIACTIVATE");
        break;
      case OLEIVERB_PROPERTIES:
        LOGGER_DEBUG<<_T("PROPERTIES");
        break;
      default:
        LOGGER_DEBUG<<lVerb;
        break;
      }

      LOGGER_DEBUG<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::DoVerb()

    /**
     *
     */
    HRESULT COControl::ShowPropertyPages()
    { HRESULT hr = S_OK;

      // show the frame ourselves if the host can't.
      //
      if( NULL!=m_pControlSite ) 
        hr = m_pControlSite->ShowPropertyFrame();

      if( hr==E_NOTIMPL )
      { IUnknown *pUnk = (IUnknown *)(IOleObject *)this;

        CAUUID pages;

        HRESULT pHR = GetPages(&pages);

        if( SUCCEEDED(pHR) && pages.cElems>0 )
        { ModalDialog(TRUE);
          hr = ::OleCreatePropertyFrame(::GetActiveWindow(),
                                        ::GetSystemMetrics(SM_CXSCREEN) / 2,
                                        ::GetSystemMetrics(SM_CYSCREEN) / 2,
                                        m_controlClassName,
                                        1,
                                        &pUnk,
                                        pages.cElems,
                                        (LPCLSID) (pages.pElems),
                                        MAKELCID(LANG_USER_DEFAULT, SORT_DEFAULT),
                                        NULL, NULL
                                       );

          ModalDialog(FALSE);

          ::CoTaskMemFree(pages.pElems);
        } // of if
      } // of if

      return hr;
    } // of COControl::ShowPropertyPages()
    
    /**
     *
     */
    STDMETHODIMP COControl::EnumVerbs(IEnumOLEVERB **ppEnumVerbs)
    { HRESULT        hr           = S_OK;
      COEnumOLEVERB* pEnum        = NULL;
      DWORD          oleMiscFlags = m_myControlClass->m_oleMiscFlags;

      if( NULL==ppEnumVerbs )
        hr = E_POINTER;
      else
      { ppEnumVerbs = NULL;
        pEnum       = new COEnumOLEVERB(IID_IEnumOLEVERB);

        if( NULL==pEnum )
          hr = E_OUTOFMEMORY;
        else
        { OLEVERB v;

          if( !(oleMiscFlags & OLEMISC_INVISIBLEATRUNTIME) || (oleMiscFlags & OLEMISC_ACTIVATEWHENVISIBLE) )
            for( int k=0;k<ARRAYSIZE(rgInPlaceVerbs);k++ )
            { rgInPlaceVerbs[k].toOLEVERB(v);
              pEnum->Add(v);
           } // of for
          
          if( !(oleMiscFlags & OLEMISC_NOUIACTIVATE) )
          { ovUIActivate.toOLEVERB(v);
            pEnum->Add(v);
          } // of if

          if( m_myControlClass->m_propPageGuid.size()>0 )
          { ovProperties.toOLEVERB(v);
            pEnum->Add(v);
          } // of if

          if( m_myControlClass->m_customVerb.size()>0 )
          { COVerbInfoV::const_iterator i=m_myControlClass->m_customVerb.begin();
            
            for( ;i!=m_myControlClass->m_customVerb.end();i++ )
            { i->toOLEVERB(v);

              pEnum->Add(v);
            } // of for
          } // of if

          if( pEnum->size()==0 )
          { delete pEnum;
            hr = OLEOBJ_E_NOVERBS;
          } // of if
          else
            *ppEnumVerbs = pEnum;
        } // of else
      } // of else

      LOGGER_DEBUG<<_T("COControl::IOleObject::EnumVerbs():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::EnumVerbs()
    
    /**
     *
     */
    STDMETHODIMP COControl::Update()
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IOleObject::Update():")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of CODispatch::Update()
    
    /**
     *
     */
    STDMETHODIMP COControl::IsUpToDate()
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IOleObject::IsUpToDate():")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of CODispatch::IsUpToDate()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetUserClassID(CLSID* pclsid)
    { HRESULT hr = GetClassID(pclsid);

      LOGGER_DEBUG<<_T("COControl::IOleObject::GetUserClassID(pclsid=")<<pclsid<<_T("):")<<setHR<<CHResult(hr)<<endl;
    
      return hr; 
    } // of COControl::GetUserClassID()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetUserType(DWORD dwFormOfType,LPOLESTR* ppszUserType)
    { HRESULT hr = S_OK;

      if( NULL==ppszUserType )
        hr = E_POINTER;
      else
        *ppszUserType = (LPOLESTR)m_controlClassName;

      LOGGER_DEBUG<<_T("COControl::IOleObject::GetUserType(dwFormOfType=")<<dwFormOfType<<_T(",ppszUserType=")<<(*ppszUserType)<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetUserType()
    
    /**
     *
     */
    STDMETHODIMP COControl::SetExtent(DWORD dwDrawAspect,SIZEL* psizel)
    { HRESULT hr = S_OK;
  
      if( NULL==psizel )
        hr = E_POINTER;
      else if( dwDrawAspect & DVASPECT_CONTENT ) 
      { if( NULL==m_pWnd )
          Wnd::HiMetricToPixel(psizel,&m_size);
        else
        { if( !m_pWnd->SetExtent(psizel) )
            hr = E_FAIL;
        } // of else
      } // of else
      else 
        hr = DV_E_DVASPECT;

      LOGGER_DEBUG<<_T("COControl::IOleObject::SetExtent(dwDrawAspect=")<<dwDrawAspect<<_T(",psizel=")<<psizel<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::SetExtent()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetExtent(DWORD  dwDrawAspect,SIZEL* pSizeLOut)
    { HRESULT hr = S_OK;
    
      if( NULL==pSizeLOut )
        hr = E_POINTER;
      else if( dwDrawAspect & DVASPECT_CONTENT ) 
      { if( NULL==m_pWnd )
          Wnd::PixelToHiMetric(&m_size, pSizeLOut);
        else
          m_pWnd->GetExtent(pSizeLOut);
      } // of else if
      else
        hr = DV_E_DVASPECT;

      LOGGER_DEBUG<<_T("COControl::IOleObject::GetExtent(dwDrawAspect=")<<dwDrawAspect<<_T(",pSizeLOut=")<<pSizeLOut<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetExtent()
    
    /**
     *
     */
    STDMETHODIMP COControl::Advise(IAdviseSink* pAdviseSink,DWORD* pdwConnection)
    { HRESULT hr=S_OK;
  
      if( NULL==m_pOleAdviseHolder ) 
        hr = ::CreateOleAdviseHolder(&m_pOleAdviseHolder);

      if( SUCCEEDED(hr) )
        hr = m_pOleAdviseHolder->Advise(pAdviseSink, pdwConnection);

      LOGGER_DEBUG<<_T("COControl::IOleObject::Advise(pdwConnection=")<<pdwConnection<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::Advise()
    
    /**
     *
     */
    STDMETHODIMP COControl::Unadvise(DWORD dwConnection)
    { HRESULT hr = S_OK;

      if( !m_pOleAdviseHolder) 
        hr = CONNECT_E_NOCONNECTION;
      else
        hr = m_pOleAdviseHolder->Unadvise(dwConnection);

      LOGGER_DEBUG<<_T("COControl::IOleObject::Unadvise(dwConnection=")<<dwConnection<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::Unadvise()
    
    /**
     *
     */
    STDMETHODIMP COControl::EnumAdvise(IEnumSTATDATA** ppEnumOut)
    { HRESULT hr = S_OK;

      if( NULL==ppEnumOut )
        hr = E_POINTER;
      else if( NULL==m_pOleAdviseHolder ) 
      { *ppEnumOut = NULL;
        hr         = E_FAIL;
      } // of else if
      else
        hr = m_pOleAdviseHolder->EnumAdvise(ppEnumOut);

      LOGGER_DEBUG<<_T("COControl::IOleObject::EnumAdvise():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::EnumAdvise()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetMiscStatus(DWORD  dwAspect,DWORD* pdwStatus)
    { HRESULT  hr = S_OK;

      if( NULL==pdwStatus )
        hr = E_POINTER;
      else
      { *pdwStatus = NULL;

        if( dwAspect==DVASPECT_CONTENT ) 
          *pdwStatus = m_myControlClass->m_oleMiscFlags;
        else 
          hr = DV_E_DVASPECT;
      } // of else

      LOGGER_DEBUG<<_T("COControl::IOleObject::GetMiscStatus(dwAspect=")<<dwAspect<<_T(",pdwStatus=")<<pdwStatus<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetMiscStatus()
    
    /**
     *
     */
    STDMETHODIMP COControl::SetColorScheme(LOGPALETTE* pLogpal)
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IOleObject::SetColorScheme():")<<setHR<<CHResult(hr)<<endl;
      
      return hr; 
    } // of COControl::SetColorScheme()
#pragma endregion

#pragma region IOleWindow
    /**
     *
     */
    STDMETHODIMP COControl::GetWindow(HWND* pHWnd)
    { HRESULT hr = S_OK;

      if( NULL!=m_pWnd && m_pWnd->GetWindowHandle()==NULL )
        hr = E_FAIL;
      else if( NULL==pHWnd )
        hr = E_INVALIDARG;
      else
      { *pHWnd = NULL!=m_pWnd ? m_pWnd->GetWindowHandle() : NULL;
        
        if( NULL==*pHWnd )
          hr = E_UNEXPECTED;
      } // of else

      LOGGER_DEBUG<<_T("COControl::IOleWindow::GetWindow():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetWindow()
    
    /**
     *
     */
    STDMETHODIMP COControl::ContextSensitiveHelp(BOOL fEnterMode)
    { HRESULT hr = E_NOTIMPL;

      LOGGER_DEBUG<<_T("COControl::IOleWindow::ContextSensitiveHelp(fEnterMode=")<<fEnterMode<<_T("):")<<setHR<<CHResult(hr)<<endl;
      
      return hr; 
    } // of COControl::ContextSensitiveHelp()
#pragma endregion

#pragma region IOleInPlaceObject
    /**
     *
     */
    void COControl::QueryInPlaceSite() 
    { RELEASE_INTERFACE(m_pInPlaceSite);
      RELEASE_INTERFACE(m_pInPlaceSiteEx);
      RELEASE_INTERFACE(m_pInPlaceSiteWndless);

      if( NULL!=m_pClientSite )
      { if( m_myControlClass->m_isWindowless )
          m_pClientSite->QueryInterface(IID_IOleInPlaceSiteWindowless, (void **)&m_pInPlaceSiteWndless);

        m_pClientSite->QueryInterface(IID_IOleInPlaceSiteEx, (void **)&m_pInPlaceSiteEx);
        m_pClientSite->QueryInterface(IID_IOleInPlaceSite, (void **)&m_pInPlaceSite);
      } // of if
    } // of COControl::QueryInPlaceSite()

    /**
     *
     */
    HRESULT COControl::InPlaceActivate(LONG lVerb)
    { HRESULT hr         = S_OK;
      HWND    hWndParent = NULL;

      try
      { if( !m_fInPlaceActive() )
        { QueryInPlaceSite();

          if( NULL!=m_pInPlaceSite && SUCCEEDED(m_pInPlaceSite->CanInPlaceActivate()) )
          { RECT                rcPos, rcClip;
            OLEINPLACEFRAMEINFO InPlaceFrameInfo;

            InPlaceFrameInfo.cb = sizeof(OLEINPLACEFRAMEINFO);

            THROW_COMEXCEPTION( m_pInPlaceSite->GetWindow(&hWndParent) );
            THROW_COMEXCEPTION( m_pInPlaceSite->GetWindowContext(&m_pInPlaceFrame, &m_pInPlaceUIWindow, &rcPos, &rcClip, &InPlaceFrameInfo) );

            if( !CreateInPlaceWindow(hWndParent,rcPos) ) 
              THROW_COMEXCEPTION(E_FAIL);
          } // of if

          SetInPlaceVisible(true);
        } // of if
      
        // if we're not already UI active, do so now.
        //
        if( (lVerb==OLEIVERB_PRIMARY || lVerb==OLEIVERB_UIACTIVATE) && !m_fUIActive() ) 
        { // take the focus  [which is what UI Activation is all about !]
          //
          m_pWnd->SetFocus(true);
  
          THROW_COMEXCEPTION( m_pInPlaceFrame->SetActiveObject((IOleInPlaceActiveObject *)this, NULL) );

          CALLCOMEX_2(m_pInPlaceUIWindow,SetActiveObject,(IOleInPlaceActiveObject *)this, NULL);

          HRESULT hr1 = m_pInPlaceFrame->SetBorderSpace(NULL);

          if( hr1!=INPLACE_E_NOTOOLSPACE )
            THROW_COMEXCEPTION(hr1);

          if( NULL!=m_pInPlaceUIWindow )
          { hr1 = m_pInPlaceUIWindow->SetBorderSpace(NULL);

            if( hr1!=INPLACE_E_NOTOOLSPACE )
              THROW_COMEXCEPTION(hr1);
          } // of if
        } // of if
      }
      catch(const COMException& comex)
      { DestroyWindow();
        
        hr = comex.GetHResult(); 
      }

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceObject::InPlaceActivate(lVerb=");

      switch(lVerb)
      {
      case OLEIVERB_PRIMARY:
        LOGGER_DEBUG<<_T("PRIMARY");
        break;
      case OLEIVERB_SHOW:
        LOGGER_DEBUG<<_T("SHOW");
        break;
      case OLEIVERB_HIDE:
        LOGGER_DEBUG<<_T("HIDE");
        break;
      case OLEIVERB_INPLACEACTIVATE:
        LOGGER_DEBUG<<_T("INPLACEACTIVATE");
        break;
      case OLEIVERB_UIACTIVATE:
        LOGGER_DEBUG<<_T("UIACTIVATE");
        break;
      case OLEIVERB_PROPERTIES:
        LOGGER_DEBUG<<_T("PROPERTIES");
        break;
      default:
        LOGGER_DEBUG<<lVerb;
        break;
      }

      LOGGER_DEBUG<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::InPlaceActivate(LONG lVerb)
    
    /**
     *
     */
    STDMETHODIMP COControl::InPlaceDeactivate()
    { HRESULT hr = S_OK;

      if( m_fInPlaceActive() )
      { UIDeactivate();
    
        DestroyWindow();

        assert( NULL!=m_pInPlaceSite );
        m_pInPlaceSite->OnInPlaceDeactivate();

        RELEASE_INTERFACE(m_pInPlaceFrame);
        RELEASE_INTERFACE(m_pInPlaceUIWindow);
        RELEASE_INTERFACE(m_pInPlaceSite);
        RELEASE_INTERFACE(m_pInPlaceSiteEx);
        RELEASE_INTERFACE(m_pInPlaceSiteWndless);
      } // of if

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceObject::InPlaceDeactivate():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::InPlaceDeactivate()

    /**
     *
     */
    STDMETHODIMP COControl::UIDeactivate()
    { HRESULT hr = S_OK;

      if( m_fUIActive() )
      { CALLCOM_2(m_pInPlaceUIWindow,SetActiveObject,NULL,NULL);

        assert( NULL!=m_pInPlaceFrame );
        m_pInPlaceFrame->SetActiveObject(NULL, NULL);
    
        assert( NULL!=m_pInPlaceSite );
        m_pInPlaceSite->OnUIDeactivate(FALSE);
      } // of if

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceObject::UIDeactivate():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::UIDeactivate()

    /**
     *
     */
    void COControl::DestroyWindow()
    { if( NULL!=m_pWnd )
      { m_pWnd->DestroyWindow(); 
        m_pWnd = NULL;
      } // of if
    } // of COControl::DestroyWindow()

    /**
     *
     */
    STDMETHODIMP COControl::SetObjectRects(LPCRECT prcPos,LPCRECT prcClip)
    { HRESULT hr = S_OK;

      if( NULL!=m_pWnd && !m_pWnd->SetObjectRects(prcPos,prcClip) )
        hr = E_FAIL;
  
      LOGGER_DEBUG<<_T("COControl::IOleInPlaceObject::SetObjectRects(prcPos=")<<prcPos<<_T(",prcClip=")<<prcClip<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // COControl::SetObjectRects()
    
    /**
     *
     */
    STDMETHODIMP COControl::ReactivateAndUndo()
    { HRESULT hr = E_NOTIMPL;

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceObject::ReactivateAndUndo():")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::ReactivateAndUndo()
#pragma endregion

#pragma region IOleInPlaceObjectWindowless
    /**
     * 
     */
    STDMETHODIMP COControl::OnWindowMessage(UINT msg,WPARAM wParam,LPARAM lParam,LRESULT* plResult)
    { HRESULT hr = S_OK;

      switch( msg ) 
      {
      case WM_KILLFOCUS:
      case WM_SETFOCUS:
        if( m_fInPlaceActive() && NULL!=m_pControlSite )
          m_pControlSite->OnFocus(msg == WM_SETFOCUS);
        break;
      } // of switch
  
      if( NULL!=m_pWnd )
        *plResult = m_pWnd->WindowProcedure(msg, wParam, lParam);

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceObjectWindowless::OnWindowMessage(msg=")<<msg<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::OnWindowMessage()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetDropTarget(IDropTarget** ppDropTarget)
    { HRESULT hr = E_NOTIMPL;

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceObjectWindowless::GetDropTarget():")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::GetDropTarget()
#pragma endregion

#pragma region IOleInPlaceActiveObject
    /**
     *
     */
    STDMETHODIMP COControl::TranslateAccelerator(LPMSG pMsg)
    { HRESULT hr = S_OK;

      if( !OnSpecialKey(pMsg) )
      { if( NULL!=m_pControlSite )
          hr = m_pControlSite->TranslateAccelerator(pMsg, Wnd::SpecialKeyState());
        else
          hr = S_FALSE;
      } // of if

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceActiveObject::TranslateAccelerator():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::TranslateAccelerator()
    
    /**
     *
     */
    STDMETHODIMP COControl::OnFrameWindowActivate(BOOL fActivate)
    { HRESULT hr = InPlaceActivate(OLEIVERB_UIACTIVATE); 

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceActiveObject::OnFrameWindowActivate():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::OnFrameWindowActivate()
    
    /**
     *
     */
    STDMETHODIMP COControl::OnDocWindowActivate(BOOL fActivate)
    { HRESULT hr = InPlaceActivate(OLEIVERB_UIACTIVATE); 

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceActiveObject::OnDocWindowActivate(fActivate=")<<fActivate<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::OnDocWindowActivate()
    
    /**
     * 
     */
    STDMETHODIMP COControl::ResizeBorder(LPCRECT prcBorder,IOleInPlaceUIWindow* pInPlaceUIWindow,BOOL fFrame)
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceActiveObject::ResizeBorder(prcBorder=")<<prcBorder<<_T(",fFrame=")<<fFrame<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::ResizeBorder()
    
    /**
     * 
     */
    STDMETHODIMP COControl::EnableModeless(BOOL fEnable)
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IOleInPlaceActiveObject::EnableModeless(fEnable=")<<fEnable<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::EnableModeless()
#pragma endregion

#pragma region IViewObjectEx

    static int gDrawCount = 0;

    /**
     *
     */
    STDMETHODIMP COControl::Draw(DWORD            dwDrawAspect,
                                 LONG             lIndex,
                                 void*            pvAspect,
                                 DVTARGETDEVICE*  ptd,
                                 HDC              hicTargetDevice,
                                 HDC              hdcDraw,
                                 LPCRECTL         prcBounds,
                                 LPCRECTL         prcWBounds,
                                 BOOL ( STDMETHODCALLTYPE *pfnContinue) (DWORD dwContinue),
                                 DWORD            dwContinue
                                )
    { HRESULT hr = S_OK;
      RECTL   rc;
      POINT   pVp;
      POINT   pW;
      BOOL    fOptimize = false;
      int     iMode;
      bool    fMetafile = false;

      if( dwDrawAspect!=DVASPECT_CONTENT && dwDrawAspect!=DVASPECT_OPAQUE && dwDrawAspect!=DVASPECT_TRANSPARENT )
        hr = DV_E_DVASPECT;
      else
      { DCBase& icTargetDev = DCBase(hicTargetDevice);

        if( ::GetDeviceCaps(hdcDraw, TECHNOLOGY)==DT_METAFILE ) 
        { fMetafile = TRUE;
  
          if( NULL==hicTargetDevice ) 
            icTargetDev = COMDC(ptd);
        } // of if
    
        if( pvAspect && ((DVASPECTINFO *)pvAspect)->cb==sizeof(DVASPECTINFO) )
          fOptimize = (((DVASPECTINFO *)pvAspect)->dwFlags & DVASPECTINFOFLAG_CANOPTIMIZE) ? true : false;
    
        if( m_fInPlaceActive() ) 
          ::memcpy(&rc, (NULL!=prcBounds ) ? prcBounds : (LPRECTL)&(m_pWnd->GetLocation()) , sizeof(rc));
        else 
        { rc = *prcBounds;
  
          if( !fMetafile ) 
          { ::LPtoDP(hdcDraw, (POINT *)&rc, 2);
            ::SetViewportOrgEx(hdcDraw, 0, 0, &pVp);
            ::SetWindowOrgEx(hdcDraw, 0, 0, &pW);
            iMode = ::SetMapMode(hdcDraw, MM_TEXT);
          } // of if
        } // of else
    
        hr = OnDraw(dwDrawAspect, hdcDraw, &rc, prcWBounds, icTargetDev, fOptimize);
            
        if( !m_fInPlaceActive() ) 
        { ::SetViewportOrgEx(hdcDraw, pVp.x, pVp.y, NULL);
          ::SetWindowOrgEx(hdcDraw, pW.x, pW.y, NULL);
          ::SetMapMode(hdcDraw, iMode);
        } // of if
      } // of else

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::Draw(dwDrawAspect=")<<dwDrawAspect
                  <<_T(",lIndex=")<<lIndex
                  <<_T(",rc=")<<((LPCRECT)&rc)
                  <<_T(",prcBounds=")<<((LPCRECT)prcBounds)
                  <<_T(",prcWBounds=")<<((LPCRECT)prcWBounds)
                  <<_T("):")<<setHR<<CHResult(hr)<<endl;

      gDrawCount++;

      return hr;
    } // of COControl::Draw()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetColorSet(DWORD            dwDrawAspect,
                                        LONG             lindex,
                                        void*            IgnoreMe,
                                        DVTARGETDEVICE*  ptd,
                                        HDC              hicTargetDevice,
                                        LOGPALETTE**     ppColorSet
                                       )
    { HRESULT hr = S_OK;

      if( dwDrawAspect!=DVASPECT_CONTENT )
        hr = DV_E_DVASPECT;
      else if( NULL==ppColorSet )
        hr = E_POINTER;
      else
      { *ppColorSet = NULL;
        
        hr = OnGetPalette(hicTargetDevice, ppColorSet) ? ((*ppColorSet) ? S_OK : S_FALSE) : E_NOTIMPL;
      } // of else

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::GetColorSet(dwDrawAspect=")<<dwDrawAspect<<_T(",lindex=")<<lindex<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetColorSet()
    
    /**
     *
     */
    STDMETHODIMP COControl::Freeze(DWORD   dwDrawAspect,LONG lIndex,void* IgnoreMe,DWORD*  pdwFreeze)
    { HRESULT hr = E_NOTIMPL;
    
      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::Freeze(dwDrawAspect=")<<dwDrawAspect<<_T(",lIndex=")<<lIndex<<_T(",pdwFreeze=")<<pdwFreeze<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::Freeze()
    
    /**
     *
     */
    STDMETHODIMP COControl::Unfreeze(DWORD dwFreeze)
    { HRESULT hr = E_NOTIMPL;
    
      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::Unfreeze(dwFreeze=")<<dwFreeze<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::Unfreeze()
    
    /**
     *
     */
    STDMETHODIMP COControl::SetAdvise(DWORD dwAspects,DWORD dwAdviseFlags,IAdviseSink* pAdviseSink)
    { HRESULT hr = S_OK;

      if( !(dwAspects & DVASPECT_CONTENT) )
        hr = DV_E_DVASPECT;
      else
      {
        m_fViewAdvisePrimeFirst = (dwAdviseFlags & ADVF_PRIMEFIRST) ? true : false;
        m_fViewAdviseOnlyOnce   = (dwAdviseFlags & ADVF_ONLYONCE)   ? true : false;
    
        RELEASE_INTERFACE(m_pViewAdviseSink);
        m_pViewAdviseSink = pAdviseSink;
        ADDREF_INTERFACE(m_pViewAdviseSink);
    
        if( m_fViewAdvisePrimeFirst )
          ViewChanged();
      } // of else

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::SetAdvise(dwAspects=")<<dwAspects<<_T(",dwAdviseFlags=")<<dwAdviseFlags<<_T("):")<<setHR<<CHResult(hr)<<endl;
  
      return hr;
    } // of COControl::SetAdvise()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetAdvise(DWORD* pdwAspects,DWORD* pdwAdviseFlags,IAdviseSink** ppAdviseSink)
    { HRESULT hr = S_OK;

      if( NULL!=pdwAspects )
        *pdwAspects = DVASPECT_CONTENT;
  
      if( NULL!=pdwAdviseFlags ) 
      { *pdwAdviseFlags = 0;

        if( m_fViewAdviseOnlyOnce ) 
          *pdwAdviseFlags |= ADVF_ONLYONCE;

        if (m_fViewAdvisePrimeFirst ) 
          *pdwAdviseFlags |= ADVF_PRIMEFIRST;
      } // of if
  
      if( NULL!=ppAdviseSink ) 
      { *ppAdviseSink = m_pViewAdviseSink;

        ADDREF_INTERFACE(*ppAdviseSink);
      } // of if

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::GetAdvise(pdwAspects=")<<pdwAspects<<_T(",pdwAdviseFlags=")<<pdwAdviseFlags<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetAdvise()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetExtent(DWORD dwDrawAspect,LONG lindex,DVTARGETDEVICE* ptd,LPSIZEL psizel)
    { HRESULT hr = GetExtent(dwDrawAspect, psizel);

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::GetExtent(dwDrawAspect=")<<dwDrawAspect<<_T(",lindex=")<<lindex<<_T(",psizel=")<<psizel<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr; 
    } // of COControl::GetExtent()
    
    
    /**
     *
     */
    bool COControl::OnGetPalette(HDC hicTargetDevice,LOGPALETTE** ppColorSet)
    { return false; }
    
    /**
     *
     */
    STDMETHODIMP COControl::GetRect(DWORD dvAspect,LPRECTL prcRect)
    { HRESULT hr = S_OK;
      RECTL   rc;
  
      if( !OnGetRect(dvAspect, &rc) ) 
        hr = DV_E_DVASPECT;
      else
      { Wnd::PixelToHiMetric((LPSIZEL)&rc, (LPSIZEL)prcRect);
        Wnd::PixelToHiMetric((LPSIZEL)(LPBYTE)&rc + sizeof(SIZEL), (LPSIZEL)((LPBYTE)prcRect + sizeof(SIZEL)));
      } // of else

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::GetRect(dvAspect=")<<dvAspect<<_T(",prcRect=")<<prcRect<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetRect()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetViewStatus(DWORD* pdwStatus)
    { HRESULT hr = S_OK;

      if( NULL==pdwStatus )
        hr = E_POINTER;
      else
        *pdwStatus = m_myControlClass->m_isOpaque ? VIEWSTATUS_OPAQUE : 0;

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::GetViewStatus(pdwStatus=")<<pdwStatus<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::GetViewStatus()
    
    /**
     *
     */
    STDMETHODIMP COControl::QueryHitPoint(DWORD    dvAspect,
                                          LPCRECT  prcBounds,
                                          POINT    ptLocation,
                                          LONG     lCloseHint,
                                          DWORD*   pdwHitResult
                                         )
    { HRESULT hr = S_OK;

      if( dvAspect!=DVASPECT_CONTENT )
        hr = DV_E_DVASPECT;
      else if( NULL==pdwHitResult )
        hr = E_POINTER;
      else
        *pdwHitResult = ::PtInRect(prcBounds, ptLocation) ? HITRESULT_HIT : HITRESULT_OUTSIDE;

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::QueryHitPoint(dvAspect=")<<dvAspect<<_T(",prcBounds=")<<prcBounds
                  <<_T(",ptLocation=")<<(&ptLocation)
                  <<_T(",lCloseHint=")<<lCloseHint
                  <<_T(",pdwHitResult=")<<pdwHitResult
                  <<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::QueryHitPoint()
    
    /**
     *
     */
    STDMETHODIMP COControl::QueryHitRect(DWORD     dvAspect,
                                         LPCRECT   prcBounds,
                                         LPCRECT   prcLocation,
                                         LONG      lCloseHint,
                                         DWORD*    pdwHitResult
                                        )
    { HRESULT hr = S_OK;
      RECT    rc;
    
      if( dvAspect != DVASPECT_CONTENT )
        hr = DV_E_DVASPECT;
      else if( NULL==pdwHitResult )
        hr = E_POINTER;
      else
        *pdwHitResult = ::IntersectRect(&rc, prcBounds, prcLocation) ? HITRESULT_HIT : HITRESULT_OUTSIDE;

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::QueryHitRect(dvAspect=")<<dvAspect<<_T(",prcBounds=")<<prcBounds
                  <<_T(",prcLocation=")<<prcLocation
                  <<_T(",lCloseHint=")<<lCloseHint
                  <<_T(",pdwHitResult=")<<pdwHitResult
                  <<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COControl::QueryHitRect()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetNaturalExtent(DWORD           dvAspect,
                                             LONG            lIndex,
                                             DVTARGETDEVICE* ptd,
                                             HDC             hicTargetDevice,
                                             DVEXTENTINFO*   pExtentInfo,
                                             LPSIZEL         pSizel
                                            )
    { HRESULT hr = E_NOTIMPL;

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::GetNaturalExtent():")<<setHR<<CHResult(hr)<<endl;      

      return hr; 
    } // of COControl::GetNaturalExtent()

    /**
     *
    `*/
    bool COControl::OnGetRect(DWORD dvAspect,LPRECTL pRect)
    { bool result = true;

      if( dvAspect!=DVASPECT_CONTENT || m_pWnd==NULL )
        result = false;
      else
        *((LPRECT)pRect) = m_pWnd->GetLocation();

      LOGGER_DEBUG<<_T("COControl::IViewObjectEx::OnGetRect(dvAspect=")<<dvAspect<<_T(",pRect=")<<pRect<<_T("):")<<result<<endl;      

      return result;
    } // of COControl::OnGetRect()
#pragma endregion

#pragma region IPersist
    /**
     *
     */
    STDMETHODIMP COControl::Load(IPropertyBag2* pPropertyBag,LPERRORLOG pErrorLog)
    { HRESULT hr          = S_OK;
      ULONG   cProperties = 0;

      if( NULL==pPropertyBag )
        hr = E_POINTER;
      else
      { m_propertySet.Read(pPropertyBag,pErrorLog);

        LoadStandardProperties();
        LoadProperties();
      } // of else

      LOGGER_DEBUG<<_T("COControl::IPersist::Load(IPropertyBag2):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::Load()

    /**
     *
     */
    STDMETHODIMP COControl::Save(IPropertyBag2* pPropertyBag, BOOL fClearDirty,BOOL fSaveAllProperties)
    { HRESULT hr=S_OK;
  
      SaveStandardProperties();
      SaveProperties();

      m_propertySet.Write(pPropertyBag);

      if( SUCCEEDED(hr) )
      { if( fClearDirty )
          m_fDirty = false;
    
        CALLCOM_0(m_pOleAdviseHolder,SendOnSave);
      } // of if

      LOGGER_DEBUG<<_T("COControl::IPersist::Save(IPropertyBag2)(fClearDirty=")<<fClearDirty<<_T(",fSaveAllProperties=")<<fSaveAllProperties<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::Save()

    /**
     *
     */
    STDMETHODIMP COControl::GetClassID(CLSID *pclsid)
    { HRESULT hr = S_OK;

      if( NULL==pclsid )
        hr = E_POINTER;
      else
        *pclsid = GetCOClassID();

      LOGGER_DEBUG<<_T("COControl::IPersist::GetClassID(pclsid=")<<pclsid<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::GetClassID()
    
    /**
     *
     */
    STDMETHODIMP COControl::IsDirty()
    { HRESULT hr = m_fDirty ? S_OK : S_FALSE; 

      LOGGER_DEBUG<<_T("COControl::IPersist::IsDirty():")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::IsDirty()
    
    /**
     *
     */
    STDMETHODIMP COControl::InitNew()
    { HRESULT hr = InitializeControl(); 
    
      LOGGER_DEBUG<<_T("COControl::IPersist::InitNew():")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::InitNew()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetSizeMax(ULARGE_INTEGER *pulMaxSize)
    { HRESULT hr = E_NOTIMPL; 
    
      LOGGER_DEBUG<<_T("COControl::IPersist::GetSizeMax():")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::GetSizeMax()
    
    /**
     *
     */
    STDMETHODIMP COControl::Load(IStream* pStream)
    { HRESULT hr = S_OK;

/*
      hr = LoadStandardState(pStream);

      if( SUCCEEDED(hr) )
        hr = LoadBinaryState(pStream);

      LOGGER_DEBUG<<_T("COControl::IPersist::Load(IStream*):")<<setHR<<CHResult(hr)<<endl;      
*/

      return hr;
    } // of COControl::Load()
    
    /**
     *
     */
    STDMETHODIMP COControl::Save(IStream* pStream,BOOL fClearDirty)
    { HRESULT hr = S_OK;

/*
      hr = m_SaveToStream(pStream);

      if( SUCCEEDED(hr) )
      { if( fClearDirty )
          m_fDirty = false;

        CALLCOM_0(m_pOleAdviseHolder,SendOnSave);
      } // of if
  
      LOGGER_DEBUG<<_T("COControl::IPersist::Save(IStream*,fClearDirty=")<<fClearDirty<<_T("):")<<setHR<<CHResult(hr)<<endl;      
*/

      return hr;
    } // of COControl::Save()
    
    /**
     *
     */
    STDMETHODIMP COControl::InitNew(IStorage* pStorage)
    { HRESULT hr = InitNew(); 

      LOGGER_DEBUG<<_T("COControl::IPersist::InitNew(IStorage*):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::InitNew()
    
    /**
     *
     */
    STDMETHODIMP COControl::Load(IStorage* pStorage)
    { HRESULT                     hr = S_OK;
      COMPtr<IPropertySetStorage> pPropSetStg;

      if( NULL!=pStorage )
      {
        THROW_COMEXCEPTION( pStorage->QueryInterface( IID_IPropertySetStorage,reinterpret_cast<void**>(&pPropSetStg) ) );

        m_propertySet.Read(pPropSetStg);

        LoadStandardProperties();
        LoadProperties();
      } // of if
  
      LOGGER_DEBUG<<_T("COControl::IPersist::Load(IStorage*):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::Load()
    
    /**
     *
     */
    STDMETHODIMP COControl::Save(IStorage* pStorage,BOOL fSameAsLoad)
    { HRESULT                     hr = S_OK;
      COMPtr<IPropertySetStorage> pPropSetStg;

      if( NULL!=pStorage )
      {
        THROW_COMEXCEPTION( pStorage->QueryInterface( IID_IPropertySetStorage,reinterpret_cast<void**>(&pPropSetStg) ) );

        SaveStandardProperties();
        SaveProperties();

        m_propertySet.Write(pPropSetStg,true);
      } // of if

      LOGGER_DEBUG<<_T("COControl::IPersist::Save(fSameAsLoad=")<<fSameAsLoad<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::Save()
    
    /**
     *
     */
    STDMETHODIMP COControl::SaveCompleted(IStorage* pStorageNew)
    { HRESULT hr = S_OK;

      if( m_fSaveSucceeded ) 
      { m_fDirty = false;

        CALLCOM_0(m_pOleAdviseHolder,SendOnSave);
      } // of if

      LOGGER_DEBUG<<_T("COControl::IPersist::SaveCompleted():")<<setHR<<CHResult(hr)<<endl;      
  
      return hr;
    } // of COControl::SaveCompleted()
    
    /**
     *
     */
    STDMETHODIMP COControl::HandsOffStorage()
    { HRESULT hr = S_OK;

      LOGGER_DEBUG<<_T("COControl::IPersist::HandsOffStorage():")<<setHR<<CHResult(hr)<<endl;      

      return hr; 
    } // of COControl::HandsOffStorage()

    /**
     *
     */
    HRESULT COControl::LoadStandardProperties()
    { HRESULT     hr = S_OK;
      PROPVARIANT propValue;
      SIZEL       slHiMetric = { 100, 50 };

      ::PropVariantInit( &propValue );
  
      if( m_propertySet.Get(_T("extendx"), propValue) && propValue.vt==VT_I8 ) 
        slHiMetric.cx = propValue.lVal;

      if( m_propertySet.Get(_T("extendy"), propValue) && propValue.vt==VT_I8 ) 
        slHiMetric.cy = propValue.lVal;

      Wnd::HiMetricToPixel(&slHiMetric, &m_size);

      return hr;
    } // of COControl::LoadStandardProperties()

    /**
     *
     */
    HRESULT COControl::SaveStandardProperties()
    { HRESULT     hr = S_OK;
      SIZEL       slHiMetric;
      PROPVARIANT propValue;

      ::PropVariantInit( &propValue );
  
      Wnd::PixelToHiMetric(&m_size, &slHiMetric);

      propValue.vt   = VT_I8;
      propValue.lVal = slHiMetric.cx;
      m_propertySet.Add(_T("extendx"),propValue);

      propValue.vt   = VT_I8;
      propValue.lVal = slHiMetric.cy;
      m_propertySet.Add(_T("extendy"),propValue);

      return hr;
    } // of COControl::SaveStandardState()

    /**
     *
     */
    HRESULT COControl::LoadProperties()
    { return S_OK; } 

    /**
     *
     */
    HRESULT COControl::SaveProperties()
    { return S_OK; }

    /**
     *
     */
    HRESULT COControl::InitializeControl()
    { return S_OK; }

#pragma endregion

#pragma region IPointerInactive
    /**
     *
     */
    STDMETHODIMP COControl::GetActivationPolicy(DWORD* pdwPolicy)
    { HRESULT hr = S_OK;

      if( NULL==pdwPolicy )
        hr = E_POINTER;
      else
        *pdwPolicy = m_myControlClass->m_activationPolicy;

      LOGGER_DEBUG<<_T("COControl::IPointerInactive::GetActivationPolicy(pdwPolicy=")<<pdwPolicy<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::GetActivationPolicy()
    
    /**
     *
     */
    STDMETHODIMP COControl::OnInactiveMouseMove(LPCRECT pRectBounds,
                                                long    x,
                                                long    y,
                                                DWORD   dwMouseMsg
                                               )
    { HRESULT hr = S_OK; 

      LOGGER_DEBUG<<_T("COControl::IPointerInactive::OnInactiveMouseMove(pRectBounds=")<<pRectBounds<<_T(",x=")<<x<<_T(",y=")<<y<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::OnInactiveMouseMove()
        
    /**
     *
     */
    STDMETHODIMP COControl::OnInactiveSetCursor(LPCRECT pRectBounds,
                                                long    x,
                                                long    y,
                                                DWORD   dwMouseMsg,
                                                BOOL    fSetAlways
                                               )
    { HRESULT hr = S_OK; 

      LOGGER_DEBUG<<_T("COControl::IPointerInactive::OnInactiveSetCursor(pRectBounds=")<<pRectBounds<<_T(",x=")<<x<<_T(",y=")<<y<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::OnInactiveSetCursor()
#pragma endregion

#pragma region IQuickActivate
    /**
     *
     */
    STDMETHODIMP COControl::QuickActivate(QACONTAINER* pContainer,QACONTROL* pControl)
    { HRESULT hr = S_OK;
      DWORD   dw;

      LOGGER_DEBUG<<_T("COControl::IQuickActivate::QuickActivate():")<<endl;

      // we need these guys.
      //
      if( NULL==pContainer || NULL==pControl || 
          pContainer->cbSize < 56 ||
          pControl->cbSize   < sizeof(QACONTROL)
        )
      { LOGGER_DEBUG<<_T(" sizeof(QACONTAINER) =")<<dec<<sizeof(QACONTAINER)<<endl;
        LOGGER_DEBUG<<_T(" sizeof(QACONTROL)   =")<<dec<<sizeof(QACONTROL)<<endl;
        LOGGER_DEBUG<<_T(" pContainer->cbSize  =")<<dec<<pContainer->cbSize<<endl;
        LOGGER_DEBUG<<_T(" pControl->cbSize    =")<<dec<<pControl->cbSize<<endl;
       
        hr = E_UNEXPECTED;
      } // of if
      else
      { LOGGER_DEBUG<<_T(" sizeof(QACONTAINER) =")<<dec<<sizeof(QACONTAINER)<<endl;
        LOGGER_DEBUG<<_T(" sizeof(QACONTROL)   =")<<dec<<sizeof(QACONTROL)<<endl;
        LOGGER_DEBUG<<_T(" pContainer->cbSize  =")<<dec<<pContainer->cbSize<<endl;
        LOGGER_DEBUG<<_T(" pControl->cbSize    =")<<dec<<pControl->cbSize<<endl;

        LOGGER_DEBUG<<_T(" pClientSite         =0x")<<hex<<pContainer->pClientSite<<endl;
        LOGGER_DEBUG<<_T(" pAdviseSink         =0x")<<hex<<pContainer->pAdviseSink<<endl;
        LOGGER_DEBUG<<_T(" pPropertyNotifySink =0x")<<hex<<pContainer->pPropertyNotifySink<<endl;
        LOGGER_DEBUG<<_T(" pOleControlSite     =0x")<<hex<<pContainer->pOleControlSite<<endl;
        LOGGER_DEBUG<<_T(" dwAmbientFlags      =0x")<<hex<<pContainer->dwAmbientFlags<<endl;
        LOGGER_DEBUG<<_T(" dwAppearance        =0x")<<hex<<pContainer->dwAppearance<<endl;
        LOGGER_DEBUG<<_T(" lcid                =0x")<<hex<<pContainer->lcid<<endl;
        
        if( NULL!=pContainer->pClientSite ) 
          hr = SetClientSite(pContainer->pClientSite);

        if( SUCCEEDED(hr) )
        { m_lcid = pContainer->lcid ? pContainer->lcid : LOCALE_NEUTRAL;

          if( NULL!=pContainer->pPropertyNotifySink ) 
          { IConnectionPoint* pCP = NULL;
            
            hr = AddConnectionPoint(IID_IPropertyNotifySink,&pCP);

            if( SUCCEEDED(hr) )
            { pContainer->pPropertyNotifySink->AddRef();

              hr = pCP->Advise(pContainer->pPropertyNotifySink, &pControl->dwPropNotifyCookie);

              if( FAILED(hr) ) 
                pContainer->pPropertyNotifySink->Release();
            } // of if
          } // of if

          if( SUCCEEDED(hr) && NULL!=pContainer->pUnkEventSink ) 
          { IConnectionPoint* pCP = NULL;
            
            hr = AddConnectionPoint(IID_IDispatch,&pCP);
            
            if( SUCCEEDED(hr) )
            { hr = pCP->Advise(pContainer->pUnkEventSink, &pControl->dwEventCookie);
             
              if( FAILED(hr) ) 
                pContainer->pUnkEventSink->Release();
            } // of if
          } // of if
      
          // finally, the advise sink.
          //
          if( SUCCEEDED(hr) && NULL!=pContainer->pAdviseSink ) 
            hr = Advise(pContainer->pAdviseSink, &dw);

          if( SUCCEEDED(hr) )
          { pControl->dwMiscStatus              = m_myControlClass->m_oleMiscFlags;
            pControl->dwViewStatus              = m_myControlClass->m_isOpaque ? VIEWSTATUS_OPAQUE : 0;
            pControl->dwPointerActivationPolicy = m_myControlClass->m_activationPolicy;

            hr = OnQuickActivate(pContainer, &(pControl->dwViewStatus));
          } // of if
        } // of if
      } // of else

      LOGGER_DEBUG<<_T("COControl::IQuickActivate::QuickActivate():")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::QuickActivate()
    
    /**
     *
     */
    STDMETHODIMP COControl::SetContentExtent(LPSIZEL pSize)
    { HRESULT hr = SetExtent(DVASPECT_CONTENT, pSize); 

      LOGGER_DEBUG<<_T("COControl::IQuickActivate::SetContentExtent(pSize=")<<pSize<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::SetContentExtent()
    
    /**
     *
     */
    STDMETHODIMP COControl::GetContentExtent(LPSIZEL pSize)
    { HRESULT hr = GetExtent(DVASPECT_CONTENT, pSize); 

      LOGGER_DEBUG<<_T("COControl::IQuickActivate::GetContentExtent(pSize=")<<pSize<<_T("):")<<setHR<<CHResult(hr)<<endl;      

      return hr;
    } // of COControl::SetContentExtent()
    
    /**
     *
     */
    HRESULT COControl::OnQuickActivate(QACONTAINER* pContainer,DWORD* pdwViewStatus)
    { HRESULT hr = S_OK; 

      return hr;
    }
#pragma endregion

#pragma region Diverses
    /**
     *
     */
    void COControl::OnVerb(LONG lVerb)
    { }

    /**
     *
     */
    bool COControl::CreateInPlaceWindow(HWND hWndParent,const RECT& rcPos)
    { m_size.cx = rcPos.right  - rcPos.left;
      m_size.cy = rcPos.bottom - rcPos.top;
      
      return true; 
    } // of COControl::CreateInPlaceWindow()

    /**
     *
     */
    bool COControl::GetAmbientProperty(DISPID  dispid,VARTYPE vt,PVOID pData)
    { DISPPARAMS dispparams;
      VARIANT    v, v2;
      HRESULT    hr;
  
      v.vt    = VT_EMPTY;
      v.lVal  = 0;
      v2.vt   = VT_EMPTY;
      v2.lVal = 0;
  
      // get a pointer to the source of ambient properties.
      //
      if( NULL==m_pDispAmbient ) 
      { if( NULL!=m_pClientSite )
          m_pClientSite->QueryInterface(IID_IDispatch, (void **)&m_pDispAmbient);

        if( !m_pDispAmbient )
          return false;
      }
  
      // now go and get the property into a variant.
      //
      ::memset(&dispparams, 0, sizeof(DISPPARAMS));
      hr = m_pDispAmbient->Invoke(dispid, IID_NULL, 0, DISPATCH_PROPERTYGET, &dispparams,&v, NULL, NULL);

      if( FAILED(hr) ) 
        return false;
  
      // we've got the variant, so now go an coerce it to the type that the user
      // wants.  if the types are the same, then this will copy the stuff to
      // do appropriate ref counting ...
      //
      hr = ::VariantChangeType(&v2, &v, 0, vt);
      if( FAILED(hr) ) 
      { ::VariantClear(&v);
        
        return false;
      }
  
      // copy the data to where the user wants it
      //
      ::CopyMemory(pData, &(v2.lVal), m_rgcbDataTypeSize[vt]);
      ::VariantClear(&v);

      return true;
    } // of COControl::GetAmbientProperty()
    
    /**
     *
     */
    bool COControl::GetAmbientFont(IFont** ppFont)
    { IDispatch* pFontDisp;
  
      // we don't have to do much here except get the ambient property and QI
      // it for the user.
      //
      *ppFont = NULL;
      if( !GetAmbientProperty(DISPID_AMBIENT_FONT, VT_DISPATCH, &pFontDisp) )
        return false;
  
      pFontDisp->QueryInterface(IID_IFont, (void **)ppFont);
      pFontDisp->Release();

      return *ppFont!=NULL ? true : false;
    } // of COControl::GetAmbientFont()
    
    /**
     *
     */
    bool COControl::DesignMode()
    { VARIANT_BOOL f;
  
      // if we don't already know our run mode, go and get it.  we'll assume
      // it's true unless told otherwise [or if the operation fails ...]
      //
      if( !m_fModeFlagValid ) 
      {
        f = TRUE;
        m_fModeFlagValid = true;

        GetAmbientProperty(DISPID_AMBIENT_USERMODE, VT_BOOL, &f);

        m_fRunMode = f==VARIANT_TRUE ? true : false;
      } // of if
  
      return !m_fRunMode;
    } // of COControl::DesignMode()
    
    /**
     *
     */
    bool COControl::SetControlSize(SIZEL* pSize)
    { SIZEL slHiMetric;
  
      Wnd::PixelToHiMetric(pSize, &slHiMetric);

      return FAILED(SetExtent(DVASPECT_CONTENT, &slHiMetric)) ? false : true;
    }

    /**
     *
     */
    void COControl::ViewChanged()
    { if( NULL!=m_pViewAdviseSink ) 
      { m_pViewAdviseSink->OnViewChange(DVASPECT_CONTENT, -1);

        if( m_fViewAdviseOnlyOnce )
          SetAdvise(DVASPECT_CONTENT, 0, NULL);
      } // of if
    }
    
    /**
     *
     */
    void COControl::SetInPlaceVisible(bool fShow)
    { if( NULL!=m_pWnd ) 
      { bool fVisible = m_pWnd->IsVisible();

        if( fVisible && !fShow )
          m_pWnd->ShowWindow(SW_HIDE);
        else if ( !fVisible && fShow )
          m_pWnd->ShowWindow(SW_SHOWNA);
      } // of if
    } // of COControl::SetInPlaceVisible()
    
    /**
     *
     */
    void COControl::AmbientPropertyChanged(DISPID dispid)
    { }
    
    /**
     *
     */
    HRESULT COControl::DoCustomVerb(LONG    lVerb)
    { return OLEOBJ_S_INVALIDVERB; }
    
    /**
     *
     */
    bool COControl::OnSpecialKey(LPMSG pmsg)
    { return FALSE; }
    
    /**
     *
     */
    void COControl::ModalDialog(bool fShow)
    { if( NULL!=m_pInPlaceFrame )
        m_pInPlaceFrame->EnableModeless(!fShow);
    }
#pragma endregion

#pragma region RegisterControlClass
    /**
     *
     */
    void COControl::RegisterControlClass(LPCTSTR controlClassName,const COControlClass& controlClass)
    { m_controlClass.erase( controlClassName );
      m_controlClass.insert( COControlClassP(controlClassName,controlClass) );
    } // of COControl::RegisterControlClass()

    /**
     *
     */
    const COControlClass* COControl::GetControlClass(LPCTSTR controlClassName)
    { COControlClassM::const_iterator i = m_controlClass.find(controlClassName);

      return i!=m_controlClass.end() ? &i->second : NULL;
    } // of COControl::GetControlClass()
#pragma endregion
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
