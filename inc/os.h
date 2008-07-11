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
#if !defined(OS_H)
#define OS_H

#ifndef _WIN32_WINNT		    // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#define _WIN32_IE 0x0600
#define WINVER 0x0501
#endif						

#include <assert.h>
#include <windows.h>
#ifndef RC_INVOKED
#include <tchar.h>
#include <process.h>
#include <ole2.h>
#include <olectl.h>
#include <DispEx.h>

#define GUID_SIZE 128

#define DELETE_POINTER(p)\
{\
  if (NULL != p)\
  {\
    delete p;\
    p = NULL;\
  }\
}

#define DELETE_GDIOBJECT(p)\
{\
  if( NULL!=p )\
  {\
    ::DeleteObject(p);\
    p = NULL;\
  }\
}

#define RELEASE_INTERFACE(p)     \
{                                \
  IUnknown* pTmp = (IUnknown*)p; \
  p = NULL;                      \
  if( NULL!=pTmp )               \
    pTmp->Release();             \
}

#define ADDREF_INTERFACE(p)      \
{ if( NULL!=p )                  \
    ((IUnknown*)p)->AddRef();    \
}


#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "bvr20983-prod.h"

// Convenient typedefs.
#ifndef PPVOID
typedef LPVOID* PPVOID;
#endif

namespace bvr20983
{
  typedef std::basic_string<TCHAR>  TString;
  
  typedef std::basic_string<BYTE>   BString;
  typedef std::vector<TString>      VTString;
  
  typedef std::map<BYTE,ULONG>      BY_UL_Map;
  typedef std::pair<BYTE, ULONG>    BY_UL_Pair;
} // of namespace bvr20983

using namespace std;
#endif

#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   ( (HIMETRIC_PER_INCH*(x) + ((ppli)>>1)) / (ppli) )
#define MAP_LOGHIM_TO_PIX(x,ppli)   ( ((ppli)*(x) + HIMETRIC_PER_INCH/2) / HIMETRIC_PER_INCH )

#define MAKE_BVR_ERROR(e)            MAKE_SCODE(SEVERITY_ERROR, FACILITY_ITF, 0x0200+(e))
#define MAKE_BVR_DISP(src,exp)       (((0x000f&src)<<8)+exp)

#define BVR_DISP_EXP(sc)             ((sc) & 0x00ff)
#define BVR_DISP_SOURCE(sc)          ((sc>>8) & 0x000f)

// Exceptions
#define IDS_EXCEPTION_MIN            1000
#define IDS_EXCEPTIONSRC_MIN         (IDS_EXCEPTION_MIN)
#define IDS_EXCEPTIONMSG_MIN         (IDS_EXCEPTION_MIN+256)

// help files
#define HELPFILE_0                   _T("bvr20983-0000.hlp")
#define HELPFILE_7                   _T("bvr20983-0007.hlp")

// Help context ID for exceptions
#define HID_EXCEPTION_MIN            0x1E100

#define BVRWNDCLS                    _T("BVRWNDCLS")

#define WM_BVR_SHELLNOTIFY           WM_USER+1

struct strless : public binary_function<LPCTSTR, LPCTSTR, bool>
{	
	bool operator()(const LPCTSTR& _Left, const LPCTSTR& _Right) const
	{	return _tcscmp(_Left,_Right)<0;	}
};

#define IDR_MANIFEST_APP            1
#define IDR_MANIFEST_DLL            2

#endif // OS_H
