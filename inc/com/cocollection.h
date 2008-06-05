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
#if !defined(COCOLLECTION_H)
#define COCOLLECTION_H

#include "com/collection.h"
#include "com/counknown.h"

#define DISPID_LISTITEM     0
#define DISPID_LISTCOUNT    (-531)

namespace bvr20983
{
  namespace COM
  {
    class COMServer;

    //
    // handmade dual interface for ICollection
    // without dependency to typelibrary
    //
    struct ICollectionInternal : public IDispatch
    {
      public:
        virtual HRESULT STDMETHODCALLTYPE get_Item(long Index,VARIANT *retval) = 0;
        virtual HRESULT STDMETHODCALLTYPE get_Count(long *retval) = 0;
        virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **retval) = 0;
    };

    class CoCollection : public COUnknown,public ICollectionInternal
    {
      public:
        CoCollection(IUnknown* pUnkOuter=NULL);
    
        DECLARE_UNKNOWN
    
        // IDispatch members
        STDMETHODIMP GetTypeInfoCount(UINT *);
        STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo **);
        STDMETHODIMP GetIDsOfNames(REFIID, OLECHAR **, UINT, LCID, DISPID *);
        STDMETHODIMP Invoke(DISPID, REFIID, LCID, WORD, DISPPARAMS *, VARIANT *, EXCEPINFO *, UINT *);

        // ICollectionInternal members
        STDMETHODIMP get_Item(long Index,VARIANT *retval);
        STDMETHODIMP get_Count(long *retval);
        STDMETHODIMP get__NewEnum(IUnknown **retval);

        virtual HRESULT InternalQueryInterface(REFIID riid,PPVOID ppv);

        bool Add(IDispatch* pUnk);
        bool Add(LPCTSTR pStr);

      private:
        Collection  m_collection;

        static LPCTSTR dispNames[];
        static DISPID  dispIds[];
    }; // of class CoCollection
  } // of namespace COM
} // of namespace bvr20983
#endif // COCOLLECTION_H
