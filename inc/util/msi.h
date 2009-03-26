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

namespace bvr20983
{
  namespace util
  {
    /**
     *
     */
    class MSI
    { 
      public:
        MSI();
        ~MSI();

        static bool IsProductInstalled(LPCTSTR productCode);

    }; // of MSI

    /**
     *
     */
    class MSIDB
    { 
      public:
        MSIDB(LPCTSTR szDatabasePath,LPCTSTR szPersist=MSIDBOPEN_READONLY);
        ~MSIDB();

        MSIHANDLE operator*() const
        { return m_hDatabase; }

      private:
        MSIHANDLE m_hDatabase;
    }; // of MSIDB

    class MSIRecord;

    /**
     *
     */
    class MSIQuery
    {
      public:
        static TCHAR productCode[];

        MSIQuery(MSIDB& msidb,LPCTSTR szQuery);
        ~MSIQuery();

        void Execute();
        void Fetch(MSIRecord& record);

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

      private:
        MSIHANDLE m_hRecord;

        void Close();
    }; //of MSIRecord

  } // of namespace util
} // of namespace bvr20983
#endif // BVRMSI_H
/*==========================END-OF-FILE===================================*/
