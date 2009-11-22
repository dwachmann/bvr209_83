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
#if !defined(MSIIDREGISTRY_H)
#define MSIIDREGISTRY_H

#include "util/xmldocument.h"

namespace bvr20983
{
  typedef std::map<TString,COMPtr<IXMLDOMElement>>  STR_DOMElement_Map;
  typedef std::pair<TString,COMPtr<IXMLDOMElement>> STR_DOMElement_Pair;

  namespace util
  {
    /**
     *
     */
    class MSIIdRegistry
    {
      public:
        MSIIdRegistry(LPCTSTR fileName);
        ~MSIIdRegistry();

        unsigned int GetUniqueId(LPCTSTR category,LPCTSTR path);

      private:
        LPCTSTR                m_fileName;
        STR_DOMElement_Map     m_ids;
        unsigned int           m_lastUniqueId;

        XMLDocument            m_doc;
        COMPtr<IXMLDOMElement> m_rootElement;
        COMPtr<IXMLDOMElement> m_guidElement;
    }; // of class MSIIdRegistry

  } // of namespace util
} // of namespace bvr20983
#endif // MSIIDREGISTRY_H
/*==========================END-OF-FILE===================================*/
