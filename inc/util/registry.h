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
#if !defined(REGISTRY_H)
#define REGISTRY_H

#include "os.h"
#include "util/logstream.h"
#include <iostream>
#include <fstream>
#include <stack>

namespace bvr20983
{
  class RegistryKey
  {
    public:
      RegistryKey(LPCTSTR path);
      RegistryKey(const TString& path);
      RegistryKey(const RegistryKey& path,LPCTSTR subkey=NULL);
      ~RegistryKey();
      
      void SetDumpFile(LPCTSTR dumpFilename);

      bool Create();
      void Open();
      void Delete(bool deep=false);
      bool Exists();

      void  SetValue(LPCTSTR name,LPCTSTR value);
      void  SetIntValue(LPCTSTR name,DWORD value);

      void  QueryValue(LPCTSTR name,TString &value);
      DWORD QueryIntValue(LPCTSTR name);

      bool IsOpened() const
      { return m_keyOpened; }
      
      bool HasSubKey();
      
      operator HKEY() const
      { if( !m_keyOpened )
          throw runtime_error("key not opened");
          
        return m_key; 
      }

      operator TString() const
      { TString result = m_mainKeyStr;
      
        result += '\\';
        result += m_subpath;
        
        return result;
      }
    
    private:
      void Close();
      void Init(LPCTSTR path);
    
      HKEY                  m_mainKey;
      TString               m_mainKeyStr;
      bool                  m_keyOpened;
      HKEY                  m_key;
      VTString              m_path;
      TString               m_subpath;
      
      basic_ostream<TCHAR>* m_pDumpFile;
  }; // of class RegistryKey

  template<class charT, class Traits>
  std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const RegistryKey& rKey);

  class Registry;
  
  class RegistryKeyEnum
  {
    public:
      RegistryKeyEnum(const TString& path,UINT32 maxDepth=1,bool onlySubKey=false);

      bool Next(TString& keyName);
      
      friend class Registry;
      
    private:
      struct State
      { RegistryKey m_key;
        UINT32      m_index;
        bool        m_leaf;
        
        State(RegistryKey key) : m_key(key),m_index(0),m_leaf(false) {}
      };

      void InternalNext(TString& keyName);
      
      stack<State> m_stack;
      UINT32       m_maxDepth;
      bool         m_onlySubKey;
  }; // of class RegistryKeyEnum

  class Registry
  {
    public:
      Registry(const TString& key,LPCTSTR dumpFileName=NULL) : m_key(key.c_str())
      { if( NULL!=dumpFileName )
        { m_dumpFileName=dumpFileName;
          m_key.SetDumpFile(dumpFileName);
        } // of if
      }

      Registry(LPCTSTR key,LPCTSTR dumpFileName=NULL) : m_key(key)
      { if( NULL!=dumpFileName )
        { m_dumpFileName=dumpFileName;
          m_key.SetDumpFile(dumpFileName);
        } // of if
      }

      void  SetKeyValue(LPCTSTR subkey,LPCTSTR name,const TString& value)
      { SetKeyValue(subkey,name,value.c_str()); }

      void  SetKeyValue(LPCTSTR subkey,LPCTSTR name,LPCTSTR value);
      void  SetKeyValue(LPCTSTR subkey,LPCTSTR name,DWORD lValue);
      void  SetKeyIntValue(LPCTSTR subkey,LPCTSTR name,DWORD value);

      void  QueryKeyValue(LPCTSTR subkey,LPCTSTR name,TString& value);

      const RegistryKey& GetKey() const
      { return m_key; }
      
    private:
      RegistryKey m_key;
      TString     m_dumpFileName;
  }; // of class Registry
  

  template<class charT, class Traits>
  std::basic_ostream<charT, Traits>& operator <<(std::basic_ostream<charT, Traits >& os,const Registry& reg);
} // of namespace bvr20983

#endif // REGISTRY_H
//=================================END-OF-FILE==============================