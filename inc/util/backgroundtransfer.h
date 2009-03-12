/*
 * $Id: $
 * 
 * Copyright (C) 2009 Dorothea Wachmann
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
#ifndef BACKGROUNDTRANSFER_H
#define BACKGROUNDTRANSFER_H

#include <bits.h>
#include "util/comptr.h"

namespace bvr20983
{
  namespace util
  {
    class BackgroundTransfer
    {
      public:
        static BackgroundTransfer* GetInstance();
        static void                DeleteInstance();

        void CreateJob(LPCTSTR jobName);
        void AddFile(LPCTSTR jobName,LPCTSTR url,LPCTSTR fileName);
        void Resume(LPCTSTR jobName);
        void Suspend(LPCTSTR jobName);
        void Cancel(LPCTSTR jobName);
        void Complete(LPCTSTR jobName);

      private:
        BackgroundTransfer();
        ~BackgroundTransfer();

        static BackgroundTransfer* m_pMe;

        COMPtr<IBackgroundCopyManager> m_BITSManager;

    }; // of class BackgroundTransfer
  } // of namespace util
} // of namespace bvr20983

STDAPI_(void) BtxCreateJob(LPCTSTR jobName);
STDAPI_(void) BtxAddFile(LPCTSTR jobName,LPCTSTR url,LPCTSTR fileName);
STDAPI_(void) BtxResume(LPCTSTR jobName);
STDAPI_(void) BtxSuspend(LPCTSTR jobName);
STDAPI_(void) BtxCancel(LPCTSTR jobName);
STDAPI_(void) BtxComplete(LPCTSTR jobName);

#endif // BACKGROUNDTRANSFER_H
/*==========================END-OF-FILE===================================*/
