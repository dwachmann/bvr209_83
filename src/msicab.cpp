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
#include "com/covariant.h"
#include "exception/bvr20983exception.h"
#include "exception/seexception.h"
#include "exception/lasterrorexception.h"

using namespace bvr20983;
using namespace bvr20983::cab;
using namespace bvr20983::util;
using namespace std;

/**
 *
 */
void xmltest(char* fName,char* xPath,char* argv[],int argc)
{ bvr20983::util::XMLDocument            xmlDoc;
  COM::COVariant                         value;
  boolean                                hasValue=false;
  bvr20983::util::XMLDocument::PropertyM props;

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

    props.insert( bvr20983::util::XMLDocument::PropertyP(propNameU,propValueU) );
  } // of for

  xmlDoc.SetProperties(props);

  if( xmlDoc.Load(fNameU) )
    hasValue = xmlDoc.GetNodeValue(xPathU,value,true);
#else
  for( int i=0;i<argc && i+1<argc;i+=2 )
    props.insert( bvr20983::util::XMLDocument::PropertyP(argv[i],argv[i+1]) );

  xmlDoc.SetProperties(props);

  if(  xmlDoc.Load(fName) )
    hasValue = xmlDoc.GetNodeValue(xPathU,value,true);
#endif

  if( hasValue )
  { LOGGER_INFO<<xPath<<_T(":")<<value<<endl; }
  else
  { LOGGER_INFO<<xPath<<_T(": NOT FOUND")<<endl; }
}

/**
 *
 */
void dirtest(char* dirName,char* prefix,UINT maxDepth)
{ DirectoryInfo dirInfo(dirName,prefix,maxDepth);

  dirInfo.Dump();
}

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
