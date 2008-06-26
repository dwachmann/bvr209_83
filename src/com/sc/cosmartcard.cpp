/*
 * $Id$
 * 
 * COSmartcard COM Object Class.
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
#include "com/sc/cosmartcard.h"
#include "com/sc/cowallet.h"
#include "com/sc/coatr.h"
#include "com/cocollection.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "exception/bvr20983exception.h"
#include "bvr20983-dispid.h"
#include "bvr20983.h"

#define LOGGER_INFO_THREAD  LogStreamT::GetLogger(_T("cosmartcardthread.cpp"))<<INF<<setlineno(__LINE__)
#define LOGGER_DEBUG_THREAD LogStreamT::GetLogger(_T("cosmartcardthread.cpp"))<<DBG<<setlineno(__LINE__)
#define LOGGER_TRACE_THREAD LogStreamT::GetLogger(_T("cosmartcardthread.cpp"))<<TRC<<setlineno(__LINE__)
#define LOGGER_WARN_THREAD  LogStreamT::GetLogger(_T("cosmartcardthread.cpp"))<<WRN<<setlineno(__LINE__)
#define LOGGER_ERROR_THREAD LogStreamT::GetLogger(_T("cosmartcardthread.cpp"))<<ERR<<setlineno(__LINE__)
#define LOGGER_FATAL_THREAD LogStreamT::GetLogger(_T("cosmartcardthread.cpp"))<<FTL<<setlineno(__LINE__)

static VARTYPE rgIUnk[] = { VT_UNKNOWN };

namespace bvr20983
{
  namespace COM
  {

     /**
      *
      */
     COEventInfo COSmartcard::m_eventInfo[] =
     { { DIID_DISmartcardEvent,DISPID_ONINSERTED,0,NULL },
       { DIID_DISmartcardEvent,DISPID_ONREMOVED,0,NULL },
       { DIID_DISmartcardEvent,DISPID_ONRECEIVED,ARRAYSIZE(rgIUnk),rgIUnk }
     };

#pragma region Construction & Deconstruction
     /**
      *
      */
     HRESULT COSmartcard::Create(REFCLSID clsid,LPUNKNOWN* ppv,LPUNKNOWN pUnkOuter)
     { HRESULT hr = NOERROR;
 
       if( NULL==ppv )
         hr = E_POINTER;
       else
       { COSmartcard* pCOSmartcard = new COSmartcard(pUnkOuter);
 
         if( NULL==pCOSmartcard )
           hr = E_OUTOFMEMORY;
         else
           *ppv = pCOSmartcard->PrivateUnknown();
       } // of else

       return hr;
     } // of COSmartcard::Create()


    /*
     * COSmartcard::COSmartcard
     *
     * Purpose:
     *   Constructor.
     *
     * Parameters:
     *   IUnknown* pUnkOuter   Pointer to the the outer Unknown.  NULL means this COM Object
     *                         is not being Aggregated.  Non NULL means it is being created
     *                         on behalf of an outside COM object that is reusing it via
     *                         aggregation.
     *   COMServer* pServer    Pointer to the server's control object.
     *
     * Return: 
     *   void
     */
#pragma warning(disable:4355)
    COSmartcard::COSmartcard(IUnknown* pUnkOuter) 
      : CODispatch(pUnkOuter,(ISmartcard*)this,LIBID_BVR20983SC,IID_ISmartcard,CLSID_Smartcard,DIID_DISmartcardEvent),
        m_workingThread(this,true)
    { m_pApplications         = NULL;
      m_pRegisteredCards      = NULL;
      m_pReaders              = NULL;
      m_pSmartCard            = NULL;

      m_workingThread.StartThread();
    }
#pragma warning(default:4355)
    
    /*
     *
     */
    COSmartcard::~COSmartcard()
    { LOGGER_DEBUG<<_T("COSmartcard::~COSmartcard()")<<endl; 

      m_workingThread.StopThread();

      RELEASE_INTERFACE(m_pApplications);
      RELEASE_INTERFACE(m_pRegisteredCards);
      RELEASE_INTERFACE(m_pReaders);

      LOGGER_DEBUG<<_T("COSmartcard::~COSmartcard() terminated")<<endl;
    }
#pragma endregion

#pragma region ISmartcard
    /**
     *
     */
    STDMETHODIMP COSmartcard::get_ATR(IUnknown **ppATR)
    { HRESULT hr = NOERROR;

      if( NULL==ppATR )
        hr = Exception(BVR_SOURCE_SMARTCARD,E_POINTER);
      else 
      { try
        { Critical crit(m_SmartCardCS);

          *ppATR = NULL;

          if( NULL==m_pSmartCard )
            hr = Exception(BVR_DISP_SC_NOTINITED);
          else if( !m_pSmartCard->IsCardPresent() )
            hr = Exception(BVR_DISP_SC_NOTCONNECTED);
          else
          { COATR* pATR = new COATR();

            ATR atr;
            m_pSmartCard->GetATR(atr);

            pATR->SetATR(atr);

            *ppATR = pATR->ExternalUnknown();
          } // of if
        }
        catch(BVR20983Exception e)
        { LOGGER_ERROR<<e<<endl;
          
          hr = e.GetErrorCode();
        }
        catch(...)
        { hr = Exception(BVR_SOURCE_SMARTCARD,E_FAIL);
        }
      } // of else
    
      return hr;
    } // of COSmartcard::GetATR()

    /**
     *
     */
    STDMETHODIMP COSmartcard::get_CardPresent(VARIANT_BOOL *pIsCardPresent)
    { HRESULT hr = NOERROR;

      *pIsCardPresent = VARIANT_FALSE;

      try
      { Critical crit(m_SmartCardCS);

        if( NULL==pIsCardPresent )
          hr = Exception(BVR_SOURCE_SMARTCARD,E_POINTER);
        else if( NULL==m_pSmartCard )
          hr = Exception(BVR_DISP_SC_NOTINITED);
        else
          *pIsCardPresent = m_pSmartCard->IsCardPresent() ? VARIANT_TRUE : VARIANT_FALSE;
      }
      catch(BVR20983Exception e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_SMARTCARD,E_FAIL);
      }

      LOGGER_DEBUG<<_T("COSmartcard::CImpISmartcard::get_CardPresent. Called. state=")<<*pIsCardPresent<<endl;

      return hr;
    } // of COSmartcard::GetCardPresent()


    /**
     *
     */
    STDMETHODIMP COSmartcard::Attribute(Attributes attrib,BSTR *pAttribute)
    { HRESULT hr = NOERROR;

      if( NULL==pAttribute )
        hr = Exception(BVR_SOURCE_SMARTCARD,E_POINTER);

      return hr;
    } // of COSmartcard::Attribute()

    /**
     *
     */
    STDMETHODIMP COSmartcard::Feature(Features feature,VARIANT_BOOL *pHasFeature)
    { HRESULT hr = NOERROR;

      if( NULL==pHasFeature )
        hr = Exception(BVR_SOURCE_SMARTCARD,E_POINTER);

      return hr;
    } // of COSmartcard::Feature()

    /**
     *
     */
    STDMETHODIMP COSmartcard::WaitForInsertEvent(unsigned short timeInSeconds)
    { HRESULT hr = NOERROR;

      try
      { Critical crit(m_SmartCardCS);

        if( NULL==m_pSmartCard )
          hr = Exception(BVR_DISP_SC_NOTINITED);
        else
          m_pSmartCard->WaitForInsertEvent(timeInSeconds);
      } 
      catch(BVR20983Exception e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_SMARTCARD,E_FAIL);
      }

      return hr;
    } // of COSmartcard::WaitForInsertEvent()

    /**
     *
     */
    STDMETHODIMP COSmartcard::WaitForRemoveEvent(unsigned short timeInSeconds)
    { HRESULT hr = NOERROR;
      
      try
      { Critical crit(m_SmartCardCS);

        if( NULL==m_pSmartCard )
          hr = Exception(BVR_DISP_SC_NOTINITED);
        else
          m_pSmartCard->WaitForRemoveEvent(timeInSeconds);
      } 
      catch(BVR20983Exception e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_SMARTCARD,E_FAIL);
      }
    
      return hr;
    } // of COSmartcard::WaitForRemoveEvent()

    /*
     * COSmartcard::GetApplications
     *
     * Purpose:
     *   returns a collection of registered smartcard applications
     *
     * Return:
     *   IUnknown** collection object
     */
    STDMETHODIMP COSmartcard::get_Applications(IUnknown** retval)
    { HRESULT hr = NOERROR;
      
      try
      { if( NULL==retval )
          hr = Exception(BVR_SOURCE_SMARTCARD,E_POINTER);
        else
        { Critical crit(m_SmartCardCS);
          
          *retval = NULL;
        
          LOGGER_DEBUG<<_T("COSmartcard::GetApplications().1")<<endl;
        
          if( NULL==m_pApplications )
          { CoCollection* pApplications = new CoCollection();
        
            if( NULL!=pApplications )
            { IWallet* pIWallet = NULL;
            
              LOGGER_DEBUG<<_T("COSmartcard::GetApplications().2")<<endl;
              
              COWallet* pWallet = new COWallet(NULL);
              
              pWallet->QueryInterface(IID_IWallet,(VOID**)&pIWallet);
              
              pApplications->Add(pIWallet);
      
              m_pApplications = pApplications;
              
              pIWallet->Release();
            } // of if
            else
              hr = Exception(BVR_SOURCE_SMARTCARD,E_OUTOFMEMORY);
          } // of if
            
          if( NULL!=m_pApplications )
          { m_pApplications->AddRef();
      
            *retval = m_pApplications;
      
            LOGGER_DEBUG<<_T("COSmartcard::GetApplications().3")<<endl;
          } // of if
        } // of else
      }
      catch(BVR20983Exception e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_SMARTCARD,E_FAIL);
      }
    
      return hr;
    }
    
    /*
     * COSmartcard::get_RegisteredCards
     *
     * Purpose:
     *   returns a collection of registered cards
     *
     * Return:
     *   IUnknown** collection object
     */
    STDMETHODIMP COSmartcard::get_RegisteredCards(IUnknown** retval)
    { HRESULT hr = NOERROR;
      
      try
      { Critical crit(m_SmartCardCS);

        if( NULL==retval )
          hr = Exception(BVR_SOURCE_SMARTCARD,E_POINTER);
        else 
        { Critical crit(m_SmartCardCS);

          if( NULL==m_pSmartCard )
            hr = BVR_DISP_SC_NOTINITED;
          else
          { *retval = NULL;
          
            LOGGER_DEBUG<<_T("COSmartcard::GetRegisteredCard().1")<<endl;
          
            if( NULL==m_pRegisteredCards )
            { CoCollection* pRegisteredCards = new CoCollection();
          
              if( NULL!=pRegisteredCards )
              { VTString cards = VTString();

                m_pSmartCard->ListCards(cards);

                VTString::iterator iter;
                
                for( iter=cards.begin();iter!=cards.end();iter++ )
                  pRegisteredCards->Add(iter->c_str());
                
                m_pRegisteredCards = pRegisteredCards;
              } // of if
              else
                hr = Exception(BVR_SOURCE_SMARTCARD,E_OUTOFMEMORY);
            } // of if
              
            if( NULL!=m_pRegisteredCards )
            { m_pRegisteredCards->AddRef();
        
              *retval = m_pRegisteredCards;
        
              LOGGER_DEBUG<<_T("COSmartcard::GetRegisteredCards().3")<<endl;
            } // of if
          } // of else
        } // of else
      }
      catch(BVR20983Exception e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_SMARTCARD,E_FAIL);
      }
    
      return hr;
    }
    
    /*
     * COSmartcard::get_Readers
     *
     * Purpose:
     *   returns a collection of registered readers
     *
     * Return:
     *   IUnknown** collection object
     */
    STDMETHODIMP COSmartcard::get_Readers(IUnknown** retval)
    { HRESULT hr = NOERROR;
      
      try
      { if( NULL==retval )
          hr = Exception(BVR_SOURCE_SMARTCARD,E_POINTER);
        else 
        { Critical crit(m_SmartCardCS);
      
          if( NULL==m_pSmartCard )
            hr = Exception(BVR_DISP_SC_NOTINITED);
          else
          { *retval = NULL;
          
            LOGGER_DEBUG<<_T("COSmartcard::GetReaders().1")<<endl;
          
            if( NULL==m_pReaders )
            { CoCollection* pReaders = new CoCollection();
          
              if( NULL!=pReaders )
              { VTString readers = VTString();
                
                m_pSmartCard->ListReaders(readers);

                VTString::iterator iter;

                for( iter=readers.begin();iter!=readers.end();iter++ )
                  pReaders->Add(iter->c_str());

                m_pReaders = pReaders;
                m_pReaders->AddRef();
              } // of if
              else
                hr = Exception(BVR_SOURCE_SMARTCARD,E_OUTOFMEMORY);
            } // of if
              
            if( NULL!=m_pReaders )
            { m_pReaders->AddRef();
        
              *retval = m_pReaders;
        
              LOGGER_DEBUG<<_T("COSmartcard::GetReaders().3")<<endl;
            } // of if
          } // of else
        } // of else
      }
      catch(BVR20983Exception e)
      { LOGGER_ERROR<<e<<endl;
        
        hr = e.GetErrorCode();
      }
      catch(...)
      { hr = Exception(BVR_SOURCE_SMARTCARD,E_FAIL);
      }
    
      return hr;
    }
#pragma endregion
    
#pragma region Events
    /*
     * COSmartcard::OnInserted
     *
     * Purpose:
     *   fire callback for card inserted event
     *
     */
    void COSmartcard::OnInserted()
    { TriggerEvent( &m_eventInfo[Event_OnInserted] ); }
    
    /*
     * COSmartcard::OnRemoved
     *
     * Purpose:
     *   fire callback for card removed event
     *
     */
    void COSmartcard::OnRemoved()
    { TriggerEvent( &m_eventInfo[Event_OnRemoved] ); }
#pragma endregion

#pragma region Smartcard Thread
    /**
     * called from working thread
     */
    bool COSmartcard::InitThread()
    { bool    result = false;
      HRESULT hr     = ::CoInitializeEx(NULL,COINIT_MULTITHREADED);
    
      if( SUCCEEDED(hr) )
      { Critical crit(m_SmartCardCS);
      
        m_pSmartCard = new Smartcard();

        result = true;
      } // of if
      else
      { LOGGER_DEBUG_THREAD<<_T("COSmartcard::InitThread(): ")<<setHR<<CHResult(hr)<<endl; }
      
      return result;
    } // of COSmartcard::InitThread()

    /**
     * called from working thread
     */
    void COSmartcard::ExitThread(HRESULT hr)  
    { { Critical crit(m_SmartCardCS);

        DELETE_POINTER(m_pSmartCard);
      }

      ::CoUninitialize();
    } // of COSmartcard::ExitThread()

    /**
     *
     */
    void COSmartcard::RunThread(HANDLE hWaitEvent)
    { 
      ::Sleep(5000);
      OnInserted();
      ::Sleep(3000);
      OnRemoved();
    } // of Run()
  } // of namespace COM
} // of namespace bvr20983
#pragma endregion
/*==========================END-OF-FILE===================================*/
