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

namespace MSXML2
{
#include <msxml2.h>
}

namespace bvr20983
{

  namespace util
  {
    class CXMLDocument
    {
      public:
        CXMLDocument();
        
        void Load(LPCTSTR fileName);
        
      private:
        COMPtr<IXMLDOMDocument> m_pXmlDoc;

    }; // of class CGUID
  } // of namespace util
} // of namespace bvr20983

#endif // XMLDOCUMENT_H
//=================================END-OF-FILE==============================