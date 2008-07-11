/*
 * $Id$
 * 
 * Implementation file for a bvr20983 windows custom controls.
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
#include <msstkppg.h>
#include "bvr20983cc-res.h"
#include "bvr20983cc-dispid.h"
#include "bvr20983cc.h"
#include "win/wndclass.h"
#include "win/ledwnd.h"
#include "util/guid.h"
#include "util/comptrbase.h"
#include "com/comserver.h"
#include "com/ocx/coled.h"
#include "com/coclassfactory.h"
#include "com/copropertypage.h"
#include "commctrl.h"

using namespace bvr20983;
using namespace bvr20983::win;
using namespace bvr20983::COM;

typedef pair<CGUID,IUnknown*> COMCLS_Pair;

map<CGUID,IUnknown*> gCOMClasses;

/**
 *
 */
STDAPI InitBVRLibrary()
{ HRESULT hr = NOERROR;

  try
  { GCOMPtrBase::Init();

    LEDWnd::RegisterClass(COMServer::GetInstanceHandle(),
                          IDI_APP_ICON,
                          (HBRUSH)::GetStockObject(NULL_BRUSH),
                          CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS
                         );

    COControlClass coClass;

    coClass.m_oleMiscFlags     = OLEMISC_SETCLIENTSITEFIRST|OLEMISC_ACTIVATEWHENVISIBLE|OLEMISC_RECOMPOSEONRESIZE|OLEMISC_CANTLINKINSIDE|OLEMISC_INSIDEOUT;
    coClass.m_activationPolicy = POINTERINACTIVE_ACTIVATEONENTRY;
    coClass.m_isOpaque         = true;
    coClass.m_isWindowless     = true;
    coClass.m_toolboxId        = IDB_TOOLBOX_BITMAP;
    coClass.m_propPageGuid.push_back(CLSID_LEDPropertyPage);
    coClass.m_propPageGuid.push_back(CLSID_StockColorPage);

    COControl::RegisterControlClass(_T("LEDCOControl"),coClass);
  }
  catch(...)
  { hr = E_FAIL; }

  return hr;
}

/**
 *
 */
STDAPI InitBVRCustomControls()
{ HRESULT hr = NOERROR;

  try
  { 
  }
  catch(...)
  { hr = E_FAIL; }

  return hr; 
}

/**
 *
 */
void InitDllGetClassObject()
{ 
  gCOMClasses.insert( COMCLS_Pair(CGUID(CLSID_LED),new COClassFactory(CLSID_LED,&COLED::Create)) );
  gCOMClasses.insert( COMCLS_Pair(CGUID(CLSID_LEDPropertyPage),new COClassFactory(CLSID_LEDPropertyPage,&COPropertyPage::Create)) );
} // of InitDllGetClassObject()


/**
 *
 */
BOOL WINAPI DllMain(HINSTANCE hDllInst,DWORD fdwReason,LPVOID lpvReserved)
{ BOOL bResult = TRUE;

  switch( fdwReason )
  { case DLL_PROCESS_ATTACH:
      OutputDebugFmt(_T("DllMain(DLL_PROCESS_ATTACH)\n"));

      InitCommonControls();
      if( NULL==COMServer::CreateInstance(hDllInst) )
        bResult = FALSE;

      { HRESULT hr = InitBVRLibrary();

        if( FAILED(hr) )
          bResult = FALSE;
      }
      break;
    case DLL_PROCESS_DETACH:
      OutputDebugFmt(_T("DllMain(DLL_PROCESS_DETACH)\n"));
      WndClass::UnregisterWindowClasses();
      COMServer::DeleteInstance();
      break;
    case DLL_THREAD_ATTACH:
      OutputDebugFmt(_T("DllMain(DLL_THREAD_ATTACH)\n"));
      InitCommonControls();
      break;
    case DLL_THREAD_DETACH:
      OutputDebugFmt(_T("DllMain(DLL_THREAD_DETACH)\n"));
      break;
    default:
      OutputDebugFmt(_T("DllMain(%ld)\n"),fdwReason);
      break;
  }

  return bResult;
} // of DllMain
/*==========================END-OF-FILE===================================*/
