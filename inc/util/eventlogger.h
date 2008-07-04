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
#ifndef EVENTLOGGER_H
#define EVENTLOGGER_H

namespace bvr20983
{
  namespace util
  {
    class EventLogger
    {
      public:
        static EventLogger* CreateInstance(LPCTSTR serviceName);
        static void         DeleteInstance();
        static EventLogger* GetInstance()
        { return m_pMe; }

        static void RegisterInRegistry(LPCTSTR serviceName);
        static void UnregisterInRegistry(LPCTSTR serviceName);

        void LogMessage(LPCTSTR logText);
        void LogError(LPCTSTR errText);
        void LogError(LPCTSTR errText, LPCTSTR extraText);
        void LogEventMessage(LPCTSTR messageText, int messageType);
        void LogFunctionError(LPCTSTR functionName);
        void LogFunctionMessage(LPCTSTR functionName, LPCTSTR messageText);

      private:
        EventLogger(LPCTSTR serviceName);
        ~EventLogger();

        static EventLogger* m_pMe;

        HANDLE m_hEventSource;
    }; // of class EventLogger
  } // of namespace util
} // of namespace bvr20983
#endif // __EVENT_LOGGER_H__
/*==========================END-OF-FILE===================================*/
