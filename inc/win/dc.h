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
#if !defined(DC_H)
#define DC_H

#include "os.h"

namespace bvr20983
{
  namespace win
  { 
    class Wnd;

    /**
     *
     */
    class DCBase
    {
      public:
        DCBase() : m_hDC(NULL)
        { }
        DCBase(HDC hDC) : m_hDC(hDC)
        { }
        virtual ~DCBase()
        { }

        void GetTextMetrics(TEXTMETRIC* tm)
        { ::GetTextMetrics(m_hDC,tm); }

        void TextOut(int x,int y,LPCTSTR s)
        { ::TextOut(m_hDC,x,y,s,_tcslen(s)); }

        void SetTextAlign(UINT align)
        { ::SetTextAlign(m_hDC,align); }

        void Polygon(const POINT* lpPoints,int nCount)
        { ::Polygon(m_hDC,lpPoints,nCount); }

        void FillRect(HBRUSH hBrush,int x,int y,int w,int h)
        { RECT r;
          
          r.left=x;
          r.top=y;
          r.right=x+w;
          r.bottom=y+h;

          ::FillRect(m_hDC,&r,hBrush); 
        }

        void Rectangle(int x,int y,int w,int h)
        { ::Rectangle(m_hDC,x,y,x+w,y+h); }

        void Rectangle(LPCRECT r)
        { if( NULL!=r ) ::Rectangle(m_hDC,r->left,r->top,r->right,r->bottom); }

        void Ellipse(int x,int y,int w,int h)
        { ::Ellipse(m_hDC,x,y,x+w,y+h); }

        void MoveTo(int x,int y)
        { ::MoveToEx(m_hDC,x,y,NULL); }

        void LineTo(int x,int y)
        { ::LineTo(m_hDC,x,y); }

        void OffsetWindowOrg(int nXOffset,int nYOffset,LPPOINT lpPoint=NULL)
        { ::OffsetWindowOrgEx(m_hDC,nXOffset,nYOffset,lpPoint); }

        void SetWindowOrg(int nX,int nY,LPPOINT lpPoint=NULL)
        { ::SetWindowOrgEx(m_hDC,nX,nY,lpPoint); }

        void SetWindowExt(int nXExtent,int nYExtent,LPSIZE lpSize=NULL)
        { ::SetWindowExtEx(m_hDC,nXExtent,nYExtent,lpSize); }

        void OffsetViewportOrg(int nXOffset,int nYOffset,LPPOINT lpPoint=NULL)
        { ::OffsetViewportOrgEx(m_hDC,nXOffset,nYOffset,lpPoint); }

        void SetViewportOrg(int nX,int nY,LPPOINT lpPoint=NULL)
        { ::SetViewportOrgEx(m_hDC,nX,nY,lpPoint); }

        void SetViewportExt(int nXExtent,int nYExtent,LPSIZE lpSize=NULL)
        { ::SetViewportExtEx(m_hDC,nXExtent,nYExtent,lpSize); }

        void SetMapMode(int fnMapMode)
        { ::SetMapMode(m_hDC,fnMapMode); }

        void SetBkMode(int bkMode)
        { ::SetBkMode(m_hDC,bkMode); }

        HGDIOBJ SelectObject(HGDIOBJ hgdiobj)
        { return hgdiobj!=NULL ? ::SelectObject(m_hDC,hgdiobj) : NULL; }

        void BitBlt(int nXDest,int nYDest,
                    int nWidth,int nHeight,
                    HDC hdcSrc,
                    int nXSrc=0,int nYSrc=0,
                    DWORD dwRop=SRCCOPY
                  )
        { ::BitBlt(m_hDC,nXDest,nYDest,nWidth,nHeight,
                   hdcSrc,nXSrc,nYSrc,
                   dwRop
                  );
        }

        void StretchBlt(int nXOriginDest,int nYOriginDest,
                        int nWidthDest,int nHeightDest,
                        HDC hdcSrc,
                        int nXOriginSrc,int nYOriginSrc,
                        int nWidthSrc,int nHeightSrc,
                        DWORD dwRop=SRCCOPY
                       )
        { ::StretchBlt(m_hDC,nXOriginDest,nYOriginDest,nWidthDest,nHeightDest,
                       hdcSrc,nXOriginSrc,nYOriginSrc,nWidthSrc,nHeightSrc,
                       dwRop
                      );
        }


        void SaveDC()
        { ::SaveDC(m_hDC); }

        void RestoreDC()
        { ::RestoreDC(m_hDC,-1); }

        int GetDeviceCaps(int index)
        { return ::GetDeviceCaps(m_hDC,index); }

        operator HDC() const
        { return m_hDC; }

        virtual const RECT* GetRepaintRect() const
        { return NULL; }

      protected:
        HDC m_hDC;
    }; // of class DCBase

    /**
     *
     */
    class DC : public DCBase
    {
      public:
        DC(HWND hWnd);
        virtual ~DC();

        void SetRepaintRect(LPCRECT rr)
        { m_rr = rr; 

          if( NULL!=m_rr )
          { HRGN hRgn = ::CreateRectRgn(m_rr->left, m_rr->top, m_rr->right, m_rr->bottom); 
    
            ::SelectClipRgn(m_hDC, hRgn); 

            DELETE_GDIOBJECT(hRgn);
          } // of if
          else
            ::SelectClipRgn(m_hDC, NULL); 
        }

        virtual const RECT* GetRepaintRect() const
        { return m_rr; }

      private:
        HWND         m_hWnd;
        LPCRECT      m_rr;
    }; // of class DC

    /**
     *
     */
    class WndDC : public DCBase
    {
      public:
        WndDC(Wnd& wnd);
        virtual ~WndDC();

      private:
        Wnd& m_wnd;
    }; // of class WndDC

    /**
     *
     */
    class PaintDC : public DCBase
    { 
      public:
        PaintDC(HWND hWnd);
        virtual ~PaintDC();

        bool ShouldEraseBackground() const
        { return m_PS.fErase==TRUE; }

        virtual const RECT* GetRepaintRect() const
        { return m_PS.rcPaint.bottom==0 && m_PS.rcPaint.top==0 && m_PS.rcPaint.right==0 && m_PS.rcPaint.bottom==0 ? NULL : &m_PS.rcPaint; }

      protected:
        PAINTSTRUCT m_PS;
        HWND        m_hWnd;
    }; // of class PaintDC

    /**
     *
     */
    class MemoryDC : public DCBase
    { 
      public:
        MemoryDC(const DCBase& dc,int w,int h,bool deleteBitmap=true,bool monocrome=false);
        MemoryDC(const MemoryDC& memDC,bool deleteBitmap=true,bool monocrome=false);
        virtual ~MemoryDC();

        operator HBITMAP() const
        { return m_hBitmap; }

      protected:
        HBITMAP m_hBitmap;
        int     m_w;
        int     m_h;
        bool    m_deleteBitmap;
        bool    m_monocrome;
    }; // of class MemoryDC

    /**
     *
     */
    class COMDC : public DCBase
    { 
      public:
        COMDC(DVTARGETDEVICE* ptd, bool fICOnly=false);
        virtual ~COMDC();

    }; // of class COMDC

    /**
     *
     */
    class MetafileDC : public DCBase
    { 
      public:
        MetafileDC(DCBase& dcRef,LPCRECT lpRect=NULL,bool shouldDelete=true,LPCTSTR fileName=NULL,LPCTSTR description=NULL);
        virtual ~MetafileDC();

        void Close();

        operator HENHMETAFILE() const
        { return m_hMF; }

      protected:
        HENHMETAFILE m_hMF;
        LPCTSTR      m_fileName;
        bool         m_shouldDelete;
    }; // of class MetafileDC
  } // of namespace win
} // of namespace bvr20983
#endif // DC_H