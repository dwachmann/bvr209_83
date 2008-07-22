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
#ifndef CABINET_H
#define CABINET_H

#include "cab/fci.h"

namespace bvr20983
{
  namespace cab
  {
    class Cabinet
    {
      public:
        Cabinet(PCCAB cabParms);
        ~Cabinet();

        void Flush(BOOL flushFolder=FALSE);
        void AddFile(char* fileName,TCOMP typeCompress=tcompTYPE_MSZIP);
        
        static void Init(PCCAB pCCab);
    
      private:
        long        m_totalCompressedSize;    /* total compressed size so far */
        long        m_totalUncompressedSize;  /* total uncompressed size so far */
        TCHAR       m_strippedName[MAX_PATH];
        HFCI        m_hfci;
        CCAB        m_ccab;
        ERF         m_erf;
        
        int  GetPercentage(unsigned long a, unsigned long b);
        void StripPath(LPCTSTR filename);

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

        static void StoreCabName(char *cabname, int iCab);

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
    }; // of class Cabinet
  } // of namespace cab
} // of namespace bvr20983
#endif CABINET_H
/*==========================END-OF-FILE===================================*/
