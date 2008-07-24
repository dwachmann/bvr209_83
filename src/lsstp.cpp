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
#include "util/comptr.h"
#include "util/guid.h"
#include "exception/bvr20983exception.h"
#include "exception/lasterrorexception.h"
#include "exception/memoryexception.h"
#include "exception/comexception.h"
#include "exception/seexception.h"
#include "com/copropertyset.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/versioninfo.h"

using namespace bvr20983;
using namespace bvr20983::COM;
using namespace bvr20983::util;
using namespace std;


/**
 *
 */
void printUsage(LPCTSTR progName)
{ LOGGER_INFO<<_T("Usage:")<<endl;
  LOGGER_INFO<<_T("  ")<<progName<<_T(" [-C] [-a] [-u] [-d] <filename> [{FMT-ID}] <property args>")<<endl;
  LOGGER_INFO<<_T("  ")<<_T("Display and manipulate COM Property Sets")<<endl;
  LOGGER_INFO<<_T("  ")<<_T(" -a: add properties to property set")<<endl;
  LOGGER_INFO<<_T("  ")<<_T(" -u: update properties to property set")<<endl;
  LOGGER_INFO<<_T("  ")<<_T(" -d: delete properties to property set")<<endl;

  LOGGER_INFO<<_T("  ")<<_T(" -C: create new property set")<<endl;
  LOGGER_INFO<<_T("  ")<<_T(" -D: delete property set")<<endl;

  ::exit(0);
} // of printUsage()


/**
 *
 */
void DumpFile(TCHAR* fileName)
{ COMPtr<IStorage> pStg;

  THROW_COMEXCEPTION(  ::StgOpenStorageEx( fileName,
                                           STGM_READ | STGM_SHARE_EXCLUSIVE,
                                           STGFMT_ANY,
                                           0,
                                           NULL,
                                           NULL,
                                           IID_IStorage,
                                           reinterpret_cast<void**>(&pStg) 
                                         ) 
                    );

  LOGGER_INFO<<pStg;
  
  //DisplayStorageTree( pStg,0 );
} // of DumpFile()

enum CommandType
{ LIST_CMD,
  ADD_CMD,
  UPDATE_CMD,
  DELETE_CMD,
  DELETEPROPERTYSET_CMD
};

/**
 *
 */
bool ChangePropertySet(bool createPropSet,CommandType cmd,TCHAR* fileName,TCHAR* propertySetId,int argvOff,int argc, TCHAR* argv[])
{ bool   result = false;
  FMTID  fmtId;

  if( SUCCEEDED(::IIDFromString(propertySetId,&fmtId)) )
  { auto_ptr<COPropertySet> pPropertySet(new COPropertySet(fileName,fmtId));

    PROPVARIANT prop;

    ::PropVariantInit( &prop );

#ifdef _UNICODE
    prop.vt = VT_LPWSTR;
#else
    prop.vt = VT_LPSTR;
#endif

    for( int i=argvOff;i<argc;i++ )
    { PROPID propId = (PROPID)::_ttol(argv[i]);

      if( cmd==ADD_CMD || cmd==UPDATE_CMD )
      {
        if( i+1>=argc )
          break;

#ifdef _UNICODE
        prop.pwszVal = argv[++i];
#else
        prop.pszVal = argv[++i];
#endif
      } // of if

      switch( cmd )
      {
      case ADD_CMD:
        pPropertySet->Add(propId,prop);
        break;
      case UPDATE_CMD:
        pPropertySet->Update(propId,prop);
        break;
      case DELETE_CMD:
        pPropertySet->Delete(propId);
        break;
      } // of switch
    } // of for

    pPropertySet->Write(createPropSet);

    result = true;
  } // of if

  return result;
} // of ChangePropertySet()


/**
 *
 */
extern "C" int __cdecl _tmain (int argc, TCHAR  * argv[])
{ LogStreamT::ReadVersionInfo();

  LONG             exHr = NOERROR;
  COMPtr<IStorage> pStg;
    
  try
  { _set_se_translator( SEException::throwException );

    { VersionInfo verInfo;
    
      verInfo.LogCopyright();
    }
    
    if( 1==argc || 0==_tcscmp(_T("-?"), argv[1]) || 0==_tcscmp( _T("/?"), argv[1]) )
      printUsage(argv[0]);

    CommandType      cmd           = LIST_CMD;
    bool             createPropSet = false;
    int              i             = 1;

    for( ;i<argc;i++ )
    { if( _tcscmp(argv[i],_T("-a"))==0 )
        cmd = ADD_CMD;
      else if( _tcscmp(argv[i],_T("-u"))==0 )
        cmd = UPDATE_CMD;
      else if( _tcscmp(argv[i],_T("-d"))==0 )
        cmd = DELETE_CMD;
      else if( _tcscmp(argv[i],_T("-D"))==0 )
        cmd = DELETEPROPERTYSET_CMD;
      else if( _tcscmp(argv[i],_T("-C"))==0 )
        createPropSet = true;
      else if( argv[i][0]==_T('-') )
        printUsage(argv[0]);
      else
        break;
    } // of for

    switch( cmd )
    { 
    case LIST_CMD:
      if( i+1!=argc )
        printUsage(argv[0]);

      DumpFile(argv[i]);
      break;
    case ADD_CMD:
    case UPDATE_CMD:
    case DELETE_CMD:
      if( i+2>=argc )
        printUsage(argv[0]);

      if( ChangePropertySet(createPropSet,cmd,argv[i],argv[i+1],i+2,argc,argv) )
        DumpFile(argv[i]);
      break;
    case DELETEPROPERTYSET_CMD:
      if( i+2!=argc )
        printUsage(argv[0]);

      { FMTID  fmtId;
        bool   success=false;

        if( SUCCEEDED(::IIDFromString(argv[i+1],&fmtId)) )
        { auto_ptr<COPropertySet> pPropertySet(new COPropertySet(argv[i],fmtId));

          pPropertySet->DeleteSet();
          success = true;
        } // of if

        if( success )
          DumpFile(argv[i]);
      } 
      break;
    default:
      printUsage(argv[0]);
      break;
    } // of switch
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
