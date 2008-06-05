/*
 * $Id$
 * 
 * Implementation class for smart COM pointer.
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
#include "util/comptrbase.h"
#include "exception/comexception.h"

namespace bvr20983
{
  /**
   *
   */
  IGlobalInterfaceTable* GCOMPtrBase::m_gpGIT=NULL;

  /**
   *
   */
  GCOMPtrBase::GCOMPtrBase()
  { }

  /**
   *
   */
  GCOMPtrBase::~GCOMPtrBase()
  { }

  /**
   *
   */
  void GCOMPtrBase::Init()
  { if( NULL==m_gpGIT )
      ::CoCreateInstance(CLSID_StdGlobalInterfaceTable,NULL,CLSCTX_INPROC_SERVER,IID_IGlobalInterfaceTable,(void **)&m_gpGIT);
  } // of COMPtrBase::Init()
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
