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
#ifndef CODISPATCH_H
#define CODISPATCH_H

#include <vector>
#include "com/counknown.h"
#include "com/comutil.h"

#define DECLARE_DISPATCH \
  STDMETHODIMP GetTypeInfoCount(UINT *pctinfo) \
  { return CODispatch::GetTypeInfoCount(pctinfo); } \
  STDMETHODIMP GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo **ppTypeInfoOut) \
  { return CODispatch::GetTypeInfo(itinfo, lcid, ppTypeInfoOut); } \
  STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR **rgszNames, UINT cnames, LCID lcid, DISPID *rgdispid)\
  { return CODispatch::GetIDsOfNames(riid, rgszNames, cnames, lcid, rgdispid); } \
  STDMETHODIMP Invoke(DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispparams, VARIANT *pVarResult, EXCEPINFO *pexcepinfo, UINT *puArgErr) \
  { return CODispatch::Invoke(dispid, riid, lcid, wFlags, pdispparams, pVarResult, pexcepinfo, puArgErr); }

#define DECLARE_PROVIDECLASSINFO \
  STDMETHODIMP GetClassInfo(ITypeInfo** ppITI) \
  { return CODispatch::GetClassInfo(ppITI); } \
  STDMETHODIMP GetGUID(DWORD dwGuidKind,GUID * pGUID) \
  { return CODispatch::GetGUID(dwGuidKind,pGUID); }

#define DECLARE_SUPPORTERRORINFO \
  STDMETHODIMP InterfaceSupportsErrorInfo(REFIID riid) \
  { return CODispatch::InterfaceSupportsErrorInfo(riid); }

#define MAX_EVENT_ARGS 10

namespace bvr20983
{
  namespace COM
  {
    // describes an event
    struct COEventInfo
    { REFIID   iid;
      DISPID   dispid;
      int      cParameters;
      VARTYPE* rgTypes;
    };

    // a connectionpoint
    struct CODispatchCP
    { IConnectionPoint* m_pCP;
      IID               m_refiid;

      CODispatchCP(IConnectionPoint* pCP,REFIID refiid) :
        m_pCP(pCP),m_refiid(refiid)
      { }

      CODispatchCP(const CODispatchCP& cp) :
        m_pCP(cp.m_pCP),m_refiid(cp.m_refiid)
      { }
    };

    typedef std::vector<CODispatchCP> CODispatchCPV;

    class CODispatch : public COUnknown,public IProvideClassInfo2,public ISupportErrorInfo,public IConnectionPointContainer
    {
      public:
        CODispatch(LPUNKNOWN      pUnkOuter,
                   LPUNKNOWN      pInstance,
                   REFGUID        typelibguid,
                   REFIID         dispiid,
                   REFIID         coclassiid,
                   REFIID         outgoing=IID_NULL,
                   unsigned short majorVersion=1,
                   unsigned short minorVersion=0
                  );
        virtual ~CODispatch();

        DECLARE_UNKNOWN

        // IDispatch members
        STDMETHODIMP GetTypeInfoCount(UINT *);
        STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo **);
        STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR **, UINT, LCID, DISPID *);
        STDMETHODIMP Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS *, VARIANT *, EXCEPINFO *, UINT *);

        //IProvideClassInfo members
        STDMETHODIMP GetClassInfo(ITypeInfo **);
    
        //IProvideClassInfo2 members
        STDMETHODIMP GetGUID(DWORD dwGuidKind,GUID * pGUID);

        //ISupportErrorInfo
        STDMETHODIMP InterfaceSupportsErrorInfo(REFIID);

        //IConnectionPointContainer members
        STDMETHODIMP EnumConnectionPoints(IEnumConnectionPoints **);
        STDMETHODIMP FindConnectionPoint(REFIID, IConnectionPoint **);
    
        HRESULT __cdecl TriggerEvent(COEventInfo* pEventInfo, ...);
        HRESULT PropertyNotifyOnRequestEdit(DISPID dispid);
        HRESULT PropertyNotifyOnChanged(DISPID dispid);
        HRESULT AddConnectionPoint(REFIID riid,IConnectionPoint** ppCP);

        virtual HRESULT InternalQueryInterface(REFIID riid,PPVOID ppv);

        static HRESULT LoadTypeInfo(REFGUID typelibguid,REFIID refiid,
                                    ITypeInfo** ppITypeInfo,
                                    LCID lcid=LOCALE_NEUTRAL,unsigned short majorVersion=1,unsigned short minorVersion=0
                                   );

        LPUNKNOWN ExternalUnknown() const
        { return m_pInstance; }

        REFIID GetCOClassID() const
        { return m_coclassiid; }


      protected:
        HRESULT Exception(WORD wException);
        HRESULT Exception(UINT idsSource,HRESULT hr);

        static const BYTE m_rgcbDataTypeSize[];

      private:
        REFGUID               m_typelibguid;
        REFIID                m_dispiid;
        REFIID                m_coclassiid;
        REFIID                m_outgoing;
        unsigned short        m_majorVersion;
        unsigned short        m_minorVersion;
        ITypeInfo*            m_pITI;      
        LPUNKNOWN             m_pInstance;
        CODispatchCPV         m_connectionPoints;
    };
  } // of namespace COM
} // of namespace bvr20983

#endif //CODISPATCH_H
