/*
 * $Id$
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
#ifndef COPROPERTYPAGE_H
#define COPROPERTYPAGE_H

#include "com/counknown.h"
#include "util/comptr.h"
#include "com/comutil.h"
#include "com/covariant.h"
#include "win/dialog.h"

using namespace bvr20983::win;

namespace bvr20983
{
  namespace COM
  {
    class COPropertyPage;

    /**
     *
     */
    class COPropertyPageInfo 
    { 
      public:
        COPropertyPageInfo(WORD wDlgResourceId,WORD wTitleId);
        void GetInfo(PROPPAGEINFO* pPropPageInfo,SIZE windowSize) const;

      private:
        friend class COPropertyPage;


        WORD      m_wDlgResourceId;
        WORD      m_wTitleId;
        WORD      m_wDocStringId;
        WORD      m_szHelpFileId;
        DWORD     m_dwHelpContextId;
        HINSTANCE m_hInstance;
    }; // of class COPropertyPageInfo()

    typedef std::vector<COMPtr<IUnknown>> VUNKNOWN;

    /**
     *
     */
    class COPropertyPage : public Dialog,public COUnknown, public IPropertyPage2 
    {
      public:
        static HRESULT Create(REFCLSID clsid,LPUNKNOWN* ppv,LPUNKNOWN pUnkOuter=NULL);

        virtual ~COPropertyPage();

        DECLARE_UNKNOWN

        // IPropertyPage methods
        //
        STDMETHODIMP SetPageSite(LPPROPERTYPAGESITE pPageSite);
        STDMETHODIMP Activate(HWND hwndParent, LPCRECT lprc, BOOL bModal);
        STDMETHODIMP Deactivate(void);
        STDMETHODIMP GetPageInfo(LPPROPPAGEINFO pPageInfo);
        STDMETHODIMP SetObjects(ULONG cObjects, LPUNKNOWN* ppunk);
        STDMETHODIMP Show(UINT nCmdShow);
        STDMETHODIMP Move(LPCRECT prect);
        STDMETHODIMP IsPageDirty(void);
        STDMETHODIMP Apply(void);
        STDMETHODIMP Help(LPCOLESTR lpszHelpDir);
        STDMETHODIMP TranslateAccelerator(LPMSG lpMsg);

        // IPropertyPage2 methods
        //
        STDMETHODIMP EditProperty(DISPID dispid);

        virtual HRESULT OnApply();
        virtual HRESULT OnEditProperty(DISPID dispid);

      protected:
        void               MakeDirty();
        virtual INT_PTR    DialogProcedure(UINT message,WPARAM wParam,LPARAM lParam);
        virtual BOOL       EnumChild(HWND hWndChild);
        virtual BOOL       InitDialog();

        IPropertyPageSite* m_pPropertyPageSite;
        COPropertyPageInfo m_propertyPageInfo;
        VUNKNOWN           m_objects;
        bool               m_fActivated;
        bool               m_fDirty;
        std::set<LONG>     m_ctrl;

      private:
        COPropertyPage(LPUNKNOWN pUnkOuter,const COPropertyPageInfo& propertyPageInfo);

                HRESULT LoadDialog(HWND hParentWindow,BOOL bModal);
        virtual HRESULT InternalQueryInterface(REFIID, void **);

        HRESULT GetObjectValue(LONG ctrlID,COVariant& v);
        HRESULT SetObjectValue(LONG ctrlID,COVariant& v);
    }; // of class COPropertyPage
  } // of namespace COM
} // of namespace bvr20983
#endif // COPROPERTYPAGE_H
/*==========================END-OF-FILE===================================*/
