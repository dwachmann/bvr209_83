/*
 * $Id: $
 * 
 * A class for download files using the windows BITS service.
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
#include "os.h"
#include "util/backgroundtransfer.h"
#include "util/logstream.h"
#include "util/guid.h"
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"
#include <sstream>

using namespace bvr20983;

namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    BackgroundTransfer::BackgroundTransfer() :
      m_BITSManager(CLSID_BackgroundCopyManager,IID_IBackgroundCopyManager,CLSCTX_LOCAL_SERVER)
    { 
    }

    /**
     *
     */
    BackgroundTransfer::~BackgroundTransfer()
    { 
    }

    /**
     *
     */
    void BackgroundTransfer::CreateJob(LPCTSTR jobName,CGUID& jobId)
    { GUID                       bitsJobId;
      COMPtr<IBackgroundCopyJob> bitsJob;

      THROW_COMEXCEPTION( m_BITSManager->CreateJob(jobName, 
                                                   BG_JOB_TYPE_DOWNLOAD, 
                                                   &bitsJobId, 
                                                   &bitsJob
                                                  ) 
                        );

      jobId = bitsJobId;
    } // of BackgroundTransfer::CreateJob()

   /**
     *
     */
    bool BackgroundTransfer::GetJob(LPCTSTR displayName,CGUID& foundJobId)
    { bool                            found        = false;
      LPTSTR                          pDisplayName = NULL;
      COMPtr<IEnumBackgroundCopyJobs> pJobs;

      THROW_COMEXCEPTION( m_BITSManager->EnumJobs(0, &pJobs) );

      foundJobId = GUID_NULL;

      for( ;; )
      { COMPtr<IBackgroundCopyJob> pJob;
        HRESULT                    hr = pJobs->Next(1, &pJob, NULL);

        if( S_FALSE==hr )
          break;

        THROW_COMEXCEPTION(hr);

        pDisplayName = NULL;
        pJob->GetDisplayName(&pDisplayName);

        if( _tcscmp(displayName,pDisplayName)==0 )
        { GUID jobId = GUID_NULL;

          if( SUCCEEDED(pJob->GetId(&jobId)) )
          { foundJobId = jobId;
            found      = true;
          } // of if

          break;
        } // of if

        ::CoTaskMemFree(pDisplayName);
      } // of for

      ::CoTaskMemFree(pDisplayName);

      return found;
    } // of BackgroundTransfer::GetJob()

    /**
     *
     */
    bool BackgroundTransfer::GetJob(const CGUID& jobId,COMPtr<IBackgroundCopyJob>& job)
    { return m_BITSManager->GetJob(jobId,&job)==S_OK;
    } // of BackgroundTransfer::GetJob()
    
    /**
     *
     */
    void BackgroundTransfer::AddFile(LPCTSTR jobName,LPCTSTR url,LPCTSTR fileName)
    {
    }

    /**
     *
     */
    void BackgroundTransfer::Resume(LPCTSTR jobName)
    {
    }

    /**
     *
     */
    void BackgroundTransfer::Suspend(LPCTSTR jobName)
    {
    }

    /**
     *
     */
    void BackgroundTransfer::Cancel(LPCTSTR jobName)
    {
    }

    /**
     *
     */
    void BackgroundTransfer::Complete(LPCTSTR jobName)
    {
    }

    /**
     *
     */
    void BackgroundTransfer::List()
    { COMPtr<IEnumBackgroundCopyJobs> pJobs;
      ULONG                           cJobCount=0;

      THROW_COMEXCEPTION( m_BITSManager->EnumJobs(0, &pJobs) );

      pJobs->GetCount(&cJobCount);

      for( ;; )
      { COMPtr<IBackgroundCopyJob> pJob;
        HRESULT                    hr = pJobs->Next(1, &pJob, NULL);

        if( S_FALSE==hr )
          break;

        THROW_COMEXCEPTION(hr);

        LPTSTR  pDescription = NULL;
        LPTSTR  pDisplayName = NULL;
        GUID    jobId;
        TString sJobId;

        pJob->GetDescription(&pDescription);
        pJob->GetDisplayName(&pDisplayName);

        if( SUCCEEDED(pJob->GetId(&jobId)) )
          sJobId = CGUID(jobId);

        OutputDebugFmt(_T("%s %s [%s]\n"),sJobId.c_str(),pDisplayName,pDescription);

        ::CoTaskMemFree(pDescription);
        ::CoTaskMemFree(pDisplayName);
      } // of for

      OutputDebugFmt(_T("Listed %ld job(s).\n"),cJobCount);
    } // of BackgroundTransfer::List()

   } // of namespace util
} // of namespace bvr20983


/**
 * exportwrapper
 */
STDAPI_(void) BtxCreateJob(LPCTSTR jobName)
{ CGUID                              jobId;
  auto_ptr<util::BackgroundTransfer> btx(new util::BackgroundTransfer());
  
  btx->CreateJob(jobName,jobId); 
}
/*==========================END-OF-FILE===================================*/
