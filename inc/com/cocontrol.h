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
#ifndef COCONTROL_H
#define COCONTROL_H

#include "com/codispatch.h"
#include <Objsafe.h>
#include "win/wnd.h"
#include "com/copropertyset.h"

using namespace bvr20983::win;

namespace bvr20983
{
  namespace COM
  {

    // this structure is like the OLEVERB structure, except that it has a resource ID
    // instead of a string for the verb's name.  better support for localization.
    struct COVerbInfo
    { LONG    m_verbID;            // verb id
      ULONG   m_verbName;          // resource ID of verb name
      DWORD   m_flags;             // verb flags
      DWORD   m_attribs;           // Specifies some combination of the verb attributes in the OLEVERBATTRIB enumeration.

      void toOLEVERB(OLEVERB& v) const;
    };

    typedef std::vector<COVerbInfo> COVerbInfoV;
    typedef std::vector<GUID>       GUIDV;

    /*
     * necessary information for a control. 
     */
    struct COControlClass
    { DWORD             m_oleMiscFlags;                // control flags
      DWORD             m_activationPolicy;            // IPointerInactive support
      bool              m_isOpaque;                    // is your control 100% opaque?
      bool              m_isWindowless;                // do we do windowless if we can?
      WORD              m_toolboxId;                   // resource ID of Toolbox Bitmap
      GUIDV             m_propPageGuid;                // array of the property page GUIDs
      COVerbInfoV       m_customVerb;                  // custom verbs
    };

    typedef std::pair<LPCTSTR, COControlClass>        COControlClassP;
    typedef std::map<LPCTSTR, COControlClass,strless> COControlClassM;

    class COControl : public CODispatch,
                      public IOleObject, 
                      public IOleControl,
                      public IOleInPlaceObjectWindowless, 
                      public IOleInPlaceActiveObject,
                      public IViewObjectEx, 
                      public IPersistPropertyBag2,
                      public IPersistStreamInit, 
                      public IPersistStorage,
                      public ISpecifyPropertyPages,
                      public IPointerInactive,
                      public IQuickActivate,
                      public IObjectSafety 
    {
      public:
        static void                  RegisterControlClass(LPCTSTR controlClassName,const COControlClass& controlClass);
        static const COControlClass* GetControlClass(LPCTSTR controlClassName);
        static const FMTID           propertySetFMTID;

        DECLARE_UNKNOWN
    
        //=--------------------------------------------------------------------------=
        // IPersist methods.  used by IPersistStream and IPersistStorage
        //
        STDMETHODIMP GetClassID(LPCLSID lpClassID);
    
        // IPersistStreamInit methods
        //
        STDMETHODIMP IsDirty();
        STDMETHODIMP Load(LPSTREAM pStm);
        STDMETHODIMP Save(LPSTREAM pStm, BOOL fClearDirty);
        STDMETHODIMP GetSizeMax(ULARGE_INTEGER FAR* pcbSize);
        STDMETHODIMP InitNew();
    
        // IPersistStorage
        //
        STDMETHODIMP InitNew(IStorage* pStg);
        STDMETHODIMP Load(IStorage* pStg);
        STDMETHODIMP Save(IStorage* pStgSave, BOOL fSameAsLoad);
        STDMETHODIMP SaveCompleted(IStorage* pStgNew);
        STDMETHODIMP HandsOffStorage();
    
        // IPersistPropertyBag2
        //
        STDMETHODIMP Load(LPPROPERTYBAG2 pPropBag,LPERRORLOG pErrorLog);
        STDMETHODIMP Save(LPPROPERTYBAG2 pPropBag, BOOL fClearDirty,BOOL fSaveAllProperties);
    
        // IOleControl methods
        //
        STDMETHODIMP GetControlInfo(LPCONTROLINFO pCI);
        STDMETHODIMP OnMnemonic(LPMSG pMsg);
        STDMETHODIMP OnAmbientPropertyChange(DISPID dispid);
        STDMETHODIMP FreezeEvents(BOOL bFreeze);
    
        // IOleObject methods
        //
        STDMETHODIMP SetClientSite(IOleClientSite* pClientSite);
        STDMETHODIMP GetClientSite(IOleClientSite** ppClientSite);
        STDMETHODIMP SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj);
        STDMETHODIMP Close(DWORD dwSaveOption);
        STDMETHODIMP SetMoniker(DWORD dwWhichMoniker, IMoniker  *pmk);
        STDMETHODIMP GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker  * *ppmk);
        STDMETHODIMP InitFromData(IDataObject  *pDataObject, BOOL fCreation, DWORD dwReserved);
        STDMETHODIMP GetClipboardData(DWORD dwReserved, IDataObject  * *ppDataObject);
        STDMETHODIMP DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite  *pActiveSite, LONG lindex,HWND hwndParent, LPCRECT lprcPosRect);
        STDMETHODIMP EnumVerbs(IEnumOLEVERB** ppEnumOleVerb);
        STDMETHODIMP Update();
        STDMETHODIMP IsUpToDate();
        STDMETHODIMP GetUserClassID(CLSID* pClsid);
        STDMETHODIMP GetUserType(DWORD dwFormOfType, LPOLESTR* pszUserType);
        STDMETHODIMP SetExtent(DWORD dwDrawAspect,SIZEL* psizel);
        STDMETHODIMP GetExtent(DWORD dwDrawAspect, SIZEL* psizel);
        STDMETHODIMP Advise(IAdviseSink* pAdvSink, DWORD* pdwConnection);
        STDMETHODIMP Unadvise(DWORD dwConnection);
        STDMETHODIMP EnumAdvise(IEnumSTATDATA** ppenumAdvise);
        STDMETHODIMP GetMiscStatus(DWORD dwAspect, DWORD* pdwStatus);
        STDMETHODIMP SetColorScheme(LOGPALETTE* pLogpal);
    
        // IOleWindow.  required for IOleInPlaceObject and IOleInPlaceActiveObject
        //
        STDMETHODIMP GetWindow(HWND* phwnd);
        STDMETHODIMP ContextSensitiveHelp(BOOL fEnterMode);
    
        // IOleInPlaceObject/IOleInPlaceObjectWindowless
        //
        STDMETHODIMP InPlaceDeactivate();
        STDMETHODIMP UIDeactivate();
        STDMETHODIMP SetObjectRects(LPCRECT lprcPosRect,LPCRECT lprcClipRect) ;
        STDMETHODIMP ReactivateAndUndo();
        STDMETHODIMP OnWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);
        STDMETHODIMP GetDropTarget(IDropTarget** ppDropTarget);
    
        // IOleInPlaceActiveObject
        //
        STDMETHODIMP TranslateAccelerator(LPMSG lpmsg);
        STDMETHODIMP OnFrameWindowActivate(BOOL fActivate);
        STDMETHODIMP OnDocWindowActivate(BOOL fActivate);
        STDMETHODIMP ResizeBorder(LPCRECT prcBorder,IOleInPlaceUIWindow* pUIWindow,BOOL fFrameWindow);
        STDMETHODIMP EnableModeless(BOOL fEnable);
    
        // IViewObject2/IViewObjectEx
        //
        STDMETHODIMP Draw(DWORD dwDrawAspect, LONG lindex, void* pvAspect,
                          DVTARGETDEVICE* ptd, HDC hdcTargetDev, HDC hdcDraw,
                          LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
                          BOOL ( STDMETHODCALLTYPE *pfnContinue) (DWORD dwContinue),
                          DWORD dwContinue
                         );
        STDMETHODIMP GetColorSet(DWORD dwDrawAspect,LONG lindex, void* pvAspect,
                                 DVTARGETDEVICE * ptd, HDC hicTargetDev,
                                 LOGPALETTE** ppColorSet
                                );
        STDMETHODIMP Freeze(DWORD dwDrawAspect, LONG lindex,void* pvAspect,DWORD* pdwFreeze);
        STDMETHODIMP Unfreeze(DWORD dwFreeze);
        STDMETHODIMP SetAdvise(DWORD aspects, DWORD advf, IAdviseSink* pAdvSink);
        STDMETHODIMP GetAdvise(DWORD *pAspects, DWORD  *pAdvf, IAdviseSink** ppAdvSink);
        STDMETHODIMP GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE __RPC_FAR* ptd, LPSIZEL lpsizel);
        STDMETHODIMP GetRect(DWORD dwAspect, LPRECTL pRect);
        STDMETHODIMP GetViewStatus(DWORD* pdwStatus);
        STDMETHODIMP QueryHitPoint(DWORD dwAspect, LPCRECT pRectBounds, POINT ptlLoc, LONG lCloseHint, DWORD* pHitResult);
        STDMETHODIMP QueryHitRect(DWORD dwAspect, LPCRECT pRectBounds, LPCRECT prcLoc, LONG lCloseHint, DWORD* pHitResult);
        STDMETHODIMP GetNaturalExtent(DWORD dwAspect, LONG lindex, DVTARGETDEVICE* ptd, HDC hicTargetDev, DVEXTENTINFO* pExtentInfo, LPSIZEL psizel);
    
        // ISpecifyPropertyPages
        //
        STDMETHODIMP GetPages(CAUUID* pPages);
    
        // IPointerInactive methods
        //
        STDMETHODIMP GetActivationPolicy(DWORD* pdwPolicy);
        STDMETHODIMP OnInactiveMouseMove(LPCRECT pRectBounds, long x, long y, DWORD dwMouseMsg);
        STDMETHODIMP OnInactiveSetCursor(LPCRECT pRectBounds, long x, long y, DWORD dwMouseMsg, BOOL fSetAlways);
    
        // IQuickActivate methods
        //
        STDMETHODIMP QuickActivate(QACONTAINER* pqacontainer, QACONTROL* pqacontrol);
        STDMETHODIMP SetContentExtent(LPSIZEL);
        STDMETHODIMP GetContentExtent(LPSIZEL);

        // IObjectSafety
        //
        STDMETHODIMP GetInterfaceSafetyOptions(REFIID riid,DWORD* pdwSupportedOptions,DWORD* pdwEnabledOptions);
        STDMETHODIMP SetInterfaceSafetyOptions(REFIID riid,DWORD dwOptionSetMask,DWORD dwEnabledOptions);

        // constructor and destructor
        //
        COControl(LPCTSTR        controlClassName,
                  LPUNKNOWN      pUnkOuter,
                  LPUNKNOWN      pInstance,
                  REFGUID        typelibguid,
                  REFIID         dispiid,
                  REFIID         coclassiid,
                  REFIID         outgoing=IID_NULL
                 );
        virtual ~COControl();
    
      protected:
    
        //=--------------------------------------------------------------------------=
        // member variables that derived controls can get at.
        //
        // derived controls Should NOT modify the following.
        //
        IOleClientSite*            m_pClientSite;             // client site
        IOleControlSite*           m_pControlSite;            // ICOControlSite ptr on client site
        IOleInPlaceSite*           m_pInPlaceSite;            // IOleInPlaceSite for managing activation
        IOleInPlaceSiteEx*         m_pInPlaceSiteEx;          // IOleInPlaceSiteEx for managing activation
        IOleInPlaceSiteWindowless* m_pInPlaceSiteWndless;     // IOleInPlaceSiteWindowless pointer
        IOleInPlaceFrame*          m_pInPlaceFrame;           // IOleInPlaceFrame ptr on client site
        IOleInPlaceUIWindow*       m_pInPlaceUIWindow;        // for negotiating border space with client
        ISimpleFrameSite*          m_pSimpleFrameSite;        // simple frame site
        IDispatch*                 m_pDispAmbient;            // ambient dispatch pointer
        Wnd*                       m_pWnd;
        SIZEL                      m_size;
        const COControlClass*      m_myControlClass;

        // does the control need to be resaved?
        bool     m_fDirty;                           
        
        // are we in place active or not?
        bool m_fInPlaceActive()                   
        { return m_pWnd!=NULL; }

        // are we UI active or not.
        bool m_fUIActive()
        { return m_pWnd!=NULL ? m_pWnd->GetFocus() : false; }
    
        //=--------------------------------------------------------------------------=
        // methods that derived controls can override, but may need to be called
        // from their versions.
        //
        virtual void      ViewChanged();
        virtual HRESULT   InternalQueryInterface(REFIID riid, void **ppvObjOut);

        virtual HRESULT   LoadProperties();
        virtual HRESULT   SaveProperties();
    
        //=--------------------------------------------------------------------------=
        // member functions that provide for derived controls, or that we use, but
        // derived controls might still find useful.
        //
        bool         DesignMode();
        bool         GetAmbientProperty(DISPID, VARTYPE, void *);
        bool         GetAmbientFont(IFont **ppFontOut);
        void         ModalDialog(bool fShow);
        bool         SetControlSize(SIZEL *pSizel);
        
        HRESULT      InPlaceActivate(LONG lVerb);
        void         SetInPlaceVisible(bool);
    
        void         QueryInPlaceSite();

        HRESULT      ShowPropertyPages();

        HWND GetParentWindow() const
        { return NULL!=m_pWnd ? m_pWnd->GetWindowParent() : NULL; }

        COPropertySet& GetPropertySet()
        { return m_propertySet; }

      private:
        STDMETHOD(OnDraw)(DWORD dvAspect, HDC hdcDraw, LPCRECTL prcBounds, LPCRECTL prcWBounds, HDC hicTargetDev, BOOL fOptimize) PURE;
        
        //=--------------------------------------------------------------------------=
        // OVERRIDABLES -- methods controls can implement for customized functionality
        //
        virtual void    AmbientPropertyChanged(DISPID dispid);
        virtual HRESULT DoCustomVerb(LONG lVerb);
        virtual bool    OnSpecialKey(LPMSG);
        virtual bool    OnGetPalette(HDC, LOGPALETTE **);
        virtual HRESULT OnQuickActivate(QACONTAINER *, DWORD *);
        virtual HRESULT InitializeControl();
        virtual bool    OnGetRect(DWORD dvAspect, LPRECTL prcRect);
        virtual void    OnVerb(LONG lVerb);
        virtual bool    CreateInPlaceWindow(HWND hWndParent,const RECT& rcPos);

        void    DestroyWindow();

        HRESULT LoadStandardProperties();
        HRESULT SaveStandardProperties();
    
        //=--------------------------------------------------------------------------=
        // member variables we don't want anybody to get their hands on, including
        // inheriting classes
        //
        LPCTSTR                m_controlClassName;
        IOleAdviseHolder*      m_pOleAdviseHolder;          // IOleObject::Advise holder object
        LPADVISESINK           m_pViewAdviseSink;           // IViewAdvise sink for IViewObject2
        unsigned short         m_nFreezeEvents;             // count of freezes versus thaws
        bool                   m_fModeFlagValid;            // we stash the mode as much as possible
        bool                   m_fSaveSucceeded;            // did an IStorage save work correctly?
        bool                   m_fViewAdvisePrimeFirst;     // for IViewobject2::setadvise
        bool                   m_fViewAdviseOnlyOnce;       // for iviewobject2::setadvise
        bool                   m_fRunMode;                  // are we in run mode or not?
        LCID                   m_lcid;
        COPropertySet          m_propertySet;

        static COControlClassM m_controlClass;
        static COVerbInfo      rgInPlaceVerbs[];
        static COVerbInfo      ovProperties;
        static COVerbInfo      ovUIActivate;
    }; // of class COControl
  } // of namespace COM
} // of namespace bvr20983
#endif // COCONTROL_H
/*==========================END-OF-FILE===================================*/
