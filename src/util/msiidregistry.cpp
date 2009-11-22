/*
 * $Id: $
 * 
 * Class the administrates unique ids for msi packages.
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
#include "util/msiidregistry.h"
#include "util/fileinfo.h"

using namespace std;

namespace bvr20983 
{
  namespace util
  {
    /**
     *
     */
    MSIIdRegistry::MSIIdRegistry(LPCTSTR fileName) : m_fileName(fileName)
    { if( FileInfo(m_fileName).IsFile() )
        m_doc.Load(m_fileName);

      if( m_doc.IsEmpty() )
      { m_doc.CreateXmlSkeleton(_T("msipackage"),m_rootElement);

        m_doc.CreateElement(_T("files"),m_filesElement);
        m_doc.AppendChildToParent(m_filesElement,m_rootElement,1);
        m_doc.AppendNewline(m_filesElement,1);
      } // of if
      else
      { m_doc.GetFirstElement(_T("msipackage"),m_rootElement);
        m_doc.GetFirstElement(_T("files"),m_filesElement);

        COMPtr<IXMLDOMNodeList> pFileList;
        COMPtr<IXMLDOMNode>     pNode;

        m_doc.GetElements(_T("file"),pFileList);

        for( HRESULT hr = pFileList->nextNode(&pNode);hr==S_OK;hr = pFileList->nextNode(&pNode) )
        { DOMNodeType type;

          THROW_COMEXCEPTION( pNode->get_nodeType(&type) );

          if( type==NODE_ELEMENT )
          { COMPtr<IXMLDOMElement> e;
            COVariant              pathValue;
            COVariant              idValue;
            const VARIANT*         v = idValue;
            
            THROW_COMEXCEPTION( pNode->QueryInterface(IID_IXMLDOMElement,reinterpret_cast<void**>(&e)) );
            if( SUCCEEDED( e->getAttribute(_T("id"),const_cast<VARIANT*>(v)) ) )
            { long id = _ttol(V_BSTR(idValue));

              if( m_lastUniqueId<(unsigned int)id )
                m_lastUniqueId = id;

              if( m_doc.GetNodeValue(pNode,_T("./path/text()"),pathValue) )
              { BSTR path = V_BSTR(pathValue);

                m_ids.insert( STR_UINT_Pair(TString(path),(UINT)id) );
              } // of if
            } // of if
          } // of if
        } // of for

        m_doc.RemoveElements(_T("file"));
        m_doc.RemoveElements(_T("media"));
        m_doc.RemoveElements(_T("directories"));
      } // of else
    }

    /**
     *
     */
    MSIIdRegistry::~MSIIdRegistry()
    { }

    /**
     *
     */
    unsigned int MSIIdRegistry::GetUniqueId(LPCTSTR category,LPCTSTR path)
    { unsigned int result = 0;

      STR_UINT_Map::const_iterator i = m_ids.find(path);

      if( i!=m_ids.end() )
        result = i->second;
      else
      { result = ++m_lastUniqueId;

        m_ids.insert( STR_UINT_Pair(TString(path),result) );
      } // of else

      return result;
    } // of MSIIdRegistry::GetUniqueId()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
