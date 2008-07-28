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
#ifndef CabinetFDI_H
#define CabinetFDI_H

#include "cab/fdi.h"

namespace bvr20983
{
  namespace cab
  {
    /**
     *
     */
    class CabinetFDI
    {
    public:
      CabinetFDI(LPCSTR cabinet_fullpath,LPCSTR destDir=NULL);
      ~CabinetFDI();

       void Extract();
       void List();

    private:
      HFDI            m_hfdi;
      FDICABINETINFO	m_fdici;
      ERF             m_erf;
      char            m_cabinetFullPath[MAX_PATH];
      char            m_cabinetPath[MAX_PATH];
      char            m_cabinetName[MAX_PATH];
      char            m_destinationDir[MAX_PATH];
      BOOL            m_listOnly;

      static FNALLOC(fdi_memalloc);
      static FNFREE(fdi_memfree);
      static FNOPEN(fdi_fileopen);
      static FNREAD(fdi_fileread);
      static FNWRITE(fdi_filewrite);
      static FNCLOSE(fdi_fileclose);
      static FNSEEK(fdi_fileseek);
      static FNFDINOTIFY(fdi_notification);

      void Init();
      int  FDINotification(FDINOTIFICATIONTYPE fdint,PFDINOTIFICATION pfdin);
    }; // of class CabinetFDI
  } // of namespace cab
} // of namespace bvr20983
#endif CabinetFDI_H
/*==========================END-OF-FILE===================================*/
