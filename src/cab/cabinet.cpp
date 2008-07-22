/*
 * $Id$
 * 
 * Wrapper class to access MS cabinet files.
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
#include "cab/cabinet.h"
#include "exception/bvr20983exception.h"
#include "exception/cabinetexception.h"
#include "util/logstream.h"

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


namespace bvr20983
{
  namespace cab
  {
    /*
     *
     */
    Cabinet::Cabinet(PCCAB cabParms) :
      m_totalCompressedSize(0),
      m_totalUncompressedSize(0),
      m_hfci(NULL)
    { Init(&m_ccab);
      
      if( NULL!=cabParms )
        m_ccab = *cabParms;
        
      m_hfci = ::FCICreate(&m_erf,
                           fci_fileplaced,
                           fci_memalloc,fci_memfree,
                           fci_open,fci_read,fci_write,fci_close,fci_seek,fci_delete,
                           fci_gettempfile,
                           &m_ccab,
                           this
                          );
    
      if( NULL==m_hfci )
        THROW_CABEXCEPTION((FCIERROR)m_erf.erfOper);
    } // of Cabinet::Cabinet()
    
    /**
     *
     */
    Cabinet::~Cabinet()
    { if( NULL!=m_hfci )
        ::FCIDestroy(m_hfci);
        
      m_hfci = NULL;
    } // of Cabinet::~Cabinet()

    /**
     *
     */
    void Cabinet::Flush(BOOL flushFolder)
    { if( NULL==m_hfci )
        return;
        
      if( ( flushFolder && !::FCIFlushFolder (m_hfci,      fci_getnextcabinet,fci_progress)) 
          ||
          (!flushFolder && !::FCIFlushCabinet(m_hfci,FALSE,fci_getnextcabinet,fci_progress))
        )
        THROW_CABEXCEPTION((FCIERROR)m_erf.erfOper);
    } // of void Cabinet::Flush()

    /**
     *
     */
    void Cabinet::AddFile(char* fileName,TCOMP typeCompress)
    { if( NULL==m_hfci )
        return;
        
      StripPath(fileName);
  
      if( !FCIAddFile(m_hfci,
                      fileName,  /* file to add */
                      m_strippedName, /* file name in cabinet file */
                      FALSE,         /* file is not executable */
                      fci_getnextcabinet,
                      fci_progress,
                      fci_getopeninfo,
                      typeCompress
                     )
        )
        THROW_CABEXCEPTION((FCIERROR)m_erf.erfOper);
    } // of Cabinet::AddFile(
      

    /**
     *
     */
    void Cabinet::StoreCabName(char *cabname, int iCab)
    {  _stprintf_s(cabname,CB_MAX_CAB_PATH, "hugo-%d.cab", iCab);
    } // of Cabinet::StoreCabName()

    /**
     *
     */
    void Cabinet::Init(PCCAB pCCab)
    { ::memset(pCCab, 0, sizeof(CCAB));
    
      pCCab->cb                = MEDIA_SIZE;
      pCCab->cbFolderThresh    = FOLDER_THRESHOLD;
    
      /*
       * Don't reserve space for any extensions
       */
      pCCab->cbReserveCFHeader = 0;
      pCCab->cbReserveCFFolder = 0;
      pCCab->cbReserveCFData   = 0;
    
      /*
       * We use this to create the cabinet name
       */
      pCCab->iCab = 1;
    
      /*
       * If you want to use disk names, use this to
       * count disks
       */
      pCCab->iDisk = 0;
    
      /*
       * Choose your own number
       */
      pCCab->setID = 12345;
    
      /*
       * Only important if CABs are spanning multiple
       * disks, in which case you will want to use a
       * real disk name.
       *
       * Can be left as an empty string.
       */
      _tcscpy_s(pCCab->szDisk,CB_MAX_DISK_NAME, "MyDisk");
    
      /* where to store the created CAB files */
      _tcscpy_s(pCCab->szCabPath,CB_MAX_CAB_PATH, "C:\\temp\\");
    
      /* store name of first CAB file */
      StoreCabName(pCCab->szCab, pCCab->iCab);
    } // of Cabinet::Init()

    /**
     *
     */
    void Cabinet::StripPath(LPCTSTR filename)
    { char* p = strrchr((LPTSTR)filename, '\\');
    
      if( p==NULL )
        _tcscpy_s(m_strippedName,ARRAYSIZE(m_strippedName), filename);
      else
        _tcscpy_s(m_strippedName,ARRAYSIZE(m_strippedName), p+1);
    } // of Cabinet::StripPath()

    /**
     *
     */
    int Cabinet::GetPercentage(unsigned long a, unsigned long b)
    { while (a > 10000000)
      {
        a >>= 3;
        b >>= 3;
      }
    
      if (b == 0)
        return 0;
    
      return ((a*100)/b);
    } // of Cabinet::GetPercentage()

    /*
     * Memory allocation function
     */
    FNFCIALLOC(Cabinet::fci_memalloc)
    { return ::malloc(cb); }
    
    
    /*
     * Memory free function
     */
    FNFCIFREE(Cabinet::fci_memfree)
    { ::free(memory); }
    
    
    /*
     * File i/o functions
     */
     
    /**
     *
     */
    int  Cabinet::FCIOpen(char FAR *pszFile, int oflag, int pmode, int FAR *err)
    { int result = -1;
    
      if( _tsopen_s(&result, pszFile, oflag, _SH_DENYNO, pmode) )
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    UINT Cabinet::FCIRead  (int hf, void FAR *memory, UINT cb, int FAR *err)
    { unsigned int result = (unsigned int) _read(hf, memory, cb);
    
      if( result!=cb )
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    UINT Cabinet::FCIWrite (int hf, void FAR *memory, UINT cb, int FAR *err)
    { unsigned int result = (unsigned int) _write(hf, memory, cb);
    
      if( result!=cb )
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    int  Cabinet::FCIClose (int hf, int FAR *err)
    { int result = _close(hf);
    
      if (result != 0)
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    long Cabinet::FCISeek  (int hf, long dist, int seektype, int FAR *err)
    { long result = _lseek(hf, dist, seektype);
    
      if (result == -1)
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    int  Cabinet::FCIDelete(char FAR *pszFile, int FAR *err)
    { int result = remove(pszFile);
    
      if( result!=0 )
        *err = errno;
    
      return result;
    }


    FNFCIOPEN(Cabinet::fci_open)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIOpen(pszFile,oflag,pmode,err) : -1; }
    
    /**
     *
     */
    FNFCIREAD(Cabinet::fci_read)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIRead(hf,memory,cb,err) : 0; }
    
    /**
     *
     */
    FNFCIWRITE(Cabinet::fci_write)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIWrite(hf,memory,cb,err) : 0; }
    
    /**
     *
     */
    FNFCICLOSE(Cabinet::fci_close)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIClose(hf,err) : -1; }
    
    /**
     *
     */
    FNFCISEEK(Cabinet::fci_seek)
    { return NULL!=pv ? ((Cabinet*)pv)->FCISeek(hf,dist,seektype,err) : -1; }
    
    /**
     *
     */
    FNFCIDELETE(Cabinet::fci_delete)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIDelete(pszFile,err) : -1; }
    
    
    /*
     * File placed function called when a file has been committed
     * to a cabinet
     */
    int Cabinet::FCIFilePlaced(PCCAB pccab,char *pszFile,long  cbFile,BOOL  fContinuation)
    { LOGGER_INFO<<_T("   placed file '")<<pszFile<<_T("' (size ")<<cbFile<<_T(") on cabinet '")<<pccab->szCab<<_T("'\n");
    
      if( fContinuation )
        LOGGER_INFO<<_T("      (Above file is a later segment of a continued file)")<<endl;
    
      return 0;
    }


    FNFCIFILEPLACED(Cabinet::fci_fileplaced)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIFilePlaced(pccab,pszFile,cbFile,fContinuation) : 0; }
    
    
    /*
     * Function to obtain temporary files
     */
    BOOL Cabinet::FCIGetTempFile(char *pszTempName,int   cbTempName)
    { char* psz = _tempnam("","xx");            // Get a name
    
      if( (psz!=NULL) && (strlen(psz) < (unsigned)cbTempName) ) 
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

    FNFCIGETTEMPFILE(Cabinet::fci_gettempfile)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIGetTempFile(pszTempName,cbTempName) : FALSE; }
    
    
    /*
     * Progress function
     */
    long Cabinet::FCIProgress(UINT typeStatus,ULONG  cb1,ULONG  cb2)
    { if( typeStatus==statusFile )
      {
        m_totalCompressedSize += cb1;
        m_totalUncompressedSize += cb2;
    
        /*
         * Compressing a block into a folder
         *
         * cb2 = uncompressed size of block
         */
        LOGGER_INFO<<_T("Compressing: ")<<m_totalUncompressedSize<<_T(" -> ")<<m_totalCompressedSize<<endl;
      }
      else if( typeStatus==statusFolder )
      {
        /*
         * Adding a folder to a cabinet
         *
         * cb1 = amount of folder copied to cabinet so far
         * cb2 = total size of folder
         */
        int percentage = GetPercentage(cb1, cb2);
    
        LOGGER_INFO<<_T("Copying folder to cabinet: ")<<percentage<<_T("%")<<endl;
      }
    
      return 0;
    } // of Cabinet::FCIProgress()
        
    /**
     *
     */
    FNFCISTATUS(Cabinet::fci_progress)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIProgress(typeStatus,cb1,cb2) : 0; }
    
    /**
     *
     */
    BOOL Cabinet::FCIGetNextCabinet(PCCAB  pccab,ULONG  cbPrevCab)
    { StoreCabName(pccab->szCab, pccab->iCab);
    
      return TRUE;
    }

    /**
     *
     */
    FNFCIGETNEXTCABINET(Cabinet::fci_getnextcabinet)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIGetNextCabinet(pccab,cbPrevCab) : FALSE; }
    
    /**
     *
     */
    int Cabinet::FCIGetOpenInfo(char   *pszName,USHORT *pdate,USHORT *ptime,USHORT *pattribs,int FAR *err)    
    { BY_HANDLE_FILE_INFORMATION finfo;
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
        return -1;
    
      if( !::GetFileInformationByHandle(handle, &finfo) )
      { ::CloseHandle(handle);
        
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
    FNFCIGETOPENINFO(Cabinet::fci_getopeninfo)
    { return NULL!=pv ? ((Cabinet*)pv)->FCIGetOpenInfo(pszName,pdate,ptime,pattribs,err) : -1; }
  } // of namespace COM
} // of namespace bvr20983
//======================================END-OF-FILE=========================
