/*
 * $Id$
 * 
 * Windows DC class.
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
#include "util/eventlogger.h"
#include "util/registry.h"
#include "com/comserver.h"
#include "util/logstream.h"
#include "msgs.h"

namespace bvr20983
{
  namespace util
  {
    EventLogger* EventLogger::m_pMe = NULL;

    /**
     *
     */
    EventLogger* EventLogger::CreateInstance(LPCTSTR serviceName)
    { if( m_pMe==NULL )
        m_pMe = new EventLogger(serviceName);
    
      return m_pMe; 
    }

    /**
     *
     */
    void EventLogger::DeleteInstance()
    { if( NULL!=m_pMe )
      { if( NULL!=m_pMe )
          delete m_pMe;
          
        m_pMe = NULL;
      } // of if
    }

    /**
     *
     */
    EventLogger::EventLogger(LPCTSTR serviceName) : m_hEventSource(NULL)
    { m_hEventSource = ::RegisterEventSource(NULL, serviceName);

	    if( NULL==m_hEventSource )
        throw "Failed to register service.";
    }

    /**
     *
     */
    EventLogger::~EventLogger()
    { if( NULL!=m_hEventSource )
        ::DeregisterEventSource(m_hEventSource);

      m_hEventSource = NULL;
    }

    /**
     *
     */
    void EventLogger::RegisterInRegistry(LPCTSTR serviceName)
    { TCHAR szModulePath[MAX_PATH];

      COM::COMServer::GetModuleFileName(szModulePath,sizeof(szModulePath)/sizeof(szModulePath[0]));

      if( _tcslen(szModulePath)>0 && NULL!=serviceName )
      { TString evtSrcRegKeyStr(_T("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"));
        evtSrcRegKeyStr += serviceName;

        RegistryKey k(evtSrcRegKeyStr);

        if( !k.Exists() )
        { Registry evtSrcRegKey(evtSrcRegKeyStr);

          OutputDebugFmt(_T("EventLogger::RegisterInRegistry(%s) <%s>\n"),serviceName,evtSrcRegKeyStr.c_str());

          evtSrcRegKey.SetKeyValue(NULL,_T("EventMessageFile"),szModulePath);
          evtSrcRegKey.SetKeyIntValue(NULL,_T("TypesSupported"),EVENTLOG_ERROR_TYPE|EVENTLOG_INFORMATION_TYPE|EVENTLOG_WARNING_TYPE);

          evtSrcRegKey.SetKeyValue(NULL,_T("CategoryMessageFile"),szModulePath);
          evtSrcRegKey.SetKeyIntValue(NULL,_T("CategoryCount"),3);
        } // of if
      } // of if
    } // of EventLogger::RegisterInRegistry()

    /**
     *
     */
    void EventLogger::UnregisterInRegistry(LPCTSTR serviceName)
    { if( NULL!=serviceName )
      { TString evtSrcRegKeyStr(_T("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"));
        evtSrcRegKeyStr += serviceName;

        RegistryKey k(evtSrcRegKeyStr);

        if( k.Exists() )
        { OutputDebugFmt(_T("EventLogger::UnregisterInRegistry(%s) <%s>\n"),serviceName,evtSrcRegKeyStr.c_str());

          k.Delete();
        } // of if
      } // of if
    } // of EventLogger::UnregisterInRegistry()

    /**
     *
     */
    void EventLogger::LogMessage(LPCTSTR logText)
    { LogEventMessage( logText, EVENT_GENERIC_INFORMATION); }


    /**
     *
     */
    void EventLogger::LogError(LPCTSTR errText)
    { LogEventMessage( errText, EVENT_GENERIC_ERROR,EVENTLOG_ERROR_TYPE); }

    /**
     *
     */
    void EventLogger::LogError(LPCTSTR errText, LPCTSTR extraText)
    { const int errTextLen   = _tcslen(errText) + 1;
	    const int extraTextLen = (extraText != NULL) ? _tcslen(extraText) : 0;
	    const int totalTextLen = errTextLen + extraTextLen;

	    LPTSTR fullText = (LPTSTR)calloc(totalTextLen,sizeof(TCHAR));
      ::memset(fullText, 0, totalTextLen);
	    _tcscpy_s(fullText,totalTextLen,errText);

      if( extraTextLen>0 )
		    _tcscpy_s(fullText + errTextLen,extraTextLen,extraText);

	    LogEventMessage( fullText, EVENT_GENERIC_ERROR,EVENTLOG_ERROR_TYPE);

	    free(fullText);
    }

    /**
     *
     */
    void EventLogger::LogEventMessage(LPCTSTR messageText, int messageType, int eventlogType)
    { if( m_hEventSource!=NULL )
	    { LPCTSTR messages[1] = { messageText };
		    
        ::ReportEvent(m_hEventSource, eventlogType, CAT_COMMON, messageType, NULL, 1, 0, messages, NULL);
	    }
    }

    /**
     *
     */
    void EventLogger::LogFunctionError(LPCTSTR functionName)
    { LPTSTR messageText = NULL;

      ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&messageText, 0, NULL);

	    LogFunctionMessage(functionName, messageText);

      ::LocalFree(messageText);
    }

    /**
     *
     */
    void EventLogger::LogFunctionMessage(LPCTSTR functionName, LPCTSTR messageText)
    { LPCTSTR messages[2] = { functionName, messageText };

	    if( m_hEventSource!=NULL )
        ::ReportEvent(m_hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_FUNCTION_FAILED, NULL, 2, 0, messages, NULL);
    }
  } // of namespace util
} // of namespace bvr20983


/**
 *
 */
STDAPI_(void) EvtLogMessage(LPCTSTR logText)
{ bvr20983::util::EventLogger::GetInstance()->LogMessage(logText); }

STDAPI_(void) EvtLogError(LPCTSTR logText)
{ bvr20983::util::EventLogger::GetInstance()->LogError(logText); }

STDAPI_(void) EvtLogError2(LPCTSTR logText, LPCTSTR extraText)
{ bvr20983::util::EventLogger::GetInstance()->LogError(logText,extraText); }

STDAPI_(void) EvtLogEventMessage(LPCTSTR messageText, int messageType)
{ bvr20983::util::EventLogger::GetInstance()->LogEventMessage(messageText,messageType); }

STDAPI_(void) EvtLogFunctionError(LPCTSTR functionName)
{ bvr20983::util::EventLogger::GetInstance()->LogFunctionError(functionName); }

STDAPI_(void) EvtLogFunctionMessage(LPCTSTR functionName, LPCTSTR messageText)
{ bvr20983::util::EventLogger::GetInstance()->LogFunctionMessage(functionName,messageText); }
/*==========================END-OF-FILE===================================*/
