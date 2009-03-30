/*
 * $Id$
 * 
 * Copyright (C) 2009 Dorothea Wachmann
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
#ifndef BVRMSI_H
#define BVRMSI_H

#include <Msi.h>
#include <Msiquery.h>

#undef PID_SECURITY // defined as ( 0x80000002 ) in objidl.h, need to redefine here

// standard property definitions, from OLE2 documentation
#define PID_DICTIONARY  ( 0 )// integer count + array of entries
#define PID_CODEPAGE  ( 0x1 )// short integer
#define PID_TITLE         2  // string
#define PID_SUBJECT       3  // string
#define PID_AUTHOR        4  // string
#define PID_KEYWORDS      5  // string
#define PID_COMMENTS      6  // string
#define PID_TEMPLATE      7  // string
#define PID_LASTAUTHOR    8  // string
#define PID_REVNUMBER     9  // string
#define PID_EDITTIME     10  // datatime
#define PID_LASTPRINTED  11  // datetime
#define PID_CREATE_DTM   12  // datetime
#define PID_LASTSAVE_DTM 13  // datetime
#define PID_PAGECOUNT    14  // integer 
#define PID_WORDCOUNT    15  // integer 
#define PID_CHARCOUNT    16  // integer 
#define PID_THUMBNAIL    17  // clipboard format + metafile/bitmap (not supported)
#define PID_APPNAME      18  // string
#define PID_SECURITY     19  // integer

// PIDs given specific meanings for Installer
#define PID_MSIVERSION     PID_PAGECOUNT  // integer, Installer version number (major*100+minor)
#define PID_MSISOURCE      PID_WORDCOUNT  // integer, type of file image, short/long, media/tree
#define PID_MSIRESTRICT    PID_CHARCOUNT  // integer, transform restrictions


namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    class MSIProduct
    { 
      public:
        MSIProduct(LPCTSTR productCode);
        ~MSIProduct();

        MSIHANDLE operator*() const
        { return m_hProduct; }

        MSIHANDLE GetProduct() const
        { return m_hProduct; }

        LPCTSTR GetProductCode() const
        { return m_productCode.c_str(); }

        void GetProperty(LPCTSTR name,TString& value) const;
        void GetInfo(LPCTSTR name,TString& value) const;

        static bool IsInstalled(LPCTSTR productCode);
        static bool IsPackageInstalled(LPCTSTR productCode,LPCTSTR packageCode);

      private:
        MSIHANDLE m_hProduct;
        TString   m_productCode;
    }; // of MSIDB

    /**
     *
     */
    class MSIDB
    { 
      public:
        MSIDB(MSIProduct& msiProduct,LPCTSTR szPersist=MSIDBOPEN_READONLY);
        MSIDB(LPCTSTR szDatabasePath,LPCTSTR szPersist=MSIDBOPEN_READONLY);
        ~MSIDB();

        MSIHANDLE operator*() const
        { return m_hDatabase; }

        MSIHANDLE GetDatabaseHandle() const
        { return m_hDatabase; }

        bool GetProductCode(TString& productCode) const;
        bool GetPackageCode(TString& productCode) const;
        bool GetProperty(LPCTSTR propertyName,TString& propertyValue) const;

      private:
        MSIHANDLE m_hDatabase;
        TString   m_dbPath;
    }; // of MSIDB

    /**
     *
     */
    class MSIDBSummaryInfo
    { 
      public:
        MSIDBSummaryInfo(const MSIDB& msiDB);
        ~MSIDBSummaryInfo();

        MSIHANDLE operator*() const
        { return m_hSummaryInfo; }

        MSIHANDLE GetSummaryInfoHandle() const
        { return m_hSummaryInfo; }

        bool GetProperty(PROPID propID,TString& propertyValue) const;
        bool GetPackageCode(TString& packageCode) const;

      private:
        MSIHANDLE m_hSummaryInfo;
    }; // of MSIDBSummaryInfo


    class MSIRecord;

    /**
     *
     */
    class MSIQuery
    {
      public:
        static TCHAR productCode[];

        MSIQuery(const MSIDB& msidb,LPCTSTR szQuery);
        ~MSIQuery();

        void Execute();
        bool Fetch(MSIRecord& record);

      private:
        MSIHANDLE m_hView;
    }; //of MSIQuery

    /**
     *
     */
    class MSIRecord
    {
      public:
        MSIRecord();
        ~MSIRecord();

        MSIRecord& operator=(const MSIRecord& msiRecord);
        MSIRecord& operator=(MSIHANDLE hRec);

        void GetString(UINT iField,TString& value);
        void GetInteger(UINT iField,UINT& value);
        UINT GetFieldCount() const;

      private:
        MSIHANDLE m_hRecord;

        void Close();
    }; //of MSIRecord

  } // of namespace util
} // of namespace bvr20983
#endif // BVRMSI_H
/*==========================END-OF-FILE===================================*/
