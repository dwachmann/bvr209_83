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
#if !defined(CCARDFILE_H)
#define CCARDFILE_H

namespace bvr20983
{
  class Smartcard;
  
  class CardFile
  {
    public:
      CardFile(bool isGsmCard=false);
      
      void Select(Smartcard& sc,WORD fid);
  
      void Read(Smartcard& sc,bool genMac=false);
      void ReadBinary(Smartcard& sc);

      void Verify(Smartcard& sc,BYTE no,LPCTSTR pin,BYTE pinLen);
      
    private:
      BYTE m_class;
      WORD m_fid;
  
  }; // of class CardFile
} // of namespace bvr20983


#endif // CCARDFILE_H
