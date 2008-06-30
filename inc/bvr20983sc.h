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
#if !defined(BVR20983SC_H)
#define BVR20983SC_H

#define BVR_SOURCE_SMARTCARD         0
                                     /* 0*256 */
#define BVR_SOURCE_RC_SMARTCARD      0 

#define BVR_SC_NOTINITED             0
#define BVR_SC_NOTCONNECTED          1
#define BVR_SC_NOCARD                2

#define BVR_DISP_SC_NOTINITED        MAKE_BVR_DISP(BVR_SOURCE_SMARTCARD,BVR_SC_NOTINITED)
#define BVR_DISP_SC_NOTCONNECTED     MAKE_BVR_DISP(BVR_SOURCE_SMARTCARD,BVR_SC_NOTCONNECTED)
#define BVR_DISP_SC_NOCARD           MAKE_BVR_DISP(BVR_SOURCE_SMARTCARD,BVR_SC_NOCARD)

#define BVR_SOURCE_WALLET            1
                                     /* 1*256 */
#define BVR_SOURCE_RC_WALLET         256 

#define BVR_WALLET_INVALIDPIN        0
#define BVR_WALLET_INVALIDTRXAMOUNT  1
#define BVR_WALLET_NEGATIVEBALANCE   2
#define BVR_WALLET_MAXIMUMBALANCE    3

#define BVR_DISP_WA_INVALIDPIN       MAKE_BVR_DISP(BVR_SOURCE_WALLET,BVR_WALLET_INVALIDPIN)
#define BVR_DISP_WA_INVALIDTRXAMOUNT MAKE_BVR_DISP(BVR_SOURCE_WALLET,BVR_WALLET_INVALIDTRXAMOUNT)
#define BVR_DISP_WA_NEGATIVEBALANCE  MAKE_BVR_DISP(BVR_SOURCE_WALLET,BVR_WALLET_NEGATIVEBALANCE)
#define BVR_DISP_WA_MAXIMUMBALANCE   MAKE_BVR_DISP(BVR_SOURCE_WALLET,BVR_WALLET_MAXIMUMBALANCE)

#endif // BVR20983SC_H
