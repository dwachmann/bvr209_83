/*
 * $Id$
 * 
 * LogLevel class.
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
#include "util/loglevel.h"

namespace bvr20983
{
  /**
   *
   */
  template< class charT,class traits >
  LPCTSTR LogLevel<charT,traits>::LevelName[] =
  { _T("OFF"),_T("FATAL"),_T("ERROR"),_T("WARN"),_T("INFO"),_T("DEBUG"),_T("TRACE"),_T("ALL") };


} // of namespace bvr20983

template class bvr20983::LogLevel<TCHAR>;
/*==========================END-OF-FILE===================================*/
