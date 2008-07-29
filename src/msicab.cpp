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
#include "util/versioninfo.h"
#include "util/xmldocument.h"
#include "util/dirinfo.h"
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
BOOL FileExists(LPCSTR fPath)
{ int hf = -1;
  
   _sopen_s( &hf, fPath, _O_BINARY | _O_RDONLY | _O_SEQUENTIAL,_SH_DENYNO,0 );

  if( hf!=-1 )
    _close(hf);
    
  return hf!=-1;
} // of FileExists()


/**
 *
 */
void msicabcreate(int num_files, char *file_list[])
{ CabFCIParameter cabParameter(file_list[0]);
  CabinetFCI      cabinet(cabParameter);
  
  for( int i=1;i<num_files;i++ )
  {
    if( !strcmp(file_list[i], "+") )
    { cabinet.Flush(TRUE);
      
      continue;
    } // of if
    
    cabinet.AddFile(file_list[i]);
  } // of for
  
  cabinet.Flush();
}

/**
 *
 */
void msicabextract(int num_files, char *file_list[])
{ CabinetFDI cabinet(file_list[0]);

  cabinet.Extract();
}

/**
 *
 */
void msicablist(int num_files, char *file_list[])
{ CabinetFDI cabinet(file_list[0]);

  cabinet.List();
}

/**
 *
 */
void xmltest(char* fName)
{ CXMLDocument xmlDoc;

#ifdef _UNICODE
  TCHAR fNameU[MAX_PATH];

  THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, fName, MAX_PATH,fNameU, MAX_PATH) );

  xmlDoc.Load(fNameU);
#else
  xmlDoc.Load(fName);
#endif
}

/**
 *
 */
void printUsage(LPCSTR progName)
{ LOGGER_INFO<<_T("Usage:")<<endl;
  LOGGER_INFO<<_T("  ")<<progName<<_T(" <cabinetname> <dir|file> [<dir|file> ]")<<endl;

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
      
    if( strcmp(argv[1],"-xml")==0 && argc>=3 )
      xmltest(argv[2]);
    else
    {
      if( !FileExists(argv[1]) )
      { if( argc>2 )
          msicabcreate(argc-1, &argv[1]);
      } // of if
      else
      { if( argc==2 )
          msicablist(argc-1, &argv[1]);
        else if( argc>=2 )
          msicabextract(argc-1, &argv[1]);
      } // of else
    } // of else
  }
  catch(BVR20983Exception& e)
  { LOGGER_ERROR<<e;

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
