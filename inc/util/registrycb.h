/*
 * $Id$
 * 
 * Copyright (C) 2009 Dorothea Wachmann
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
#if !defined(REGISTRYCB_H)
#define REGISTRYCB_H

#include "os.h"

typedef bool (CALLBACK* REGISTRYINFOPROC)    (LPARAM lParam, bool startSection, LPCTSTR mainKey, LPCTSTR key, LPCTSTR name, LPCTSTR value);
typedef void (WINAPI*   ENUMREGISTRATIONPROC)(REGISTRYINFOPROC pEnumProc, LPARAM lParam);

#endif // REGISTRYCB_H
//=================================END-OF-FILE==============================