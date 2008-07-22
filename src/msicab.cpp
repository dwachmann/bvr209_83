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
#include "cab/cabinet.h"
#include "exception/bvr20983exception.h"
#include "exception/seexception.h"
#include "util/logstream.h"

using namespace bvr20983;
using namespace bvr20983::cab;
using namespace std;

/**
 *
 */
BOOL msicab(int num_files, char *file_list[])
{ CCAB cabParameter;
  
  Cabinet::Init(&cabParameter);
  
  { Cabinet cabinet(&cabParameter);
  
    for( int i=0;i<num_files;i++ )
    {
      /*
       * Flush the folder 
       */
      if( !strcmp(file_list[i], "+") )
      { cabinet.Flush(TRUE);
        
        continue;
      } // of if
      
      cabinet.AddFile(file_list[i]);
    } // of for
    
    cabinet.Flush();
  }

  return TRUE;
}

/**
 *
 */
void printUsage(LPCTSTR progName)
{ LOGGER_INFO<<progName<<_T(" [-C] [-a] [-u] [-d] <filename> [{FMT-ID}] <property args>")<<endl;
  LOGGER_INFO<<_T("Display and manipulate COM Property Sets")<<endl;
  LOGGER_INFO<<_T(" -a: add properties to property set")<<endl;
  LOGGER_INFO<<_T(" -u: update properties to property set")<<endl;
  LOGGER_INFO<<_T(" -d: delete properties to property set")<<endl;

  LOGGER_INFO<<_T(" -C: create new property set")<<endl;
  LOGGER_INFO<<_T(" -D: delete property set")<<endl;

  ::exit(0);
} // of printUsage()

/**
 *
 */
extern "C" int __cdecl _tmain (int argc, TCHAR  * argv[])
{ LogStreamT::ReadVersionInfo();

  LONG exHr = NOERROR;
  
  //__debugbreak();
    
  try
  { _set_se_translator( SEException::throwException );

    if( 1==argc || 0==_tcscmp(_T("-?"), argv[1]) || 0==_tcscmp( _T("/?"), argv[1]) )
      printUsage(argv[0]);

    if( msicab(argc-1, &argv[1]) )
      LOGGER_INFO<<_T("cabinets created ")<<endl;
    else
      LOGGER_INFO<<_T("msicap failed")<<endl;
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
  
  return (int)exHr;
} // of main()
//======================================END-OF-FILE=========================
