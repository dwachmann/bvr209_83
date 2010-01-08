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
#include "util/guid.h"
#include "util/comptr.h"

#if !defined(COVARIANT_H)
#define COVARIANT_H

namespace bvr20983
{
  namespace COM
  {
    /**
     *
     */
    class COVariant
    {
      public:
        COVariant();
        COVariant(const VARIANT& value);
        COVariant(const COVariant& value);
        COVariant(LPCTSTR value,UINT len=0);
        COVariant(const TString& value);
        COVariant(bool value);
        ~COVariant();

        COVariant& operator=(const VARIANT& value);
        COVariant& operator=(const COVariant& value);

        VARTYPE GetType() const
        { return m_value.vt; }

        operator const VARIANT&() const
        { return m_value; }

        operator const VARIANT*() const
        { return &m_value; }

        const VARIANT* operator->() const
        { return &m_value; }

        HRESULT ChangeType(VARTYPE vt,const COVariant& v);
        HRESULT ChangeType(VARTYPE vt);

        boolean IsEmpty() const
        { return m_value.vt==VT_EMPTY; }

        boolean IsNULL() const
        { return m_value.vt==VT_NULL; }

        boolean IsSet() const
        { return m_value.vt!=VT_NULL && m_value.vt!=VT_EMPTY; }

//        VARIANT* operator&() 
//        { return &m_value; }

      private:
        VARIANT m_value;

    }; // of class COVariant

    /**
     *
     */
    class COPropVariant
    {
      public:
        COPropVariant();
        COPropVariant(const PROPVARIANT& value);
        COPropVariant(const COPropVariant& value);
        ~COPropVariant();

        COPropVariant& operator=(const PROPVARIANT& value);
        COPropVariant& operator=(const COPropVariant& value);

        COPropVariant& operator=(const VARIANT& value);
        COPropVariant& operator=(const COVariant& value);

        operator const PROPVARIANT&() const
        { return m_value; }

        operator const PROPVARIANT*() const
        { return &m_value; }

//        PROPVARIANT* operator&() 
//        { return &m_value; }

//        PROPVARIANT& GetValue()
//        { return m_value; }

        const PROPVARIANT& GetConstValue() const
        { return m_value; }

        bool GetBool(bool& b) const;
        bool GetString(TString s) const;
        bool GetString(BSTR* s) const;
        bool GetULong(ULONG& v) const;

      private:
        PROPVARIANT m_value;

    }; // of class COPropVariant
  } // of namespace COM
} // of namespace bvr20983
#endif // COVARIANT_H


