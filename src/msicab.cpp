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
void xmltest(LPTSTR fName,LPTSTR xPath,LPTSTR argv[],int argc)
{ util::XMLDocument            xmlDoc;
  COVariant                    value;
  boolean                      hasValue=false;
  util::XMLDocument::PropertyM props;

  for( int i=0;i<argc && i+1<argc;i+=2 )
    props.insert( util::XMLDocument::PropertyP(argv[i],argv[i+1]) );

  xmlDoc.SetProperties(props);

  if( xmlDoc.Load(fName) )
    hasValue = xmlDoc.GetNodeValue(xPath,value,true);

  if( hasValue )
  { LOGGER_INFO<<xPath<<_T(":")<<value<<endl; }
  else
  { LOGGER_INFO<<xPath<<_T(": NOT FOUND")<<endl; }
} // of xmltest()

/**
 *
 */
void dirtest(LPTSTR dirName,LPTSTR prefix,UINT maxDepth)
{ DirectoryInfo dirInfo(dirName,prefix,maxDepth);

  dirInfo.Dump();
} // of dirtest()

/**
 *
 */
struct MSICABAddFileCB : bvr20983::cab::CabinetFCIAddFileCB
{

#ifdef _UNICODE
  MSICABAddFileCB(wofstream& componentIDT,wofstream& fileIDT,LPCTSTR compId,LPCTSTR msiguid,LPCTSTR msidir) :
#else
  MSICABAddFileCB(ofstream& componentIDT,ofstream& fileIDT,LPCTSTR compId,LPCTSTR msiguid,LPCTSTR msidir) :
#endif
    m_componentIDT(componentIDT),
    m_fileIDT(fileIDT),
    m_msiguid(msiguid),
    m_compId(compId),
    m_componentEntryWritten(false),
    m_msidir(msidir)
  { LPTSTR s = const_cast<LPTSTR>(m_msiguid);

    for( ;*s!=_T('\0');s++ )
      *s = toupper(*s);
  }

  void FileAdded(LPCTSTR fileName,LPCTSTR addedFileName,int seqNo)
  { TCHAR strippedCompFileName[MAX_PATH];
    DirectoryInfo::_StripFilename(strippedCompFileName,MAX_PATH,fileName);

    LPTSTR s = strippedCompFileName;
    for( ;*s!=_T('\0');s++ )
      *s = tolower(*s);

    TCHAR shortCompFileName[MAX_PATH];
    TCHAR shortStrippedCompFileName[MAX_PATH];

    ::GetShortPathName(fileName,shortCompFileName,MAX_PATH);
    DirectoryInfo::_StripFilename(shortStrippedCompFileName,MAX_PATH,shortCompFileName);

    VersionInfo verInfo(fileName);
    LPCTSTR fileVersion = (LPCTSTR)verInfo.GetStringInfo(_T("FileVersion"));

    if( NULL==fileVersion )
      fileVersion = _T("");

    DWORD fileSize=0;
    DirectoryInfo::_GetFileSize(fileName,&fileSize);

    if( !m_componentEntryWritten )
    { m_componentIDT<<m_compId<<_T("\t{")<<m_msiguid<<_T('}')<<_T("\t")<<m_msidir<<_T("\t0\t\t")<<addedFileName<<endl;

      m_componentEntryWritten = true;
    } // of if

    m_fileIDT<<addedFileName<<_T('\t')<<m_compId<<_T('\t')<<shortStrippedCompFileName<<_T("|")<<strippedCompFileName<<_T('\t')<<fileSize<<_T('\t')<<fileVersion<<_T('\t')<<1033<<_T('\t')<<0<<_T('\t')<<seqNo<<endl;
  }

private:
#ifdef _UNICODE
    wofstream& m_componentIDT;
    wofstream& m_fileIDT;
#else
    ofstream&  m_componentIDT;
    ofstream&  m_fileIDT;
#endif

    LPCTSTR m_compId;
    LPCTSTR m_msiguid;
    LPCTSTR m_msidir;
    bool    m_componentEntryWritten;
}; // of class MSICABAddFileCB

/**
 *
 */
void msicab(LPTSTR fName,LPTSTR compDir,LPTSTR cabName,LPTSTR templateDir,LPTSTR argv[],int argc)
{ util::XMLDocument            xmlDoc;
  COMPtr<IXMLDOMNodeList>      pXMLDomNodeList;
  COMPtr<IXMLDOMNode>          pNode;
  util::XMLDocument::PropertyM props;

  for( int i=0;i<argc && i+1<argc;i+=2 )
    props.insert( util::XMLDocument::PropertyP(argv[i],argv[i+1]) );

  xmlDoc.SetProperties(props);

  if( xmlDoc.Load(fName) )
  { 
    COVariant productidValue;

    if( xmlDoc.GetNodeValue(_T("//v:product/@id"),productidValue,true) )
    { CabFCIParameter cabParameter(cabName,CabFCIParameter::CDROM_SIZE);
      CabinetFCI      cabinet(cabParameter);

      TString msiComponentFName(templateDir);
      msiComponentFName += _T("\\");
      msiComponentFName += _T("Component.idt");

      TString msiFileFName(templateDir);
      msiFileFName += _T("\\");
      msiFileFName += _T("File.idt");

      TString msiMediaFName(templateDir);
      msiMediaFName += _T("\\");
      msiMediaFName += _T("Media.idt");

#ifdef _UNICODE
      wofstream componentIDT(msiComponentFName.c_str(),ios_base::app);
      wofstream fileIDT(msiFileFName.c_str(),ios_base::app);
      wofstream mediaIDT(msiMediaFName.c_str(),ios_base::app);
#else
      ofstream componentIDT(msiComponentFName.c_str(),ios_base::app);
      ofstream fileIDT(msiFileFName.c_str(),ios_base::app);
      ofstream mediaIDT(msiMediaFName.c_str(),ios_base::app);
#endif

      LOGGER_INFO<<_T("product:")<<productidValue<<endl; 

      xmlDoc.GetSelection(_T("//v:component[descendant::v:msiguid]/@id"),pXMLDomNodeList);

      if( !pXMLDomNodeList.IsNULL() )
      { for( HRESULT hr = pXMLDomNodeList->nextNode(&pNode);hr==S_OK;hr = pXMLDomNodeList->nextNode(&pNode) )
        { COMString       nodeName;
          COVariant       nodeValue;
          const VARIANT*  v = nodeValue;
        
          THROW_COMEXCEPTION( pNode->get_nodeName(&nodeName) );
          THROW_COMEXCEPTION( pNode->get_nodeValue(const_cast<VARIANT*>(v)) );

          if( xmlDoc.GetProperty(pNode,nodeValue) )
          { LOGGER_INFO<<_T("id:")<<nodeValue<<endl; 

            TString compName(V_BSTR(nodeValue));

            COVariant compVersionValue;
            TString compVersion = _T("//v:component[@id='");
            compVersion += compName;
            compVersion += _T("']/v:version/@major");

            COVariant filenameValue;
            TString filename = _T("//v:component[@id='");
            filename += compName;
            filename += _T("']/v:filename/text()");

            COVariant msiguidValue;
            TString msiguid = _T("//v:component[@id='");
            msiguid += compName;
            msiguid += _T("']/v:msiguid/text()");

            COVariant msidirValue;
            TString msidir = _T("//v:component[@id='");
            msidir += compName;
            msidir += _T("']/v:msidir/text()");

            COVariant compTypeValue;
            TString compType = _T("//v:component[@id='");
            compType += compName;
            compType += _T("']/@type");

            if( xmlDoc.GetNodeValue(filename.c_str(),filenameValue,true) &&
                xmlDoc.GetNodeValue(msiguid.c_str(),msiguidValue,true)   &&
                xmlDoc.GetNodeValue(compType.c_str(),compTypeValue,true) &&
                xmlDoc.GetNodeValue(compVersion.c_str(),compVersionValue,true)
              )
            { 
              TString compId(compName);
              compId += _T(".");
              compId += V_BSTR(compVersionValue);

              TString msiCompDir = _T("BVRDIR");
              if( xmlDoc.GetNodeValue(msidir.c_str(),msidirValue,true) )
                msiCompDir = V_BSTR(msidirValue);
              
              LOGGER_INFO<<_T("    compId:")<<compId<<endl; 
              LOGGER_INFO<<_T("  filename:")<<filenameValue<<endl; 
              LOGGER_INFO<<_T("  comptype:")<<compTypeValue<<endl; 
              LOGGER_INFO<<_T("   msiguid:")<<msiguidValue<<endl; 
              LOGGER_INFO<<_T("    msidir:")<<msiCompDir<<endl; 

              TString compFileName(compDir);
              compFileName += _T("\\");
              compFileName += V_BSTR(filenameValue);

              if( _tcscmp(V_BSTR(compTypeValue),_T("dll"))==0 || _tcscmp(V_BSTR(compTypeValue),_T("exe"))==0 )
              { compFileName += _T(".");
                compFileName += V_BSTR(compTypeValue);

                if( DirectoryInfo::_IsFile(compFileName.c_str())  )
                { MSICABAddFileCB addFileCB(componentIDT,fileIDT,compId.c_str(),V_BSTR(msiguidValue),msiCompDir.c_str());
                  
                  TCHAR compcabfilename[MAX_PATH];
                  _tcscpy_s(compcabfilename,MAX_PATH,compId.c_str());

                  cabinet.SetAddFileCallback(&addFileCB);

                  cabinet.AddFile(compFileName.c_str(),NULL,compcabfilename);
                } // of if
              } // of if
              else if( _tcscmp(V_BSTR(compTypeValue),_T("data"))==0 && DirectoryInfo::_IsDirectory(compFileName.c_str()) )
              { 
                MSICABAddFileCB addFileCB(componentIDT,fileIDT,compId.c_str(),V_BSTR(msiguidValue),msiCompDir.c_str());

                cabinet.SetAddFileCallback(&addFileCB);

                cabinet.AddFile(compFileName.c_str());
              } // of else if
            } // of if
          } // of if
        } // of for

        cabinet.Flush();
        
        TCHAR strippedCabName[MAX_PATH];
        
        DirectoryInfo::_StripFilename(strippedCabName,MAX_PATH,cabName);
        
        mediaIDT<<_T("1")<<_T('\t')<<cabinet.GetSequenceNo()<<_T('\t')<<_T("1")<<_T("\t#")<<strippedCabName<<_T('\t')<<_T("DISK1")<<_T('\t')<<endl;
      } // of if
    } // of if
  } // of if
} // of msicab()


/**
 *
 */
void printUsage(LPCTSTR progName)
{ LOGGER_INFO<<_T("Usage: "<<progName<<" -msicab <versions file> <component dir> <cabname> <templatedir>")<<endl;
  LOGGER_INFO<<endl;
  
  LOGGER_INFO<<_T("Usage: "<<progName<<" [options] command cabfile [files] [dest_dir]")<<endl;
  LOGGER_INFO<<endl;
  LOGGER_INFO<<_T("Commands:")<<endl;
  LOGGER_INFO<<_T("   t   List contents of cabinet")<<endl;
  LOGGER_INFO<<_T("   c   Create new cabinet")<<endl;
  LOGGER_INFO<<_T("   x   Extract file(s) from cabinet")<<endl;
  LOGGER_INFO<<endl;
  LOGGER_INFO<<_T("Options:")<<endl;
  LOGGER_INFO<<_T("  -i   Set cabinet set ID when creating cabinets")<<endl;
  LOGGER_INFO<<endl;
  LOGGER_INFO<<_T("Examples:")<<endl;
  LOGGER_INFO<<_T("  create cabinet: msicab c data.cab <instdir>")<<endl;
  LOGGER_INFO<<_T("    list cabinet: msicab t data.cab")<<endl;
  LOGGER_INFO<<_T(" extract cabinet: msicab x data.cab <extractdir>")<<endl;

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
  
    { VersionInfo verInfo;
    
      verInfo.LogCopyright();
    }
    
    if( argc<2 )
      printUsage(argv[0]);
      
    if( _tcscmp(argv[1],_T("-xml"))==0 && argc>=4 )
      xmltest(argv[2],argv[3],argc>=5 ? &argv[4] : NULL,argc-4);
    else if( _tcscmp(argv[1],_T("-msicab"))==0 )
    { if( argc>=6 )
        msicab(argv[2],argv[3],argv[4],argv[5],argc>6 ? &argv[6] : NULL,argc-6);
      else
        printUsage(argv[0]);
    } // of else if
    else if( _tcscmp(argv[1],_T("-dir"))==0 && argc>=3 )
      dirtest(argv[2],argc>3 ? argv[3] : NULL,argc>4 ? _tstoi(argv[4]) : 0);
    else
    { TCHAR command = _T('\0');
      int   i       = 1;
      int   diskid  = CabFCIParameter::DISKID;
    
      for( ;i<argc;i++ )
      { 
        // options
        if( argv[i][0]==_T('-') )
        {
        } // of if
        else if( command==_T('\0') )
        { command=argv[i][0];
        
          if( i==argc-1 )
            printUsage(argv[0]);
            
          i++;
          
          break;
        } // of else if
      } // of for
      
      switch( command )
      { case 't':
          { CabinetFDI cabinet(argv[i]);

            cabinet.List();
          }
          break;
        case 'c':
          { CabFCIParameter cabParameter(argv[i],CabFCIParameter::CDROM_SIZE,diskid);
            CabinetFCI      cabinet(cabParameter);
            
            for( i++;i<argc;i++ )
            {
              if( !_tcscmp(argv[i], _T("+")) )
              { cabinet.Flush(true);
                
                continue;
              } // of if
              
              if( DirectoryInfo::_IsDirectory(argv[i]) )
                cabinet.AddFile(argv[i],argv[i]);
              else    
                cabinet.AddFile(argv[i]);
            } // of for
            
            cabinet.Flush();
          }
          break;
        case 'x':
          { CabinetFDI cabinet(argv[i],i+1<argc ? argv[i+1] : NULL);
          
            cabinet.Extract();
          }
          break;
        default:
          printUsage(argv[0]);
          break;
      } // of switch
    } // of else
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
