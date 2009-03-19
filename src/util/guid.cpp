/*
 * $Id$
 * 
 * Wrapper class for GUID.
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
#include "util/guid.h"
#include "exception/comexception.h"

namespace bvr20983
{
  /**
   *
   */
  CGUID::CGUID() : m_guid(GUID_NULL)
  { }

  /**
   *
   */
  CGUID::CGUID(REFGUID guid) : m_guid(GUID_NULL)
  { Init(guid);
  }

  /**
   *
   */
  CGUID::CGUID(const TString& guid) : m_guidStr(guid),m_guid(GUID_NULL)
  { THROW_COMEXCEPTION( ::CLSIDFromString((LPOLESTR)guid.c_str(),&m_guid) ); 
  }

  /**
   *
   */
  void CGUID::Init(REFGUID guid)
  { m_guid = guid;

    TCHAR szCLSID[GUID_SIZE+1];
  
    ::StringFromGUID2(m_guid,szCLSID, GUID_SIZE);
    
    m_guidStr = TString(szCLSID);
  } // of CGUID::Init()

  /**
   *
   */
  CGUID& CGUID::operator=(const CGUID& cGUID)
  { Init(cGUID.m_guid);

    return *this;
  }

  /**
   *
   */
  CGUID& CGUID::operator=(REFGUID guid)
  { Init(guid);

    return *this;
  }
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
