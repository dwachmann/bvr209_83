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
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "cab/fci.h"
#include "exception/bvr20983exception.h"
#include "exception/lasterrorexception.h"
#include "exception/memoryexception.h"
#include "exception/comexception.h"
#include "exception/seexception.h"
#include "util/logstream.h"

using namespace bvr20983;
using namespace std;


/*
 * When a CAB file reaches this size, a new CAB will be created
 * automatically.  This is useful for fitting CAB files onto disks.
 *
 * If you want to create just one huge CAB file with everything in
 * it, change this to a very very large number.
 */
#define MEDIA_SIZE      300000

/*
 * When a folder has this much compressed data inside it,
 * automatically flush the folder.
 *
 * Flushing the folder hurts compression a little bit, but
 * helps random access significantly.
 */
#define FOLDER_THRESHOLD  900000


/*
 * Compression type to use
 */

#define COMPRESSION_TYPE    tcompTYPE_MSZIP




/*
 * Our internal state
 *
 * The FCI APIs allow us to pass back a state pointer of our own
 */
typedef struct
{
  long  total_compressed_size;    /* total compressed size so far */
  long  total_uncompressed_size;  /* total uncompressed size so far */
} client_state;

/**
 *
 */
void strip_path(char *filename, char *stripped_name)
{ char  *p;

  p = strrchr(filename, '\\');

  if (p == NULL)
    _tcscpy_s(stripped_name,MAX_PATH, filename);
  else
    _tcscpy_s(stripped_name,MAX_PATH, p+1);
}

/**
 *
 */
char *return_fci_error_string(FCIERROR err)
{ switch (err)
  {
    case FCIERR_NONE:
      return _T("No error");

    case FCIERR_OPEN_SRC:
      return _T("Failure opening file to be stored in cabinet");
    
    case FCIERR_READ_SRC:
      return _T("Failure reading file to be stored in cabinet");
    
    case FCIERR_ALLOC_FAIL:
      return _T("Insufficient memory in FCI");

    case FCIERR_TEMP_FILE:
      return _T("Could not create a temporary file");

    case FCIERR_BAD_COMPR_TYPE:
      return _T("Unknown compression type");

    case FCIERR_CAB_FILE:
      return _T("Could not create cabinet file");

    case FCIERR_USER_ABORT:
      return _T("Client requested abort");

    case FCIERR_MCI_FAIL:
      return _T("Failure compressing data");

    default:
      return "Unknown error";
  }
} // of return_fci_error_string()

/**
 *
 */
int get_percentage(unsigned long a, unsigned long b)
{
  while (a > 10000000)
  {
    a >>= 3;
    b >>= 3;
  }

  if (b == 0)
    return 0;

  return ((a*100)/b);
}

/*
 * Memory allocation function
 */
FNFCIALLOC(mem_alloc)
{
  return malloc(cb);
}


/*
 * Memory free function
 */
FNFCIFREE(mem_free)
{
  free(memory);
}


/*
 * File i/o functions
 */
FNFCIOPEN(fci_open)
{ int result = -1;

  if( _tsopen_s(&result, pszFile, oflag, _SH_DENYNO, pmode) )
    *err = errno;

  return result;
}

/**
 *
 */
FNFCIREAD(fci_read)
{ unsigned int result = (unsigned int) _read(hf, memory, cb);

  if( result!=cb )
    *err = errno;

  return result;
}

/**
 *
 */
FNFCIWRITE(fci_write)
{ unsigned int result = (unsigned int) _write(hf, memory, cb);

  if( result!=cb )
    *err = errno;

  return result;
}

/**
 *
 */
FNFCICLOSE(fci_close)
{ int result = _close(hf);

  if (result != 0)
    *err = errno;

  return result;
}

/**
 *
 */
FNFCISEEK(fci_seek)
{ long result = _lseek(hf, dist, seektype);

  if (result == -1)
    *err = errno;

  return result;
}

/**
 *
 */
FNFCIDELETE(fci_delete)
{ int result = remove(pszFile);

  if( result!=0 )
    *err = errno;

  return result;
}


/*
 * File placed function called when a file has been committed
 * to a cabinet
 */
FNFCIFILEPLACED(file_placed)
{
  LOGGER_INFO<<_T("   placed file '")<<pszFile<<_T("' (size ")<<cbFile<<_T(") on cabinet '")<<pccab->szCab<<_T("'\n");

  if( fContinuation )
    LOGGER_INFO<<_T("      (Above file is a later segment of a continued file)")<<endl;

  return 0;
}


/*
 * Function to obtain temporary files
 */
FNFCIGETTEMPFILE(get_temp_file)
{ char* psz;

  psz = _tempnam("","xx");            // Get a name

  if ((psz != NULL) && (strlen(psz) < (unsigned)cbTempName)) 
  {
    _tcscpy_s(pszTempName,cbTempName,psz); // Copy to caller's buffer
    
    ::free(psz);                      // Free temporary name buffer

    return TRUE;                    // Success
  }
  
  //** Failed
  if( psz )
    ::free(psz);

  return FALSE;
}


/*
 * Progress function
 */
FNFCISTATUS(progress)
{ client_state* cs = (client_state *) pv;

  if( typeStatus==statusFile )
  {
    cs->total_compressed_size += cb1;
    cs->total_uncompressed_size += cb2;

    /*
     * Compressing a block into a folder
     *
     * cb2 = uncompressed size of block
     */
    LOGGER_INFO<<_T("Compressing: ")<<cs->total_uncompressed_size<<_T(" -> ")<<cs->total_compressed_size<<endl;
  }
  else if (typeStatus == statusFolder)
  {
    /*
     * Adding a folder to a cabinet
     *
     * cb1 = amount of folder copied to cabinet so far
     * cb2 = total size of folder
     */
    int percentage = get_percentage(cb1, cb2);

    LOGGER_INFO<<_T("Copying folder to cabinet: ")<<percentage<<_T("%")<<endl;
  }

  return 0;
}

/**
 *
 */
void store_cab_name(char *cabname, int iCab)
{
  _stprintf_s(cabname,CB_MAX_CAB_PATH, "hugo-%d.cab", iCab);
}


/**
 *
 */
FNFCIGETNEXTCABINET(get_next_cabinet)
{
  store_cab_name(pccab->szCab, pccab->iCab);

  return TRUE;
}


/**
 *
 */
FNFCIGETOPENINFO(get_open_info)
{
  BY_HANDLE_FILE_INFORMATION finfo;
  FILETIME                   filetime;
  HANDLE                     handle;
  DWORD                      attrs;
  int                        hf;
  
  LOGGER_INFO<<_T("get_open_info(): pszName=")<<pszName<<endl;

    /*
     * Need a Win32 type handle to get file date/time
     * using the Win32 APIs, even though the handle we
     * will be returning is of the type compatible with
     * _open
     */
  handle = ::CreateFile(pszName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,NULL);
   
  if( handle==INVALID_HANDLE_VALUE )
  {
    return -1;
  }

  if( !::GetFileInformationByHandle(handle, &finfo) )
  {
    ::CloseHandle(handle);
    
    return -1;
  }
   
  FileTimeToLocalFileTime(&finfo.ftLastWriteTime, &filetime);

  FileTimeToDosDateTime(&filetime,pdate,ptime);

  attrs = GetFileAttributes(pszName);

  if (attrs == 0xFFFFFFFF)
    *pattribs = 0;
  else
  {
    /*
     * Mask out all other bits except these four, since other
     * bits are used by the cabinet format to indicate a
     * special meaning.
     */
    *pattribs = (int) (attrs & (_A_RDONLY | _A_SYSTEM | _A_HIDDEN | _A_ARCH));
  }

  ::CloseHandle(handle);


  /*
   * Return handle using _open
   */
  if( _tsopen_s( &hf, pszName, _O_RDONLY | _O_BINARY,_SH_DENYNO,0 ) )
    return -1; // abort on error
   
  return hf;
}


/**
 *
 */
void set_cab_parameters(PCCAB cab_parms)
{
  ::memset(cab_parms, 0, sizeof(CCAB));

  cab_parms->cb = MEDIA_SIZE;
  cab_parms->cbFolderThresh = FOLDER_THRESHOLD;

  /*
   * Don't reserve space for any extensions
   */
  cab_parms->cbReserveCFHeader = 0;
  cab_parms->cbReserveCFFolder = 0;
  cab_parms->cbReserveCFData   = 0;

  /*
   * We use this to create the cabinet name
   */
  cab_parms->iCab = 1;

  /*
   * If you want to use disk names, use this to
   * count disks
   */
  cab_parms->iDisk = 0;

  /*
   * Choose your own number
   */
  cab_parms->setID = 12345;

  /*
   * Only important if CABs are spanning multiple
   * disks, in which case you will want to use a
   * real disk name.
   *
   * Can be left as an empty string.
   */
  _tcscpy_s(cab_parms->szDisk,CB_MAX_DISK_NAME, "MyDisk");

  /* where to store the created CAB files */
  _tcscpy_s(cab_parms->szCabPath,CB_MAX_CAB_PATH, "C:\\temp\\");

  /* store name of first CAB file */
  store_cab_name(cab_parms->szCab, cab_parms->iCab);
}


/**
 *
 */
BOOL msicab(int num_files, char *file_list[])
{
  HFCI      hfci;
  ERF       erf;
  CCAB      cab_parameters;
  int       i;
  client_state  cs;

  /*  
   * Initialise our internal state
   */
  cs.total_compressed_size   = 0;
  cs.total_uncompressed_size = 0;

  set_cab_parameters(&cab_parameters);

  hfci = FCICreate(&erf,file_placed,mem_alloc,mem_free,fci_open,fci_read,fci_write,fci_close,fci_seek,fci_delete,get_temp_file,&cab_parameters,&cs);

  if (hfci == NULL)
  {
    LOGGER_ERROR<<_T("FCICreate() failed: code ")<<erf.erfOper<<_T("[")<<return_fci_error_string((FCIERROR)erf.erfOper)<<_T("]")<<endl;

    return FALSE;
  }

  for (i = 0; i < num_files; i++)
  {
    char stripped_name[MAX_PATH];

    /*
     * Flush the folder?
     */
    if( !strcmp(file_list[i], "+") )
    {
      if( !FCIFlushFolder(hfci,get_next_cabinet,progress) )
      { LOGGER_ERROR<<_T("FCIFlushFolder() failed: code ")<<erf.erfOper<<_T("[")<<return_fci_error_string((FCIERROR)erf.erfOper)<<_T("]")<<endl;

        FCIDestroy(hfci);

        return FALSE;
      }

      continue;
    }

    /*
     * Don't store the path name in the cabinet file!
     */
    strip_path(file_list[i], stripped_name);

    if( !FCIAddFile(hfci,
                    file_list[i],  /* file to add */
                    stripped_name, /* file name in cabinet file */
                    FALSE,         /* file is not executable */
                    get_next_cabinet,
                    progress,
                    get_open_info,
                    COMPRESSION_TYPE
                   )
      )
    {
      LOGGER_ERROR<<_T("FCIAddFile() failed: code ")<<erf.erfOper<<_T("[")<<return_fci_error_string((FCIERROR)erf.erfOper)<<_T("]")<<endl;

      FCIDestroy(hfci);
  
      return FALSE;
    }
  }   

  /*
   * This will automatically flush the folder first
   */
  if( !FCIFlushCabinet(hfci,FALSE,get_next_cabinet,progress) )
  { DWORD result    = 0;
    DWORD lastError = GetLastError();
    LPSTR errorMsg  = NULL;

    LOGGER_ERROR<<_T("FCIFlushCabinet() failed: code ")<<erf.erfOper<<_T("[")<<return_fci_error_string((FCIERROR)erf.erfOper)<<_T("]")<<endl;

    FCIDestroy(hfci);

    return FALSE;
  }

  if( !FCIDestroy(hfci) )
  {
    LOGGER_ERROR<<_T("FCIDestroy() failed: code ")<<erf.erfOper<<_T("[")<<return_fci_error_string((FCIERROR)erf.erfOper)<<_T("]")<<endl;

    return FALSE;
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
