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
#include "cab/cabinetfci.h"
#include "exception/bvr20983exception.h"
#include "exception/cabinetexception.h"
#include "util/logstream.h"

using namespace std;

namespace bvr20983
{
  namespace cab
  {
    /**
     *
     */
    CabFCIParameter::CabFCIParameter(ULONG mediaSize,ULONG folderThreshold,int iDisk)
    { Init(mediaSize,folderThreshold,iDisk);
      
    } // of CabFCIParameter::CabFCIParameter()

    /**
     *
     */
    void CabFCIParameter::Init(ULONG mediaSize,ULONG folderThreshold,int iDisk)
    { ::memset(&m_ccab, 0, sizeof(CCAB));

      m_ccab.cb                = mediaSize;
      m_ccab.cbFolderThresh    = folderThreshold;
    
      /*
       * Don't reserve space for any extensions
       */
      m_ccab.cbReserveCFHeader = 0;
      m_ccab.cbReserveCFFolder = 0;
      m_ccab.cbReserveCFData   = 0;
    
      /*
       * We use this to create the CabinetFCI name
       */
      m_ccab.iCab = 1;
    
      /*
       * If you want to use disk names, use this to
       * count disks
       */
      m_ccab.iDisk = 0;
    
      /*
       * Choose your own number
       */
      m_ccab.setID = iDisk;
    
      /*
       * Only important if CABs are spanning multiple
       * disks, in which case you will want to use a
       * real disk name.
       *
       * Can be left as an empty string.
       */
      _tcscpy_s(m_ccab.szDisk,CB_MAX_DISK_NAME, "DISK");
    
      /* where to store the created CAB files */

      ::GetTempPath(CB_MAX_CAB_PATH,m_ccab.szCabPath);
    
      /* store name of first CAB file */
      StoreCabName(m_ccab.szCab, m_ccab.iCab);
    } // of CabFCIParameter::Init()

    /**
     *
     */
    void CabFCIParameter::StoreCabName(char *cabname, int iCab)
    {  _stprintf_s(cabname,CB_MAX_CAB_PATH, "hugo-%d.cab", iCab);
    } // of CabFCIParameter::StoreCabName()

    /*
     *
     */
    CabinetFCI::CabinetFCI() :
      m_totalCompressedSize(0),
      m_totalUncompressedSize(0),
      m_hfci(NULL)
    { Init(); }


    /*
     *
     */
    CabinetFCI::CabinetFCI(const CabFCIParameter& parameter) :
      m_totalCompressedSize(0),
      m_totalUncompressedSize(0),
      m_hfci(NULL),
      m_parameter(parameter)
    { Init(); }

    /*
     *
     */
    void CabinetFCI::Init()
    { m_hfci = ::FCICreate(&m_erf,
                           fci_fileplaced,
                           fci_memalloc,fci_memfree,
                           fci_open,fci_read,fci_write,fci_close,fci_seek,fci_delete,
                           fci_gettempfile,
                           &m_parameter,
                           this
                          );
    
      if( NULL==m_hfci )
        THROW_CABEXCEPTION((FCIERROR)m_erf.erfOper);
    } // of CabinetFCI::CabinetFCI()

    /**
     *
     */
    CabinetFCI::~CabinetFCI()
    { if( NULL!=m_hfci )
        ::FCIDestroy(m_hfci);
        
      m_hfci = NULL;
    } // of CabinetFCI::~CabinetFCI()

    /**
     *
     */
    void CabinetFCI::Flush(BOOL flushFolder)
    { if( NULL==m_hfci )
        return;
        
      if( ( flushFolder && !::FCIFlushFolder (m_hfci,      fci_getnextcabinet,fci_progress)) 
          ||
          (!flushFolder && !::FCIFlushCabinet(m_hfci,FALSE,fci_getnextcabinet,fci_progress))
        )
        THROW_CABEXCEPTION((FCIERROR)m_erf.erfOper);
    } // of void CabinetFCI::Flush()

    /**
     *
     */
    void CabinetFCI::AddFile(char* fileName,TCOMP typeCompress)
    { if( NULL==m_hfci )
        return;
        
      TCHAR strippedName[MAX_PATH];
      char* p = strrchr((LPTSTR)fileName, '\\');
    
      if( p==NULL )
        _tcscpy_s(strippedName,ARRAYSIZE(strippedName), fileName);
      else
        _tcscpy_s(strippedName,ARRAYSIZE(strippedName), p+1);

      if( !FCIAddFile(m_hfci,
                      fileName,           /* file to add */
                      strippedName,       /* file name in CabinetFCI file */
                      FALSE,              /* file is not executable */
                      fci_getnextcabinet,
                      fci_progress,
                      fci_getopeninfo,
                      typeCompress
                     )
        )
        THROW_CABEXCEPTION((FCIERROR)m_erf.erfOper);
    } // of CabinetFCI::AddFile(
      


    /**
     *
     */
    int CabinetFCI::GetPercentage(unsigned long a, unsigned long b)
    { while (a > 10000000)
      { a >>= 3;
        b >>= 3;
      } // of while
    
      if( b==0 )
        return 0;
    
      return (a*100)/b;
    } // of CabinetFCI::GetPercentage()

    /*
     * Memory allocation function
     */
    FNFCIALLOC(CabinetFCI::fci_memalloc)
    { return ::malloc(cb); }
    
    
    /*
     * Memory free function
     */
    FNFCIFREE(CabinetFCI::fci_memfree)
    { ::free(memory); }
    
    
    /*
     * File i/o functions
     */
     
    /**
     *
     */
    int  CabinetFCI::FCIOpen(char FAR *pszFile, int oflag, int pmode, int FAR *err)
    { int result = -1;
    
      if( _tsopen_s(&result, pszFile, oflag, _SH_DENYNO, pmode) )
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    UINT CabinetFCI::FCIRead  (int hf, void FAR *memory, UINT cb, int FAR *err)
    { unsigned int result = (unsigned int) _read(hf, memory, cb);
    
      if( result!=cb )
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    UINT CabinetFCI::FCIWrite (int hf, void FAR *memory, UINT cb, int FAR *err)
    { unsigned int result = (unsigned int) _write(hf, memory, cb);
    
      if( result!=cb )
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    int  CabinetFCI::FCIClose (int hf, int FAR *err)
    { int result = _close(hf);
    
      if (result != 0)
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    long CabinetFCI::FCISeek  (int hf, long dist, int seektype, int FAR *err)
    { long result = _lseek(hf, dist, seektype);
    
      if (result == -1)
        *err = errno;
    
      return result;
    }

    /**
     *
     */
    int  CabinetFCI::FCIDelete(char FAR *pszFile, int FAR *err)
    { int result = remove(pszFile);
    
      if( result!=0 )
        *err = errno;
    
      return result;
    }


    FNFCIOPEN(CabinetFCI::fci_open)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIOpen(pszFile,oflag,pmode,err) : -1; }
    
    /**
     *
     */
    FNFCIREAD(CabinetFCI::fci_read)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIRead(hf,memory,cb,err) : 0; }
    
    /**
     *
     */
    FNFCIWRITE(CabinetFCI::fci_write)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIWrite(hf,memory,cb,err) : 0; }
    
    /**
     *
     */
    FNFCICLOSE(CabinetFCI::fci_close)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIClose(hf,err) : -1; }
    
    /**
     *
     */
    FNFCISEEK(CabinetFCI::fci_seek)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCISeek(hf,dist,seektype,err) : -1; }
    
    /**
     *
     */
    FNFCIDELETE(CabinetFCI::fci_delete)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIDelete(pszFile,err) : -1; }
    
    
    /*
     * File placed function called when a file has been committed
     * to a CabinetFCI
     */
    int CabinetFCI::FCIFilePlaced(PCCAB pccab,char *pszFile,long  cbFile,BOOL  fContinuation)
    { LOGGER_INFO<<_T("   placed file '")<<pszFile<<_T("' (size ")<<cbFile<<_T(") on CabinetFCI '")<<pccab->szCab<<_T("'\n");
    
      if( fContinuation )
        LOGGER_INFO<<_T("      (Above file is a later segment of a continued file)")<<endl;
    
      return 0;
    }


    FNFCIFILEPLACED(CabinetFCI::fci_fileplaced)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIFilePlaced(pccab,pszFile,cbFile,fContinuation) : 0; }
    
    
    /*
     * Function to obtain temporary files
     */
    BOOL CabinetFCI::FCIGetTempFile(char *pszTempName,int cbTempName)
    { TCHAR tempPath[MAX_PATH];
      TCHAR tempFName[MAX_PATH];

      if( GetTempPath(ARRAYSIZE(tempPath),tempPath)==0 )
        return FALSE;

      if( GetTempFileName(tempPath,_T("cab"),0,tempFName)==0 )
        return FALSE;

      if( _tcslen(tempFName) >= (unsigned)cbTempName )
        return FALSE;

      _tcscpy_s(pszTempName,cbTempName,tempFName);

      ::DeleteFile(pszTempName);

      return TRUE;
    } // of CabinetFCI::FCIGetTempFile()

    /**
     *
     */
    FNFCIGETTEMPFILE(CabinetFCI::fci_gettempfile)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIGetTempFile(pszTempName,cbTempName) : FALSE; }
    
    
    /*
     * Progress function
     */
    long CabinetFCI::FCIProgress(UINT typeStatus,ULONG  cb1,ULONG  cb2)
    { if( typeStatus==statusFile )
      {
        m_totalCompressedSize   += cb1;
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
         * Adding a folder to a CabinetFCI
         *
         * cb1 = amount of folder copied to CabinetFCI so far
         * cb2 = total size of folder
         */
        int percentage = GetPercentage(cb1, cb2);
    
        LOGGER_INFO<<_T("Copying folder to CabinetFCI: ")<<percentage<<_T("%")<<endl;
      }
    
      return 0;
    } // of CabinetFCI::FCIProgress()
        
    /**
     *
     */
    FNFCISTATUS(CabinetFCI::fci_progress)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIProgress(typeStatus,cb1,cb2) : 0; }
    
    /**
     *
     */
    BOOL CabinetFCI::FCIGetNextCabinet(PCCAB  pccab,ULONG  cbPrevCab)
    { CabFCIParameter::StoreCabName(pccab->szCab, pccab->iCab);
    
      return TRUE;
    }

    /**
     *
     */
    FNFCIGETNEXTCABINET(CabinetFCI::fci_getnextcabinet)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIGetNextCabinet(pccab,cbPrevCab) : FALSE; }
    
    /**
     *
     */
    int CabinetFCI::FCIGetOpenInfo(char   *pszName,USHORT *pdate,USHORT *ptime,USHORT *pattribs,int FAR *err)    
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
       
      ::FileTimeToLocalFileTime(&finfo.ftLastWriteTime, &filetime);
      ::FileTimeToDosDateTime(&filetime,pdate,ptime);
    
      attrs = ::GetFileAttributes(pszName);
    
      if( attrs==0xFFFFFFFF )
        *pattribs = 0;
      else
      {
        /*
         * Mask out all other bits except these four, since other
         * bits are used by the CabinetFCI format to indicate a
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
    FNFCIGETOPENINFO(CabinetFCI::fci_getopeninfo)
    { return NULL!=pv ? ((CabinetFCI*)pv)->FCIGetOpenInfo(pszName,pdate,ptime,pattribs,err) : -1; }
  } // of namespace COM
} // of namespace bvr20983
//======================================END-OF-FILE=========================
