/*
 * $Id:$
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
#if !defined(MSIPACKAGE_H)
#define MSIPACKAGE_H

#include <Msi.h>
#include "util/xmldocument.h"
#include "util/msidirinfo.h"
#include "util/msiidregistry.h"

namespace bvr20983
{
  namespace util
  {
    typedef std::map<TString,std::set<TString>>  STR_TStringSet_Map;
    typedef std::pair<TString,std::set<TString>> STR_TStringSet_Pair;

    typedef std::map<TString,TString>            STR_STR_Map;
    typedef std::pair<TString,TString>           STR_STR_Pair;

    /**
     *
     */
    class MSIPackage
    {
      public:
        MSIPackage(LPCTSTR fileName,util::XMLDocument versionsDoc,MSIIdRegistry& idRegistry);
        ~MSIPackage();

        void Save(util::XMLDocument versionsDoc,MSIIdRegistry& idRegistry);
        void AddMedia(long seqNo,LPCTSTR cabName);
        void AddDirectoryInfo(VMSIDirInfoT& dirInfo);
        void StartRegistryInfo();
        void AddRegistryInfo(LPCTSTR id,LPCTSTR guid,bool startSection, LPCTSTR mainKey, LPCTSTR key, LPCTSTR name, LPCTSTR value);
        bool AddFileInfo(LPCTSTR id,LPCTSTR guid,long seqNo,LPCTSTR dirId,DWORD fileSize,LPCTSTR strippedFilePath,LPCTSTR fileName,LPCTSTR shortStrippedFileName,LPCTSTR fileVersion);
        void AddHash(MSIFILEHASHINFO& msiHash);
        void AppendNewline();

      private:
        LPCTSTR                m_fileName;
        XMLDocument            m_doc;
        COMPtr<IXMLDOMElement> m_rootElement;
        COMPtr<IXMLDOMElement> m_filesElement;
        COMPtr<IXMLDOMElement> m_lastRegistryentriesElement;
        COMPtr<IXMLDOMElement> m_lastFileElement;
        COMPtr<IXMLDOMElement> m_lastElement;

        STR_TStringSet_Map     m_comp2feature;
        STR_STR_Map            m_filename2comp;
        STR_STR_Map            m_dirname2comp;
        STR_STR_Map            m_filename2compid;
        STR_STR_Map            m_dirpath2dirid;

        void    LoadFeatures(util::XMLDocument versionsDoc);
        void    LoadFileNames(util::XMLDocument versionsDoc);
        void    LoadDirectoryNames(util::XMLDocument versionsDoc);
        void    LoadRegistryEntries(util::XMLDocument versionsDoc,MSIIdRegistry& idRegistry);
        void    LoadProperties(util::XMLDocument versionsDoc);
        void    LoadShortcuts(util::XMLDocument versionsDoc);
        bool    AddFeatures(COMPtr<IXMLDOMElement>& featuresElement,LPCTSTR strippedFilePath,LPCTSTR fileName);
        void    AddFeatureInfo(util::XMLDocument versionsDoc);
	  }; // of class MSIPackage

  } // of namespace util
} // of namespace bvr20983
#endif // MSIPACKAGE_H
/*==========================END-OF-FILE===================================*/
