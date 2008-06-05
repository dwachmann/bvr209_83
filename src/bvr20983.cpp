/*
 * $Id$
 * 
 * COM Component server providing several Smartcard-related COM Components.
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
#include "ibvr20983.h"
#include "com/coclassfactory.h"
#include "com/comserver.h"
#include "com/sc/cosmartcard.h"
#include "util/guid.h"

using namespace bvr20983;
using namespace bvr20983::COM;

map<CGUID,IUnknown*> gCOMClasses;

typedef pair<CGUID,IUnknown*> COMCLS_Pair;

/**
 *
 */
BOOL WINAPI DllMain(HINSTANCE hDllInst,DWORD fdwReason,LPVOID lpvReserved)
{ BOOL bResult = TRUE;

  switch( fdwReason )
  { case DLL_PROCESS_ATTACH:
      if( NULL==COMServer::CreateInstance(hDllInst) )
        bResult = FALSE;
      break;
    case DLL_PROCESS_DETACH:
      COMServer::DeleteInstance();
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    default:
      break;
  }

  return bResult;
} // of DllMain

/**
 *
 */
void InitDllGetClassObject()
{ gCOMClasses.insert( COMCLS_Pair(CGUID(CLSID_Smartcard),new COClassFactory(CLSID_Smartcard,&COSmartcard::Create)) );
} // of InitDllGetClassObject()
/*==========================END-OF-FILE===================================*/
