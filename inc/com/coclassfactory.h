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
#if !defined(COCLASSFACTORY_H)
#define COCLASSFACTORY_H

namespace bvr20983
{
  namespace COM
  {
    class COMServer;

    typedef HRESULT (*FNCOCREATE)(REFCLSID clsid,LPUNKNOWN* ppv,LPUNKNOWN pUnkOuter) ;
    
    class COClassFactory : public IClassFactory
    {
      public:
        // Interface Implementation Constructor & Destructor.
        COClassFactory(REFCLSID clsid,FNCOCREATE fnCoCreate);
    
        // IUnknown methods.
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
    
        // IClassFactory methods.
        STDMETHODIMP         CreateInstance(IUnknown*, REFIID, PPVOID);
        STDMETHODIMP         LockServer(BOOL);
    
      private:
        ULONG                m_cRefs;       // Interface Ref Count (for debugging).
        REFCLSID             m_clsid;
        FNCOCREATE           m_fnCoCreate;
    };
  } // of namespace COM
} // of namespace bvr20983

#endif // COCLASSFACTORY_H
