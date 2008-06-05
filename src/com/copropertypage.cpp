/*
 * $Id$
 * 
 * COM PropertyPage base class.
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
#include "com/copropertypage.h"
#include "com/comserver.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "bvr20983cc-res.h"

const GUID CUST_DIALOGID    = {0x7dc6e86d,0x0000,0x1000,{0x00,0x03,0x42,0x56,0x52,0x20,0x09,0x83}};
const GUID CUST_DIALOGTITLE = {0x7dc6e86d,0x0000,0x1000,{0x00,0x04,0x42,0x56,0x52,0x20,0x09,0x83}};

namespace bvr20983
{
  namespace COM
  {

    /**
     *
     */
    COPropertyPageInfo::COPropertyPageInfo(WORD wDlgResourceId,WORD wTitleId)
    { m_wDlgResourceId = wDlgResourceId;
      m_wTitleId       = wTitleId;
      m_wDocStringId   = 0;
      m_szHelpFileId   = 0;
      m_dwHelpContextId= 0;
      m_hInstance      = COMServer::GetInstanceHandle();
    } // of COPropertyPageInfo::COPropertyPageInfo()

    /**
     *
     */
    void COPropertyPageInfo::GetInfo(PROPPAGEINFO* pPropPageInfo,SIZE windowSize) const
    { if( NULL!=pPropPageInfo && pPropPageInfo->cb>=sizeof(PROPPAGEINFO) )
      { ::memset(pPropPageInfo, 0, sizeof(PROPPAGEINFO));

        pPropPageInfo->cb            = sizeof(PROPPAGEINFO);
        pPropPageInfo->pszTitle      = m_wTitleId!=0     ? COMServer::LoadString(m_wTitleId,m_hInstance)     : NULL;
        pPropPageInfo->pszDocString  = m_wDocStringId!=0 ? COMServer::LoadString(m_wDocStringId,m_hInstance) : NULL;
        pPropPageInfo->pszHelpFile   = m_szHelpFileId!=0 ? COMServer::LoadString(m_szHelpFileId,m_hInstance) : NULL;
        pPropPageInfo->dwHelpContext = m_dwHelpContextId;
        pPropPageInfo->size          = windowSize;
      } // of if
    } // of COPropertyPageInfo::GetInfo()

   /**
    *
    */
   HRESULT COPropertyPage::Create(REFCLSID clsid,LPUNKNOWN* ppv,LPUNKNOWN pUnkOuter)
   { HRESULT hr = S_OK;

     if( NULL==ppv )
       hr = E_POINTER;
     else
     { TCHAR szModulePath[MAX_PATH];
       COMPtr<ITypeLib> pTLib;

       COMServer::GetModuleFileName(szModulePath,sizeof(szModulePath)/sizeof(szModulePath[0]));

       hr = ::LoadTypeLibEx(szModulePath,REGKIND_NONE,&pTLib);

       if( SUCCEEDED(hr) )
       { COMPtr<ITypeInfo>  pTypeInfo;
         COMPtr<ITypeInfo2> pTypeInfo2;
         VARIANT            var;
         WORD               dlgID    = 0;
         WORD               dlgTitle = 0;

         hr = pTLib->GetTypeInfoOfGuid(clsid,&pTypeInfo);

         if( SUCCEEDED(hr) )
         { THROW_COMEXCEPTION( pTypeInfo->QueryInterface(IID_ITypeInfo2,(void**)&pTypeInfo2) );

           if( pTypeInfo2->GetCustData(CUST_DIALOGID,&var)==S_OK && VT_I4==V_VT(&var) )
             dlgID = (WORD)V_I4(&var);

           if( pTypeInfo2->GetCustData(CUST_DIALOGTITLE,&var)==S_OK && VT_I4==V_VT(&var) )
             dlgTitle = (WORD)V_I4(&var);

           COPropertyPageInfo ppi(dlgID,dlgTitle);

           COPropertyPage* pPropertyPage = new COPropertyPage(pUnkOuter,ppi);

           if( NULL==pPropertyPage )
             hr = E_OUTOFMEMORY;
           else
             *ppv = pPropertyPage->PrivateUnknown();
         } // of if
       } // of if
     } // of else

     return hr;
   } // of COLED::Create()

#pragma warning(disable:4355)
    /**
     *
     */
    COPropertyPage::COPropertyPage(LPUNKNOWN pUnkOuter,const COPropertyPageInfo& propertyPageInfo)
      : COUnknown(pUnkOuter),
        m_propertyPageInfo(propertyPageInfo)
    { m_pPropertyPageSite = NULL;
      m_fDirty            = false; 
      m_fActivated        = false;
    }
#pragma warning(default:4355)  // using 'this' in constructor

    /**
     *
     */
    COPropertyPage::~COPropertyPage()
    { m_objects.clear();
      m_ctrl.clear();

      RELEASE_INTERFACE(m_pPropertyPageSite);
    } // of COPropertyPage::~COPropertyPage()

#pragma region InternalQI
    /**
     *
     */
    HRESULT COPropertyPage::InternalQueryInterface(REFIID riid,void** ppv)
    { HRESULT hr = S_OK;

      if( NULL==ppv )
        hr = E_POINTER;
      else
      { *ppv = NULL;

        if( IID_IPropertyPage==riid )
          *ppv = (IPropertyPage*)this;
        else if( IID_IPropertyPage2==riid )
          *ppv = (IPropertyPage2*)this;

        if( NULL!=*ppv )
          ((LPUNKNOWN)(*ppv))->AddRef();
        else
          hr = COUnknown::InternalQueryInterface(riid,ppv);
      } // of else

      LOGGER_DEBUG<<_T("COPropertyPage::InternalQueryInterface(riid=")<<riid<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::InternalQueryInterface()
#pragma endregion

    /**
     *
     */
    STDMETHODIMP COPropertyPage::SetPageSite(IPropertyPageSite *pPropertyPageSite)
    { HRESULT hr = S_OK;

      RELEASE_INTERFACE(m_pPropertyPageSite);
      m_pPropertyPageSite = pPropertyPageSite;
      ADDREF_INTERFACE(m_pPropertyPageSite);

      LOGGER_DEBUG<<_T("COPropertyPage::SetPageSite():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::SetPageSite()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::Activate(HWND hwndParent,LPCRECT prcBounds,BOOL fModal)
    { HRESULT hr = S_OK;

      // first make sure the dialog window is loaded and created.
      //
      hr = LoadDialog(hwndParent,fModal);
      if( SUCCEEDED(hr) )
      { m_fActivated = true;

        Move(prcBounds);
        ShowWindow(SW_SHOW);
      } // of if

      LOGGER_DEBUG<<_T("COPropertyPage::SetPageSite():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::Activate()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::Deactivate(void)
    { HRESULT hr = S_OK;

      DestroyWindow();

      m_fActivated = false;

      LOGGER_DEBUG<<_T("COPropertyPage::Deactivate():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::Deactivate()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::GetPageInfo(PROPPAGEINFO *pPropPageInfo)
    { HRESULT hr = S_OK;

      m_propertyPageInfo.GetInfo(pPropPageInfo,GetSize());

      LOGGER_DEBUG<<_T("COPropertyPage::GetPageInfo():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::GetPageInfo()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::SetObjects(ULONG cObjects,IUnknown **ppUnkObjects)
    { HRESULT hr=S_OK;

      m_objects.clear();

      if( NULL!=ppUnkObjects )
        for( UINT i=0;i<cObjects;i++ )
          m_objects.push_back( COMPtr<IUnknown>(ppUnkObjects[i]) );

      m_fDirty = false;

      LOGGER_DEBUG<<_T("COPropertyPage::SetObjects(cObjects=")<<cObjects<<_T("):")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::SetObjects()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::Show(UINT nCmdShow)
    { HRESULT hr = S_OK;

      ShowWindow(nCmdShow);

      LOGGER_DEBUG<<_T("COPropertyPage::Show():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::Show()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::Move(LPCRECT prcBounds)
    { HRESULT hr = S_OK;

      MoveWindow(prcBounds);

      LOGGER_DEBUG<<_T("COPropertyPage::Move():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::Move()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::IsPageDirty()
    { HRESULT hr = m_fDirty ? S_OK : S_FALSE;

      LOGGER_DEBUG<<_T("COPropertyPage::IsPageDirty():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::IsPageDirty()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::Apply(void)
    { HRESULT hr = S_OK;

      if( NULL!=GetWindowHandle() ) 
      { hr = OnApply();
        
        if( SUCCEEDED(hr) )
        { if( m_fDirty ) 
          { m_fDirty = false;

            if( NULL!=m_pPropertyPageSite )
              m_pPropertyPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
          } // of if
        } // of if
      } // of if
      else
        hr = E_UNEXPECTED;

      LOGGER_DEBUG<<_T("COPropertyPage::Apply():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::Apply()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::Help(LPCOLESTR pszHelpDir)
    { HRESULT hr = S_OK;
      BOOL    f  = FALSE;

      if( NULL!=GetWindowHandle() )
      { TString* helpFile = NULL;

        COMServer::LoadString(m_propertyPageInfo.m_szHelpFileId,&helpFile,m_propertyPageInfo.m_hInstance);

        if( NULL!=helpFile )
          hr = WinHelp(GetWindowHandle(),helpFile->c_str(), HELP_CONTEXT, m_propertyPageInfo.m_dwHelpContextId) ? S_OK : E_FAIL;
        else
          hr = E_UNEXPECTED;
      } // of if
      else
        hr = E_UNEXPECTED;

      LOGGER_DEBUG<<_T("COPropertyPage::Help():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::Help()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::TranslateAccelerator(LPMSG pmsg)
    { HRESULT hr = S_OK;

      if( NULL!=GetWindowHandle() )
        hr = ::IsDialogMessage(GetWindowHandle(), pmsg) ? S_OK : S_FALSE;
      else
        hr = E_UNEXPECTED;

      LOGGER_DEBUG<<_T("COPropertyPage::TranslateAccelerator():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::TranslateAccelerator()

    /**
     *
     */
    STDMETHODIMP COPropertyPage::EditProperty(DISPID dispid)
    { HRESULT hr = OnEditProperty(dispid);

      LOGGER_DEBUG<<_T("COPropertyPage::EditProperty():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of COPropertyPage::EditProperty()


    /**
     *
     */
    HRESULT COPropertyPage::OnApply()
    { TCHAR   ctrlWndCls[255];
      TCHAR   ctrlItemText[1024];
      HRESULT hr = S_OK;

      for( std::set<LONG>::const_iterator i=m_ctrl.begin();i!=m_ctrl.end();i++ )
      { LONG ctrlID    = *i;
        HWND hWndChild = ::GetDlgItem(GetWindowHandle(),ctrlID);
        LONG ctrlStyle = ::GetWindowLong(hWndChild,GWL_STYLE);

        ::GetClassName(hWndChild,ctrlWndCls,ARRAYSIZE(ctrlWndCls));

        if( _tcscmp(_T("Edit"),ctrlWndCls)==0 && ::GetDlgItemText(GetWindowHandle(),ctrlID,ctrlItemText,ARRAYSIZE(ctrlItemText))>0 )
        { COVariant v(ctrlItemText);

          hr = SetObjectValue(ctrlID,v);

          if( FAILED(hr) )
            break;
        } // of if
        else if( _tcscmp(_T("Button"),ctrlWndCls)==0 && (ctrlStyle&BS_AUTOCHECKBOX)!=0 )
        { UINT buttonChecked = ::IsDlgButtonChecked(GetWindowHandle(),ctrlID);

          if( buttonChecked==BST_CHECKED || buttonChecked==BST_UNCHECKED )
          { COVariant v(buttonChecked==BST_CHECKED);

            hr = SetObjectValue(ctrlID,v);

            if( FAILED(hr) )
              break;
          } // of if
        } // of else if
      } // of for

      return hr;
    } // of COPropertyPage::OnApply()

    /**
     *
     */
    HRESULT COPropertyPage::OnEditProperty(DISPID dispid)
    { HRESULT hr = E_NOTIMPL;

      return hr;
    } // of COPropertyPage::OnEditProperty()

    /**
     *
     */
    HRESULT COPropertyPage::LoadDialog(HWND hParentWindow,BOOL bModal)
    { HRESULT hr = S_OK;

      if( NULL==GetWindowHandle() && NULL==Modeless(MAKEINTRESOURCE(m_propertyPageInfo.m_wDlgResourceId),hParentWindow,m_propertyPageInfo.m_hInstance) )
        hr = HRESULT_FROM_WIN32(GetLastError());

      return hr;
    } // of COPropertyPage::LoadDialog()

    /**
     *
     */
    void COPropertyPage::MakeDirty()
    { m_fDirty = true;
      
      if( NULL!=m_pPropertyPageSite )
        m_pPropertyPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY|PROPPAGESTATUS_VALIDATE);
    } // of COPropertyPage::MakeDirty()

    /**
     *
     */
    BOOL COPropertyPage::InitDialog()
    { BOOL result = Dialog::InitDialog();

      if( result )
        EnumChildren();

      return result; 
    } // of COPropertyPage::InitDialog()


    /**
     *
     */
    BOOL COPropertyPage::EnumChild(HWND hWndChild)
    { TCHAR ctrlWndCls[255];
      LONG  ctrlID    = ::GetWindowLong(hWndChild,GWL_ID);
      LONG  ctrlStyle = ::GetWindowLong(hWndChild,GWL_STYLE);

      ::GetClassName(hWndChild,ctrlWndCls,ARRAYSIZE(ctrlWndCls));
      
      LOGGER_DEBUG<<_T("COPropertyPage::EnumChild(hWndChild=0x")<<hex<<hWndChild<<_T(")");
      LOGGER_DEBUG<<_T(" dlgId=")<<dec<<ctrlID;
      LOGGER_DEBUG<<_T(" wndCls=")<<ctrlWndCls;
      LOGGER_DEBUG<<_T(" wndStyle=0x")<<hex<<ctrlStyle;
      LOGGER_DEBUG<<endl;

      if( _tcscmp(_T("Edit"),ctrlWndCls)==0 )
      { COVariant v;

        GetObjectValue(ctrlID,v);

        if( SUCCEEDED(v.ChangeType(VT_BSTR)) )
        { ::SetDlgItemText(GetWindowHandle(),ctrlID,v->bstrVal);

          m_ctrl.insert(ctrlID);
        } // of if
      } // of if
      else if( _tcscmp(_T("Button"),ctrlWndCls)==0 && (ctrlStyle&BS_AUTOCHECKBOX)!=0 )
      { COVariant v;

        GetObjectValue(ctrlID,v);

        if( SUCCEEDED(v.ChangeType(VT_BOOL)) )
        { ::CheckDlgButton(GetWindowHandle(),ctrlID,
                           v->boolVal==VARIANT_TRUE ? BST_CHECKED : BST_UNCHECKED
                          );

          m_ctrl.insert(ctrlID);
        } // of if

      } // of else if

      return TRUE;
    } // of COPropertyPage::EnumChild()

    /**
     *
     */
    HRESULT COPropertyPage::GetObjectValue(LONG ctrlID,COVariant& value)
    { HRESULT    hr = S_FALSE;
      DISPPARAMS dispparams;
  
      if( !m_objects.empty() )
      { COMPtr<IDispatch> disp;

        const COMPtr<IUnknown>& o = m_objects.at(0);

        if( SUCCEEDED(o->QueryInterface(IID_IDispatch,reinterpret_cast<void**>(&disp))) )
        { ::memset(&dispparams, 0, sizeof(DISPPARAMS));

          hr = disp->Invoke(ctrlID, 
                            IID_NULL, 0, 
                            DISPATCH_PROPERTYGET, 
                            &dispparams,
                            const_cast<VARIANT*>((const VARIANT*)value),
                            NULL, NULL
                           );
        } // of if
      } // of if

      return hr;
    } // of COPropertyPage::GetObjectValue()

    /**
     *
     */
    HRESULT COPropertyPage::SetObjectValue(LONG ctrlID,COVariant& value)
    { HRESULT    hr = S_FALSE;
      DISPPARAMS dispparams;
      DISPID     dispIDParam = DISPID_PROPERTYPUT;
  
      if( !m_objects.empty() )
      { ::memset(&dispparams, 0, sizeof(DISPPARAMS));

        dispparams.cArgs             = 1;
        dispparams.rgvarg            = const_cast<VARIANT*>((const VARIANT*)value);
        dispparams.cNamedArgs        = 1;
        dispparams.rgdispidNamedArgs = &dispIDParam;

        for( VUNKNOWN::const_iterator i=m_objects.begin();i!=m_objects.end();i++ )
        { COMPtr<IDispatch> disp;

          if( SUCCEEDED((*i)->QueryInterface(IID_IDispatch,reinterpret_cast<void**>(&disp))) )
          { hr = disp->Invoke(ctrlID, 
                              IID_NULL, 0, 
                              DISPATCH_PROPERTYPUT, 
                              &dispparams,
                              NULL,
                              NULL, NULL
                             );

            if( FAILED(hr) )
              break;
          } // of if
        } // of for
      } // of if

      return hr;
    } // of COPropertyPage::SetObjectValue()

    /**
     *
     */
    INT_PTR COPropertyPage::DialogProcedure(UINT message,WPARAM wParam,LPARAM lParam)
    { INT_PTR result = FALSE;
      
      switch( message )
      { 
      case WM_COMMAND:
        { switch( HIWORD(wParam) )
          { 
          case BN_CLICKED:
          case EN_CHANGE:
            LONG ctrlID = ::GetWindowLong((HWND)lParam,GWL_ID);

            if( !m_ctrl.empty() && m_ctrl.find(ctrlID)!=m_ctrl.end() )
              MakeDirty();

            break;
          } // of switch
        }
        break;
      } // of switch

      return result;
    } // of COPropertyPage::DialogProcedure()
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
