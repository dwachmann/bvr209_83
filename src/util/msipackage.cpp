/*
 * $Id: $
 * 
 * Class the generated msi package description as xml document.
 * 
 * Copyright (C) 2008-2009 Dorothea Wachmann
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
#include "util/msipackage.h"
#include "util/yastring.h"

namespace bvr20983 
{
  namespace util
  {
    /**
     *
     */
    MSIPackage::MSIPackage(LPCTSTR fileName) : 
      m_fileName(fileName)
    { m_doc.CreateXmlSkeleton(_T("msipackage"),m_rootElement);

      m_doc.CreateElement(_T("files"),m_filesElement);
      m_doc.AppendChildToParent(m_filesElement,m_rootElement,1);
      m_doc.AppendNewline(m_filesElement,1);
    } // of MSIPackage::MSIPackage()

    /**
     *
     */
    MSIPackage::~MSIPackage()
    { }

    /**
     *
     */
    void MSIPackage::AddMedia(long seqNo,LPCTSTR cabName)
    { COMPtr<IXMLDOMElement> mediaElement;

      m_doc.CreateElement(_T("media"),mediaElement);
      m_doc.AppendChild(mediaElement,1);

      m_doc.AddAttribute(mediaElement,_T("diskid"),_T("1"));
      m_doc.AddAttribute(mediaElement,_T("lastSequence"),YAString(seqNo).c_str());
      m_doc.AppendElement(mediaElement,_T("cabname"),cabName,2);
    } // of MSIPackage::AddMedia()

    /**
     *
     */
    void MSIPackage::StartRegistryInfo()
    { m_doc.CreateElement(_T("registryentries"),m_lastRegistryentriesElement);
      m_doc.AppendChildToParent(m_lastRegistryentriesElement,m_lastFileElement,2);
      m_doc.AppendNewline(m_lastRegistryentriesElement,2);
    } // of MSIPackage::StartRegistryInfo

    /**
     *
     */
    void MSIPackage::AddRegistryInfo(bool startSection, LPCTSTR key, LPCTSTR name, LPCTSTR value)
    { if( !m_lastRegistryentriesElement.IsNULL() )
      { COMPtr<IXMLDOMElement> registryElement;

        m_doc.CreateElement(_T("registry"),registryElement);
        m_doc.AppendChildToParent(registryElement,m_lastRegistryentriesElement,3);

        m_doc.AppendElement(registryElement,_T("key"),key,4);

        if( NULL!=name )
        { m_doc.AppendElement(registryElement,_T("name"),name,4);

          if( NULL!=value )
            m_doc.AppendElement(registryElement,_T("value"),value,4);
        } // of if

        m_doc.AppendNewline(registryElement,3);
      } // of if
    } // of MSIPackage::AddRegistryInfo()

    /**
     *
     */
    void MSIPackage::AddDirectoryInfo(VMSIDirInfoT& dirInfo)
    { VMSIDirInfoT::const_iterator iter;

      if( !m_rootElement.IsNULL() )
      { COMPtr<IXMLDOMElement> directoriesElement;

        m_doc.CreateElement(_T("directories"),directoriesElement);
        m_doc.AppendChildToParent(directoriesElement,m_rootElement,1);

        for( iter=dirInfo.begin();iter!=dirInfo.end();iter++ )
        { COMPtr<IXMLDOMElement> directoryElement;

          m_doc.CreateElement(_T("directory"),directoryElement);
          m_doc.AddAttribute(directoryElement,_T("id"),iter->m_dirId);

          if( _tcslen(iter->m_parentId)>0 )
            m_doc.AddAttribute(directoryElement,_T("parentid"),iter->m_parentId);

          m_doc.AppendChildToParent(directoryElement,directoriesElement,2);

          m_doc.AppendElement(directoryElement,_T("path"),iter->m_dirPath,3);
          m_doc.AppendElement(directoryElement,_T("name"),iter->m_dirName,3);
          m_doc.AppendElement(directoryElement,_T("shortname"),iter->m_dirShortName,3);
        } // of for

        m_doc.AppendNewline(directoriesElement,1);
      } // of if
    } // of AddDirectoryInfo()

    /**
     *
     */
    void MSIPackage::AddFileInfo(long id,LPCTSTR guid,long seqNo,LPCTSTR dirId,DWORD fileSize,LPCTSTR strippedFilePath,LPCTSTR fileName,LPCTSTR shortStrippedFileName,LPCTSTR fileVersion)
    { m_doc.CreateElement(_T("file"),m_lastFileElement);
      m_doc.AppendChildToParent(m_lastFileElement,m_filesElement,1);

      m_doc.AddAttribute(m_lastFileElement,_T("id"),YAString((long)id).c_str());
      m_doc.AddAttribute(m_lastFileElement,_T("guid"),guid);
      m_doc.AddAttribute(m_lastFileElement,_T("diskid"),_T("1"));
      m_doc.AddAttribute(m_lastFileElement,_T("no"),YAString((long)seqNo).c_str());

      if( NULL!=dirId )
        m_doc.AddAttribute(m_lastFileElement,_T("directoryid"),dirId);

      m_doc.AddAttribute(m_lastFileElement,_T("size"),YAString(fileSize).c_str());
      m_doc.AppendElement(m_lastFileElement,_T("path"),strippedFilePath,2);
      m_doc.AppendElement(m_lastFileElement,_T("name"),fileName,2);
      m_doc.AppendElement(m_lastFileElement,_T("shortname"),shortStrippedFileName,2);

      if( NULL!=fileVersion )
        m_doc.AppendElement(m_lastFileElement,_T("version"),fileVersion,2);
    } // of MSIPackage::AddFileInfo()

    /**
     *
     */
    void MSIPackage::AddHash(MSIFILEHASHINFO& msiHash)
    { COMPtr<IXMLDOMElement> hashElement;

      m_doc.CreateElement(_T("hash"),hashElement);
      m_doc.AppendChildToParent(hashElement,m_lastFileElement,2);

      m_doc.AddAttribute(hashElement,_T("id0"),YAString((long)msiHash.dwData[0]).c_str());
      m_doc.AddAttribute(hashElement,_T("id1"),YAString((long)msiHash.dwData[1]).c_str());
      m_doc.AddAttribute(hashElement,_T("id2"),YAString((long)msiHash.dwData[2]).c_str());
      m_doc.AddAttribute(hashElement,_T("id3"),YAString((long)msiHash.dwData[3]).c_str());
    } // of MSIPackage::AddHash()

    /**
     *
     */
    void MSIPackage::AppendNewline()
    { m_doc.AppendNewline(m_lastElement,1); }

    /**
     *
     */
    void MSIPackage::Save()
    { m_doc.Save(m_fileName);
    } // of MSIPackage::Save()

  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
