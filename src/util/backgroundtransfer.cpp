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
#include "exception/lasterrorexception.h"
#include "exception/comexception.h"
#include <sstream>

using namespace bvr20983;

namespace bvr20983
{
  namespace util
  {
    BackgroundTransfer* BackgroundTransfer::m_pMe = NULL;

    /**
     *
     */
    BackgroundTransfer* BackgroundTransfer::GetInstance()
    { if( m_pMe==NULL )
        m_pMe = new BackgroundTransfer();
    
      return m_pMe; 
    }

    /**
     *
     */
    void BackgroundTransfer::DeleteInstance()
    { if( NULL!=m_pMe )
      { if( NULL!=m_pMe )
          delete m_pMe;
          
        m_pMe = NULL;
      } // of if
    }

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
    void BackgroundTransfer::CreateJob(LPCTSTR jobName)
    { GUID                       bitsJobId;
      COMPtr<IBackgroundCopyJob> bitsJob;

      HRESULT hr = m_BITSManager->CreateJob(jobName, 
                                            BG_JOB_TYPE_DOWNLOAD, 
                                            &bitsJobId, 
                                            &bitsJob
                                           );

      THROW_COMEXCEPTION( hr );
    }

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
  } // of namespace util
} // of namespace bvr20983


/**
 * exportwrapper
 */
STDAPI_(void) BtxCreateJob(LPCTSTR jobName)
{ bvr20983::util::BackgroundTransfer::GetInstance()->CreateJob(jobName); }

STDAPI_(void) BtxAddFile(LPCTSTR jobName,LPCTSTR url,LPCTSTR fileName)
{ bvr20983::util::BackgroundTransfer::GetInstance()->AddFile(jobName,url,fileName); }

STDAPI_(void) BtxResume(LPCTSTR jobName)
{ bvr20983::util::BackgroundTransfer::GetInstance()->Resume(jobName); }

STDAPI_(void) BtxSuspend(LPCTSTR jobName)
{ bvr20983::util::BackgroundTransfer::GetInstance()->Suspend(jobName); }

STDAPI_(void) BtxCancel(LPCTSTR jobName)
{ bvr20983::util::BackgroundTransfer::GetInstance()->Cancel(jobName); }

STDAPI_(void) BtxComplete(LPCTSTR jobName)
{ bvr20983::util::BackgroundTransfer::GetInstance()->Complete(jobName); }


#ifdef _UNICODE
#define _BitsCreateJob_ BitsCreateJobW
#else
#define _BitsCreateJob_ BitsCreateJobA
#endif

/**
 *
 */
void PrintBtxCreateJob(HWND hwnd)
{ basic_ostringstream<TCHAR> msgStream;
  msgStream<<_T("Usage: rundll32 <dllname>,BtxCreateJob jobname");

  TString msg = msgStream.str();

  ::MessageBox(hwnd,msg.c_str(),_T("BackgroundTransfer"),MB_OK | MB_ICONINFORMATION);
} // of PrintBtxCreateJob()

STDAPI_(void) _BitsCreateJob_(HWND hwnd, HINSTANCE hinst, LPWSTR lpszCmdLine,int nCmdShow)
{ ::CoInitialize(NULL);

  try
  { OutputDebugFmt(_T("BitsCreateJob(): <%s>\n"),lpszCmdLine);

    TCHAR jobname[MAX_PATH];

    ::memset(jobname,_T('\0'),MAX_PATH);

    int     i         = 0;
    boolean stop      = false;
    LPTSTR  nextToken = NULL;
    for( LPTSTR tok=_tcstok_s(lpszCmdLine,_T(" "),&nextToken);NULL!=tok && !stop;tok=_tcstok_s(NULL,_T(" "),&nextToken),i++ )
    {
      switch( i )
      { 
      case 0:
        _tcscpy_s(jobname,MAX_PATH,tok);
        stop = true;
        break;
      default:
        stop = true;
        break;
      } // of switch
    } // of for

    if( jobname[0]!=_T('\0') )
    { 
      BtxCreateJob(jobname);
      
    } // of if
    else 
      PrintBtxCreateJob(hwnd);
  }
  catch(BVR20983Exception e)
  { OutputDebugFmt(_T("BitsCreateJob(): Exception \"%s\" [%ld]>\n"),e.GetErrorMessage(),e.GetErrorCode());
  }
  catch(exception& e) 
  { OutputDebugFmt(_T("BitsCreateJob(): Exception <%s,%s>\n"),typeid(e).name(),e.what()); }
  catch(...)
  { OutputDebugFmt(_T("BitsCreateJob(): Exception\n")); }

  ::CoUninitialize();
} // of _BitsCreateJob_()
/*==========================END-OF-FILE===================================*/
