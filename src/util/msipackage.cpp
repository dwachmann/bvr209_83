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
#include "util/logstream.h"

using namespace bvr20983::COM;

namespace bvr20983 
{
  namespace util
  {
    /**
     *
     */
    MSIPackage::MSIPackage(LPCTSTR fileName,util::XMLDocument versionsDoc,MSIIdRegistry& idRegistry) : 
      m_fileName(fileName)
    { m_doc.CreateXmlSkeleton(_T("msipackage"),m_rootElement);

      AddFeatureInfo(versionsDoc);

      m_doc.CreateElement(_T("files"),m_filesElement);
      m_doc.AppendChildToParent(m_filesElement,m_rootElement,1);
      m_doc.AppendNewline(m_filesElement,1);

      LoadFeatures(versionsDoc);
      LoadFileNames(versionsDoc);
      LoadDirectoryNames(versionsDoc);
      LoadRegistryEntries(versionsDoc,idRegistry);
      LoadProperties(versionsDoc);
    } // of MSIPackage::MSIPackage()

    /**
     *
     */
    MSIPackage::~MSIPackage()
    { }

    /**
     *
     */
    void MSIPackage::AddFeatureInfo(util::XMLDocument versionsDoc)
    { COMPtr<IXMLDOMElement>  featuresElement;
      COMPtr<IXMLDOMNodeList> pXMLDomNodeList;
      COMPtr<IXMLDOMNode>     pNode;

      versionsDoc.GetSelection(_T("//v:version[1]//v:msifeature"),pXMLDomNodeList);

      if( !pXMLDomNodeList.IsNULL() )
        for( HRESULT hr = pXMLDomNodeList->nextNode(&pNode);hr==S_OK;hr=pXMLDomNodeList->nextNode(&pNode) )
        { COVariant id;
          COVariant directory;
          COVariant level;
          COVariant attribute;
          COVariant title;
          COVariant description;

          if( versionsDoc.GetAttribute(pNode,_T("id"),id) &&
              versionsDoc.GetAttribute(pNode,_T("directory"),directory) &&
              versionsDoc.GetAttribute(pNode,_T("level"),level) &&
              versionsDoc.GetAttribute(pNode,_T("attribute"),attribute) &&
              versionsDoc.GetAttribute(pNode,_T("title"),title) &&
              versionsDoc.GetAttribute(pNode,_T("description"),description)
            )
          { COMPtr<IXMLDOMNode> pParentNode;
            COVariant           parentId;
          
            THROW_COMEXCEPTION( pNode->get_parentNode(&pParentNode) );

            if( !pParentNode.IsNULL() && versionsDoc.IsElement(pParentNode,_T("msifeature")) )
              versionsDoc.GetAttribute(pParentNode,_T("id"),parentId);

            if( featuresElement.IsNULL() )
            { versionsDoc.CreateElement(_T("features"),featuresElement);
              versionsDoc.AppendChildToParent(featuresElement,m_rootElement,1);
            } // of if
      
            COMPtr<IXMLDOMElement> featureElement;
            versionsDoc.CreateElement(_T("feature"),featureElement);
            versionsDoc.AppendChildToParent(featureElement,featuresElement,2);

            versionsDoc.AddAttribute(featureElement,_T("id"),V_BSTR(id));
            versionsDoc.AddAttribute(featureElement,_T("directory"),V_BSTR(directory));
            versionsDoc.AddAttribute(featureElement,_T("level"),V_BSTR(level));
            versionsDoc.AddAttribute(featureElement,_T("attribute"),V_BSTR(attribute));
            versionsDoc.AddAttribute(featureElement,_T("title"),V_BSTR(title));
            versionsDoc.AddAttribute(featureElement,_T("description"),V_BSTR(description));

            if( parentId.IsSet() )
              versionsDoc.AddAttribute(featureElement,_T("parentid"),V_BSTR(parentId));
          } // of if
        } // of for
    } // of MSIPackage::AddFeatureInfo()

    /**
     *
     */
    void MSIPackage::LoadFeatures(util::XMLDocument versionsDoc)
    { COMPtr<IXMLDOMNodeList> pXMLDomNodeList;
      COMPtr<IXMLDOMNode>     pNode;

      versionsDoc.GetSelection(_T("//v:version[1]//v:msicomponent"),pXMLDomNodeList);

      if( !pXMLDomNodeList.IsNULL() )
        for( HRESULT hr = pXMLDomNodeList->nextNode(&pNode);hr==S_OK;hr=pXMLDomNodeList->nextNode(&pNode) )
        { COVariant compid;

          if( versionsDoc.GetAttribute(pNode,_T("compid"),compid) )
          { COMPtr<IXMLDOMNode> pParentNode;
            COVariant           id;
            COVariant           title;
          
            THROW_COMEXCEPTION( pNode->get_parentNode(&pParentNode) );

            if( !pParentNode.IsNULL()                               &&
                versionsDoc.IsElement(pParentNode,_T("msifeature")) &&
                versionsDoc.GetAttribute(pParentNode,_T("id"),id)   &&
                versionsDoc.GetAttribute(pParentNode,_T("title"),title)
              )
            { TString compIdKey(V_BSTR(compid));
              TString featureIdKey(V_BSTR(id));

              STR_TStringSet_Map::iterator it = m_comp2feature.find(compIdKey);

              if( it==m_comp2feature.end() )
              { std::set<TString> features;

                features.insert(TString(V_BSTR(id)));

                m_comp2feature.insert(STR_TStringSet_Pair(compIdKey,features));
              } // of if
              else
                it->second.insert(featureIdKey);
            } // of if
          } // of if
        } // of for
    } // of MSIPackage::LoadFeatures()

    /**
     *
     */
    void MSIPackage::LoadFileNames(util::XMLDocument versionsDoc)
    { COMPtr<IXMLDOMNodeList> pXMLDomNodeList;
      COMPtr<IXMLDOMNode>     pNode;

      versionsDoc.GetSelection(_T("//v:component[@type='dll' or @type='exe' or @type='file']"),pXMLDomNodeList);

      if( !pXMLDomNodeList.IsNULL() )
        for( HRESULT hr = pXMLDomNodeList->nextNode(&pNode);hr==S_OK;hr=pXMLDomNodeList->nextNode(&pNode) )
        { COVariant id;
          COVariant type;

          if( versionsDoc.GetAttribute(pNode,_T("id"),id) &&
              versionsDoc.GetAttribute(pNode,_T("type"),type) 
            )
          { TString filenameKey(_T("//v:component[@id='"));
            filenameKey += V_BSTR(id);
            filenameKey += _T("']/v:filename/text()");

            COVariant filename;

            if( versionsDoc.GetNodeValue(filenameKey.c_str(),filename,true) )
            { TString fullFilename(V_BSTR(filename));

              if( _tcscmp(V_BSTR(type),_T("dll"))==0 || _tcscmp(V_BSTR(type),_T("exe"))==0 )
              { fullFilename += _T(".");
                fullFilename += V_BSTR(type);
              } // of if

              m_filename2comp.insert( STR_STR_Pair(fullFilename,V_BSTR(id)) );

              if( _tcscmp(V_BSTR(type),_T("dll"))==0 )
              { TString fullFilename(V_BSTR(filename));

                fullFilename += _T(".lib");

                m_filename2comp.insert( STR_STR_Pair(fullFilename,V_BSTR(id)) );
              }
            } // of if
          } // of if
        } // of for
    } // of MSIPackage::LoadFileNames()

    /**
     *
     */
    void MSIPackage::LoadDirectoryNames(util::XMLDocument versionsDoc)
    { COMPtr<IXMLDOMNodeList> pXMLDomNodeList;
      COMPtr<IXMLDOMNode>     pNode;

      versionsDoc.GetSelection(_T("//v:component[@type='dir']"),pXMLDomNodeList);

      if( !pXMLDomNodeList.IsNULL() )
        for( HRESULT hr = pXMLDomNodeList->nextNode(&pNode);hr==S_OK;hr=pXMLDomNodeList->nextNode(&pNode) )
        { COVariant id;
          COVariant type;

          if( versionsDoc.GetAttribute(pNode,_T("id"),id) )
          { TString filenameKey(_T("//v:component[@id='"));
            filenameKey += V_BSTR(id);
            filenameKey += _T("']/v:filename/text()");

            COVariant filename;

            if( versionsDoc.GetNodeValue(filenameKey.c_str(),filename,true) )
              m_dirname2comp.insert( STR_STR_Pair(V_BSTR(filename),V_BSTR(id)) );
          } // of if
        } // of for
    } // of MSIPackage::LoadDirectoryNames()

    /**
     *
     */
    void MSIPackage::LoadRegistryEntries(util::XMLDocument versionsDoc,MSIIdRegistry& idRegistry)
    { COMPtr<IXMLDOMNodeList> pXMLDomNodeList;
      COMPtr<IXMLDOMNode>     pNode;

      versionsDoc.GetSelection(_T("//v:product/v:registryentries/v:registry"),pXMLDomNodeList);

      if( !pXMLDomNodeList.IsNULL() )
        for( HRESULT hr = pXMLDomNodeList->nextNode(&pNode);hr==S_OK;hr=pXMLDomNodeList->nextNode(&pNode) )
        { COVariant hive;

          if( versionsDoc.GetNodeValue(pNode,_T("./v:key/@hive"),hive) )
          { COVariant key;
            COVariant name;
            COVariant value;

            versionsDoc.GetNodeValue(pNode,_T("./v:key/text()"),key,true);
            versionsDoc.GetNodeValue(pNode,_T("./v:name/text()"),name,true);
            versionsDoc.GetNodeValue(pNode,_T("./v:value/text()"),value,true);

            MSIId    uniqueId;
            YAString regPath(V_BSTR(key));

            if( !name.IsEmpty() )
            { regPath.Append(_T("\\"));
              regPath.Append(V_BSTR(name));
            } // of if

            idRegistry.GetUniqueId(_T("registry"),regPath.c_str(),uniqueId);

            YAString registryId;
            registryId.Format(_T("R%08d"),uniqueId.id);

            if( m_lastRegistryentriesElement.IsNULL() )
            { m_doc.CreateElement(_T("registryentries"),m_lastRegistryentriesElement);
              m_doc.AppendChildToParent(m_lastRegistryentriesElement,m_rootElement,1);
              m_doc.AppendNewline(m_lastRegistryentriesElement,2);
            } // of if

            AddRegistryInfo(registryId.c_str(),uniqueId.guid,false,V_BSTR(hive),V_BSTR(key),!name.IsEmpty() ? V_BSTR(name) : NULL, V_BSTR(value));
          } // of if
        } // of for

      m_lastRegistryentriesElement.Release();
    } // of MSIPackage::LoadRegistryEntries()

    /**
     *
     */
    void MSIPackage::LoadProperties(util::XMLDocument versionsDoc)
    { COMPtr<IXMLDOMNodeList> pXMLDomNodeList;
      COMPtr<IXMLDOMNode>     pNode;
      COMPtr<IXMLDOMElement>  pProperties;

      versionsDoc.GetSelection(_T("//v:version[1]//v:msiproperties/v:msiproperty"),pXMLDomNodeList);

      if( !pXMLDomNodeList.IsNULL() )
        for( HRESULT hr = pXMLDomNodeList->nextNode(&pNode);hr==S_OK;hr=pXMLDomNodeList->nextNode(&pNode) )
        { COVariant key;

          if( versionsDoc.GetAttribute(pNode,_T("key"),key) )
          { COVariant value;

            versionsDoc.GetNodeValue(pNode,_T(".//text()"),value,true);

            if( pProperties.IsNULL() )
            { m_doc.CreateElement(_T("properties"),pProperties);
              m_doc.AppendChildToParent(pProperties,m_rootElement,1);
              m_doc.AppendNewline(pProperties,2);
            } // of if

            COMPtr<IXMLDOMElement> pProperty;
            m_doc.AppendElement(pProperty,pProperties,_T("property"),V_BSTR(value),2);
            m_doc.AddAttribute(pProperty,_T("key"),V_BSTR(key));
          } // of if
        } // of for
    } // of MSIPackage::LoadProperties()

    /**
     *
     */
    bool MSIPackage::AddFeatures(COMPtr<IXMLDOMElement>& featuresElement,LPCTSTR strippedFilePath,LPCTSTR fileName)
    { bool                        result = false;
      TString                     compid;
      STR_STR_Map::const_iterator it0    = m_filename2comp.find(fileName);

      if( it0!=m_filename2comp.end() )
        compid = it0->second;
      else
      { STR_STR_Map::const_iterator it1 = m_dirname2comp.find(strippedFilePath);

        if( it1!=m_dirname2comp.end() )
          compid = it1->second;
        else if( _tcschr(strippedFilePath,_T('\\'))!=NULL )
        { YAString fileDirName(strippedFilePath);
          
          for( STR_STR_Map::const_reverse_iterator it2=m_dirname2comp.rbegin();it2!=m_dirname2comp.rend();it2++ )
          { if( fileDirName.IndexOf(it2->first.c_str())==0 )
            { compid = it2->second;

              break;
            } // of if
          } // of for
        } // of else
      } // of else

      if( compid.length()>0 )
      { STR_TStringSet_Map::const_iterator it1    = m_comp2feature.find(compid);

        if( it1!=m_comp2feature.end() )
        { std::set<TString> features = it1->second;

          for( std::set<TString>::const_iterator it2=features.begin();it2!=features.end();it2++ )
          { if( featuresElement.IsNULL() )
              m_doc.CreateElement(_T("features"),featuresElement);

            m_doc.AppendElement(featuresElement,_T("feature"),it2->c_str(),3);
            
            result = true;
          } // of for
        } // of if
      } // of if

      return result;
    } // of MSIPackage::AddFeatures()

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
    void MSIPackage::AddRegistryInfo(LPCTSTR id,LPCTSTR guid,bool startSection, LPCTSTR mainKey, LPCTSTR key, LPCTSTR name, LPCTSTR value)
    { if( !m_lastRegistryentriesElement.IsNULL() )
      { COMPtr<IXMLDOMElement> registryElement,keyElement;

        m_doc.CreateElement(_T("registry"),registryElement);
        m_doc.AddAttribute(registryElement,_T("id"),id);
        m_doc.AddAttribute(registryElement,_T("guid"),guid);

        m_doc.AppendChildToParent(registryElement,m_lastRegistryentriesElement,3);

        m_doc.AppendElement(keyElement,registryElement,_T("key"),key,4);
        m_doc.AddAttribute(keyElement,_T("hive"),mainKey);

        if( NULL!=name )
          m_doc.AppendElement(registryElement,_T("name"),name,4);

        if( NULL!=value )
          m_doc.AppendElement(registryElement,_T("value"),value,4);

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
    bool MSIPackage::AddFileInfo(LPCTSTR id,LPCTSTR guid,long seqNo,LPCTSTR dirId,
                                 DWORD fileSize,LPCTSTR strippedFilePath,LPCTSTR fileName,LPCTSTR shortStrippedFileName,
                                 LPCTSTR fileVersion
                                )
    { bool result = false;

      COMPtr<IXMLDOMElement> featuresElement;
      if( AddFeatures(featuresElement,strippedFilePath,fileName) )
      { m_doc.CreateElement(_T("file"),m_lastFileElement);
        m_doc.AppendChildToParent(m_lastFileElement,m_filesElement,1);

        m_doc.AppendChildToParent(featuresElement,m_lastFileElement,2);

        m_doc.AddAttribute(m_lastFileElement,_T("id"),id);
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

        result = true;
      }
     
      return result;
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
