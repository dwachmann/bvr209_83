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
        EventLogger(LPCTSTR serviceName,bool registerInRegistry=false);
        ~EventLogger();

        void logMessage(LPCTSTR logText);
        void logError(LPCTSTR errText);
        void logError(LPCTSTR errText, LPCTSTR extraText);
        void logEventMessage(LPCTSTR messageText, int messageType);
        void logFunctionError(LPCTSTR functionName);
        void logFunctionMessage(LPCTSTR functionName, LPCTSTR messageText);

      private:
        HANDLE m_hEventSource;
    }; // of class EventLogger
  } // of namespace util
} // of namespace bvr20983
#endif // __EVENT_LOGGER_H__
/*==========================END-OF-FILE===================================*/
