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
#ifndef COCONNECTIONPOINT_H
#define COCONNECTIONPOINT_H

#include "util/comptr.h"

#define CCONNMAX 10

namespace bvr20983
{
  namespace COM
  {
    class COConnectionPoint : public IConnectionPoint
    {
      public:
        COConnectionPoint(IUnknown*, REFGUID,REFIID);
        ~COConnectionPoint();
    
        //IUnknown members
        STDMETHODIMP         QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
    
        //IConnectionPoint members
        STDMETHODIMP GetConnectionInterface(IID *);
        STDMETHODIMP GetConnectionPointContainer(IConnectionPointContainer **);
        STDMETHODIMP Advise(LPUNKNOWN, DWORD *);
        STDMETHODIMP Unadvise(DWORD);
        STDMETHODIMP EnumConnections(IEnumConnections **);
    
      private:
        ULONG             m_cRef;       // Object reference count
        LPUNKNOWN         m_pObj;       // Containing object
    
        REFGUID           m_typelibGUID;
        IID               m_iid;        // Our relevant interface
        ITypeInfo*        m_pITypeInfo;

        GCOMPtr<IUnknown> m_gpUnknown[CCONNMAX];
    
        UINT              m_cConn;
    } ; // of class COConnectionPoint
  } // of namespace COM
} // of namespace bvr20983

#endif COCONNECTIONPOINT_H
/*==========================END-OF-FILE===================================*/
