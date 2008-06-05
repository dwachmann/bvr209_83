/*
 * $Id$
 * 
 * Dual IDispatch COM Object Class.
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
#include <stdarg.h>
#include "com/codispatch.h"
#include "com/comserver.h"
#include "com/cocollection.h"
#include "com/coconnectionpoint.h"
#include "com/coenum.h"
#include "com/comutil.h"
#include "com/cotypeinfo.h"
#include "util/logstream.h"
#include "util/comlogstream.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace COM
  {
    // this table is used for copying data around, and persisting properties.
    // basically, it contains the size of a given data type
    //
    const BYTE CODispatch::m_rgcbDataTypeSize[] = 
    { 0,                      // VT_EMPTY= 0,
      0,                      // VT_NULL= 1,
      sizeof(short),          // VT_I2= 2,
      sizeof(long),           // VT_I4 = 3,
      sizeof(float),          // VT_R4  = 4,
      sizeof(double),         // VT_R8= 5,
      sizeof(CURRENCY),       // VT_CY= 6,
      sizeof(DATE),           // VT_DATE = 7,
      sizeof(BSTR),           // VT_BSTR = 8,
      sizeof(IDispatch *),    // VT_DISPATCH    = 9,
      sizeof(SCODE),          // VT_ERROR    = 10,
      sizeof(VARIANT_BOOL),   // VT_BOOL    = 11,
      sizeof(VARIANT),        // VT_VARIANT= 12,
      sizeof(IUnknown *),     // VT_UNKNOWN= 13,
    };

#pragma region Construction & Deconstruction
    /**
     *
     */
    CODispatch::CODispatch(LPUNKNOWN      pUnkOuter,
                           LPUNKNOWN      pInstance,
                           REFGUID        typelibguid,
                           REFIID         dispiid,
                           REFIID         coclassiid,
                           REFIID         outgoing,
                           unsigned short majorVersion,
                           unsigned short minorVersion
                          ) 
      : COUnknown(pUnkOuter),
        m_pInstance(pInstance),
        m_typelibguid(typelibguid),
        m_dispiid(dispiid),
        m_coclassiid(coclassiid),
        m_outgoing(outgoing),
        m_majorVersion(majorVersion),
        m_minorVersion(minorVersion),
        m_pITI(NULL)
    { AddConnectionPoint(m_outgoing!=IID_NULL ? m_outgoing : IID_IDispatch,NULL);
    }
    
    /**
     *
     */
    CODispatch::~CODispatch()
    { RELEASE_INTERFACE(m_pITI); 
    }
#pragma endregion

#pragma region InternalQI
    /**
     *
     */
    HRESULT CODispatch::InternalQueryInterface(REFIID  riid,PPVOID ppv)
    { HRESULT hr = NOERROR;
    
      if( NULL==ppv )
        hr = E_POINTER;
      else
      { *ppv = NULL;

        if( m_dispiid==riid || IID_IDispatch==riid )
        { *ppv = (IDispatch*)m_pInstance;
        
          LOGGER_DEBUG<<_T("CODispatch::InternalQueryInterface: IID_IDispatch==riid")<<endl;
        }
        else if( IID_ISupportErrorInfo==riid )
        { *ppv = (ISupportErrorInfo*)this;
      
          LOGGER_DEBUG<<_T("CODispatch::InternalQueryInterface: IID_ISupportErrorInfo==riid")<<endl;
        } 
        else if( IID_IProvideClassInfo==riid || IID_IProvideClassInfo2==riid )
        { *ppv = (IProvideClassInfo2*)this;
      
          LOGGER_DEBUG<<_T("CODispatch::InternalQueryInterface: IID_IProvideClassInfo2==riid")<<endl;
        } 
        else if( IID_IConnectionPointContainer==riid )
        { *ppv = (IConnectionPointContainer*)this;
      
          LOGGER_DEBUG<<_T("CODispatch::InternalQueryInterface: IID_IConnectionPointContainer==riid")<<endl;
        } 
      
        if( NULL!=*ppv )
          ((LPUNKNOWN)(*ppv))->AddRef();
        else
          hr = COUnknown::InternalQueryInterface(riid,ppv);
      } // of else
      
      return hr;
    }
#pragma endregion

#pragma region IDispatch
    /**
     *
     */
    STDMETHODIMP CODispatch::GetTypeInfoCount(UINT *pctInfo)
    { *pctInfo=1;
    
      LOGGER_DEBUG<<_T("CODispatch::GetTypeInfoCount(*pctInfo=")<<*pctInfo<<_T(")")<<endl;
     
      return NOERROR;
    }
    
    /*
     *
     */
    STDMETHODIMP CODispatch::GetTypeInfo(UINT itInfo,LCID lcid,ITypeInfo** ppITypeInfo)
    { HRESULT     hr = NOERROR;
    
      if( 0!=itInfo )
        return TYPE_E_ELEMENTNOTFOUND;
    
      if( NULL==m_pITI )
        hr = LoadTypeInfo(m_typelibguid,m_dispiid,&m_pITI,lcid);

      *ppITypeInfo = m_pITI;

      if( SUCCEEDED(hr) && NULL!=*ppITypeInfo )
        (*ppITypeInfo)->AddRef();

      LOGGER_DEBUG<<_T("CODispatch::GetTypeInfo(itInfo=")<<itInfo<<_T(",lcid=")<<lcid<<_T("): ")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of CODispatch::GetTypeInfo()
    
    /*
     *
     */
    STDMETHODIMP CODispatch::GetIDsOfNames(REFIID    riid, 
                                           OLECHAR** rgszNames, 
                                           UINT      cNames, 
                                           LCID      lcid, 
                                           DISPID*   rgDispID
                                          )
    { HRESULT     hr=NOERROR;
      ITypeInfo  *pTI;
    
      if( IID_NULL!=riid )
        return DISP_E_UNKNOWNINTERFACE;

      LOGGER_DEBUG<<_T("CODispatch::GetIDsOfNames(")<<_T("lcid=")<<lcid<<_T(")")<<endl;

      //Get the right ITypeInfo for lcid.
      hr = GetTypeInfo(0, lcid, &pTI);

      if( SUCCEEDED(hr) )
      { hr = ::DispGetIDsOfNames(pTI, rgszNames, cNames, rgDispID);
        pTI->Release();
      } // of if

      if( SUCCEEDED(hr) )
      { for( UINT i=0;i<cNames;i++ )
        { LOGGER_DEBUG<<rgszNames[i]<<_T(":")<<dec<<rgDispID[i]<<endl;
        } // of for
      }
      else
      { for( UINT i=0;i<cNames;i++ )
        { LOGGER_DEBUG<<rgszNames[i]<<endl;
        } // of for
      }

      LOGGER_DEBUG<<_T("CODispatch::GetIDsOfNames(): hr=")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of CODispatch::GetIDsOfNames()
    
    /*
     * CODispatch::Invoke
     *
     * Purpose:
     *  Calls a method in the dispatch interface or manipulates a
     *  property.
     *
     * Parameters:
     *  dispID          DISPID of the method or property of interest.
     *  riid            REFIID reserved, must be IID_NULL.
     *  lcid            LCID of the locale.
     *  wFlags          USHORT describing the context of the invocation.
     *  pDispParams     DISPPARAMS * to the array of arguments.
     *  pVarResult      VARIANT * in which to store the result.  Is
     *                  NULL if the caller is not interested.
     *  pExcepInfo      EXCEPINFO * to exception information.
     *  puArgErr        UINT * in which to store the index of an
     *                  invalid parameter if DISP_E_TYPEMISMATCH
     *                  is returned.
     *
     * Return Value:
     *  HRESULT         NOERROR or a general error code.
     */
    STDMETHODIMP CODispatch::Invoke(DISPID         dispID, 
                                    REFIID         riid, 
                                    LCID           lcid, 
                                    unsigned short wFlags, 
                                    DISPPARAMS*    pDispParams, 
                                    VARIANT*       pVarResult, 
                                    EXCEPINFO*     pExcepInfo, 
                                    UINT*          puArgErr
                                   )
    { HRESULT     hr;
      ITypeInfo*  pTI;
      LANGID      langID=PRIMARYLANGID(lcid);

      //riid is supposed to be IID_NULL always
      if( IID_NULL!=riid )
        return DISP_E_UNKNOWNINTERFACE;

      LOGGER_DEBUG<<_T("CODispatch::Invoke(dispID=0x")<<hex<<dispID<<_T(",wFlags=")<<wFlags<<_T(")")<<endl;
    
      //Get the ITypeInfo for lcid
      hr = GetTypeInfo(0, lcid, &pTI);
    
      if( SUCCEEDED(hr) )
      { COMServer::SetLangID(&langID);
      
        //Clear exceptions
        ::SetErrorInfo(0L, NULL);
      
        //This is exactly what DispInvoke does--so skip the overhead.
        hr = pTI->Invoke(m_pInstance, dispID, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
      
        //Exception handling is done within ITypeInfo::Invoke
      
        pTI->Release();
      } // of if

      LOGGER_DEBUG<<_T("CODispatch::Invoke():")<<setHR<<CHResult(hr)<<endl;

      return hr;
    } // of CODispatch::Invoke()
#pragma endregion

#pragma region IProvideClassInfo
    /*
     *
     */
    STDMETHODIMP CODispatch::GetClassInfo(ITypeInfo **ppITI)
    { HRESULT hr = E_FAIL;
    
      hr = LoadTypeInfo(m_typelibguid,m_coclassiid,ppITI);
        
      LOGGER_DEBUG<<_T("CODispatch::GetClassInfo(): ")<<m_coclassiid<<_T(" hr=")<<setHR<<CHResult(hr)<<endl;
    
      return hr;
    } // of CODispatch::GetClassInfo()
    
    
    /*
     *
     */
    STDMETHODIMP CODispatch::GetGUID(DWORD dwGuidKind,GUID* pGUID)
    { HRESULT hr = S_OK;
    
      if( NULL==pGUID )
        hr = E_POINTER;
      else if( GUIDKIND_DEFAULT_SOURCE_DISP_IID!=dwGuidKind )
        hr = E_INVALIDARG;
      else if( IID_NULL==m_outgoing )
        hr = E_UNEXPECTED;
      else
        *pGUID = m_outgoing;
    
      LOGGER_DEBUG<<_T("CODispatch::GetGUID(): ")<<*pGUID<<endl;
    
      return hr;
    } // of CODispatch::GetGUID()
#pragma endregion

#pragma region Helper
    /**
     *
     */
    STDMETHODIMP CODispatch::InterfaceSupportsErrorInfo(REFIID riid)
    { if( m_dispiid==riid )
        return NOERROR;
    
      return S_FALSE;
    }

    /*
     *
     */
    HRESULT CODispatch::LoadTypeInfo(REFGUID typelibguid,REFIID refiid,
                                     ITypeInfo** ppITypeInfo,
                                     LCID lcid,unsigned short majorVersion,unsigned short minorVersion
                                    )
    { HRESULT    hr        = NOERROR;
      ITypeLib*  pITypeLib = NULL;
      ITypeInfo* pTypeInfo = NULL;

      if( NULL==ppITypeInfo )
        hr = E_POINTER;
      else
      { LOGGER_DEBUG<<_T("CODispatch::LoadTypeInfo(refiid=")<<refiid<<_T(", lcid=")<<lcid<<_T(", majorVersion=")<<majorVersion<<_T(", minorVersion=")<<minorVersion<<_T(") try to load type library")<<endl;
    
        //hr = ::LoadRegTypeLib(typelibguid, majorVersion, minorVersion, PRIMARYLANGID(lcid), &pITypeLib);
        hr = ::LoadRegTypeLib(typelibguid, majorVersion, minorVersion, LOCALE_NEUTRAL, &pITypeLib);
        
        LOGGER_DEBUG<<_T("CODispatch::LoadTypeInfo() load result 0x")<<setHR<<CHResult(hr)<<endl;

        if( FAILED(hr) )
        { hr = ::LoadRegTypeLib(typelibguid, majorVersion, minorVersion, LOCALE_NEUTRAL, &pITypeLib);

          LOGGER_DEBUG<<_T("CODispatch::LoadTypeInfo() load result 0x")<<setHR<<CHResult(hr)<<endl;
        } // of if

        if( SUCCEEDED(hr) )
        { hr = pITypeLib->GetTypeInfoOfGuid(refiid, &pTypeInfo);

          if( SUCCEEDED(hr) )
            *ppITypeInfo = new COTypeInfo(pTypeInfo);

          pITypeLib->Release();
        } // of if
      } // of else

      return hr;
    } // of CODispatch::LoadTypeInfo()

    /*
     * CODispatch::Exception
     *
     * Purpose:
     *  Raises an exception for Invoke from within ITypeInfo::Invoke
     *  using the CreateErrorInfo API and the ICreateErrorInfo interface.
     *
     * Parameters:
     *  wException      WORD exception code.
     */
    HRESULT CODispatch::Exception(WORD wException)
    { HRESULT             hr;
      ICreateErrorInfo*   pICreateErr;
      BOOL                fSuccess;
      LPOLESTR            pszHelp;
      UINT                idsSource;
      UINT                idsException;
      DWORD               dwHelpID;
      LANGID              langID=COMServer::GetLangID();
    
      /*
       * Thread-safe exception handling means that we call
       * CreateErrorInfo which gives us an ICreateErrorInfo pointer
       * that we then use to set the error information (basically
       * to set the fields of an EXCEPINFO structure.  We then
       * call SetErrorInfo to attach this error to the current
       * thread.  ITypeInfo::Invoke will look for this when it
       * returns from whatever function was invokes by calling
       * GetErrorInfo.
       */
    
      if( FAILED(::CreateErrorInfo(&pICreateErr)) )
        return E_FAIL;
    
      auto_ptr<TCHAR> psz=auto_ptr<TCHAR>(new TCHAR[1024]);
    
      if( NULL==psz.get() )
      { pICreateErr->Release();

        return E_FAIL;
      } // of if
    
      fSuccess=FALSE;
      
      pICreateErr->SetGUID(m_dispiid);
    
      idsSource    = BVR_DISP_SOURCE(wException);
      idsException = BVR_DISP_EXP(wException);
      dwHelpID     = HID_EXCEPTION_MIN    + wException;
      idsSource    = IDS_EXCEPTIONSRC_MIN + idsSource;
      idsException = IDS_EXCEPTIONMSG_MIN + idsException;
    
      switch (langID)
      { case LANG_GERMAN:
          pszHelp      = (LPOLESTR)HELPFILE_7;
          break;
        case LANG_ENGLISH:
        case LANG_NEUTRAL:
        default:
          pszHelp      = (LPOLESTR)HELPFILE_0;
          break;
      }
      fSuccess=TRUE;
    
      if (fSuccess)
      { IErrorInfo *pIErr;
    
        /*
         * If you have a help file, call the functions
         * ICreateErrorInfo::SetHelpFile and
         * ICreateErrorInfo::SetHelpContext as well.  If you
         * set the help file to NULL the context is ignored.
         */
        pICreateErr->SetHelpFile(pszHelp);
        pICreateErr->SetHelpContext(dwHelpID);

        COMServer* pServer = COMServer::GetInstance();
    
        if( pServer!=NULL )
        { pServer->LoadString(idsSource, psz.get(), 1024);
          pICreateErr->SetSource((LPOLESTR)psz.get());
      
          pServer->LoadString(idsException, psz.get(), 1024);
          pICreateErr->SetDescription((LPOLESTR)psz.get());
        } // of if
    
        hr=pICreateErr->QueryInterface(IID_IErrorInfo, (PPVOID)&pIErr);
    
        if( SUCCEEDED(hr) )
        { SetErrorInfo(0L, pIErr);
          pIErr->Release();
        } // of if
      }
    
      //SetErrorInfo holds the object's IErrorInfo
      pICreateErr->Release();

      return MAKE_BVR_ERROR(wException);
    }

    /*
     * CODispatch::Exception
     *
     * Purpose:
     *  Raises an exception for Invoke from within ITypeInfo::Invoke
     *  using the CreateErrorInfo API and the ICreateErrorInfo interface.
     *
     * Parameters:
     *  UINT       idsSource
     *  HRESULT    hr
     */
    HRESULT CODispatch::Exception(UINT idsSource,HRESULT hrResult)
    { ICreateErrorInfo*   pICreateErr;
      HRESULT             hr;
      LANGID              langID=COMServer::GetLangID();
    
      if( FAILED(::CreateErrorInfo(&pICreateErr)) )
        return E_FAIL;
    
      pICreateErr->SetGUID(m_dispiid);

      auto_ptr<TCHAR> psz=auto_ptr<TCHAR>(new TCHAR[1024]);

      idsSource = IDS_EXCEPTIONSRC_MIN + idsSource;

      IErrorInfo *pIErr;
    
      COMServer* pServer = COMServer::GetInstance();
    
      if( pServer!=NULL )
      { pServer->LoadString(idsSource, psz.get(), 1024);
        
        pICreateErr->SetSource((LPOLESTR)psz.get());
      } // of if
  
      hr=pICreateErr->QueryInterface(IID_IErrorInfo, (PPVOID)&pIErr);
  
      if( SUCCEEDED(hr) )
      { ::SetErrorInfo(0L, pIErr);
        pIErr->Release();
      }

      //SetErrorInfo holds the object's IErrorInfo
      pICreateErr->Release();

      return hrResult;
    }
#pragma endregion

#pragma region IConnectionPoint
    /*
     *
     */
    HRESULT CODispatch::AddConnectionPoint(REFIID riid,IConnectionPoint** ppCP)
    { HRESULT                       hr  = NOERROR;
      IConnectionPoint*             pCP = NULL;
      CODispatchCPV::const_iterator i;

      for( i=m_connectionPoints.begin();i!=m_connectionPoints.end();i++ )
        if( i->m_refiid==riid )
        { pCP = i->m_pCP;

          hr = S_FALSE;

          break;
        } // of if

      if( i==m_connectionPoints.end() )
      { pCP = new COConnectionPoint(PrivateUnknown(),m_typelibguid, riid);

        m_connectionPoints.push_back(CODispatchCP(pCP,riid));
      } // of if

      if( NULL!=ppCP )
        *ppCP = pCP;
    
      return hr;
    } // of CODispatch::AddConnectionPoint()

    /*
     *
     */
    STDMETHODIMP CODispatch::EnumConnectionPoints(LPENUMCONNECTIONPOINTS *ppEnum)
    { HRESULT                 hr = NOERROR;

      if( NULL==ppEnum )
        hr = E_POINTER;
      else 
      { COEnumConnectionPoints* pEnum = new COEnumConnectionPoints(IID_IEnumConnectionPoints);

        if( NULL==pEnum )
          hr = E_OUTOFMEMORY;
        else
        { *ppEnum=NULL;
          
          CODispatchCPV::const_iterator i;

          for( i=m_connectionPoints.begin();i!=m_connectionPoints.end();i++ )
            pEnum->Add((IConnectionPoint*)i->m_pCP);
        
          *ppEnum=pEnum;
        } // of else
      } // of else

      return hr;
    } // of CODispatch::EnumConnectionPoints()
    
    /*
     *
     */
    STDMETHODIMP CODispatch::FindConnectionPoint(REFIID riid, IConnectionPoint **ppCP)
    { HRESULT hr = NOERROR;
    
      LOGGER_DEBUG<<_T("CODispatch::FindConnectionPoint(")<<riid<<_T(")")<<endl;

      if( NULL==ppCP )
        hr = E_POINTER;
      else
      { *ppCP=NULL;

        CODispatchCPV::const_iterator i;

        for( i=m_connectionPoints.begin();i!=m_connectionPoints.end();i++ )
          if( i->m_refiid==riid || IID_IDispatch==riid )
          { hr = i->m_pCP->QueryInterface(IID_IConnectionPoint, (PPVOID)ppCP);
          
            break;
          } // of if

        if( i==m_connectionPoints.end() )
          hr = E_NOINTERFACE;
      } // of else
    
      LOGGER_DEBUG<<_T("CODispatch::FindConnectionPoint(): ")<<setHR<<CHResult(hr)<<endl;
    
      return hr;
    } // of CODispatch::FindConnectionPoint()
    
    /*
     *
     */
    HRESULT __cdecl CODispatch::TriggerEvent(COEventInfo* pEventInfo, ...)
    { va_list             valist;
      DISPPARAMS          dispparams;
      VARIANT             rgvParameters[MAX_EVENT_ARGS];
      VARIANT*            pv;
      VARTYPE             vt;
      int                 iParameter;
      int                 cbSize;
      IEnumConnections*   pEnum;
      CONNECTDATA         cd;
      LCID                lcid = LOCALE_SYSTEM_DEFAULT;
      VARIANTARG          va;
      EXCEPINFO           exInfo;
      HRESULT             hr=NOERROR;

      if( NULL==pEventInfo )
        return E_POINTER;

      assert(pEventInfo->cParameters <= MAX_EVENT_ARGS);

      va_start(valist, pEventInfo);

      SETNOPARAMS(dispparams);

      // copy the Parameters into the rgvParameters array.  make sure we reverse
      // them for automation
      //
      if( pEventInfo->cParameters>0 )
      { pv = &(rgvParameters[pEventInfo->cParameters - 1]);
        for( iParameter=0;iParameter<pEventInfo->cParameters;iParameter++ ) 
        { vt = pEventInfo->rgTypes[iParameter];

          if( vt==VT_VARIANT )
            *pv = va_arg(valist, VARIANT);
          else 
          { pv->vt = vt;

            if( vt&VT_BYREF )
              cbSize = sizeof(void *);
            else
              cbSize = m_rgcbDataTypeSize[vt];

            if( cbSize==sizeof(short) )
              V_I2(pv) = va_arg(valist, short);
            else if( cbSize==4 )
              V_I4(pv) = va_arg(valist, long);
            else 
            { assert(cbSize == 8);

              V_CY(pv) = va_arg(valist, CURRENCY);
            }
          } // of else

          pv--;
        } // of for

        dispparams.rgvarg = rgvParameters;
        dispparams.cArgs  = pEventInfo->cParameters;
      } // of if

      CODispatchCPV::const_iterator i;

      for( i=m_connectionPoints.begin();i!=m_connectionPoints.end();i++ )
      { if( i->m_refiid==pEventInfo->iid )
          break;
      } // of for

      if( i==m_connectionPoints.end() )
        return E_POINTER;

      LOGGER_DEBUG<<_T("CODispatch::TriggerEvent(0x")<<hex<<pEventInfo->dispid<<_T(")")<<endl;

      hr = i->m_pCP->EnumConnections(&pEnum);
    
      if( FAILED(hr) )
        return hr;

      while( NOERROR==pEnum->Next(1, &cd, NULL) )
      { IDispatch* pIDispatch = NULL;
      
        if( NULL==cd.pUnk )
          continue;

        HRESULT hr = cd.pUnk->QueryInterface(IID_IDispatch,(PPVOID)&pIDispatch);
        
        if( FAILED(hr) )
          continue;

        INITEXCEPINFO(exInfo);
        
        LOGGER_DEBUG<<_T("CODispatch::TriggerEvent() dispid=0x")<<hex<<pEventInfo->dispid<<_T(" lcid=0x")<<lcid<<endl;
        
        hr = pIDispatch->Invoke(pEventInfo->dispid,IID_NULL,lcid,DISPATCH_METHOD,&dispparams,&va,&exInfo,NULL);
        
        if( hr==DISP_E_EXCEPTION  )
        { LOGGER_DEBUG<<_T("CODispatch::TriggerEvent() wCode=0x")<<hex<<exInfo.wCode<<endl;
          LOGGER_DEBUG<<_T("CODispatch::TriggerEvent() bstrSource=")<<exInfo.bstrSource<<endl;
        } // of if
        else
          LOGGER_DEBUG<<_T("CODispatch::TriggerEvent().1: ")<<setHR<<CHResult(hr)<<endl;

        pIDispatch->Release();

        if( FAILED(hr) )
          break;
      } // of while
    
      pEnum->Release();
      
      va_end(valist);

      return hr;
    } // of CODispatch::TriggerEvent()

    /**
     *
     */
    HRESULT CODispatch::PropertyNotifyOnChanged(DISPID dispid)
    { HRESULT           hr    = NOERROR;
      IEnumConnections* pEnum = NULL;
      CONNECTDATA       cd;

      CODispatchCPV::const_iterator i;

      for( i=m_connectionPoints.begin();i!=m_connectionPoints.end();i++ )
      { if( i->m_refiid==IID_IPropertyNotifySink )
          break;
      } // of for

      if( i==m_connectionPoints.end() )
        return E_POINTER;

      hr = i->m_pCP->EnumConnections(&pEnum);
    
      if( FAILED(hr) )
        return hr;
        
      while( NOERROR==pEnum->Next(1, &cd, NULL) )
      { IPropertyNotifySink* pPropNotifySink = NULL;
      
        if( NULL==cd.pUnk || FAILED( cd.pUnk->QueryInterface(IID_IPropertyNotifySink,(PPVOID)&pPropNotifySink) ) )
          continue;

        hr = pPropNotifySink->OnChanged(dispid);
        
        pPropNotifySink->Release();

        if( FAILED(hr) )
          break;
      } // of while
    
      pEnum->Release();
      
      return hr;
    } // of CODispatch::PropertyNotifyOnChanged()

    /**
     *
     */
    HRESULT CODispatch::PropertyNotifyOnRequestEdit(DISPID dispid)
    { HRESULT           hr    = NOERROR;
      IEnumConnections* pEnum = NULL;
      CONNECTDATA       cd;

      CODispatchCPV::const_iterator i;

      for( i=m_connectionPoints.begin();i!=m_connectionPoints.end();i++ )
      { if( i->m_refiid==IID_IPropertyNotifySink )
          break;
      } // of for

      if( i==m_connectionPoints.end() )
        return E_POINTER;

      hr = i->m_pCP->EnumConnections(&pEnum);
    
      if( FAILED(hr) )
        return hr;
        
      while( NOERROR==pEnum->Next(1, &cd, NULL) )
      { IPropertyNotifySink* pPropNotifySink = NULL;
      
        if( NULL==cd.pUnk || FAILED( cd.pUnk->QueryInterface(IID_IPropertyNotifySink,(PPVOID)&pPropNotifySink) ) )
          continue;

        hr = pPropNotifySink->OnRequestEdit(dispid);
        
        pPropNotifySink->Release();

        if( FAILED(hr) )
          break;
      } // of while
    
      pEnum->Release();
      
      return hr;
    } // of CODispatch::PropertyNotifyOnRequestEdit()
#pragma endregion
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
