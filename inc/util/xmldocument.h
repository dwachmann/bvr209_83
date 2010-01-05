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
#if !defined(XMLDOCUMENT_H)
#define XMLDOCUMENT_H

#include "os.h"
#include "util/comptr.h"
#include "com/covariant.h"

namespace MSXML2
{
#include <msxml2.h>
}

namespace bvr20983
{

  namespace util
  {
    class XMLDocument
    {
      public:
        typedef std::pair<LPCTSTR, LPCTSTR>        PropertyP;
        typedef std::map<LPCTSTR, LPCTSTR,strless> PropertyM;

        XMLDocument(LPCOLESTR domDocProgId=_T("Msxml2.DOMDocument.6.0"));
        
        boolean Load(LPCTSTR fileName);
        void    Save(LPCTSTR fileName);
        void    DumpSelection(LPCTSTR xpathExpression);
        void    GetSelection(LPCTSTR xpathExpression,COMPtr<IXMLDOMNodeList>& pXMLDomNodeList);
        void    GetElements(LPCTSTR tagName,COMPtr<IXMLDOMNodeList>& pXMLDomNodeList);
        void    RemoveElements(LPCTSTR tagName);
        void    GetFirstElement(LPCTSTR tagName,COMPtr<IXMLDOMElement> &ppElement);
        void    SetProperties(const PropertyM& prop)
        { m_properties = prop; }
        
        boolean GetNodeValue(LPCTSTR xpath,COM::COVariant& value,boolean evalProperty=false);
        boolean GetNodeValue(COMPtr<IXMLDOMNode>& node,LPCTSTR xpath,COM::COVariant& value,boolean evalProperty=false);

        boolean GetProperty(COMPtr<IXMLDOMNode>& node,COM::COVariant& value);
        
        void    CreateXmlSkeleton(LPCTSTR rootElementName,COMPtr<IXMLDOMElement> &ppRoot);
        void    CreateElement(LPCTSTR elementName,COMPtr<IXMLDOMElement> &ppElement);
        void    CreateProcessingInstruction(LPCTSTR target,LPCTSTR data,COMPtr<IXMLDOMProcessingInstruction>& ppPI);
        void    CreateTextNode(LPCTSTR text,COMPtr<IXMLDOMText>& ppText);
        void    AppendChild(IXMLDOMNode* pChild,int appendNewline=-1);
        void    AppendChildToParent(IXMLDOMNode* pChild,IXMLDOMElement* pRoot,int appendNewline=-1);
        void    AppendNewline(IXMLDOMElement* pRoot,int appendNewline);

        void    AddAttribute(IXMLDOMElement* pElement,LPCTSTR attribName,LPCTSTR attribValue);

        void    AppendElement(IXMLDOMElement* pParent,LPCTSTR elementName,LPCTSTR textValue,int appendNewline=-1);
        void    AppendElement(COMPtr<IXMLDOMElement>& e,IXMLDOMElement* pParent,LPCTSTR elementName,LPCTSTR textValue,int appendNewline=-1);

        boolean IsEmpty() const;
        
      private:
        COMPtr<MSXML2::IXMLDOMDocument2> m_pXmlDoc;
        PropertyM                        m_properties;

    }; // of class CGUID
  } // of namespace util
} // of namespace bvr20983

#endif // XMLDOCUMENT_H
//=================================END-OF-FILE==============================