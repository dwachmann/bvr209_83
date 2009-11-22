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
      m_msiCompIdPattern(msiCompIdPattern)
    { if( FileInfo(m_fileName).IsFile() )
        m_doc.Load(m_fileName);

      if( m_doc.IsEmpty() )
      { m_doc.CreateXmlSkeleton(_T("msiguid"),m_rootElement);

        m_doc.CreateElement(_T("guids"),m_guidElement);
        m_doc.AppendChildToParent(m_guidElement,m_rootElement,1);
        m_doc.AppendNewline(m_guidElement,1);
      } // of if
      else
      { m_doc.GetFirstElement(_T("msiguid"),m_rootElement);
        m_doc.GetFirstElement(_T("guid"),m_guidElement);

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
    { m_doc.Save(m_fileName);
    }

    /**
     *
     */
    void MSIIdRegistry::GetUniqueId(LPCTSTR category,LPCTSTR path,MSIId& uniqueId)
    { COMPtr<IXMLDOMElement> result;
      COVariant              idValue;
      const VARIANT*         v = idValue;
      unsigned int           id = 0;

      STR_DOMElement_Map::const_iterator i = m_ids.find(path);

      if( i!=m_ids.end() )
      { result = i->second;
        result->setAttribute(_T("status"),COVariant(_T("modified")));
      } // of if
      else
      { m_doc.CreateElement(_T("guid"),result);

        m_doc.AppendChildToParent(result,m_guidElement,1);
        m_doc.AppendNewline(result,1);

        result->setAttribute(_T("id"),COVariant(YAString((unsigned long)(++m_lastUniqueId))));
        result->setAttribute(_T("status"),COVariant(_T("new")));

        m_ids.insert( STR_DOMElement_Pair(TString(path),result) );
      } // of else

      result->getAttribute(_T("id"),const_cast<VARIANT*>(v));

      uniqueId.id   = _ttoi(V_BSTR(v));
      uniqueId.guid.Format(_T("%s%08X"),m_msiCompIdPattern,uniqueId.id);
    } // of MSIIdRegistry::GetUniqueId()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
