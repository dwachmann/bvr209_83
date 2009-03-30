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
      { if( productCode[0]!=_T('{') )
        { m_productCode  = _T("{");
          m_productCode += productCode;
          m_productCode += _T("}");
        } // of if
        else
          m_productCode = productCode;

        THROW_LASTERROREXCEPTION( ::MsiOpenProduct(m_productCode.c_str(), &m_hProduct) );
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
    { bool result = false;

      if( NULL!=productCode )
      { TString intProductCode;

        if( productCode[0]!=_T('{') )
        { intProductCode  = _T("{");
          intProductCode += productCode;
          intProductCode += _T("}");
        } // of if
        else
          intProductCode = productCode;
        
        INSTALLSTATE installState = ::MsiQueryProductState(intProductCode.c_str());

        result = installState==INSTALLSTATE_DEFAULT;
      } // of if

      return result;
    } // of MSIProduct::IsInstalled()

    /**
     *
     */
    bool MSIProduct::IsPackageInstalled(LPCTSTR productCode,LPCTSTR packageCode)
    { bool result = false;

      if( NULL!=productCode && NULL!=packageCode )
      { TString intProductCode;
        TString intPackageCode;

        if( productCode[0]!=_T('{') )
        { intProductCode  = _T("{");
          intProductCode += productCode;
          intProductCode += _T("}");
        } // of if
        else
          intProductCode = productCode;

        if( packageCode[0]!=_T('{') )
        { intPackageCode  = _T("{");
          intPackageCode += packageCode;
          intPackageCode += _T("}");
        } // of if
        else
          intPackageCode = packageCode;

        if( INSTALLSTATE_DEFAULT==::MsiQueryProductState(intProductCode.c_str()) )
        { MSIProduct msiProduct(intProductCode.c_str());

          TString installedPackageCode;
          msiProduct.GetInfo(INSTALLPROPERTY_PACKAGECODE,installedPackageCode);
          
          result = _tcscmp(installedPackageCode.c_str(),intPackageCode.c_str())==0;
        } // of if
      } // of if

      return result;
    } // of MSIProduct::IsPackageInstalled()


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
    bool MSIDB::GetProductCode(TString& productCode) const
    { return GetProperty(_T("ProductCode"),productCode);
    } // of MSIDB::GetProductCode()

    /**
     *
     */
    bool MSIDB::GetProperty(LPCTSTR propertyName,TString& propertyValue) const
    { bool result = false;
    
      propertyValue.clear();

      if( NULL!=m_hDatabase ) 
      { TString query(_T("SELECT `Value` FROM `Property` WHERE `Property`='"));
      
        query += propertyName;
        query += _T("'");

        MSIQuery  msiQuery(*this,query.c_str());
        MSIRecord msiRecord;

        msiQuery.Execute();

        if( msiQuery.Fetch(msiRecord) )
        { msiRecord.GetString(1,propertyValue);

          result = true;
        } // of if
      } // of if

      return result;
    } // of MSIDB::GetProperty()


    /**
     *
     */
    bool MSIDB::GetPackageCode(TString& packageCode) const
    { bool result = false;
      
      packageCode.clear();

      if( NULL!=m_hDatabase ) 
      { MSIDBSummaryInfo msiDBSummaryInfo(*this);

        result = msiDBSummaryInfo.GetPackageCode(packageCode);
      } // of if

      return result;
    } // of MSIDB::GetPackageCode()


    /**
     *
     */ 
    MSIDBSummaryInfo::MSIDBSummaryInfo(const MSIDB& msiDB) :
      m_hSummaryInfo(NULL)
    { 
      THROW_LASTERROREXCEPTION( ::MsiGetSummaryInformation(msiDB.GetDatabaseHandle(),NULL,0, &m_hSummaryInfo) );
    }

    /**
     *
     */
    MSIDBSummaryInfo::~MSIDBSummaryInfo()
    { if( NULL!=m_hSummaryInfo ) 
        ::MsiCloseHandle(m_hSummaryInfo);

      m_hSummaryInfo = NULL;
    }

    /**
     *
     */
    bool MSIDBSummaryInfo::GetProperty(PROPID propID,TString& propertyValue) const
    { bool result = false;
    
      propertyValue.clear();

      if( NULL!=m_hSummaryInfo ) 
      { TCHAR buffer[1024];
        DWORD bufferSize = ARRAYSIZE(buffer);
        UINT  dataType = NULL;

        //propID = PID_REVNUMBER;

        UINT  msiResult = ::MsiSummaryInfoGetProperty(m_hSummaryInfo,propID,&dataType,NULL,NULL,buffer,&bufferSize);

        if( ERROR_UNKNOWN_PROPERTY==msiResult )
          result = false;
        else if( ERROR_SUCCESS==msiResult )
        { propertyValue = buffer;
          result        = true;
        } // of else if
        else
          THROW_LASTERROREXCEPTION3( msiResult );
      } // of if

      return result;
    } // of MSIDBSummaryInfo::GetProperty()

    /**
     *
     */
    bool MSIDBSummaryInfo::GetPackageCode(TString& packageCode) const
    { return GetProperty(PID_REVNUMBER,packageCode); }

    /**
     *
     */
    MSIQuery::MSIQuery(const MSIDB& msidb,LPCTSTR szQuery) :
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
