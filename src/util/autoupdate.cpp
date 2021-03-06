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
#include "util/xmldocument.h"
#include "util/msi.h"
#include "util/dirinfo.h"
#include "util/combuffer.h"
#include "com/covariant.h"
#include "exception/lasterrorexception.h"

using namespace bvr20983;
using namespace bvr20983::cab;
using namespace bvr20983::util;
using namespace bvr20983::COM;

/**
 * Files:
 * MD5: the file that contains the md5 checksum of cabinet file
 * CAB: the signed cabinet file that contains the msiversions.xml file
 * XML: the msiversions.xml file that contains the version info
 * MSI: the MSI package
 *
 * (1) check if CAB exists
 *   yes: download MD5
 *        check, if MD5 checksum is equal to md5 checksum of CAB
 *        yes: EXIT, product is uptodate
 *    no: download MD5 and CAB
 * (2) check if MD5 checksum is equal to md5 checksum of CAB and if CAB signature is valid
 *    no: CAB corrupted: EXIT
 *   yes: extract CAB
 *        use msiversions.xml to get information about actual versions
 * (3) check if actual version is installed
 *   yes: EXIT
 *    no: download actual MSI and install it
 */
namespace bvr20983
{
  namespace util
  {
    /*
     * 
     */
    AutoUpdate::AutoUpdate(auto_ptr<BackgroundTransfer>& btx) :
      m_btx(btx)
    { m_state = IDLE;
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

      if( !GetFilePath(m_destinationDir,NULL) )
        return false;

      m_baseURL               = baseURL;

      m_jobNameVersions       = m_productPrefix;
      m_jobNameVersions      += _T(".versions");

      m_jobNameMSI            = m_productPrefix;
      m_jobNameMSI           += _T(".msi");

      if( m_btx->GetJob(m_jobNameMSI.c_str(),m_jobId) )
        m_state = MSI;
      else if( m_btx->GetJob(m_jobNameVersions.c_str(),m_jobId) )
        m_state = VERSIONS;

      if( m_state==IDLE )
      { TString localMSIVersionsMD5(m_destinationDir);
        TString localMSIVersionsCAB(m_destinationDir);
        TString remoteMSIVersionsMD5(baseURL);
        TString remoteMSIVersionsCAB(baseURL);

        localMSIVersionsMD5 += _T("\\msiversions.md5");
        localMSIVersionsCAB += _T("\\msiversions.cab");

        remoteMSIVersionsMD5 += _T("/msiversions.md5");
        remoteMSIVersionsCAB += _T("/msiversions.cab");
        
        m_btx->CreateJob(m_jobNameVersions.c_str(),m_jobId);

        if( m_btx->GetJob(m_jobId,m_job) )
        { bool md5Exists = DirectoryInfo::_IsFile(localMSIVersionsMD5.c_str());
          bool cabExists = DirectoryInfo::_IsFile(localMSIVersionsCAB.c_str());

          if( !cabExists )
          { THROW_COMEXCEPTION( m_job->AddFile(remoteMSIVersionsMD5.c_str(),localMSIVersionsMD5.c_str()) );
            THROW_COMEXCEPTION( m_job->AddFile(remoteMSIVersionsCAB.c_str(),localMSIVersionsCAB.c_str()) );
          } // of if
          else
          { 
            THROW_COMEXCEPTION( m_job->AddFile(remoteMSIVersionsMD5.c_str(),localMSIVersionsMD5.c_str()) );
          }
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
        { BackgroundTransfer::EnumFiles(m_job,m_transferedFiles);

          THROW_COMEXCEPTION( m_job->Complete() );

          if( m_state==VERSIONS )
            CheckVersions();
          else
            InstallPackage();
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
    void AutoUpdate::CheckVersions()
    { TString localMSIVersionsCAB(m_destinationDir);

      localMSIVersionsCAB += _T("\\msiversions.cab");
          
      if( m_transferedFiles.size()==1 )
      { if( MD5Sum::CheckHash(localMSIVersionsCAB.c_str(),m_transferedFiles[0].c_str()) )
        { CheckInstalledPackage();
        } // of if
        else
        { TString localMSIVersionsMD5(m_destinationDir);
          TString remoteMSIVersionsMD5(m_baseURL);
          TString remoteMSIVersionsCAB(m_baseURL);

          localMSIVersionsMD5  += _T("\\msiversions.md5");
          remoteMSIVersionsMD5 += _T("/msiversions.md5");
          remoteMSIVersionsCAB += _T("/msiversions.cab");
          
          m_btx->CreateJob(m_jobNameVersions.c_str(),m_jobId);

          m_job.Release();

          if( m_btx->GetJob(m_jobId,m_job) )
          { THROW_COMEXCEPTION( m_job->AddFile(remoteMSIVersionsMD5.c_str(),localMSIVersionsMD5.c_str()) );
            THROW_COMEXCEPTION( m_job->AddFile(remoteMSIVersionsCAB.c_str(),localMSIVersionsCAB.c_str()) );

            THROW_COMEXCEPTION( m_job->Resume() );
          } // of if
        } // of if
      } // of if
      else
      { if( m_transferedFiles.size()==2 &&
            MD5Sum::CheckHash(m_transferedFiles[1].c_str(),m_transferedFiles[0].c_str()) &&
            VerifyFile::Verify(m_transferedFiles[1].c_str()) 
          )
        { CabinetFDI cabinet(m_transferedFiles[1].c_str(),m_destinationDir.c_str());
          cabinet.Extract();

          CheckInstalledPackage();
        } // of if
      } // of else
    } // of AutoUpdate::CheckVersions()

    /**
     *
     */
    bool AutoUpdate::GetActualPackageInfo(TString& productCode,TString& packageCode,TString& packageName)
    { bool                    result = false;
      XMLDocument             xmlDoc;
      COMPtr<IXMLDOMNodeList> pXMLDomNodeList;
      COMPtr<IXMLDOMNode>     pNode;
      COVariant               msiProductCode;
      COVariant               msiPackageCode;
      COVariant               msiPackageName;
      TString                 msiversions(m_destinationDir);
      
      msiversions += _T("\\msiversions.xml");

      productCode.clear();
      packageCode.clear();
      packageName.clear();
          
      if( DirectoryInfo::_IsFile(msiversions.c_str()) &&
          xmlDoc.Load(msiversions.c_str()) && 
          xmlDoc.GetNodeValue(_T("//v:msiversions//v:package[1]//v:productcode//text()"),msiProductCode,true) &&
          xmlDoc.GetNodeValue(_T("//v:msiversions//v:package[1]//v:packagecode//text()"),msiPackageCode,true) &&
          xmlDoc.GetNodeValue(_T("//v:msiversions//v:package[1]//v:name//text()"),msiPackageName,true)
        )
      { productCode = V_BSTR(msiProductCode);
        packageCode = V_BSTR(msiPackageCode);
        packageName = V_BSTR(msiPackageName);

        result = true;
      } // of if

      return result;
    } // of AutoUpdate::GetActualPackageInfo()

    /**
     *
     */
    void AutoUpdate::CheckInstalledPackage()
    { TString productCode;
      TString packageCode;
      TString packageName;
          
      if( GetActualPackageInfo(productCode,packageCode,packageName) &&
          ( !MSIProduct::IsInstalled(productCode.c_str()) ||
            !MSIProduct::IsPackageInstalled(productCode.c_str(),packageCode.c_str())
          )
        )
      { LOGGER_INFO<<packageName.c_str()<<_T(": download and install ")<<productCode.c_str()
                                        <<_T(" packagecode:")<<packageCode.c_str()<<endl;

        TString remoteMSIPackage(m_baseURL);
        TString localMSIPackage(m_destinationDir);

        remoteMSIPackage += _T("/");
        remoteMSIPackage += packageName;
        localMSIPackage  += _T("\\");
        localMSIPackage  += packageName;

        m_btx->CreateJob(m_jobNameMSI.c_str(),m_jobId);

        m_job.Release();

        if( m_btx->GetJob(m_jobId,m_job) )
        { THROW_COMEXCEPTION( m_job->AddFile(remoteMSIPackage.c_str(),localMSIPackage.c_str()) );
          THROW_COMEXCEPTION( m_job->Resume() );
        } // of if
      } // of else
    } // of AutoUpdate::CheckInstalledPackage()

    /**
     *
     */
    void AutoUpdate::InstallPackage()
    { if( m_transferedFiles.size()==1 )
      { MSIDB msiDB(m_transferedFiles[0].c_str());

        TString productCode;
        TString packageCode;

        if( msiDB.GetProductCode(productCode) && msiDB.GetPackageCode(packageCode) )
        { if( !MSIProduct::IsInstalled(productCode.c_str()) )
          { INSTALLUILEVEL dwUILevel = INSTALLUILEVEL_DEFAULT; //INSTALLUILEVEL_FULL
            
            ::MsiSetInternalUI(dwUILevel,NULL);

            UINT result = ::MsiInstallProduct(m_transferedFiles[0].c_str(),_T("BVRDIR=c:\\temp\\bvr"));

            LOGGER_INFO<<_T("MsiInstallProduct(")<<m_transferedFiles[0].c_str()<<_T("):")<<result<<endl;
          } // of if
          else if( !MSIProduct::IsPackageInstalled(productCode.c_str(),packageCode.c_str()) )
          { DumpPackageInfo(productCode);
            
            DWORD reinstallMode = REINSTALLMODE_PACKAGE     | REINSTALLMODE_FILEOLDERVERSION | 
                                  REINSTALLMODE_MACHINEDATA | REINSTALLMODE_USERDATA         |
                                  REINSTALLMODE_SHORTCUT;

            UINT result = ::MsiReinstallProduct(m_transferedFiles[0].c_str(),reinstallMode);

            LOGGER_INFO<<_T("MsiReinstallProduct(")<<m_transferedFiles[0].c_str()<<_T("):")<<result<<endl;
          } // of else if
        } // of if
      } // of if
    } // of AutoUpdate::InstallPackage()

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
    void AutoUpdate::ReadVersionInfo(HINSTANCE hModule)
    { util::VersionInfo verInfo(hModule);

      LPVOID prodPrefix = verInfo.GetStringInfo(_T("ProductPrefix"));
      LPVOID compPrefix = verInfo.GetStringInfo(_T("ComponentPrefix"));

      if( NULL!=prodPrefix )
        m_productPrefix = (LPCTSTR)prodPrefix;

      if( NULL!=compPrefix )
        m_componentPrefix = (LPCTSTR)compPrefix;
    } // of AutoUpdate::ReadVersionInfo()

    /**
     *
     */
    void AutoUpdate::DumpPackageInfo(const TString& productCode)
    { MSIProduct msiProduct(productCode.c_str());
      TString productVersion;

      msiProduct.GetProperty(_T("ProductVersion"),productVersion);
      LOGGER_INFO<<_T("  ProductVersion:")<<productVersion.c_str()<<endl;

      TString prodInfo;

      msiProduct.GetInfo(INSTALLPROPERTY_URLUPDATEINFO,prodInfo);
      LOGGER_INFO<<_T("  URLUPDATEINFO:")<<prodInfo.c_str()<<endl;

      msiProduct.GetInfo(INSTALLPROPERTY_VERSIONSTRING,prodInfo);
      LOGGER_INFO<<_T("  VERSIONSTRING:")<<prodInfo.c_str()<<endl;

      TString installedPackageCode;
      msiProduct.GetInfo(INSTALLPROPERTY_PACKAGECODE,installedPackageCode);
      LOGGER_INFO<<_T("  PACKAGECODE:")<<installedPackageCode.c_str()<<endl;

      MSIDB     msiDB(msiProduct);
      MSIQuery  msiQuery(msiDB,_T("SELECT `Property`,`Value` FROM `Property`"));
      MSIRecord msiRecord;

      msiQuery.Execute();

      while( msiQuery.Fetch(msiRecord) )
      { UINT fieldCount = msiRecord.GetFieldCount();

        for( UINT i=1;i<=fieldCount;i++ )
        { TString value;

          msiRecord.GetString(i,value);

          LOGGER_INFO<<value.c_str()<<_T(' ');
        } // of for

        LOGGER_INFO<<endl;
      } // of while
    } // of DumpPackageInfo()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/