/*
 * $Id$
 * 
 * Windows DC class.
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
#include "win/dc.h"
#include "win/wnd.h"
#include "exception/windowsexception.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace win
  {
#pragma region Construction & Deconstruction
    /**
     *
     */
    DC::DC(HWND hWnd)
    { m_hWnd = hWnd;
      m_hDC  = ::GetDC(m_hWnd); 
      m_rr   = NULL;
    }

    /**
     *
     */
    DC::~DC()
    { ::ReleaseDC(m_hWnd,m_hDC); }


    /**
     *
     */
    WndDC::WndDC(Wnd& wnd) : m_wnd(wnd)
    { m_hDC = m_wnd.GetDC(); 
    }

    /**
     *
     */
    WndDC::~WndDC()
    { m_wnd.ReleaseDC(m_hDC); }

    /**
     *
     */
    PaintDC::PaintDC(HWND hWnd)
    { m_hWnd = hWnd;
      m_hDC  = ::BeginPaint(m_hWnd,&m_PS); 
    }

    /**
     *
     */
    PaintDC::~PaintDC()
    { ::EndPaint(m_hWnd,&m_PS); }

    /**
     *
     */
    MemoryDC::MemoryDC(const DCBase& dc,int w,int h,bool deleteBitmap,bool monocrome) 
      : m_w(w),m_h(h),m_deleteBitmap(deleteBitmap),m_monocrome(monocrome)
    { m_hDC     = ::CreateCompatibleDC(dc);
      m_hBitmap = monocrome ? ::CreateBitmap(w,h,1,1,NULL) : ::CreateCompatibleBitmap(dc,w,h);

      ::SelectObject(m_hDC,m_hBitmap);
    }

    /**
     *
     */
    MemoryDC::MemoryDC(const MemoryDC& memDC,bool deleteBitmap,bool monocrome) 
    { m_hDC          = ::CreateCompatibleDC(memDC.m_hDC);
      m_w            = memDC.m_w;
      m_h            = memDC.m_h;
      m_deleteBitmap = deleteBitmap;
      m_monocrome    = monocrome;
      m_hBitmap      = monocrome ? ::CreateBitmap(m_w,m_h,1,1,NULL) : ::CreateCompatibleBitmap(memDC.m_hDC,m_w,m_h);

      ::SelectObject(m_hDC,m_hBitmap);

      ::BitBlt(m_hDC,0,0,m_w,m_h,memDC,0,0,SRCCOPY);
    }

    /**
     *
     */
    MemoryDC::~MemoryDC()
    { if( m_deleteBitmap ) 
        DELETE_GDIOBJECT(m_hBitmap); 

      ::DeleteDC(m_hDC);
    }

    /**
     *
     */
    COMDC::COMDC(DVTARGETDEVICE* ptd, bool fICOnly)
    { LPTSTR    pszDriver = _T("DISPLAY");
      LPTSTR    pszDevice = NULL;
      LPTSTR    pszPort   = NULL;
      LPDEVMODE pDevMode  = NULL;
      
      if( NULL!=ptd )
      { LPBYTE pDevNames = (LPBYTE)ptd;

        pDevMode  = (ptd->tdExtDevmodeOffset==0) ? NULL : (LPDEVMODE)(pDevNames+ptd->tdExtDevmodeOffset);
        pszDriver = (LPTSTR)(pDevNames+ptd->tdDriverNameOffset);
        pszDevice = (LPTSTR)(pDevNames+ptd->tdDeviceNameOffset);
        pszPort   = (LPTSTR)(pDevNames+ptd->tdPortNameOffset);
      } // of else

      m_hDC = fICOnly ? ::CreateIC(pszDriver, pszDevice, pszPort, pDevMode) : 
                        ::CreateDC(pszDriver, pszDevice, pszPort, pDevMode) ;
    } // of COMDC::COMDC()

    /**
     *
     */
    COMDC::~COMDC()
    { ::DeleteDC(m_hDC); }

    /**
     *
     */
    MetafileDC::MetafileDC(DCBase& dcRef,LPCRECT lpRect,bool shouldDelete,LPCTSTR fileName,LPCTSTR description) :
      m_hMF(NULL),
      m_fileName(fileName),
      m_shouldDelete(shouldDelete)
    { m_hDC = ::CreateEnhMetaFile(dcRef,fileName,lpRect,description);
    } // of MetafileDC::MetafileDC

    /**
     *
     */
    MetafileDC::~MetafileDC()
    { Close();

      if( NULL==m_fileName && NULL!=m_hMF && m_shouldDelete )
        ::DeleteEnhMetaFile(m_hMF);
    } // of MetafileDC::~MetafileDC()

    /**
     *
     */
    void MetafileDC::Close()
    { if( NULL!=m_hDC )
        m_hMF = ::CloseEnhMetaFile(m_hDC);

      m_hDC = NULL;
    } // of MetafileDC::Close()
#pragma endregion

#pragma region Implementation
#pragma endregion
  } // of namespace win
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
