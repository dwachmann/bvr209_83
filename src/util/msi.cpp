/*
 * $Id: $
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
    MSI::MSI()
    { }

    /**
     *
     */
    MSI::~MSI()
    { }

    /**
     *
     */
    bool MSI::IsProductInstalled(LPCTSTR productCode)
    { INSTALLSTATE installState = ::MsiQueryProductState(productCode);

      return installState==INSTALLSTATE_DEFAULT;
    } // of MSI::IsProductInstalled()

    /**
     *
     */
    MSIDB::MSIDB(LPCTSTR szDatabasePath,LPCTSTR szPersist) :
      m_hDatabase(NULL)
    { THROW_LASTERROREXCEPTION( ::MsiOpenDatabase(szDatabasePath,szPersist, &m_hDatabase) );
    }

    /**
     *
     */
    MSIDB::~MSIDB()
    { if( NULL!=m_hDatabase) 
        ::MsiCloseHandle(m_hDatabase);

      m_hDatabase = NULL;
    }

    /**
     *
     */
    MSIQuery::MSIQuery(MSIDB& msidb,LPCTSTR szQuery) :
      m_hView(NULL)
    { THROW_LASTERROREXCEPTION( ::MsiDatabaseOpenView(*msidb,szQuery, &m_hView) );
    }

    /**
     *
     */
    MSIQuery::~MSIQuery()
    { if( NULL!=m_hView) 
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
    void MSIQuery::Fetch(MSIRecord& record)
    { if( NULL!=m_hView )
      { MSIHANDLE hRec=NULL;

        ::MsiViewFetch(m_hView,&hRec);

        record = hRec;
      } // of if
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

  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
