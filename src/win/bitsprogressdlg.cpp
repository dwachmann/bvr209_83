/*
 * $Id$
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
#include "util/combuffer.h"
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
    { m_finished = false;
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
    { m_removeName = remoteName;
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
    void BITSProgressDlg::SetProgress(WPARAM percent)
    { TCHAR msg[1024];

      ::_stprintf_s(msg,1023,_T("%s [%d %%]"),m_removeName.c_str(),percent/10);
    
      ::SendDlgItemMessage(m_hWnd,IDC_URL,WM_SETTEXT,0,(LPARAM)msg);
      ::SendDlgItemMessage(m_hWnd,IDC_PROGRESS1,PBM_SETPOS,percent,0);
    } // of BITSProgressDlg::SetProgress()


    /**
     *
     */
    void BITSProgressDlg::TriggerState(bool isInitial)
    { BG_JOB_STATE    jobState;
      BG_JOB_PROGRESS jobProgress;

      THROW_COMEXCEPTION( m_job->GetState(&jobState) ); 
      THROW_COMEXCEPTION( m_job->GetProgress(&jobProgress) ); 

      ::SendDlgItemMessage(m_hWnd,IDG_GROUP0,WM_SETTEXT,0,(LPARAM)gBgStates[jobState]);

      switch( jobState )
      {
      case BG_JOB_STATE_SUSPENDED:
        if( isInitial )
        { THROW_COMEXCEPTION( m_job->Resume() ); 

          ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),TRUE);
          ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),FALSE);
          ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),TRUE);
        }
        break;
      case BG_JOB_STATE_CONNECTING:
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),TRUE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),TRUE);
        break;
      case BG_JOB_STATE_TRANSFERRING:
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),TRUE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),TRUE);
        SetProgress( (WPARAM) (jobProgress.BytesTransferred * 1000 / jobProgress.BytesTotal) );
        break;
      case BG_JOB_STATE_TRANSFERRED:
        { TCHAR   msg[1024];
          TString fileHash;

          THROW_COMEXCEPTION( m_job->Complete() );

          ::SendDlgItemMessage(m_hWnd,IDC_PROGRESS1,PBM_SETBARCOLOR,0,RGB(255,255,0));
          SetProgress( 1000 );
         
          CalcChecksum(fileHash);

          ::_stprintf_s(msg,1023,_T("md5: %s"),fileHash.c_str());
          ::SendDlgItemMessage(m_hWnd,IDC_MD5,WM_SETTEXT,0,(LPARAM)msg);

          ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),FALSE);
          ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),FALSE);
          ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),FALSE);

          //EndDialog(0);
        }
        break;
      case BG_JOB_STATE_ACKNOWLEDGED:
      case BG_JOB_STATE_CANCELLED:
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),FALSE);
        StopTimer();

        StartTimer(15000);

        m_finished = true;
        break;
      // fall through
      case BG_JOB_STATE_TRANSIENT_ERROR:
      case BG_JOB_STATE_ERROR:
        THROW_COMEXCEPTION( m_job->Cancel() ); 

        ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),FALSE);
        StopTimer();

        StartTimer(15000);

        m_finished = true;
        break;
      default:
        break;
      } // of switch()
    } // of BITSProgressDlg::TriggerState()


    /**
     *
     */
    void BITSProgressDlg::CalcChecksum(TString& fileHash)
    { COMPtr<IEnumBackgroundCopyFiles> files;

      THROW_COMEXCEPTION( m_job->EnumFiles(&files) );

      for( ;; )
      { COMPtr<IBackgroundCopyFile> aFile;

        if( S_OK==files->Next(1,&aFile,NULL) )
        { COMBuffer<TCHAR> localFileName;

          if( SUCCEEDED(aFile->GetLocalName(&localFileName)) )
          { MD5Sum               md5sum;
            auto_ptr<CryptoHash> hash;
            
            md5sum.CalcFileHash(localFileName,hash);

            CryptoHash* pHash = hash.get();
            pHash->Get(fileHash);
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
    { if( !m_finished )
        TriggerState(false);
      else
      { StopTimer();
        EndDialog(0);
      } // of else
    } // of BITSProgressDlg::OnTimer()

    /**
     *
     */
    bool BITSProgressDlg::OnCommand(WPARAM command)
    { bool result  = false;

      switch( command )
      { 
      case IDC_STOP:
        THROW_COMEXCEPTION( m_job->Suspend() );
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),TRUE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),TRUE);
        break;
      case IDC_START:
        THROW_COMEXCEPTION( m_job->Resume() );
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),TRUE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),TRUE);
        break;
      case IDC_CANCEL:
        THROW_COMEXCEPTION( m_job->Cancel() );
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),FALSE);
        ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),FALSE);
        break;
        break;
      } // of switch

      return result;
    } // of BITSProgressDlg::OnCommand()


    /**
     *
     */
    BOOL BITSProgressDlg::InitDialog()
    { ::SendDlgItemMessage(m_hWnd,IDC_PROGRESS1,PBM_SETRANGE,0,MAKELPARAM (0,1000));
      ::SendDlgItemMessage(m_hWnd,IDC_PROGRESS1,PBM_SETPOS,0,0);
      ::SendDlgItemMessage(m_hWnd,IDC_FILENAME,WM_SETTEXT,0,(LPARAM) _T(""));
      ::SendDlgItemMessage(m_hWnd,IDC_MD5,WM_SETTEXT,0,(LPARAM) _T(""));
      ::SendDlgItemMessage(m_hWnd,IDC_URL,WM_SETTEXT,0,(LPARAM) m_removeName.c_str());
      ::SendDlgItemMessage(m_hWnd,IDC_FILENAME,WM_SETTEXT,0,(LPARAM) m_localName.c_str());
      ::EnableWindow(GetDlgItem(m_hWnd,IDC_START),FALSE);
      ::EnableWindow(GetDlgItem(m_hWnd,IDC_STOP),FALSE);
      ::EnableWindow(GetDlgItem(m_hWnd,IDC_CANCEL),FALSE);

      TriggerState(true);

      StartTimer(1000);
      
      return TRUE; 
    } // of BITSProgressDlg::InitDialog()
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
