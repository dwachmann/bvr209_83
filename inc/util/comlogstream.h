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
#if !defined(COMLOGSTREAM_H)
#define COMLOGSTREAM_H

#include "os.h"
#include "util/logstream.h"
#include "util/comstring.h"
#include "util/hr.h"

namespace bvr20983
{
  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, REFIID refiid);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, CLSID* pclsid);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, LPCRECT lpcRect);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, const CHResult& hr);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, SIZEL* pSizeL);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, POINT* pPoint);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, DWORD* pdword);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os,const VARIANT& var);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os,const COMString& s);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os,const STATPROPSTG& statpropstg);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, const PROPVARIANT& propvar);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, IPropertySetStorage *pPropSetStg);

  template< class charT,class traits >
  basic_ostream<charT,traits>& operator<<(basic_ostream<charT,traits>& os, IStorage *pStg);

  TCHAR* getPIDName(UINT index,ULONG pid);
} // of namespace bvr20983

#endif // COMLOGSTREAM_H
//=================================END-OF-FILE==============================