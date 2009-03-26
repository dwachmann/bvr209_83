/*
 * $Id$
 * 
 * Copyright (C) 2008-2009 Dorothea Wachmann
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
#if !defined(BITSPROGRESSDLG_H)
#define BITSPROGRESSDLG_H

#include "os.h"
#include "win/dialog.h"
#include "util/backgroundtransfer.h"

namespace bvr20983
{
  namespace win
  {
    /**
     *
     */
    class BITSProgressDlg : public Dialog 
    {
      public:
        BITSProgressDlg(auto_ptr<util::BackgroundTransfer>& btx);
        virtual ~BITSProgressDlg();

        int Show(HWND hWnd,HINSTANCE hInstance=NULL);

        void Init(LPCTSTR jobName,LPCTSTR remoteName,LPCTSTR localName);

        

      protected:
        virtual BOOL InitDialog();
        virtual void OnTimer();
        virtual bool OnCommand(WPARAM command);

      private:
        auto_ptr<util::BackgroundTransfer>& m_btx;
        CGUID                               m_jobId;
        COMPtr<IBackgroundCopyJob>          m_job;

        TString                             m_removeName;
        TString                             m_localName;
        bool                                m_finished;

        void CalcChecksum(TString& fileHash);
        void SetProgress(WPARAM percent);
        void TriggerState(bool isInitial);
    }; // of class BITSProgressDlg
  } // of namespace win
} // of namespace bvr20983
#endif // BITSPROGRESSDLG_H
