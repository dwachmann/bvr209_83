/*
 * $Id$
 * 
 * Wrapper class to access MS Cabinet files.
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
#include <sys/stat.h>
#include "cab/cabinetfdi.h"
#include "exception/bvr20983exception.h"
#include "exception/cabinetexception.h"
#include "exception/lasterrorexception.h"
#include "util/logstream.h"

using namespace std;

namespace bvr20983
{
  namespace cab
  {
    /*
     *
     */
    CabinetFDI::CabinetFDI(LPCSTR cabinet_fullpath,LPCSTR destDir) :
      m_hfdi(NULL),
      m_listOnly(TRUE)
    { strcpy_s(m_cabinetFullPath,ARRAYSIZE(m_cabinetFullPath),cabinet_fullpath);

      if( NULL==destDir )
        strcpy_s(m_destinationDir,ARRAYSIZE(m_destinationDir),".\\");
      else
      { strcpy_s(m_destinationDir,ARRAYSIZE(m_destinationDir),destDir);
       
        if( NULL==strrchr((LPSTR)m_destinationDir, '\\') )
          strcpy_s(m_destinationDir,ARRAYSIZE(m_destinationDir),"\\");
      } // of else

      char* p = strrchr((LPSTR)cabinet_fullpath, '\\');
    
      if( p==NULL )
      { strcpy_s(m_cabinetPath,ARRAYSIZE(m_cabinetPath), ".\\");
        strcpy_s(m_cabinetName,ARRAYSIZE(m_cabinetName), cabinet_fullpath);
      } // of if
      else
      { strncpy_s(m_cabinetPath,ARRAYSIZE(m_cabinetPath),cabinet_fullpath,p-cabinet_fullpath);
        strcat_s(m_cabinetPath,ARRAYSIZE(m_cabinetPath),"\\");
        
        strcpy_s(m_cabinetName,ARRAYSIZE(m_cabinetName), p+1);
      } // of else
      
      Init(); 
    }

    /*
     *
     */
    void CabinetFDI::Init()
    { m_hfdi = ::FDICreate( fdi_memalloc,
                            fdi_memfree,
                            fdi_fileopen,
                            fdi_fileread,
                            fdi_filewrite,
                            fdi_fileclose,
                            fdi_fileseek,
                            cpu80386,
                            &m_erf
                          );
    
      if( NULL==m_hfdi )
        THROW_CABEXCEPTION((FDIERROR)m_erf.erfOper);

      int hf = -1;
      
       _sopen_s( &hf, m_cabinetFullPath, _O_BINARY | _O_RDONLY | _O_SEQUENTIAL,_SH_DENYNO,0 );
      BOOL isCabinet = hf!=-1 && ::FDIIsCabinet(m_hfdi,hf,&m_fdici);

      if( hf!=-1 )
        _close(hf);

      if( !isCabinet )
        THROW_CABEXCEPTION(FDIERROR_NOT_A_CABINET);
        
      LOGGER_INFO<<_T("Listing of cabinet file '")<<m_cabinetFullPath<<_T("'")<<endl<<_T("  ");
      LOGGER_INFO<<m_fdici.cFiles<<_T(" file(s), ");
      LOGGER_INFO<<m_fdici.cFolders<<_T(" folder(s), ");
      LOGGER_INFO<<m_fdici.setID<<_T(" set ID 12345, ");
      LOGGER_INFO<<_T(" cabinet #")<<m_fdici.iCabinet<<_T(",");
      LOGGER_INFO<<_T(" reserve area ")<<m_fdici.fReserve<<_T(",");
      LOGGER_INFO<<_T(" Chained to prev ")<<m_fdici.hasprev;
      LOGGER_INFO<<_T(" / next ")<<m_fdici.hasnext;
      LOGGER_INFO<<endl;
    } // of CabinetFDI::Init()

    /**
     *
     */
    CabinetFDI::~CabinetFDI()
    { if( NULL!=m_hfdi )
        ::FDIDestroy(m_hfdi);
        
      m_hfdi = NULL;
    } // of CabinetFDI::~CabinetFDI()

    /**
     *
     */
    void CabinetFDI::Extract()
    { m_listOnly = FALSE;
    
      if( !::FDICopy(m_hfdi,m_cabinetName,m_cabinetPath,0,fdi_notification,NULL,this) )
        THROW_CABEXCEPTION((FDIERROR)m_erf.erfOper);
    }

    /**
     *
     */
    void CabinetFDI::List()
    { m_listOnly = TRUE;
      
      if( !::FDICopy(m_hfdi,m_cabinetName,m_cabinetPath,0,fdi_notification,NULL,this) )
        THROW_CABEXCEPTION((FDIERROR)m_erf.erfOper);
    }

    /*
     * Memory allocation function
     */
    FNALLOC(CabinetFDI::fdi_memalloc)
    { return ::malloc(cb); }


    /*
     * Memory free function
     */
    FNFREE(CabinetFDI::fdi_memfree)
    { ::free(pv); }

    /*
     * 
     */
    FNOPEN(CabinetFDI::fdi_fileopen)
    { int hf = -1;
      
      _sopen_s(&hf, pszFile, oflag, _SH_DENYNO, pmode);
      
      return hf;
    }

    /*
     * 
     */    
    FNREAD(CabinetFDI::fdi_fileread)
    { return _read(hf, pv, cb); }

    /*
     * 
     */
    FNWRITE(CabinetFDI::fdi_filewrite)
    { return _write(hf, pv, cb); }

    /*
     * 
     */
    FNCLOSE(CabinetFDI::fdi_fileclose)
    { return _close(hf); }

    /*
     * 
     */
    FNSEEK(CabinetFDI::fdi_fileseek)
    { return _lseek(hf, dist, seektype); }

    /*
     * 
     */
    FNFDINOTIFY(CabinetFDI::fdi_notification)
    { return NULL!=pfdin->pv ? ((CabinetFDI*)pfdin->pv)->FDINotification(fdint,pfdin) : -1; }

    /*
     * 
     */
    int CabinetFDI::FDINotification(FDINOTIFICATIONTYPE fdint,PFDINOTIFICATION pfdin)
    { int    result  = 0;
      DWORD  attrs   = 0;
      HANDLE fHandle = NULL;
      char   destination[MAX_PATH];
    
      switch( fdint )
      {
        case fdintCABINET_INFO: // general information about the cabinet
          LOGGER_INFO<<_T("fdintCABINET_INFO")<<endl;
          LOGGER_INFO<<_T("  next cabinet     = ")<<pfdin->psz1<<endl;
          LOGGER_INFO<<_T("  next disk        = ")<<pfdin->psz2<<endl;
          LOGGER_INFO<<_T("  cabinet path     = ")<<pfdin->psz3<<endl;
          LOGGER_INFO<<_T("  cabinet set ID   = ")<<pfdin->setID<<endl;
          LOGGER_INFO<<_T("  cabinet # in set = ")<<pfdin->iCabinet<<endl;
          break;
        case fdintPARTIAL_FILE: // first file in cabinet is continuation
          LOGGER_INFO<<_T("fdintPARTIAL_FILE")<<endl;
          LOGGER_INFO<<_T("   name of continued file            = ")<<pfdin->psz1<<endl;
          LOGGER_INFO<<_T("   name of cabinet where file starts = ")<<pfdin->psz2<<endl;
          LOGGER_INFO<<_T("   name of disk where file starts    = ")<<pfdin->psz3<<endl;
          break;

        case fdintCOPY_FILE:  // file to be copied
          LOGGER_INFO<<_T("fdintCOPY_FILE[")<<m_listOnly<<_T("]")<<endl;
          LOGGER_INFO<<_T("  file name in cabinet = ")<<pfdin->psz1<<endl;
          LOGGER_INFO<<_T("  uncompressed file size = ")<<pfdin->cb<<endl;

          sprintf_s(destination,MAX_PATH, "%s%s", m_destinationDir,pfdin->psz1);

          if( m_listOnly )
            result = 0;
          else
            _sopen_s( &result, destination,_O_BINARY | _O_CREAT | _O_WRONLY | _O_SEQUENTIAL,_SH_DENYNO,_S_IREAD | _S_IWRITE);
          break;

        case fdintCLOSE_FILE_INFO:  // close the file, set relevant info
          LOGGER_INFO<<_T("fdintCLOSE_FILE_INFO")<<endl;
          LOGGER_INFO<<_T("   file name in cabinet = ")<<pfdin->psz1<<endl;

          _close(pfdin->hf);

#ifdef _UNICODE
          TCHAR destinationU[MAX_PATH];
    
          THROW_LASTERROREXCEPTION1( ::MultiByteToWideChar( CP_ACP, 0, destination, MAX_PATH,destinationU, MAX_PATH) );
#endif

          sprintf_s(destination,MAX_PATH, "%s%s", m_destinationDir,pfdin->psz1);

#ifdef _UNICODE
          fHandle = ::CreateFile(destinationU,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
#else
          fHandle = ::CreateFile(destination,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
#endif

          if( INVALID_HANDLE_VALUE!=fHandle )
          {
            FILETIME datetime;

            if( ::DosDateTimeToFileTime(pfdin->date,pfdin->time,&datetime) )
            {
              FILETIME local_filetime;

              if( ::LocalFileTimeToFileTime(&datetime,&local_filetime) )
              {
                ::SetFileTime(fHandle,&local_filetime,NULL,&local_filetime);
                ::CloseHandle(fHandle);
              }

              /*
               * Mask out attribute bits other than readonly,
               * hidden, system, and archive, since the other
               * attribute bits are reserved for use by
               * the cabinet format.
               */
#ifdef _UNICODE
              ::SetFileAttributes(destinationU,pfdin->attribs & (_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH) );
#else
              ::SetFileAttributes(destination,pfdin->attribs & (_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH) );
#endif
            } // of if
          } // of if
            
          result = TRUE;
          break;

        case fdintNEXT_CABINET: // file continued to next cabinet
          LOGGER_INFO<<_T("fdintNEXT_CABINET")<<endl;
          LOGGER_INFO<<_T("   name of next cabinet where file continued = ")<<pfdin->psz1<<endl;
          LOGGER_INFO<<_T("   name of next disk where file continued    = ")<<pfdin->psz2<<endl;
          LOGGER_INFO<<_T("   cabinet path name                         = ")<<pfdin->psz3<<endl;
          break;
      } // of switch()

      return result;
    } // of CabinetFDI::FDINotification()
  } // of namespace COM
} // of namespace bvr20983
//======================================END-OF-FILE=========================
