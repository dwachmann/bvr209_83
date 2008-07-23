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
#include "util/logstream.h"

using namespace std;

namespace bvr20983
{
  namespace cab
  {
    TCHAR CabinetFDI::m_destinationDir[MAX_PATH];

    /*
     *
     */
    CabinetFDI::CabinetFDI(LPCTSTR cabinet_fullpath,LPCTSTR destDir) :
      m_hfdi(NULL)
    { _tcscpy_s(m_cabinetFullPath,ARRAYSIZE(m_cabinetFullPath),cabinet_fullpath);

      if( NULL==destDir )
        _tcscpy_s(m_destinationDir,ARRAYSIZE(m_destinationDir),_T(".\\"));
      else
        _tcscpy_s(m_destinationDir,ARRAYSIZE(m_destinationDir),destDir);

      char* p = strrchr((LPTSTR)cabinet_fullpath, '\\');
    
      if( p==NULL )
      { _tcscpy_s(m_cabinetPath,ARRAYSIZE(m_cabinetPath), _T(".\\"));
        _tcscpy_s(m_cabinetName,ARRAYSIZE(m_cabinetName), cabinet_fullpath);
      } // of if
      else
      { _tcsncpy_s(m_cabinetPath,ARRAYSIZE(m_cabinetPath),cabinet_fullpath,p-cabinet_fullpath);
        _tcscat_s(m_cabinetPath,ARRAYSIZE(m_cabinetPath),_T("\\"));
        
        _tcscpy_s(m_cabinetName,ARRAYSIZE(m_cabinetName), p+1);
      } // of else
      
      Init(); 
    }

    /*
     *
     */
    void CabinetFDI::Init()
    { m_hfdi = ::FDICreate( mem_alloc,
		                        mem_free,
		                        file_open,
		                        file_read,
		                        file_write,
		                        file_close,
		                        file_seek,
		                        cpu80386,
		                        &m_erf
	                        );
    
      if( NULL==m_hfdi )
        THROW_CABEXCEPTION((FDIERROR)m_erf.erfOper);

	    int  hf        = file_open(m_cabinetFullPath,_O_BINARY | _O_RDONLY | _O_SEQUENTIAL,0);
      BOOL isCabinet = hf!=-1 && ::FDIIsCabinet(m_hfdi,hf,&m_fdici);

      if( hf!=-1 )
        _close(hf);

      if( !isCabinet )
        THROW_CABEXCEPTION(FDIERROR_NOT_A_CABINET);

      printf(
			"Information on cabinet file '%s'\n"
			"   Total length of cabinet file : %d\n"
			"   Number of folders in cabinet : %d\n"
			"   Number of files in cabinet   : %d\n"
			"   Cabinet set ID               : %d\n"
			"   Cabinet number in set        : %d\n"
			"   RESERVE area in cabinet?     : %s\n"
			"   Chained to prev cabinet?     : %s\n"
			"   Chained to next cabinet?     : %s\n"
			"\n",
			m_cabinetFullPath,
			m_fdici.cbCabinet,
			m_fdici.cFolders,
			m_fdici.cFiles,
			m_fdici.setID,
			m_fdici.iCabinet,
			m_fdici.fReserve == TRUE ? "yes" : "no",
			m_fdici.hasprev == TRUE ? "yes" : "no",
			m_fdici.hasnext == TRUE ? "yes" : "no"
		);
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
    { if( !FDICopy(m_hfdi,m_cabinetPath,m_cabinetName,0,notification_function,NULL,NULL) )
        THROW_CABEXCEPTION((FDIERROR)m_erf.erfOper);
    }

    /*
     * Memory allocation function
     */
    FNALLOC(CabinetFDI::mem_alloc)
    {
	    return malloc(cb);
    }


    /*
     * Memory free function
     */
    FNFREE(CabinetFDI::mem_free)
    {
	    free(pv);
    }


    FNOPEN(CabinetFDI::file_open)
    {
	    return _open(pszFile, oflag, pmode);
    }


    FNREAD(CabinetFDI::file_read)
    {
	    return _read(hf, pv, cb);
    }


    FNWRITE(CabinetFDI::file_write)
    {
	    return _write(hf, pv, cb);
    }


    FNCLOSE(CabinetFDI::file_close)
    {
	    return _close(hf);
    }


    FNSEEK(CabinetFDI::file_seek)
    {
	    return _lseek(hf, dist, seektype);
    }


    FNFDINOTIFY(CabinetFDI::notification_function)
    {
	    switch (fdint)
	    {
		    case fdintCABINET_INFO: // general information about the cabinet
			    printf(
				    "fdintCABINET_INFO\n"
				    "  next cabinet     = %s\n"
				    "  next disk        = %s\n"
				    "  cabinet path     = %s\n"
				    "  cabinet set ID   = %d\n"
				    "  cabinet # in set = %d (zero based)\n"
				    "\n",
				    pfdin->psz1,
				    pfdin->psz2,
				    pfdin->psz3,
				    pfdin->setID,
				    pfdin->iCabinet
			    );

			    return 0;

		    case fdintPARTIAL_FILE: // first file in cabinet is continuation
			    printf(
				    "fdintPARTIAL_FILE\n"
				    "   name of continued file            = %s\n"
				    "   name of cabinet where file starts = %s\n"
				    "   name of disk where file starts    = %s\n",
				    pfdin->psz1,
				    pfdin->psz2,
				    pfdin->psz3
			    );
			    return 0;

		    case fdintCOPY_FILE:	// file to be copied
		    {
			    int		response;
			    int		handle;
			    char	destination[256];

			    printf(
				    "fdintCOPY_FILE\n"
				    "  file name in cabinet = %s\n"
				    "  uncompressed file size = %d\n"
				    "  copy this file? (y/n): ",
				    pfdin->psz1,
				    pfdin->cb
			    );

			    do
			    {
				    response = getc(stdin);
				    response = toupper(response);
			    } while (response != 'Y' && response != 'N');

			    printf("\n");

			    if (response == 'Y')
			    {
				    sprintf(
					    destination, 
					    "%s%s",
					    m_destinationDir,
					    pfdin->psz1
				    );

				    handle = file_open(
					    destination,
					    _O_BINARY | _O_CREAT | _O_WRONLY | _O_SEQUENTIAL,
					    _S_IREAD | _S_IWRITE 
				    );

				    return handle;
			    }
			    else
			    {
				    return 0; /* skip file */
			    }
		    }

		    case fdintCLOSE_FILE_INFO:	// close the file, set relevant info
            {
                HANDLE  handle;
                DWORD   attrs;
                char    destination[256];

 			    printf(
				    "fdintCLOSE_FILE_INFO\n"
				    "   file name in cabinet = %s\n"
				    "\n",
				    pfdin->psz1
			    );

                sprintf(
                    destination, 
                    "%s%s",
                    m_destinationDir,
                    pfdin->psz1
                );

			    file_close(pfdin->hf);


                /*
                 * Set date/time
                 *
                 * Need Win32 type handle for to set date/time
                 */
                handle = CreateFile(
                    destination,
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL
                );

                if (handle != INVALID_HANDLE_VALUE)
                {
                    FILETIME    datetime;

                    if (TRUE == DosDateTimeToFileTime(
                        pfdin->date,
                        pfdin->time,
                        &datetime))
                    {
                        FILETIME    local_filetime;

                        if (TRUE == LocalFileTimeToFileTime(
                            &datetime,
                            &local_filetime))
                        {
                            (void) SetFileTime(
                                handle,
                                &local_filetime,
                                NULL,
                                &local_filetime
                            );
                         }
                    }

                    CloseHandle(handle);
                }

                /*
                 * Mask out attribute bits other than readonly,
                 * hidden, system, and archive, since the other
                 * attribute bits are reserved for use by
                 * the cabinet format.
                 */
                attrs = pfdin->attribs;

                attrs &= (_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH);

                (void) SetFileAttributes(
                    destination,
                    attrs
                );

			    return TRUE;
            }

		    case fdintNEXT_CABINET:	// file continued to next cabinet
			    printf(
				    "fdintNEXT_CABINET\n"
				    "   name of next cabinet where file continued = %s\n"
                    "   name of next disk where file continued    = %s\n"
				    "   cabinet path name                         = %s\n"
				    "\n",
				    pfdin->psz1,
				    pfdin->psz2,
				    pfdin->psz3
			    );
			    return 0;
	    }

	    return 0;
    }
  } // of namespace COM
} // of namespace bvr20983
//======================================END-OF-FILE=========================
