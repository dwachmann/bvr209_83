/*
 * $Id$
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
#ifndef CABINETFCI_H
#define CABINETFCI_H

#include "cab/fci.h"
#include "util/dirinfo.h"


namespace bvr20983
{
  namespace cab
  {
    class CabinetFCI;

    /**
     *
     */
    struct CabinetFCIDirInfo : public util::DirIterator
    {
      CabinetFCIDirInfo(LPCWSTR prefix);
      CabinetFCIDirInfo(LPCSTR prefix=NULL);
      
      bool Next(util::DirectoryInfo& dirInfo,const WIN32_FIND_DATA& findData,int depth,void* p);
      
    private:
      char m_prefix[MAX_PATH];  
    };

    /**
     *
     */
    struct CabFCIParameter
    {
      static const ULONG CDROM_SIZE       = 650*1024*1024;
      static const ULONG FLOPPY_SIZE      = 1440*1024;
      static const ULONG FOLDER_THRESHOLD = 650*1024*1024;
      static const int   DISKID           = 42;

      CabFCIParameter(LPCWSTR cabName,ULONG mediaSize=CDROM_SIZE,int iDisk=DISKID,ULONG folderThreshold=FOLDER_THRESHOLD);
      CabFCIParameter(LPCSTR  cabName=NULL,ULONG mediaSize=CDROM_SIZE,int iDisk=DISKID,ULONG folderThreshold=FOLDER_THRESHOLD);

      PCCAB operator&() 
      { return &m_ccab; }

      PCCAB operator->()
      { return &m_ccab; }

    private:
      CCAB m_ccab;
      char m_cabPattern[MAX_PATH];

      friend class CabinetFCI;

      void Init(LPCSTR cabName,ULONG mediaSize,ULONG folderThreshold,int iDisk);
      void StoreCabName(LPSTR cabname, int iCab);
    }; // of struct CabFCIParameter

    class CabinetFCI;

    /**
     *
     */
    struct CabinetFCIAddFileCB
    {
      virtual bool DirectoryStarted(util::DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,int depth)=0;
      virtual bool AddFile(LPCTSTR prefix,LPCTSTR fileName,LPTSTR addedFileName,int addedFileNameMaxLen,int seqNo,util::DirectoryInfo* pDirInfo)=0;
    };

    /**
     *
     */
    class CabinetFCI
    {
    public:
      CabinetFCI(CabinetFCIAddFileCB* pAddFileCB=NULL);
      CabinetFCI(const CabFCIParameter& parameter,CabinetFCIAddFileCB* pAddFileCB=NULL);
      ~CabinetFCI();

      void Flush(bool flushFolder=false);

      void AddFileA(LPCSTR fileName,LPCSTR prefix=NULL,LPCSTR addFileName=NULL,TCOMP typeCompress=tcompTYPE_MSZIP,util::DirectoryInfo* pDirInfo=NULL);
      void AddFileW(LPCWSTR fileName,LPCWSTR prefix=NULL,LPCWSTR addFileName=NULL,TCOMP typeCompress=tcompTYPE_MSZIP,util::DirectoryInfo* pDirInfo=NULL);
      void AddFile(LPCTSTR fileName,LPCTSTR prefix=NULL,LPCTSTR addFileName=NULL,TCOMP typeCompress=tcompTYPE_MSZIP,util::DirectoryInfo* pDirInfo=NULL);

      void SetAddFileCallback(CabinetFCIAddFileCB* pAddFileCB)
      { this->m_pAddFileCB = pAddFileCB; }
      
      int GetSequenceNo() const
      { return m_seqNo; }

    private:
      long                 m_totalCompressedSize;    /* total compressed size so far */
      long                 m_totalUncompressedSize;  /* total uncompressed size so far */
      HFCI                 m_hfci;
      ERF                  m_erf;
      CabFCIParameter      m_parameter;
      CabinetFCIAddFileCB* m_pAddFileCB;
      int                  m_seqNo;

      int  GetPercentage(unsigned long a, unsigned long b);
      void Init();

      bool DirectoryStarted(util::DirectoryInfo& dirInfo,const WIN32_FIND_DATAW& findData,int depth);

      int  FCIOpen(char FAR *pszFile, int oflag, int pmode, int FAR *err);
      UINT FCIRead(int hf, void FAR *memory, UINT cb, int FAR *err);
      UINT FCIWrite(int hf, void FAR *memory, UINT cb, int FAR *err);
      int  FCIClose(int hf, int FAR *err);
      long FCISeek(int hf, long dist, int seektype, int FAR *err);
      int  FCIDelete(char FAR *pszFile, int FAR *err);
      int  FCIFilePlaced(PCCAB pccab,char *pszFile,long  cbFile,BOOL  fContinuation);
      BOOL FCIGetTempFile(char *pszTempName,int   cbTempName);
      long FCIProgress(UINT typeStatus,ULONG  cb1,ULONG  cb2);
      BOOL FCIGetNextCabinet(PCCAB pccab,ULONG  cbPrevCab);
      int  FCIGetOpenInfo(char* pszName,USHORT *pdate,USHORT *ptime,USHORT *pattribs,int FAR *err);

      friend struct CabinetFCIDirInfo;

      static FNFCIALLOC(fci_memalloc);
      static FNFCIFREE(fci_memfree);
      static FNFCIOPEN(fci_open);
      static FNFCIREAD(fci_read);
      static FNFCIWRITE(fci_write);
      static FNFCICLOSE(fci_close);
      static FNFCISEEK(fci_seek);
      static FNFCIDELETE(fci_delete);
      static FNFCIFILEPLACED(fci_fileplaced);
      static FNFCIGETTEMPFILE(fci_gettempfile);
      static FNFCISTATUS(fci_progress);
      static FNFCIGETNEXTCABINET(fci_getnextcabinet);
      static FNFCIGETOPENINFO(fci_getopeninfo);
    }; // of class CabinetFCI
  } // of namespace cab
} // of namespace bvr20983
#endif CABINETFCI_H
/*==========================END-OF-FILE===================================*/
