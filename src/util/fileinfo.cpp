/*
 * $Id: dirinfo.cpp 184 2009-03-26 16:04:08Z dwachmann $
 * 
 * Windows DC class.
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
#include "util/fileinfo.h"
#include "util/logstream.h"
#include "util/yanew.h"
#include "util/yaallocatorpool.h"
#include "exception/lasterrorexception.h"

namespace bvr20983 
{
  namespace util
  {
    YAAllocatorBase* FileInfo::m_pClassAllocator = FileInfo::RegisterAllocator();

    /**
     *
     */
    YAAllocatorBase* FileInfo::RegisterAllocator()
    { YAAllocatorPool::AddAllocator(_T("FileInfo"),new YAAllocator<FileInfo>);
    
      return YAAllocatorPool::GetAllocator(_T("FileInfo"));
    }

    /**
     *
     */
    FileInfo::FileInfo(const YAPtr<YAString>& fileName)
    { m_fileName = YAPTR1(YAString,fileName->c_str());
    }

    /**
     *
     */
    FileInfo::FileInfo(const YAString& fileName)
    { m_fileName = YAPTR1(YAString,fileName);
    }

    /**
     *
     */
    FileInfo::FileInfo(LPCTSTR fileName)
    { m_fileName = YAPTR1(YAString,fileName);
    }

    /**
     *
     */
    FileInfo::~FileInfo()
    { 
    } // of FileInfo::~FileInfo()

    /**
     *
     */
    YAPtr<YAString> FileInfo::GetFullPath() const
    { YAPtr<YAString> result;

      if( !m_fileName.IsNull() )
      { result = YAPTR(YAString);

        DWORD pathLen = ::GetFullPathName(m_fileName->c_str(),0,NULL,NULL);

        THROW_LASTERROREXCEPTION1( pathLen );

        result->Resize(pathLen);
          
        THROW_LASTERROREXCEPTION1( ::GetFullPathName(m_fileName->c_str(),pathLen,const_cast<LPTSTR>(result->c_str()),NULL) );
      } // of if

      return result;
    } // of FileInfo::GetFullPath()

    /**
     *
     */
    YAPtr<YAString> FileInfo::GetShortName() const
    { YAPtr<YAString> result;

      if( !m_fileName.IsNull() )
      { result = YAPTR(YAString);

        DWORD pathLen = ::GetShortPathName(m_fileName->c_str(),NULL,0);

        THROW_LASTERROREXCEPTION1( pathLen );

        result->Resize(pathLen);
          
        THROW_LASTERROREXCEPTION1( ::GetShortPathName(m_fileName->c_str(),const_cast<LPTSTR>(result->c_str()),pathLen) );
      } // of if

      return result;
    } // of FileInfo::GetShortName()

    /**
     *
     */
    YAPtr<YAString> FileInfo::GetName() const
    { YAPtr<YAString> result;

      LPCTSTR p = _tcsrchr(m_fileName->c_str(), _T('\\'));
      
      if( !m_fileName.IsNull() )
      { if( p==NULL )
          result = YAPTR1(YAString,m_fileName->c_str());
        else
          result = YAPTR1(YAString,p+1);
      } // of if

      return result;
    } // of FileInfo::GetName()

    /**
     *
     */
    YAPtr<YAString> FileInfo::GetFileName() const
    { return m_fileName; }

    /**
     *
     */
    template<class charT, class Traits>
    basic_ostream<charT, Traits>& operator <<(basic_ostream<charT, Traits >& os,const FileInfo& fInfo)
    { os<<_T("FileInfo[")<<fInfo.GetFileName()<<_T("]"); 

      return os;
    }

    template basic_ostream<TCHAR,char_traits<TCHAR>>& bvr20983::operator << <TCHAR,char_traits<TCHAR>>( basic_ostream<TCHAR,char_traits<TCHAR>>&,const FileInfo&);
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
