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
#include "msgs.h"

namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    EventLogger::EventLogger(LPCTSTR serviceName,bool registerInRegistry)
      : m_hEventSource(NULL)
    { 
      if( registerInRegistry )
      { TCHAR szModulePath[MAX_PATH];

        szModulePath[0] = _T('0');

        ::GetModuleFileName(NULL,szModulePath,sizeof(szModulePath)/sizeof(szModulePath[0]));

        if( _tcslen(szModulePath)>0 && NULL!=serviceName )
        { TString evtSrcRegKeyStr(_T("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\"));
          evtSrcRegKeyStr += serviceName;

          RegistryKey k(evtSrcRegKeyStr);

          if( !k.Exists() )
          { Registry evtSrcRegKey(evtSrcRegKeyStr);

            evtSrcRegKey.SetKeyValue(NULL,_T("EventMessageFile"),szModulePath);
            evtSrcRegKey.SetKeyIntValue(NULL,_T("TypesSupported"),EVENTLOG_ERROR_TYPE|EVENTLOG_INFORMATION_TYPE|EVENTLOG_WARNING_TYPE);
          } // of if
        } // of if
      } // of if
      
      m_hEventSource = ::RegisterEventSource(NULL, serviceName);

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
    void EventLogger::logMessage(LPCTSTR logText)
    { logEventMessage( logText, EVENT_GENERIC_INFORMATION); }


    /**
     *
     */
    void EventLogger::logError(LPCTSTR errText)
    { logEventMessage( errText, EVENT_GENERIC_ERROR); }

    /**
     *
     */
    void EventLogger::logError(LPCTSTR errText, LPCTSTR extraText)
    { const int errTextLen   = _tcslen(errText) + 1;
	    const int extraTextLen = (extraText != NULL) ? _tcslen(extraText) : 0;
	    const int totalTextLen = errTextLen + extraTextLen;

	    LPTSTR fullText = (LPTSTR)calloc(totalTextLen,sizeof(TCHAR));
      ::memset(fullText, 0, totalTextLen);
	    _tcscpy_s(fullText,totalTextLen,errText);

      if( extraTextLen>0 )
		    _tcscpy_s(fullText + errTextLen,extraTextLen,extraText);

	    logEventMessage( fullText, EVENT_GENERIC_ERROR);

	    free(fullText);
    }

    /**
     *
     */
    void EventLogger::logEventMessage(LPCTSTR messageText, int messageType)
    { if (m_hEventSource != NULL)
	    { LPCTSTR messages[1] = { messageText };
		    
        ::ReportEvent(m_hEventSource, EVENTLOG_INFORMATION_TYPE, 0, messageType, NULL, 1, 0, messages, NULL);
	    }
    }

    /**
     *
     */
    void EventLogger::logFunctionError(LPCTSTR functionName)
    { LPTSTR messageText = NULL;

      ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&messageText, 0, NULL);

	    logFunctionMessage(functionName, messageText);

      ::LocalFree(messageText);
    }

    /**
     *
     */
    void EventLogger::logFunctionMessage(LPCTSTR functionName, LPCTSTR messageText)
    { LPCTSTR messages[2] = { functionName, messageText };

	    if (m_hEventSource != NULL)
        ::ReportEvent(m_hEventSource, EVENTLOG_ERROR_TYPE, 0, EVENT_FUNCTION_FAILED, NULL, 2, 0, messages, NULL);
    }
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
