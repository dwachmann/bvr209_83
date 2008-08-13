/*
 * $Id$
 * 
 * Lists the structure of COM compound storage.
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
void xmltest(char* fName,char* xPath,char* argv[],int argc)
{ util::XMLDocument            xmlDoc;
  COVariant                    value;
  boolean                      hasValue=false;
  util::XMLDocument::PropertyM props;

#ifdef _UNICODE
  TCHAR fNameU[MAX_PATH];
  TCHAR xPathU[MAX_PATH];
  TCHAR propNameU[MAX_PATH];
  TCHAR propValueU[MAX_PATH];

  THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, fName, -1,fNameU, MAX_PATH) );
  THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, xPath, -1,xPathU, MAX_PATH) );

  for( int i=0;i<argc && i+1<argc;i+=2 )
  { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, argv[i]  , -1,propNameU, MAX_PATH) );
    THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, argv[i+1], -1,propValueU, MAX_PATH) );

    props.insert( util::XMLDocument::PropertyP(propNameU,propValueU) );
  } // of for

  xmlDoc.SetProperties(props);

  if( xmlDoc.Load(fNameU) )
    hasValue = xmlDoc.GetNodeValue(xPathU,value,true);
#else
  for( int i=0;i<argc && i+1<argc;i+=2 )
    props.insert( util::XMLDocument::PropertyP(argv[i],argv[i+1]) );

  xmlDoc.SetProperties(props);

  if(  xmlDoc.Load(fName) )
    hasValue = xmlDoc.GetNodeValue(xPathU,value,true);
#endif

  if( hasValue )
  { LOGGER_INFO<<xPath<<_T(":")<<value<<endl; }
  else
  { LOGGER_INFO<<xPath<<_T(": NOT FOUND")<<endl; }
} // of xmltest()

/**
 *
 */
void dirtest(char* dirName,char* prefix,UINT maxDepth)
{ DirectoryInfo dirInfo(dirName,prefix,maxDepth);

  dirInfo.Dump();
} // of dirtest()

/**
 *
 */
void msicab(char* fName,char* argv[],int argc)
{ util::XMLDocument            xmlDoc;
  COMPtr<IXMLDOMNodeList>      pXMLDomNodeList;
  COMPtr<IXMLDOMNode>          pNode;
  util::XMLDocument::PropertyM props;

#ifdef _UNICODE
  TCHAR fNameU[MAX_PATH];
  TCHAR propNameU[MAX_PATH];
  TCHAR propValueU[MAX_PATH];

  THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, fName, -1,fNameU, MAX_PATH) );

  for( int i=0;i<argc && i+1<argc;i+=2 )
  { THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, argv[i]  , -1,propNameU, MAX_PATH) );
    THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, argv[i+1], -1,propValueU, MAX_PATH) );

    props.insert( util::XMLDocument::PropertyP(propNameU,propValueU) );
  } // of for

  xmlDoc.SetProperties(props);

  if( xmlDoc.Load(fNameU) )
  { 
#else
  for( int i=0;i<argc && i+1<argc;i+=2 )
    props.insert( util::XMLDocument::PropertyP(argv[i],argv[i+1]) );

  xmlDoc.SetProperties(props);

  if(  xmlDoc.Load(fName) )
  { 
#endif
    COVariant productidValue;

    if( xmlDoc.GetNodeValue(_T("//v:product/@id"),productidValue,true) )
    { TString cabfilename = V_BSTR(productidValue);
      cabfilename += _T(".cab");

#ifdef _UNICODE
      char cabfilenameA[MAX_PATH];

      THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, cabfilename.c_str(), -1,cabfilenameA, MAX_PATH, NULL, NULL ) );

      CabFCIParameter cabParameter(cabfilenameA,CabFCIParameter::CDROM_SIZE);
#else
      CabFCIParameter cabParameter(cabfilename.c_str(),CabFCIParameter::CDROM_SIZE);
#endif

      ofstream componentIDT("component.idt");
      ofstream fileIDT("file.idt");

      componentIDT<<"Component\tComponentId\tDirectory_\tAttributes\tCondition\tKeyPath"<<endl;
      componentIDT<<"s72\tS38\ts72\ti2\tS255\tS72"<<endl;
      componentIDT<<"Component\tComponent"<<endl;

      fileIDT<<"File\tComponent_\tFileName\tFileSize\tVersion\tLanguage\tAttributes\tSequence"<<endl;
      fileIDT<<"s72\ts72\tl255\ti4\tS72\tS20\tI2\ti2"<<endl;
      fileIDT<<"File\tFile"<<endl;

      CabinetFCI cabinet(cabParameter);

      LOGGER_INFO<<_T("product:")<<productidValue<<endl; 

      xmlDoc.GetSelection(_T("//v:component[descendant::v:msiguid]/@id"),pXMLDomNodeList);

      int seqNo = 0;
      
      if( !pXMLDomNodeList.IsNULL() )
      { for( HRESULT hr = pXMLDomNodeList->nextNode(&pNode);hr==S_OK;hr = pXMLDomNodeList->nextNode(&pNode),seqNo++ )
        { COMString       nodeName;
          COVariant       nodeValue;
          const VARIANT*  v = nodeValue;
        
          THROW_COMEXCEPTION( pNode->get_nodeName(&nodeName) );
          THROW_COMEXCEPTION( pNode->get_nodeValue(const_cast<VARIANT*>(v)) );

          if( xmlDoc.GetProperty(pNode,nodeValue) )
          { LOGGER_INFO<<_T("id:")<<nodeValue<<endl; 

            COVariant filenameValue;
            TString filename = _T("//v:component[@id='");
            filename += V_BSTR(nodeValue);
            filename += _T("']/v:filename/text()");

            COVariant msiguidValue;
            TString msiguid = _T("//v:component[@id='");
            msiguid += V_BSTR(nodeValue);
            msiguid += _T("']/v:msiguid/text()");

            COVariant compTypeValue;
            TString compType = _T("//v:component[@id='");
            compType += V_BSTR(nodeValue);
            compType += _T("']/@type");

            if( xmlDoc.GetNodeValue(filename.c_str(),filenameValue,true) &&
                xmlDoc.GetNodeValue(msiguid.c_str(),msiguidValue,true) &&
                xmlDoc.GetNodeValue(compType.c_str(),compTypeValue,true)
              )
            { LOGGER_INFO<<_T("  filename:")<<filenameValue<<endl; 
              LOGGER_INFO<<_T("  comptype:")<<compTypeValue<<endl; 
              LOGGER_INFO<<_T("   msiguid:")<<msiguidValue<<endl; 

              TString compFileName = V_BSTR(filenameValue);
              compFileName += _T(".");
              compFileName += V_BSTR(compTypeValue);

              VersionInfo verInfo(compFileName.c_str());

              LPVOID fileVersion = verInfo.GetStringInfo(_T("FileVersion"));

#ifdef _UNICODE
              char fNameA[MAX_PATH];
              char msiguidA[MAX_PATH];
              char compcabfilename[MAX_PATH];
              char compIdA[MAX_PATH];
              char fileVersionA[MAX_PATH];

              THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, compFileName.c_str(), -1,fNameA, MAX_PATH, NULL, NULL ) );
              THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, V_BSTR(msiguidValue), -1,msiguidA, MAX_PATH, NULL, NULL ) );
              THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, V_BSTR(nodeValue), -1,compIdA, MAX_PATH, NULL, NULL ) );
              THROW_LASTERROREXCEPTION1( ::WideCharToMultiByte( CP_ACP, 0, (LPCTSTR)fileVersion, -1,fileVersionA, MAX_PATH, NULL, NULL ) );

              strcpy_s(compcabfilename,MAX_PATH,"_");
              strcat_s(compcabfilename,MAX_PATH,msiguidA);
              strcat_s(compcabfilename,MAX_PATH,"_");

              cabinet.AddFile(fNameA,NULL,compcabfilename);
#else
              cabinet.AddFile(V_BSTR(filenameValue));
#endif


              componentIDT<<compIdA<<"\t{"<<msiguidA<<'}'<<"\tBVRDIR\t0\t\t"<<compcabfilename<<endl;

              ULONG fileSize=42;

              fileIDT<<compcabfilename<<'\t'<<compIdA<<'\t'<<fNameA<<'\t'<<fileSize<<'\t'<<fileVersionA<<'\t'<<1033<<'\t'<<0<<'\t'<<seqNo<<endl;
            } // of if
          } // of if
        } // of for

        cabinet.Flush();
      } // of if
    } // of if
  } // of if
} // of msicab()

/**
 *
 */
void printUsage(LPCSTR progName)
{ LOGGER_INFO<<_T("Usage: "<<progName<<" [options] command cabfile [files] [dest_dir]")<<endl;
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
extern "C" int __cdecl main (int argc, char* argv[])
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
      
    if( strcmp(argv[1],"-xml")==0 && argc>=4 )
      xmltest(argv[2],argv[3],argc>=5 ? &argv[4] : NULL,argc-4);
    if( strcmp(argv[1],"-msicab")==0 && argc>=3 )
      msicab(argv[2],argc>=4 ? &argv[3] : NULL,argc-3);
    else if( strcmp(argv[1],"-dir")==0 && argc>=3 )
      dirtest(argv[2],argc>3 ? argv[3] : NULL,argc>4 ? atoi(argv[4]) : 0);
    else
    { char command = '\0';
      int  i       = 1;
      int  diskid  = CabFCIParameter::DISKID;
    
      for( ;i<argc;i++ )
      { 
        // options
        if( argv[i][0]=='-' )
        {
        } // of if
        else if( command=='\0' )
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
              if( !strcmp(argv[i], "+") )
              { cabinet.Flush(true);
                
                continue;
              } // of if
              
              if( DirectoryInfo::IsDirectoryA(argv[i]) )
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
