/*
 * $Id$
 * 
 * A class for download files using the windows BITS service.
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
#include "os.h"
#include "util/msi.h"
#include "util/logstream.h"
#include "exception/lasterrorexception.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace util
  {

    TCHAR MSIQuery::productCode[] = _T("SELECT `Value` FROM `Property` WHERE `Property`='ProductCode'");

    /**
     *
     */
    MSIProduct::MSIProduct(LPCTSTR productCode) :
      m_hProduct(NULL)
    { if( NULL!=productCode )
      { m_productCode = productCode;

        THROW_LASTERROREXCEPTION( ::MsiOpenProduct(productCode, &m_hProduct) );
      }
    } // of MSIProduct::MSIProduct()


    /**
     *
     */
    MSIProduct::~MSIProduct()
    { if( NULL!=m_hProduct ) 
        ::MsiCloseHandle(m_hProduct);

      m_hProduct = NULL;
    }

    /**
     *
     */
    bool MSIProduct::IsInstalled(LPCTSTR productCode)
    { INSTALLSTATE installState = ::MsiQueryProductState(productCode);

      return installState==INSTALLSTATE_DEFAULT;
    } // of MSIProduct::IsInstalled()

    /**
     *
     */
    void MSIProduct::GetProperty(LPCTSTR name,TString& value) const
    { TCHAR buffer[1024];
      DWORD bufferSize = ARRAYSIZE(buffer);

      value.clear();

      if( NULL!=m_hProduct && ERROR_SUCCESS==::MsiGetProductProperty(m_hProduct,name,buffer,&bufferSize) )
        value = buffer;
    } // of MSIProduct::GetProperty()

    /**
     *
     */
    void MSIProduct::GetInfo(LPCTSTR name,TString& value) const
    { TCHAR buffer[1024];
      DWORD bufferSize = ARRAYSIZE(buffer);

      value.clear();

      if( NULL!=m_hProduct && ERROR_SUCCESS==::MsiGetProductInfo(m_productCode.c_str(),name,buffer,&bufferSize) )
        value = buffer;
    } // of MSIProduct::GetInfo()

    /**
     *
     */ 
    MSIDB::MSIDB(MSIProduct& msiProduct,LPCTSTR szPersist) :
      m_hDatabase(NULL)
    { TString localPackage;

      msiProduct.GetInfo(INSTALLPROPERTY_LOCALPACKAGE,localPackage);

      if( !localPackage.empty() )
      { m_dbPath = localPackage;

         THROW_LASTERROREXCEPTION( ::MsiOpenDatabase(localPackage.c_str(),szPersist, &m_hDatabase) );
      } // of if
    }

    /**
     *
     */
    MSIDB::MSIDB(LPCTSTR szDatabasePath,LPCTSTR szPersist) :
      m_hDatabase(NULL)
    { if( NULL!=szDatabasePath )
      { m_dbPath = szDatabasePath;

         THROW_LASTERROREXCEPTION( ::MsiOpenDatabase(szDatabasePath,szPersist, &m_hDatabase) );
      } // of if
    } // of MSIDB::MSIDB()


    /**
     *
     */
    MSIDB::~MSIDB()
    { if( NULL!=m_hDatabase ) 
        ::MsiCloseHandle(m_hDatabase);

      m_hDatabase = NULL;
    }

    /**
     *
     */
    MSIQuery::MSIQuery(MSIDB& msidb,LPCTSTR szQuery) :
      m_hView(NULL)
    { 
      THROW_LASTERROREXCEPTION( ::MsiDatabaseOpenView(msidb.GetDatabaseHandle(),szQuery, &m_hView) );
    }

    /**
     *
     */
    MSIQuery::~MSIQuery()
    { if( NULL!=m_hView ) 
        ::MsiCloseHandle(m_hView);

      m_hView = NULL;
    }

    /**
     *
     */
    void MSIQuery::Execute()
    { if( NULL!=m_hView )
        THROW_LASTERROREXCEPTION( ::MsiViewExecute(m_hView,NULL) );
    } // of MSIQuery::Execute()

    /**
     *
     */
    bool MSIQuery::Fetch(MSIRecord& record)
    { bool moreData = false;

      if( NULL!=m_hView )
      { MSIHANDLE hRec=NULL;

        UINT result = ::MsiViewFetch(m_hView,&hRec);

        if( result==ERROR_SUCCESS )
          moreData = true;
        else if( result==ERROR_NO_MORE_ITEMS )
          moreData = false;
        else
        { THROW_LASTERROREXCEPTION3(result); }

        record = hRec;
      } // of if

      return moreData;
    } // of MSIQuery::Execute()


    /**
     *
     */
    MSIRecord::MSIRecord() :
      m_hRecord(NULL)
    { 
    } // of MSIRecord::MSIRecord()

    /**
     *
     */
    MSIRecord::~MSIRecord()
    { Close();
    } // of MSIRecord::~MSIRecord()

    /**
     *
     */
    void MSIRecord::Close()
    { if( NULL!=m_hRecord ) 
        ::MsiCloseHandle(m_hRecord);

      m_hRecord = NULL;
    } // of MSIRecord::Close()

    /**
     *
     */
    MSIRecord& MSIRecord::operator=(const MSIRecord& msiRecord)
    { if( m_hRecord!=msiRecord.m_hRecord )
        Close();

      m_hRecord = msiRecord.m_hRecord;

      return *this;
    } // of MSIRecord::operator=()

    /**
     *
     */
    MSIRecord& MSIRecord::operator=(MSIHANDLE hRec)
    { if( m_hRecord!=hRec )
        Close();

      m_hRecord = hRec;

      return *this;
    } // of MSIRecord::operator=()

    /**
     *
     */
    void MSIRecord::GetString(UINT iField,TString& value)
    { if( NULL!=m_hRecord ) 
      { TCHAR valueBuf[1024];
        DWORD cchValueBuf = ARRAYSIZE(valueBuf)-1;

        THROW_LASTERROREXCEPTION( ::MsiRecordGetString(m_hRecord,iField,valueBuf,&cchValueBuf) );

        value = valueBuf;
      } // of if
    } // of MSIRecord::GetString()

    /**
     *
     */
    void MSIRecord::GetInteger(UINT iField,UINT& value)
    { if( NULL!=m_hRecord ) 
      { value = ::MsiRecordGetInteger(m_hRecord,iField);
      } // of if
    } // of MSIRecord::GetInteger()

    /**
     *
     */
    UINT MSIRecord::GetFieldCount() const
    { UINT result = 0;

      if(  NULL!=m_hRecord ) 
      { result = ::MsiRecordGetFieldCount(m_hRecord);
      } // of if

      return result;
    } // of MSIRecord::GetFieldCount()

  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
