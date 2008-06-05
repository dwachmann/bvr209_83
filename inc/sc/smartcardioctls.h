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
#if !defined(SMARTCARDIOCTLS_H)
#define SMARTCARDIOCTLS_H

#define CM_IOCTL_GET_FEATURE_REQUEST SCARD_CTL_CODE(3400)


#pragma pack(1)

typedef struct
{
  BYTE  tag;
  BYTE  length;
  ULONG value; /**< This value is always in BIG ENDIAN format as documented in PCSC v2 part 10 ch 2.2 page 2. You can use ntohl() for example */
} PCSC_TLV;

typedef struct
{ BYTE  SAD;
  BYTE  DAD;
  WORD  bufferLength;
  BYTE  buffer[1];
} MCTUniversal;

typedef struct
{
  BYTE  bTimerOut;  /**< timeout is seconds (00 means use default timeout) */
  BYTE  bTimerOut2; /**< timeout in seconds after first key stroke */
  BYTE  bmFormatString; /**< formatting options */
  BYTE  bmPINBlockString; /**< bits 7-4 bit size of PIN length in APDU,
                          * bits 3-0 PIN block size in bytes after
                          * justification and formatting */
  BYTE  bmPINLengthFormat; /**< bits 7-5 RFU,
                           * bit 4 set if system units are bytes, clear if
                           * system units are bits,
                           * bits 3-0 PIN length position in system units */
  WORD  wPINMaxExtraDigit; /**< 0xXXYY where XX is minimum PIN size in digits,
                              and YY is maximum PIN size in digits */
  BYTE  bEntryValidationCondition; /**< Conditions under which PIN entry should
                                   * be considered complete */
  BYTE  bNumberMessage; /**< Number of messages to display for PIN verification */
  WORD  wLangId; /**< Language for messages */
  BYTE  bMsgIndex; /**< Message index (should be 00) */
  BYTE  bTeoPrologue[3]; /**< T=1 block prologue field to use (fill with 00) */
  ULONG ulDataLength; /**< length of Data to be sent to the ICC */
  BYTE  abData[1]; /**< Data to send to the ICC */
} PIN_VERIFY;

typedef struct
{
  BYTE  bTimerOut;           /**< timeout is seconds (00 means use default timeout) */
  BYTE  bTimerOut2;          /**< timeout in seconds after first key stroke */
  BYTE  bmFormatString;      /**< formatting options */
  BYTE  bmPINBlockString;    /**< bits 7-4 bit size of PIN length in APDU,
                                 * bits 3-0 PIN block size in bytes after
                                 * justification and formatting */
  BYTE  bmPINLengthFormat;   /**< bits 7-5 RFU,
                                 * bit 4 set if system units are bytes, clear if
                                 * system units are bits,
                                 * bits 3-0 PIN length position in system units */
  BYTE  bInsertionOffsetOld; /**< Insertion position offset in bytes for
                                 * the current PIN */
  BYTE  bInsertionOffsetNew; /**< Insertion position offset in bytes for
                                  * the new PIN */
  WORD  wPINMaxExtraDigit;   /**< 0xXXYY where XX is minimum PIN size in digits,
                                 * and YY is maximum PIN size in digits */
  BYTE  bConfirmPIN;         /**< Flags governing need for confirmation of new PIN */
  BYTE  bEntryValidationCondition; /**< Conditions under which PIN entry should
                                       * be considered complete */
  BYTE  bNumberMessage; /**< Number of messages to display for PIN verification*/
  WORD  wLangId; /**< Language for messages */
  BYTE  bMsgIndex1; /**< index of 1st prompting message */
  BYTE  bMsgIndex2; /**< index of 2d prompting message */
  BYTE  bMsgIndex3; /**< index of 3d prompting message */
  BYTE  bTeoPrologue[3]; /**< T=1 block prologue field to use (fill with 00) */
  ULONG ulDataLength; /**< length of Data to be sent to the ICC */
  BYTE  abData[1]; /**< Data to send to the ICC */
} PIN_MODIFY;

#pragma pack()

#endif // SMARTCARDIOCTLS_H
//=================================END-OF-FILE==============================