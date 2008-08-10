/*
 * $Id$
 * 
 * Wrapper class for GUID.
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
#include "util/xmldocument.h"
#include "util/comptr.h"
#include "util/comstring.h"
#include "util/logstream.h"
#include "util/comlogstream.h"
#include "exception/comexception.h"

using namespace bvr20983::COM;

namespace bvr20983
{

  namespace util
  {
    
    const CLSID CLSID_Msxml2_DOMDocument_5_0 = 
    {0x88D969E5,0xF192,0x11D4,{0xA6,0x5F,0x00,0x40,0x96,0x32,0x51,0xE5}};
    
    const IID IID_IXMLDOMDocument = 
    {0x2933BF81,0x7B36,0x11D2,{0xB2,0x0E,0x00,0xC0,0x4F,0x98,0x3E,0x60}};
    
    /**
     *
     */
    XMLDocument::XMLDocument(LPCOLESTR domDocProgId) :
      m_pXmlDoc(domDocProgId,_T("IXMLDOMDocument2"))
    { 
      THROW_COMEXCEPTION( m_pXmlDoc->put_async(VARIANT_FALSE) );
      THROW_COMEXCEPTION( m_pXmlDoc->put_validateOnParse(VARIANT_TRUE) );
      THROW_COMEXCEPTION( m_pXmlDoc->put_resolveExternals(VARIANT_TRUE) );
      
      THROW_COMEXCEPTION( m_pXmlDoc->setProperty(_T("SelectionLanguage"),COVariant(_T("XPath"))) );
      THROW_COMEXCEPTION( m_pXmlDoc->setProperty(_T("SelectionNamespaces"),COVariant( _T("xmlns:v='http://bvr20983.berlios.de'"))) );
    }
    
    /**
     *
     */
    boolean XMLDocument::Load(LPCTSTR fileName)
    { boolean                   result = false;
      VARIANT_BOOL              status;
      COMPtr<IXMLDOMParseError> pXMLErr;
      COMString                 bstr;
      COVariant                 var(fileName);
      
      THROW_COMEXCEPTION( m_pXmlDoc->load(var,&status) );
      
      if( status!=VARIANT_TRUE ) 
      {
        THROW_COMEXCEPTION( m_pXmlDoc->get_parseError(&pXMLErr) );
        THROW_COMEXCEPTION( pXMLErr->get_reason(&bstr) );
        
        LOGGER_ERROR<<_T("Failed to load DOM from file ")<<fileName<<_T(". ")<<bstr<<endl;
      } // of if
      else
        result = true;

/*
      else
      { 
        THROW_COMEXCEPTION(m_pXmlDoc->get_xml(&bstr));

        LOGGER_INFO<<_T("Parsed XML-Document:")<<endl;
        LOGGER_INFO<<bstr<<endl;
      } 
*/
      return result;
    } // of XMLDocument::Load()
    
    /**
     *
     */
    void XMLDocument::DumpSelection(LPCTSTR xpathExpression)
    { if( !m_pXmlDoc.IsNULL() )
      { COMPtr<IXMLDOMElement>  pXMLDocElement;
        COMPtr<IXMLDOMNodeList> pXMLDomNodeList;
        
        THROW_COMEXCEPTION( m_pXmlDoc->get_documentElement(&pXMLDocElement) );
        THROW_COMEXCEPTION( pXMLDocElement->selectNodes(const_cast<LPTSTR>(xpathExpression),&pXMLDomNodeList) );
        
        COMPtr<IXMLDOMNode> node;

        for( HRESULT hr = pXMLDomNodeList->nextNode(&node);hr==S_OK;hr = pXMLDomNodeList->nextNode(&node) )
        { COMString nodeName;
          COVariant nodeValue;
          const VARIANT*  v = nodeValue;
        
          THROW_COMEXCEPTION( node->get_nodeName(&nodeName) );
          THROW_COMEXCEPTION( node->get_nodeValue(const_cast<VARIANT*>(v)) );
          
          LOGGER_INFO<<nodeName<<_T(":")<<nodeValue<<endl;
        } // of for
      } // of if
    } // of XMLDocument::DumpSelection()

    /**
     *
     */
    boolean XMLDocument::GetNodeValue(LPCTSTR xpath,COVariant& value,boolean evalProperty)
    { boolean result = false;
    
      if( !m_pXmlDoc.IsNULL() )
      { COMPtr<IXMLDOMElement>  pXMLDocElement;
        
        THROW_COMEXCEPTION( m_pXmlDoc->get_documentElement(&pXMLDocElement) );
        
        COMPtr<IXMLDOMNode> node(pXMLDocElement);
        
        result = GetNodeValue(node,xpath,value,evalProperty);
      } // of if
      
      return result;
    } // of XMLDocument::GetNodeValue()
    
    /**
     *
     */
    boolean XMLDocument::GetNodeValue(COMPtr<IXMLDOMNode>& node,LPCTSTR xpath,COVariant& value,boolean evalProperty)
    { boolean            result = false;
      COMPtr<IXMLDOMNode> selectedNode;
    
      if( !node.IsNULL() )
      { HRESULT hr = node->selectSingleNode(const_cast<LPTSTR>(xpath),&selectedNode);
        THROW_COMEXCEPTION( hr );
        
        if( hr==S_OK )
        { const VARIANT* v = value;
          
          THROW_COMEXCEPTION( selectedNode->get_nodeValue(const_cast<VARIANT*>(v)) );
          
          if( evalProperty )
            GetProperty(selectedNode,value);
  
          result = true;
        } // of if
      } // of if
      
      return result;
    } // of XMLDocument::GetNodeValue()

    /**
     *
     */
    boolean XMLDocument::GetProperty(COMPtr<IXMLDOMNode>& node,COM::COVariant& value)
    { boolean result = false;
    
      if( value.GetType()==VT_BSTR )
      { LPTSTR  v0       = V_BSTR(value);
        LPTSTR  v        = V_BSTR(value);
        UINT    vLen     = ::SysStringLen(v);
        BSTR    vEnd     = v+vLen;
        boolean evalExpr = false;
        UINT    varDepth = 0;
        LPTSTR  begin    = NULL;
        LPTSTR  end      = NULL;
        TString prop;
        
        do
        { LPTSTR propStart = _tcsstr(v,_T("${"));
          LPTSTR exprStart = _tcsstr(v,_T("#{"));
          LPTSTR beginExpr = NULL!=propStart && NULL!=exprStart            ? 
                            (propStart<exprStart ? propStart : exprStart) : 
                            (propStart!=NULL     ? propStart : exprStart);
          LPTSTR endExpr   = _tcschr(v,_T('}'));
          
          if( NULL!=beginExpr && NULL!=endExpr && beginExpr<endExpr )
          { varDepth++;
          
            if( NULL==begin )
              begin = beginExpr;
            
            if( !evalExpr && 
                ((NULL==propStart && NULL!=exprStart) || 
                 (NULL!=propStart && NULL!=exprStart && exprStart<propStart)
                ) 
              )
              evalExpr = NULL!=exprStart;
                                
            if( v+2<vEnd )
              v = beginExpr+2;
          } // of if          
          else
          { end = _tcschr(v,_T('}'));

            if( NULL!=end )
            { v = end + 1;
              varDepth--;
            } // of if
          } // of else
            
          if( varDepth<=0 )
          { if( NULL!=begin && NULL!=end && end>begin )
            { COVariant v1(begin+2,end-begin-2);
              COVariant v2;
              LPCTSTR   xpath = V_BSTR(v1);

              if( begin>v0 )
                prop += TString(v0,begin-v0);

              if( evalExpr )
              { 
                //
                // HACK: in the vbs version the vbs eval routine is called
                // here we only implement the two actual functions that are used in versions.xml
                //
                if( _tcsstr(xpath,_T("LCase(\""))==xpath )
                { TString param(xpath+7,_tcslen(xpath)-2-7);
                  COVariant v4(param);
                  
                  GetProperty(node,v4);

                  LPTSTR s = const_cast<LPTSTR>(V_BSTR(v4));

                  for( ;*s!=_T('\0');s++ )
                    *s = toupper(*s);

                  prop += V_BSTR(v4);
                } // of if
                else if( _tcsstr(xpath,_T("zerofill(\""))==xpath )
                { TString params( xpath+10,_tcslen(xpath)-1-10);

                  LPCTSTR comma = _tcschr(params.c_str(),_T(','));

                  if( NULL!=comma )
                  { TString param1(params.c_str(),comma-1-params.c_str());
                    COVariant v4(param1);

                    GetProperty(node,v4);

                    TString param2(comma+1);

                    int c = _ttoi(param2.c_str()) - _tcslen(V_BSTR(v4));

                    TString result(V_BSTR(v4));

                    for( int i=0;i<c;i++ )
                      result = _T("0") + result;

                    prop += result.c_str();
                  } // of if
                  else
                    prop += params;
                } // of else if

                evalExpr = false;
              } // of if
              else
              { 
                if( !m_properties.empty() && m_properties.find(xpath)!=m_properties.end() )
                  xpath = m_properties.find(xpath)->second;
                
                if( NULL==_tcschr(xpath,_T('/')) && NULL==_tcschr(xpath,_T(':')) && 
                    NULL==_tcschr(xpath,_T('@')) && NULL==_tcschr(xpath,_T('\'')) &&
                    NULL==_tcschr(xpath,_T('[')) && NULL==_tcschr(xpath,_T(']'))
                  )
                { COVariant v3;
                  TString parameterPath = _T("/v:versions/v:parameter[@name='");
                  parameterPath += xpath;
                  parameterPath += _T("']/text()");

                  if( GetNodeValue(parameterPath.c_str(),v3,false) && v3.GetType()==VT_BSTR )
                  { GetProperty(node,v3);

                    xpath = V_BSTR(v3);
                  } // of if
                } // of if

                boolean getNodeValueResult = false;
                try
                { getNodeValueResult = GetNodeValue(node,xpath,v2,true); }
                catch(COMException&)
                { }

                if( getNodeValueResult && v2.GetType()==VT_BSTR )
                  prop += V_BSTR(v2);
                else
                  prop += xpath;
              } // of else

              v0 = v = end + 1;
              begin = end = NULL;
            } // of if
            else
            { prop += v0;
              
              v0 = v = vEnd;
              begin = end = NULL;
            } // of else
          } // of if
        } while( v<vEnd );
        
        value = prop.c_str();
        
        result = true;
      } // of if
      
      return result;
    } // of XMLDocument::GetProperty()
  } // of namespace util
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
