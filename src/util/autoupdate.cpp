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
#include <shlobj.h>
#include "cab/cabinetfdi.h"
#include "util/md5sum.h"
#include "util/logstream.h"
#include "util/versioninfo.h"
#include "util/autoupdate.h"
#include "util/verifyfile.h"
#include "util/md5sum.h"
#include "util/handle.h"
#include "exception/lasterrorexception.h"

using namespace bvr20983;
using namespace bvr20983::cab;

namespace bvr20983
{
  namespace util
  {
    /*
     * 
     */
    AutoUpdate::AutoUpdate(auto_ptr<BackgroundTransfer>& btx) :
      m_btx(btx)
    { 
    } // of AutoUpdate::AutoUpdate()
    
    /*
     * 
     */
    AutoUpdate::~AutoUpdate()
    { }

    /**
     *
     */
    bool AutoUpdate::Init(HINSTANCE hModule,LPCTSTR baseURL)
    { ReadVersionInfo(hModule);

      m_jobName = m_productPrefix;

      if( !GetFilePath(m_localMSIVersionsMD5,_T("msiversions.md5")) )
        return false;

      m_remoteMSIVersionsMD5  = baseURL;
      m_remoteMSIVersionsMD5 += _T("/");
      m_remoteMSIVersionsMD5 += _T("msiversions.md5");

      if( !GetFilePath(m_localMSIVersionsCAB,_T("msiversions.cab")) )
        return false;

      m_remoteMSIVersionsCAB  = baseURL;
      m_remoteMSIVersionsCAB += _T("/");
      m_remoteMSIVersionsCAB += _T("msiversions.cab");

      if( !m_btx->GetJob(m_jobName.c_str(),m_jobId) )
      { m_btx->CreateJob(m_jobName.c_str(),m_jobId);

        if( m_btx->GetJob(m_jobId,m_job) )
        { THROW_COMEXCEPTION( m_job->AddFile(m_remoteMSIVersionsMD5.c_str(),m_localMSIVersionsMD5.c_str()) );
          THROW_COMEXCEPTION( m_job->AddFile(m_remoteMSIVersionsCAB.c_str(),m_localMSIVersionsCAB.c_str()) );
        } // of if
      } // of if
      else 
        m_btx->GetJob(m_jobId,m_job);

      return true;
    } // of AutoUpdate::Init()


    /**
     *
     */
    void AutoUpdate::Run()
    { BG_JOB_STATE    jobState;
      BG_JOB_PROGRESS jobProgress;

      THROW_COMEXCEPTION( m_job->GetState(&jobState) ); 
      THROW_COMEXCEPTION( m_job->GetProgress(&jobProgress) ); 

      switch( jobState )
      {
      case BG_JOB_STATE_SUSPENDED:
        THROW_COMEXCEPTION( m_job->Resume() ); 
        break;
      case BG_JOB_STATE_CONNECTING:
        break;
      case BG_JOB_STATE_TRANSFERRING:
        break;
      case BG_JOB_STATE_TRANSFERRED:
        { THROW_COMEXCEPTION( m_job->Complete() );

          Install();
        }
        break;
      case BG_JOB_STATE_ACKNOWLEDGED:
      case BG_JOB_STATE_CANCELLED:
        break;
      // fall through
      case BG_JOB_STATE_TRANSIENT_ERROR:
      case BG_JOB_STATE_ERROR:
        THROW_COMEXCEPTION( m_job->Cancel() ); 
        break;
      default:
        break;
      } // of switch()
    } // of AutoUpdate::Run()

    /**
     *
     */
    bool AutoUpdate::GetFilePath(TString& filePath,LPCTSTR srcFileName)
    { bool  result = true;
      TCHAR path[MAX_PATH];

      if( SUCCEEDED( ::SHGetFolderPath(NULL,CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL,SHGFP_TYPE_CURRENT,path)) ) 
      { filePath  = path;
        filePath += _T("\\");
        filePath += m_productPrefix;

        //
        // directory doesnt exist
        //
        if( !::CreateDirectory(filePath.c_str(),NULL) && ::GetLastError()!=ERROR_ALREADY_EXISTS )
          result = false;
        
        filePath += _T("\\");
        filePath += m_componentPrefix;

        if( !::CreateDirectory(filePath.c_str(),NULL) && ::GetLastError()!=ERROR_ALREADY_EXISTS )
          result = false;

        if( NULL!=srcFileName )
        { filePath += _T("\\");
          filePath += srcFileName;
        } // of if
      } // of if
      else
        result = false;

      return result;
    } // of AutoUpdate::GetFilePath()


    /**
     *
     */
    void AutoUpdate::Install()
    { if( !MD5Sum::CheckHash(m_localMSIVersionsCAB.c_str(),m_localMSIVersionsMD5.c_str()) )
        LOGGER_WARN<<_T("File ")<<m_localMSIVersionsCAB<<_T(" is corrupted.");
      else 
      { if( VerifyFile::Verify(m_localMSIVersionsCAB.c_str()) )
        { TString destDir;

          GetFilePath(destDir,NULL);

          CabinetFDI cabinet(m_localMSIVersionsCAB.c_str(),destDir.c_str());
        
          cabinet.Extract();
        } // of if
        else
          LOGGER_WARN<<_T("File ")<<m_localMSIVersionsCAB<<_T(" could not be verified.");
      } // of else
    } // of AutoUpdate::Install()


    /**
     *
     */
    void AutoUpdate::ReadVersionInfo(HINSTANCE hModule)
    { util::VersionInfo verInfo(hModule);

      LPVOID prodPrefix = verInfo.GetStringInfo(_T("ProductPrefix"));
      LPVOID compPrefix = verInfo.GetStringInfo(_T("ComponentPrefix"));

      if( NULL!=prodPrefix )
        m_productPrefix = (LPCTSTR)prodPrefix;

      if( NULL!=compPrefix )
        m_componentPrefix = (LPCTSTR)compPrefix;
    } // of AutoUpdate::ReadVersionInfo()
   } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
