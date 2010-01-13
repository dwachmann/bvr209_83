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
#if !defined(REGISTRYUTIL_H)
#define REGISTRYUTIL_H

#include "os.h"
#include "util/logstream.h"
#include "util/registry.h"
#include "util/registrycb.h"
#include <iostream>
#include <fstream>
#include <stack>

namespace bvr20983
{
  /**
   *
   */
  class RegistryParameter
  {
    public:
      static DWORD CALLBACK     GetValueCB(LPARAM lParam, HINSTANCE hDllInst,LPCTSTR key, LPTSTR value, DWORD maxValueLen);
      static EnumRegistration*  GetEnumRegistration();
      static RegistryParameter* GetInstance();

      LPCTSTR GetProductPrefix() const
      { return m_prodPrefix.c_str(); }

      LPCTSTR GetComponentPrefix() const
      { return m_compPrefix.c_str(); }

    private:
      static EnumRegistration* m_pEnumRegistration;

      TCHAR   m_szModulePath[MAX_PATH];
      TCHAR   m_szWindowsDir[MAX_PATH];
      TString m_prodPrefix;
      TString m_compPrefix;

      RegistryParameter();
      DWORD GetValue(HINSTANCE hDllInst,LPCTSTR key, LPTSTR value, DWORD maxValueLen);
  }; // of class RegistryParameter

  /**
   *
   */
  class RegistryUtil
  {
    public:
      enum COMRegistrationType
      { CLASSES_ROOT,
        PER_MACHINE,
        PER_USER,
        AUTO
      };

      static void RegisterComObjectsInTypeLibrary(Registry& reg,
                                                  COMRegistrationType registrationType=CLASSES_ROOT,
                                                  EnumRegistration* pEnumRegistration=RegistryParameter::GetEnumRegistration()
                                                 );

      static void RegisterCoClass(Registry& reg,
                                  TLIBATTR* pTypeLib,LPCTSTR typelibName,
                                  REFGUID typeGUID,LPCTSTR typeName,LPCTSTR typeDesc,WORD typeVersion,
                                  ITypeInfo2& rTypeInfo2,
                                  bool isControl=false,
                                  COMRegistrationType registrationType=CLASSES_ROOT,
                                  EnumRegistration* pEnumRegistration=RegistryParameter::GetEnumRegistration()
                                 );

      static void RegisterInterface(Registry& reg,
                                    REFGUID typelibGUID,
                                    WORD majorVersion,WORD minorVersion,
                                    REFGUID typeGUID,LPCTSTR typeName,LPCTSTR typeDesc,
                                    COMRegistrationType registrationType=CLASSES_ROOT,
                                    EnumRegistration* pEnumRegistration=RegistryParameter::GetEnumRegistration()
                                   );

      static void RegisterTypeLib(Registry& registry,
                                  LPCTSTR typelibDesc,
                                  REFGUID typelibGUID,
                                  LCID lcid,USHORT majorVersion,USHORT minorVersion,
                                  COMRegistrationType registrationType=CLASSES_ROOT,
                                  EnumRegistration* pEnumRegistration=RegistryParameter::GetEnumRegistration()
                                 );

      static void RegisterTypeLib(bool register4User,
                                  REFGUID typelibGUID,
                                  LCID lcid,USHORT majorVersion,USHORT minorVersion,
                                  EnumRegistration* pEnumRegistration=RegistryParameter::GetEnumRegistration()
                                 );

      static void GetKeyPrefix(COMRegistrationType registrationType,TString& keyPrefix);
  }; // of class RegistryUtil
} // of namespace bvr20983
#endif // REGISTRYUTIL_H
//=================================END-OF-FILE==============================