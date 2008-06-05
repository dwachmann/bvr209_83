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
#if !defined(BVR20983CC_H)
#define BVR20983CC_H

#define BVR_SOURCE_LED               2
                                     /* 2*256 */
#define BVR_SOURCE_RC_LED            512

#define BVR_LED_ISRUNNING            0
#define BVR_LED_ISNOTRUNNING         1

#define BVR_DISP_LED_ISRUNNING       MAKE_BVR_DISP(BVR_SOURCE_LED,BVR_LED_ISRUNNING)
#define BVR_DISP_LED_ISNOTRUNNING    MAKE_BVR_DISP(BVR_SOURCE_LED,BVR_LED_ISNOTRUNNING)

/*
 * windows messages
 */
#define LEDM_STARTCLOCK        WM_USER+10
#define LEDM_STOPCLOCK         WM_USER+11
#define LEDM_ISRUNNING         WM_USER+12

#define IDC_EDIT_COPY          1


/*
 * windows styles
 */
#define LEDS_CLOCK             0x0001L  
#define LEDS_24HOUR            0x0002L  
#define LEDS_SUPPRESS          0x0004L
#define LEDS_16SEGEGMENT       0x0008L


/*
 * windows class names
 */
#define LEDWNDCLS   _T("LED")
#define LEDWNDMAGIC 0x4c45446d


typedef struct LEDCREATESTRUCT 
{ DWORD       magic;
  size_t      dwSize;
  COLORREF    bgColor;
  COLORREF    fgColor;
  COLORREF    fgColor1;
  bool        initialStart;
  bool        useThread;
  bool        drawDirectInThread;
  UINT        displayLen;
  void*       pStatusCB;
} LEDCREATESTRUCT, *LPLEDCREATESTRUCT;

/**
 * API
 */
STDAPI InitBVRCustomControls();

#endif // BVR20983CC_H
