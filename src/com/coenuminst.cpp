/*
 * $Id$
 * 
 * Generic COM Enum class.
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
#include "com/coenum.tmpl"

using namespace bvr20983::COM;

template class COEnum<IEnumFORMATETC,FORMATETC>;

template class COEnum<IEnumConnectionPoints,IConnectionPoint*,
                      AddrefAllocFtr<IConnectionPoint*>,
                      ReleaseFreeFtr<IConnectionPoint*>,
                      AddrefFtr<IConnectionPoint*>
                     >;

template class COEnum<IEnumOLEVERB,OLEVERB>;

template class COEnum<IEnumString,LPOLESTR,StringAllocFtr,StringFreeFtr>;

template class COEnum<IEnumConnections,CONNECTDATA,AddrefCPAllocFtr,ReleaseCPFreeFtr,AddrefCPFtr>;
/*==========================END-OF-FILE===================================*/
