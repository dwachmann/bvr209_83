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
#include <Msi.h>
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
struct MSIDirectoryInfo
{
  MSIDirectoryInfo()
  { }

  MSIDirectoryInfo(LPCTSTR dirName) : m_dirName(dirName)
  { }

  MSIDirectoryInfo(LPCTSTR dirName,LPCTSTR dirShortName) : m_dirName(dirName),m_dirShortName(dirShortName)
  { }

  YAString m_dirId;
  YAString m_parentId;
  YAString m_dirName;
  YAString m_dirShortName;
  YAString m_dirPath;
}; // of struct MSICABAddFileCB

typedef std::vector<MSIDirectoryInfo> VMSIDirInfoT;

/**
 *
 */
bool CALLBACK RegistryInfoCB(LPARAM lParam, bool startSection, LPCTSTR key, LPCTSTR name, LPCTSTR value);

/**
 *
 */
struct MSICABAddFile1CB : bvr20983::cab::CabinetFCIAddFileCB
{
  MSICABAddFile1CB(util::XMLDocument& msiPackageDoc,LPCTSTR msiCompIdPattern) :
    m_msiPackageDoc(msiPackageDoc),
    m_msiCompIdPattern(msiCompIdPattern)
  { 
    if( m_msiPackageDoc.IsEmpty() )
    { m_msiPackageDoc.CreateXmlSkeleton(_T("msipackage"),m_rootElement);

      m_msiPackageDoc.CreateElement(_T("files"),m_filesElement);
      m_msiPackageDoc.AppendChildToParent(m_filesElement,m_rootElement,1);
      m_msiPackageDoc.AppendNewline(m_filesElement,1);
    } // of if
    else
    { m_msiPackageDoc.GetFirstElement(_T("msipackage"),m_rootElement);
      m_msiPackageDoc.GetFirstElement(_T("files"),m_filesElement);

      m_msiPackageDoc.RemoveElements(_T("media"));
      m_msiPackageDoc.RemoveElements(_T("directories"));
    } // of else
  } // of MSICABAddFile1CB()

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
  bool RegistryInfo(bool startSection, LPCTSTR key, LPCTSTR name, LPCTSTR value)
  { if( !m_lastRegistryentriesElement.IsNULL() )
    { COMPtr<IXMLDOMElement> registryElement;

      m_msiPackageDoc.CreateElement(_T("registry"),registryElement);
      m_msiPackageDoc.AppendChildToParent(registryElement,m_lastRegistryentriesElement,3);

      m_msiPackageDoc.AppendElement(registryElement,_T("key"),key,4);

      if( NULL!=name )
      { m_msiPackageDoc.AppendElement(registryElement,_T("name"),name,4);

        if( NULL!=value )
          m_msiPackageDoc.AppendElement(registryElement,_T("value"),value,4);
      } // of if

      m_msiPackageDoc.AppendNewline(registryElement,3);
    } // of if

    return true;
  } // of registryInfo()

  /**
   *
   */
  void close()
  { m_msiPackageDoc.AppendNewline(m_rootElement,0);
  }

  /**
   *
   */
  bool DirectoryStarted(util::DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,int depth)
  { bool result = true;

    if( _tcsstr(dirInfo.GetBaseDirectory(),_T(".svn"))==NULL )
    { MSIDirectoryInfo d(dirInfo.GetName(),dirInfo.GetShortName());

      (d.m_dirId = _T("DIR_")) += (unsigned long)dirInfo.GetId();

      if( NULL!=dirInfo.GetParentDirInfo() )
        (d.m_parentId = _T("DIR_")) += (unsigned long)dirInfo.GetParentDirInfo()->GetId();

      TCHAR dirPath[MAX_PATH];

      dirInfo._GetFullName(dirPath,ARRAYSIZE(dirPath));

      d.m_dirPath = dirInfo.GetBaseDirectory();

      m_dirInfo.push_back(d);
    } // of if

    return result;
  } // of DirectoryStarted()

  /**
   *
   */
  void DumpDirectoryInfo()
  { VMSIDirInfoT::const_iterator iter;

    if( !m_rootElement.IsNULL() )
    { COMPtr<IXMLDOMElement> directoriesElement;

      m_msiPackageDoc.CreateElement(_T("directories"),directoriesElement);
      m_msiPackageDoc.AppendChildToParent(directoriesElement,m_rootElement,1);

      for( iter=m_dirInfo.begin();iter!=m_dirInfo.end();iter++ )
      { COMPtr<IXMLDOMElement> directoryElement;

        m_msiPackageDoc.CreateElement(_T("directory"),directoryElement);
        m_msiPackageDoc.AddAttribute(directoryElement,_T("id"),iter->m_dirId);

        if( _tcslen(iter->m_parentId)>0 )
          m_msiPackageDoc.AddAttribute(directoryElement,_T("parentid"),iter->m_parentId);

        m_msiPackageDoc.AppendChildToParent(directoryElement,directoriesElement,2);

        m_msiPackageDoc.AppendElement(directoryElement,_T("path"),iter->m_dirPath,3);
        m_msiPackageDoc.AppendElement(directoryElement,_T("name"),iter->m_dirName,3);
        m_msiPackageDoc.AppendElement(directoryElement,_T("shortname"),iter->m_dirShortName,3);
      } // of for

      m_msiPackageDoc.AppendNewline(directoriesElement,1);
    } // of if
  } // of DumpDirectoryInfo()


  /**
   *
   */
  bool AddFile(LPCTSTR prefix,LPCTSTR filePath,LPTSTR addedFileName,int addedFileNameMaxLen,int seqNo,util::DirectoryInfo* pDirInfo)
  { TCHAR guid[MAX_PATH];

    // ignore subversion subdirectories
    if( _tcsstr(filePath,_T(".svn"))!=NULL )
      return false;

    FileInfo        fInfo(filePath);
    YAPtr<YAString> suffix                = fInfo.GetSuffix();
    YAPtr<YAString> fileName              = fInfo.GetName();
    YAPtr<YAString> strippedFilePath      = fInfo.GetPartialPath(prefix);
    YAPtr<YAString> shortStrippedFileName = FileInfo(fInfo.GetShortName()).GetName();

    COMPtr<IXMLDOMElement> fileElement;

    m_msiPackageDoc.CreateElement(_T("file"),fileElement);
    m_msiPackageDoc.AppendChildToParent(fileElement,m_filesElement,1);

    _stprintf_s(addedFileName,addedFileNameMaxLen,_T("_%08X"),seqNo);
    _stprintf_s(guid,MAX_PATH,_T("%08X"),seqNo);

    m_msiPackageDoc.AddAttribute(fileElement,_T("id"),addedFileName);
    m_msiPackageDoc.AddAttribute(fileElement,_T("guid"),YAString(m_msiCompIdPattern).Append(guid).c_str());
    m_msiPackageDoc.AddAttribute(fileElement,_T("diskid"),_T("1"));
    m_msiPackageDoc.AddAttribute(fileElement,_T("no"),YAString((long)seqNo).c_str());

    if( NULL!=pDirInfo )
      m_msiPackageDoc.AddAttribute(fileElement,_T("directoryid"),YAString(_T("DIR_")).Append((unsigned long)pDirInfo->GetId()).c_str());

    DWORD fileSize=0;
    DirectoryInfo::_GetFileSize(filePath,&fileSize);
      
    LOGGER_DEBUG<<_T("::AddFile() suffix=<")<<suffix<<_T(">")<<endl;

    m_msiPackageDoc.AddAttribute(fileElement,_T("size"),YAString(fileSize).c_str());

    m_msiPackageDoc.AppendElement(fileElement,_T("path"),strippedFilePath->c_str(),2);
    m_msiPackageDoc.AppendElement(fileElement,_T("name"),fileName->c_str(),2);
    m_msiPackageDoc.AppendElement(fileElement,_T("shortname"),shortStrippedFileName->c_str(),2);

    VersionInfo verInfo(filePath);
    LPCTSTR fileVersion  = (LPCTSTR)verInfo.GetStringInfo(_T("FileVersion"));
    
    if( NULL!=fileVersion )
      m_msiPackageDoc.AppendElement(fileElement,_T("version"),fileVersion,2);
    else
    { MSIFILEHASHINFO msiHash;

      ::memset(&msiHash,0,sizeof(msiHash));
      msiHash.dwFileHashInfoSize = sizeof(msiHash);

      THROW_LASTERROREXCEPTION( ::MsiGetFileHash(filePath,0,&msiHash) );

      COMPtr<IXMLDOMElement> hashElement;

      m_msiPackageDoc.CreateElement(_T("hash"),hashElement);
      m_msiPackageDoc.AppendChildToParent(hashElement,fileElement,2);

      m_msiPackageDoc.AddAttribute(hashElement,_T("id0"),YAString((long)msiHash.dwData[0]).c_str());
      m_msiPackageDoc.AddAttribute(hashElement,_T("id1"),YAString((long)msiHash.dwData[1]).c_str());
      m_msiPackageDoc.AddAttribute(hashElement,_T("id2"),YAString((long)msiHash.dwData[2]).c_str());
      m_msiPackageDoc.AddAttribute(hashElement,_T("id3"),YAString((long)msiHash.dwData[3]).c_str());
    } // of if

    if( *suffix == _T(".dll") )
    { SharedLibrary shLib(filePath);
    
      ENUMREGISTRATIONPROC enumProc = (ENUMREGISTRATIONPROC)shLib.GetProcAddress(_T("DllEnumRegistrationInfo"),false);

      if( enumProc )
      { m_msiPackageDoc.CreateElement(_T("registryentries"),m_lastRegistryentriesElement);
        m_msiPackageDoc.AppendChildToParent(m_lastRegistryentriesElement,fileElement,2);

        enumProc(RegistryInfoCB,(LPARAM)this);

        m_msiPackageDoc.AppendNewline(m_lastRegistryentriesElement,2);
      } // of if
    } // of if

    m_msiPackageDoc.AppendNewline(fileElement,1);

    LOGGER_INFO<<_T(" File ")<<filePath<<_T(" as ")<<addedFileName<<endl;

    return true;
  } // of AddFile()

private:
  VMSIDirInfoT           m_dirInfo;
  LPCTSTR                m_msiCompIdPattern;
  util::XMLDocument&     m_msiPackageDoc;
  COMPtr<IXMLDOMElement> m_rootElement;
  COMPtr<IXMLDOMElement> m_filesElement;
  COMPtr<IXMLDOMElement> m_lastRegistryentriesElement;
}; // of class MSICABAddFile1CB

/**
 *
 */
bool CALLBACK RegistryInfoCB(LPARAM lParam, bool startSection, LPCTSTR key, LPCTSTR name, LPCTSTR value)
{ bool result = false;

  if( NULL!=lParam )
    result = ((MSICABAddFile1CB*)lParam)->RegistryInfo(startSection,key,name,value);

  return result;
} // of RegistryInfoCB()

/**
 *
 */
void msicab(LPTSTR versionFName,LPTSTR msiPackageFName,LPTSTR compDir,LPTSTR cabName,LPTSTR argv[],int argc)
{ util::XMLDocument            versionsDoc;
  util::XMLDocument            msiPackageDoc;
  COMPtr<IXMLDOMNodeList>      pXMLDomNodeList;
  COMPtr<IXMLDOMNode>          pNode;
  util::XMLDocument::PropertyM props;

  for( int i=0;i<argc && i+1<argc;i+=2 )
    props.insert( util::XMLDocument::PropertyP(argv[i],argv[i+1]) );

  versionsDoc.SetProperties(props);
  msiPackageDoc.SetProperties(props);

  if( FileInfo(msiPackageFName).IsFile() )
    msiPackageDoc.Load(msiPackageFName);

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

      if( versionsDoc.GetNodeValue(_T("//v:versions//v:product//v:versionhistory/v:version[1]//v:msicomponent//text()"),msiComponentID,true) &&
          DirectoryInfo::_IsDirectory(fullCompDir->c_str())
        )
      { MSICABAddFile1CB addFileCB(msiPackageDoc,V_BSTR(msiComponentID));

        cabinet.SetAddFileCallback(&addFileCB);
        cabinet.AddFile(fullCompDir->c_str(),fullCompDir->c_str());
        addFileCB.DumpDirectoryInfo();

        COMPtr<IXMLDOMElement> mediaElement;

        msiPackageDoc.CreateElement(_T("media"),mediaElement);
        msiPackageDoc.AppendChild(mediaElement,1);

        msiPackageDoc.AddAttribute(mediaElement,_T("diskid"),_T("1"));
        msiPackageDoc.AddAttribute(mediaElement,_T("lastSequence"),YAString((long)cabinet.GetSequenceNo()).c_str());
        msiPackageDoc.AppendElement(mediaElement,_T("cabname"),cabName,2);

        addFileCB.close();

        cabinet.SetAddFileCallback(NULL);
        cabinet.AddFile(strippedCabName->c_str(),NULL,_T("msipackage.xml"));

        cabinet.Flush();
      } // of if
    } // of if

    msiPackageDoc.Save(msiPackageFName);
  } // of if
} // of msicab()

/**
 *
 */
void printUsage(LPCTSTR progName)
{ LOGGER_INFO<<_T("Usage: "<<progName<<" -msicab <versions file> <msipackage file> <component dir> <cabname>")<<endl;
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
    { if( argc>=6 )
        msicab(argv[2],argv[3],argv[4],argv[5],argc>6 ? &argv[6] : NULL,argc-6);
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
