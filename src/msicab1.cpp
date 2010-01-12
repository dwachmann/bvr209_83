/*
 * $Id$
 * 
 * Create cabinet files and necessary msi information.
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
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "cab/cabinetfci.h"
#include "cab/cabinetfdi.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/versioninfo.h"
#include "util/xmldocument.h"
#include "util/dirinfo.h"
#include "util/comstring.h"
#include "util/md5sum.h"
#include "util/verifyfile.h"
#include "util/sharedlibrary.h"
#include "util/registrycb.h"
#include "util/yastring.h"
#include "util/yanew.h"
#include "util/yaallocatorpool.h"
#include "util/fileinfo.h"
#include "util/msiidregistry.h"
#include "util/msipackage.h"
#include "util/msidirinfo.h"
#include "com/covariant.h"
#include "exception/bvr20983exception.h"
#include "exception/seexception.h"
#include "exception/lasterrorexception.h"
#include <fstream>

using namespace bvr20983;
using namespace bvr20983::cab;
using namespace bvr20983::util;
using namespace bvr20983::COM;
using namespace std;

/**
 *
 */
bool CALLBACK RegistryInfoCB(LPARAM lParam, bool startSection, LPCTSTR mainKey, LPCTSTR key, LPCTSTR name, LPCTSTR value);

/**
 *
 */
struct MSICABAddFile1CB : bvr20983::cab::CabinetFCIAddFileCB
{
  /**
   *
   */
  MSICABAddFile1CB(MSIPackage& msiPackageDoc,MSIIdRegistry& msiIdRegistry) :
    m_msiPackageDoc(msiPackageDoc),
    m_msiIdRegistry(msiIdRegistry)
  { }

  /**
   *
   */
  ~MSICABAddFile1CB()
  { }

  /**
   *   <registryentries>
   *     <registry>
   *       <key>HKEY_CLASSES_ROOT\BVR20983_1_CC.LED</key>
   *       <name>@</name>
   *       <value>"BVR20983 LED Control"</value>
   *     </registry>
   */
  bool RegistryInfo(bool startSection, LPCTSTR mainKey,LPCTSTR key, LPCTSTR name, LPCTSTR value)
  { MSIId    uniqueId;
    YAString regPath(key);

    if( NULL!=name )
    { regPath.Append(_T("\\"));
      regPath.Append(name);
    } // of if

    m_msiIdRegistry.GetUniqueId(_T("registry"),regPath.c_str(),uniqueId);

    YAString registryId;

    registryId.Format(_T("R%08d"),uniqueId.id);

    m_msiPackageDoc.AddRegistryInfo(registryId,uniqueId.guid,startSection,mainKey,key,name,value);

    return true;
  } // of RegistryInfo()

  /**
   *
   */
  void close()
  { m_msiPackageDoc.AppendNewline(); }

  /**
   *
   */
  bool DirectoryStarted(util::DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,int depth)
  { bool result = true;

    if( _tcsstr(dirInfo.GetBaseDirectory(),_T(".svn"))==NULL )
    { MSIDirectoryInfo d(dirInfo.GetName(),dirInfo.GetShortName());

      d.m_dirId = dirInfo.GetDirId();

      if( NULL!=dirInfo.GetParentDirInfo() )
        d.m_parentId = dirInfo.GetParentDirInfo()->GetDirId();

      d.m_dirPath = dirInfo.GetBaseDirectory();

      if( m_baseDir.length()==0 )
      { m_baseDir = d.m_dirPath;
        m_baseDir += _T("\\");

        d.m_dirPath      = _T("");
        d.m_dirName      = _T("");
        d.m_dirShortName = _T("");
      } // of if
      else
      { YAString dir(d.m_dirPath);

        YAPtr<YAString> dir1 = dir.Strip(m_baseDir.c_str());

        d.m_dirPath      = dir1->c_str();
      } // of else

      m_dirInfo.push_back(d);
    } // of if

    return result;
  } // of DirectoryStarted()

  /**
   *
   */
  void DumpDirectoryInfo()
  { m_msiPackageDoc.AddDirectoryInfo(m_dirInfo); }


  /**
   *
   */
  bool AddFile(LPCTSTR prefix,LPCTSTR filePath,LPTSTR addedFileName,int addedFileNameMaxLen,int seqNo,util::DirectoryInfo* pDirInfo)
  { bool result = false;

    // ignore subversion or git subdirectories
    if( _tcsstr(filePath,_T(".svn"))==NULL && _tcsstr(filePath,_T(".git"))==NULL )
    { FileInfo               fInfo(filePath);
      YAPtr<YAString>        suffix                = fInfo.GetSuffix();
      YAPtr<YAString>        fileName              = fInfo.GetName();
      YAPtr<YAString>        shortStrippedFileName = FileInfo(fInfo.GetShortName()).GetName();
      YAString               directoryId;
      YAString               fileId;
      MSIId                  uniqueId;

      m_msiIdRegistry.GetUniqueId(_T("file"),fInfo.GetPartialPath(prefix)->c_str(),uniqueId);

      fileId.Format(_T("F%08d"),uniqueId.id);

      _tcscpy_s(addedFileName,addedFileNameMaxLen,fileId);

      if( NULL!=pDirInfo )
        directoryId = pDirInfo->GetDirId();

      DWORD fileSize=0;
      DirectoryInfo::_GetFileSize(filePath,&fileSize);
        
      LOGGER_DEBUG<<_T("::AddFile() suffix=<")<<suffix<<_T(">")<<endl;

      VersionInfo verInfo(filePath);
      LPCTSTR fileVersion  = (LPCTSTR)verInfo.GetStringInfo(_T("FileVersion"));
      
      if( m_msiPackageDoc.AddFileInfo(fileId,
                                      uniqueId.guid,
                                      seqNo,
                                      directoryId.c_str(),
                                      fileSize,
                                      fInfo.GetPathWithoutFilename(prefix)->c_str(),
                                      fileName->c_str(),
                                      shortStrippedFileName->c_str(),
                                      fileVersion
                                     )
        )
      { if( NULL==fileVersion )
        { MSIFILEHASHINFO msiHash;

          ::memset(&msiHash,0,sizeof(msiHash));
          msiHash.dwFileHashInfoSize = sizeof(msiHash);

          THROW_LASTERROREXCEPTION( ::MsiGetFileHash(filePath,0,&msiHash) );

          m_msiPackageDoc.AddHash(msiHash);
        } // of if

        if( *suffix == _T(".dll") )
        { SharedLibrary shLib(filePath);
        
          ENUMREGISTRATIONPROC enumProc = (ENUMREGISTRATIONPROC)shLib.GetProcAddress(_T("DllEnumRegistrationInfo"),false);

          if( enumProc )
          { m_msiPackageDoc.StartRegistryInfo();

            enumProc(RegistryInfoCB,(LPARAM)this);
          } // of if
        } // of if

        m_msiPackageDoc.AppendNewline();

        LOGGER_DEBUG<<_T(" File ")<<filePath<<_T(" as ")<<addedFileName<<endl;

        result = true;
      } // of if
      else
      { LOGGER_WARN<<_T(" File ")<<filePath<<_T(" ignored, not feature mapping found.")<<endl;

        result = false;
      } // of else
    } // of if

    return result;
  } // of AddFile()

private:
  VMSIDirInfoT           m_dirInfo;
  util::MSIPackage&      m_msiPackageDoc;
  util::MSIIdRegistry&   m_msiIdRegistry;
  TString                m_baseDir;
}; // of class MSICABAddFile1CB

/**
 *
 */
bool CALLBACK RegistryInfoCB(LPARAM lParam, bool startSection, LPCTSTR mainKey, LPCTSTR key, LPCTSTR name, LPCTSTR value)
{ bool result = false;

  if( NULL!=lParam )
    result = ((MSICABAddFile1CB*)lParam)->RegistryInfo(startSection,mainKey,key,name,value);

  return result;
} // of RegistryInfoCB()

/**
 *
 */
void msicab(LPTSTR versionFName,LPTSTR msiIdRegistryFName,LPTSTR msiPackageFName,LPTSTR compDir,LPTSTR cabName,LPTSTR argv[],int argc)
{ util::XMLDocument            versionsDoc;
  COMPtr<IXMLDOMNodeList>      pXMLDomNodeList;
  COMPtr<IXMLDOMNode>          pNode;
  util::XMLDocument::PropertyM props;

  for( int i=0;i<argc && i+1<argc;i+=2 )
    props.insert( util::XMLDocument::PropertyP(argv[i],argv[i+1]) );

  versionsDoc.SetProperties(props);

  if( versionsDoc.Load(versionFName) )
  { COVariant productidValue;

    if( versionsDoc.GetNodeValue(_T("//v:product/@id"),productidValue,true) )
    { CabFCIParameter cabParameter(cabName,CabFCIParameter::CDROM_SIZE);
      CabinetFCI      cabinet(cabParameter);
      
      YAPtr<YAString> fullCabName = FileInfo(cabName).GetFullPath();
      YAPtr<YAString> fullCompDir = FileInfo(compDir).GetFullPath();
      YAPtr<YAString> strippedCabName;

      if( fullCabName->LastIndexOf(_T('.'))>=0 )
        strippedCabName = fullCabName->Substring(0,fullCabName->LastIndexOf(_T('.')));
      else
        strippedCabName = YACLONE(fullCabName);

      strippedCabName->Append(_T(".xml"));
      
      COVariant msiComponentID;

      if( versionsDoc.GetNodeValue(_T("/v:versions/v:product/v:versionhistory/v:version[1]/v:msicomponent/text()"),msiComponentID,true) &&
          DirectoryInfo::_IsDirectory(fullCompDir->c_str())
        )
      { 
        { MSIPackage       msiPackageDoc(msiPackageFName,versionsDoc);
          MSIIdRegistry    msiIdRegistry(msiIdRegistryFName,V_BSTR(msiComponentID));
          MSICABAddFile1CB addFileCB(msiPackageDoc,msiIdRegistry);

          cabinet.SetAddFileCallback(&addFileCB);
          cabinet.AddFile(fullCompDir->c_str(),fullCompDir->c_str());
          addFileCB.DumpDirectoryInfo();

          msiPackageDoc.AddMedia((long)cabinet.GetSequenceNo(),cabName);
          addFileCB.close();

          msiPackageDoc.Save();
        }

        cabinet.SetAddFileCallback(NULL);
        cabinet.AddFile(msiPackageFName,NULL,_T("msipackage.xml"));
        cabinet.AddFile(msiIdRegistryFName,NULL,_T("msiid.xml"));

        cabinet.Flush();
      } // of if
    } // of if
  } // of if
} // of msicab()

/**
 *
 */
void printUsage(LPCTSTR progName)
{ LOGGER_INFO<<_T("Usage: "<<progName<<" -msicab <versions file> <msiidregistry file> <msipackage file> <component dir> <cabname>")<<endl;
  LOGGER_INFO<<endl;
  
  ::exit(0);
} // of printUsage()

/**
 *
 */
extern "C" int __cdecl _tmain (int argc, TCHAR  * argv[])
{ ::CoInitialize(NULL);
  
  LogStreamT::ReadVersionInfo();

  LONG exHr = NOERROR;
  
  //__debugbreak();
    
  try
  { _set_se_translator( SEException::throwException );
  
    if( argc<2 )
    { VersionInfo verInfo;
    
      verInfo.LogCopyright();
      printUsage(argv[0]);
    }
      
    if( _tcscmp(argv[1],_T("-msicab"))==0 )
    { if( argc>=7 )
        msicab(argv[2],argv[3],argv[4],argv[5],argv[6],argc>7 ? &argv[7] : NULL,argc-7);
      else
        printUsage(argv[0]);
    } // of else if
  }
  catch(BVR20983Exception& e)
  { LOGGER_ERROR<<e<<endl;

    exHr = e.GetErrorCode();
  }
  catch(exception& e) 
  { LOGGER_ERROR<<"Exception "<<typeid(e).name()<<":"<<e.what()<<endl;

    exHr = -4;
  }
  catch(LPCTSTR& e)
  { LOGGER_ERROR<<e<<endl;

    exHr = -3;
  }
  catch(...)
  { LOGGER_ERROR<<_T("Exception")<<endl;

    exHr = -2;
  }
  
  ::CoUninitialize();
  
  return (int)exHr;
} // of main()
//======================================END-OF-FILE=========================
