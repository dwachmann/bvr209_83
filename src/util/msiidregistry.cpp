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
#include "com/covariant.h"

using namespace bvr20983::COM;

namespace bvr20983 
{
  namespace util
  {
    /**
     *
     */
    MSIIdRegistry::MSIIdRegistry(LPCTSTR fileName,LPCTSTR msiCompIdPattern) : 
      m_fileName(fileName),
      m_msiCompIdPattern(msiCompIdPattern),
      m_lastUniqueId(0)
    { if( FileInfo(m_fileName).IsFile() )
        m_doc.Load(m_fileName);

      if( m_doc.IsEmpty() )
      { m_doc.CreateXmlSkeleton(_T("msiguid"),m_rootElement);

        m_doc.CreateElement(_T("guids"),m_guidElements);
        m_doc.AppendChildToParent(m_guidElements,m_rootElement,1);
        m_doc.AppendNewline(m_guidElements,1);
      } // of if
      else
      { m_doc.GetFirstElement(_T("msiguid"),m_rootElement);
        m_doc.GetFirstElement(_T("guids"),m_guidElements);

        COMPtr<IXMLDOMNodeList> pGuidList;
        COMPtr<IXMLDOMNode>     pNode;

        m_doc.GetElements(_T("guid"),pGuidList);

        for( HRESULT hr = pGuidList->nextNode(&pNode);hr==S_OK;hr = pGuidList->nextNode(&pNode) )
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

                m_ids.insert( STR_DOMElement_Pair(TString(path),e) );
              } // of if
            } // of if

            e->setAttribute(_T("status"),COVariant(_T("deleted")));
          } // of if
        } // of for
      } // of else
    }

    /**
     *
     */
    MSIIdRegistry::~MSIIdRegistry()
    { m_doc.Save(m_fileName); }

    /**
     *
     */
    void MSIIdRegistry::GetUniqueId(LPCTSTR category,LPCTSTR path,MSIId& uniqueId)
    { COMPtr<IXMLDOMElement> guidElement;
      COMPtr<IXMLDOMElement> pathElement;
      COMPtr<IXMLDOMText>    pathText;
      COVariant              idValue;
      const VARIANT*         v = idValue;
      unsigned int           id = 0;

      STR_DOMElement_Map::const_iterator i = m_ids.find(path);

      if( i!=m_ids.end() )
      { guidElement = i->second;
        guidElement->setAttribute(_T("status"),COVariant(_T("used")));
      } // of if
      else
      { m_doc.CreateElement(_T("guid"),guidElement);
        m_doc.CreateElement(_T("path"),pathElement);

        m_doc.CreateTextNode(path,pathText);

        m_doc.AppendChildToParent(pathText,pathElement);
        m_doc.AppendChildToParent(pathElement,guidElement);
        m_doc.AppendChildToParent(guidElement,m_guidElements,2);

        guidElement->setAttribute(_T("id"),COVariant(YAString((unsigned long)(++m_lastUniqueId))));
        guidElement->setAttribute(_T("status"),COVariant(_T("new")));

        m_ids.insert( STR_DOMElement_Pair(TString(path),guidElement) );
      } // of else

      guidElement->getAttribute(_T("id"),const_cast<VARIANT*>(v));

      uniqueId.id   = _ttoi(V_BSTR(v));
      uniqueId.guid.Format(_T("%s%08X"),m_msiCompIdPattern,uniqueId.id+1024);
    } // of MSIIdRegistry::GetUniqueId()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
