/*
 * $Id$
 * 
 * COMServer server-related utility C++ object.  This object encapsulates the 
 * server's internal control of global server object and lock counts.
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
#include "com/comserver.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "util/guid.h"

using namespace bvr20983;
using namespace bvr20983::COM;

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: DllCanUnloadNow

  Summary:  The standard exported function that the COM service library
            uses to determine if this server DLL can be unloaded.

  Args:     void.

  Returns:  HRESULT
              S_OK if this DLL server can be unloaded.
              S_FALSE if this DLL can not be unloaded.
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/
STDAPI DllCanUnloadNow()
{ HRESULT hr = COMServer::ShouldUnload() ? S_OK : S_FALSE;

  LOGGER_DEBUG<<_T("DllCanUnloadNow(): hr=")<<setHR<<CHResult(hr)<<endl;

  return hr;
}

/*F+F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F+++F
  Function: DllGetClassObject

  Summary:  The standard exported function that the COM service library
            uses to obtain an object class of the class factory for a
            specified component provided by this server DLL.

  Args:     REFCLSID rclsid,
              [in] The CLSID of the requested Component.
            REFIID riid,
              [in] GUID of the requested interface on the Class Factory.
            PPVOID ppv)
              [out] Address of the caller's pointer variable that will
              receive the requested interface pointer.

  Returns:  HRESULT
              E_FAIL if requested component isn't supported.
              E_OUTOFMEMORY if out of memory.
              Error code out of the QueryInterface.
F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F---F-F*/

extern map<CGUID,IUnknown*> gCOMClasses;

void InitDllGetClassObject();

STDAPI DllGetClassObject(REFCLSID rclsid,REFIID riid,PPVOID ppv)
{ HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

  InitDllGetClassObject();

  map<CGUID,IUnknown*>::const_iterator iter = gCOMClasses.find(CGUID(rclsid));

  if( iter!=gCOMClasses.end() )
  { IUnknown* pCOF = iter->second;
  
    hr = pCOF->QueryInterface(riid, ppv);
  } // of if

  return hr;
} // of DllGetClassObject()
/*==========================END-OF-FILE===================================*/
