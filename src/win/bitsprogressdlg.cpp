/*
 * $Id: dialog.cpp 52 2008-06-30 22:45:31Z dwachmann $
 * 
 * windows dialog class.
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
#include "bvr20983update-res.h"
#include <assert.h>
#include <commctrl.h>
#include <sstream>
#include "util/md5sum.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/comptr.h"
#include "win/bitsprogressdlg.h"
#include "exception/windowsexception.h"
#include "exception/lasterrorexception.h"

/**
 *
 */
static LPTSTR gBgStates[] =
{ _T("QUEUED"),
	_T("CONNECTING"),
	_T("TRANSFERRING"),
	_T("SUSPENDED"),
	_T("ERROR"),
	_T("TRANSIENT_ERROR"),
	_T("TRANSFERRED"),
	_T("ACKNOWLEDGED"),
	_T("CANCELLED")
};

using namespace bvr20983;
using namespace bvr20983::util;

namespace bvr20983
{
  namespace win
  {
    /*
     * 
     */
    BITSProgressDlg::BITSProgressDlg(auto_ptr<BackgroundTransfer>& btx) :
      m_btx(btx)
    { 
    } // of BITSProgressDlg::BITSProgressDlg()
    
    /*
     * 
     */
    BITSProgressDlg::~BITSProgressDlg()
    { }

    /*
     * 
     */
    int BITSProgressDlg::Show(HWND hWnd,HINSTANCE hInstance)
    { int dlgResult = Dialog::Show(MAKEINTRESOURCE(IDD_PROGRESS),hWnd,hInstance);

      return dlgResult;
    } // of 

    /**
     *
     */
    void BITSProgressDlg::Init(LPCTSTR jobName,LPCTSTR remoteName,LPCTSTR localName)
    { COMPtr<IBackgroundCopyJob> job;

      m_removeName = remoteName;
      m_localName  = localName;

      if( !m_btx->GetJob(jobName,m_jobId) )
      { m_btx->CreateJob(jobName,m_jobId);

        if( m_btx->GetJob(m_jobId,m_job) )
          THROW_COMEXCEPTION( m_job->AddFile(remoteName,localName) );
      } // of if
      else 
        m_btx->GetJob(m_jobId,m_job);
    } // of BITSProgressDlg::Init()


    /**
     *
     */
    void BITSProgressDlg::TriggerState()
    { BG_JOB_STATE    jobState;
      BG_JOB_PROGRESS jobProgress;

      THROW_COMEXCEPTION( m_job->GetState(&jobState) ); 
      THROW_COMEXCEPTION( m_job->GetProgress(&jobProgress) ); 

      switch( jobState )
      {
      case BG_JOB_STATE_SUSPENDED:
        THROW_COMEXCEPTION( m_job->Resume() ); 
        break;
      case BG_JOB_STATE_TRANSFERRING:
        { TCHAR msg[1024];
          WPARAM percent = (WPARAM) (jobProgress.BytesTransferred * 1000 / jobProgress.BytesTotal);

          ::_stprintf_s(msg,1023,_T("%s [%d %%]"),m_removeName.c_str(),percent/10);
        
          ::SendDlgItemMessage(m_hWnd,IDC_FILENAME,WM_SETTEXT,0,(LPARAM)msg);
          ::SendDlgItemMessage(m_hWnd,IDC_PROGRESS1,PBM_SETPOS,percent,0);
        }
        break;
      case BG_JOB_STATE_TRANSFERRED:
        THROW_COMEXCEPTION( m_job->Complete() );
         
        CalcChecksum();

        EndDialog(0);
        break;
      case BG_JOB_STATE_ERROR:
        THROW_COMEXCEPTION( m_job->Cancel() ); 

        EndDialog(-1);
        break;
      default:
        break;
      } // of switch()
    } // of BITSProgressDlg::TriggerState()


    /**
     *
     */
    void BITSProgressDlg::CalcChecksum()
    { COMPtr<IEnumBackgroundCopyFiles> files;

      THROW_COMEXCEPTION( m_job->EnumFiles(&files) );

      for( ;; )
      { COMPtr<IBackgroundCopyFile> aFile;

        if( S_OK==files->Next(1,&aFile,NULL) )
        { LPTSTR localFileName = NULL;

          if( SUCCEEDED(aFile->GetLocalName(&localFileName)) )
          { MD5Sum               md5sum;
            auto_ptr<CryptoHash> hash;
            auto_ptr<BYTE>       hashValue;
            DWORD                hashValueLen;

            md5sum.CalcFileHash(localFileName,hash);

            CryptoHash* pHash = hash.get();

            hashValueLen = pHash->Get(hashValue);

            if( hashValueLen>0 )
            { basic_ostringstream<TCHAR> hashValueStr;

              hashValueStr<<setfill(_T('0'))<<setw(2)<<hex;

              BYTE* pBuffer = hashValue.get();

              for( DWORD i=0;i<hashValueLen;i++ )
                hashValueStr<<pBuffer[i];

              OutputDebugFmt(_T("%s: md5{%ld}[%s]\n"),localFileName,hashValueLen,hashValueStr.str());
            } // of if

            ::CoTaskMemFree(localFileName);
          } // of if
        } // of if
        else
          break;
      } // of for
    } // of BITSProgressDlg::CalcChecksum()

    /**
     *
     */
    void BITSProgressDlg::OnTimer()
    { TriggerState();
    } // of BITSProgressDlg::OnTimer()

    /**
     *
     */
    BOOL BITSProgressDlg::InitDialog()
    { ::SendDlgItemMessage(m_hWnd,IDC_PROGRESS1,PBM_SETRANGE,0,MAKELPARAM (0,1000));
      ::SendDlgItemMessage(m_hWnd,IDC_PROGRESS1,PBM_SETPOS,0,0);
      ::SendDlgItemMessage(m_hWnd,IDC_FILENAME,WM_SETTEXT,0,(LPARAM) m_removeName.c_str());

      TriggerState();

      StartTimer(1000);
      
      return TRUE; 
    } // of BITSProgressDlg::InitDialog()
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
