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

#define REG_QUERY_PARAMETER(a,key,value,maxValueLen,hDllInst) if( NULL!=a && NULL!=a->pQueryParam ) a->pQueryParam(a->lParam,hDllInst,key,value,maxValueLen);

typedef bool  (CALLBACK* REGISTRYINFOPROC)    (LPARAM lParam, bool startSection, LPCTSTR mainKey, LPCTSTR key, LPCTSTR name, LPCTSTR value);
typedef DWORD (CALLBACK* REGISTRYPARAMPROC)   (LPARAM lParam, HINSTANCE hDllInst,LPCTSTR key, LPTSTR value, DWORD maxValueLen);

struct EnumRegistration
{ REGISTRYINFOPROC  pEnumProc;
  REGISTRYPARAMPROC pQueryParam;
  LPARAM            lParam;

  EnumRegistration(REGISTRYINFOPROC pEnumProc=NULL,LPARAM lParam=NULL,REGISTRYPARAMPROC pQueryParam=NULL)
  { this->pEnumProc   = pEnumProc;
    this->pQueryParam = pQueryParam;
    this->lParam      = lParam; 
  }
};

typedef void (WINAPI* ENUMREGISTRATIONPROC)(EnumRegistration* pEnumRegistration);

#endif // REGISTRYCB_H
//=================================END-OF-FILE==============================