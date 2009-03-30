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
#if !defined(AUTOUPDATE_H)
#define AUTOUPDATE_H

#include "os.h"
#include "win/dialog.h"
#include "util/backgroundtransfer.h"

namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    class AutoUpdate
    {
      public:
        enum StateType
        { IDLE,
          VERSIONS,
          MSI
        };

        AutoUpdate(auto_ptr<util::BackgroundTransfer>& btx);
        virtual ~AutoUpdate();

        bool Init(HINSTANCE hModule,LPCTSTR baseURL);
        void Run();

      private:
        auto_ptr<util::BackgroundTransfer>& m_btx;
        CGUID                               m_jobId;
        COMPtr<IBackgroundCopyJob>          m_job;

        StateType                           m_state;
        TString                             m_baseURL;
        TString                             m_jobNameVersions;
        TString                             m_jobNameMSI;
        TString                             m_destinationDir;
        TString                             m_productPrefix;
        TString                             m_componentPrefix;
        VTString                            m_transferedFiles;

        void ReadVersionInfo(HINSTANCE hModule);
        bool GetFilePath(TString& filePath,LPCTSTR srcFileName);
        void CheckVersions();
        void CheckInstalledPackage();
        void InstallPackage();
        bool GetActualPackageInfo(TString& productCode,TString& packageCode,TString& packageName);
        void DumpPackageInfo(const TString& productCode);
    }; // of class AutoUpdate
  } // of namespace util
} // of namespace bvr20983
#endif // AUTOUPDATE_H
