/*
 * $Id$
 * 
 * COM VARIANT and PROPVARIANT Helper.
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
#include "com/covariant.h"
#include "util/logstream.h"
#include "util/comlogstream.h"

using namespace bvr20983;

namespace bvr20983
{
  namespace COM
  {
    /**
     *
     */
    COVariant::COVariant()
    { ::VariantInit(&m_value); }

    /**
     *
     */
    COVariant::COVariant(const VARIANT& value)
    { ::VariantInit(&m_value);

      *this = value;
    }

    /**
     *
     */
    COVariant::COVariant(const COVariant& value)
    { ::VariantInit(&m_value);

      *this = value;
    }

    /**
     *
     */
    COVariant::COVariant(LPCTSTR value,UINT len)
    { ::VariantInit(&m_value);

      V_VT(&m_value) = VT_BSTR;

      TCHAR  n = _T('\0');
      LPTSTR v = NULL;
      
      if( NULL!=value && len>0 && len<=::_tcslen(value) )
      { v      = const_cast<LPTSTR>(value);
        n      = v[len];
        v[len] = _T('\0');
      } // of if
      
      V_BSTR(&m_value) = ::SysAllocString(value);

      if( NULL!=v )
        v[len] = n;
    }

    /**
     *
     */
    COVariant::COVariant(const TString& value)
    { ::VariantInit(&m_value);

      V_VT(&m_value)   = VT_BSTR;
      V_BSTR(&m_value) = ::SysAllocString(value.c_str());
    }

    /**
     *
     */
    COVariant::COVariant(bool value)
    { ::VariantInit(&m_value);

      V_VT(&m_value)   = VT_BOOL;
      V_BOOL(&m_value) = value ? VARIANT_TRUE : VARIANT_FALSE;
    }

    /**
     *
     */
    COVariant::~COVariant()
    { ::VariantClear(&m_value); }

    /**
     *
     */
    COVariant& COVariant::operator=(const VARIANT& value)
    { ::VariantClear(&m_value);

      ::VariantCopy(&m_value,const_cast<VARIANT*>(&value));

      return *this;
    } // of COVariant::operator=()

    /**
     *
     */
    COVariant& COVariant::operator=(const COVariant& value)
    { ::VariantClear(&m_value);

      ::VariantCopy(&m_value,const_cast<VARIANT*>(&value.m_value));

      return *this;
    } // of COVariant::operator=()

    /**
     *
     */
    HRESULT COVariant::ChangeType(VARTYPE vt,const COVariant& value)
    { const VARIANT* v = value;

      ::VariantClear(&m_value);

      return ::VariantChangeType(&m_value, const_cast<VARIANT*>(v), 0, vt);
    } // of COVariant::ChangeType()

    /**
     *
     */
    HRESULT COVariant::ChangeType(VARTYPE vt)
    { return ::VariantChangeType(&m_value,&m_value, 0, vt);
    } // of COVariant::ChangeType()

    /**
     *
     */
    COPropVariant::COPropVariant()
    { ::PropVariantInit(&m_value); }

    /**
     *
     */
    COPropVariant::COPropVariant(const PROPVARIANT& value)
    { ::PropVariantInit(&m_value);

      *this = value;
    }

    /**
     *
     */
    COPropVariant::COPropVariant(const COPropVariant& value)
    { ::PropVariantInit(&m_value);

      *this = value;
    }

    /**
     *
     */
    COPropVariant::~COPropVariant()
    { ::PropVariantClear(&m_value); }

    /**
     *
     */
    COPropVariant& COPropVariant::operator=(const PROPVARIANT& value)
    { ::PropVariantClear(&m_value);
      ::PropVariantCopy(&m_value,&value);

      return *this;
    } // of COPropVariant::operator=()

    /**
     *
     */
    COPropVariant& COPropVariant::operator=(const COPropVariant& value)
    { ::PropVariantClear(&m_value);
      ::PropVariantCopy(&m_value,&value.m_value);

      return *this;
    } // of COPropVariant::operator=()

    /**
     *
     */
    COPropVariant& COPropVariant::operator=(const VARIANT& value)
    { ::PropVariantClear(&m_value);

      m_value.vt = value.vt & VT_TYPEMASK;
      switch( m_value.vt )
      { 
      case VT_EMPTY:
      case VT_NULL:
        break;
      case VT_I1:
        m_value.cVal = value.cVal;
        break;
      case VT_UI1:
        m_value.bVal = value.bVal;
        break;
      case VT_I2:
        m_value.iVal = value.iVal;
        break;
      case VT_UI2:
        m_value.uiVal = value.uiVal;
        break;
      case VT_I4:
        m_value.lVal = value.lVal;
        break;
      case VT_UI4:
        m_value.ulVal = value.ulVal;
        break;
      case VT_INT:
        m_value.intVal = value.intVal;
        break;
      case VT_UINT:
        m_value.uintVal = value.uintVal;
        break;
      case VT_R4:
        m_value.fltVal = value.fltVal;
        break;
      case VT_R8:
        m_value.dblVal = value.dblVal;
        break;
      case VT_CY:
        m_value.cyVal = value.cyVal;
        break;
      case VT_DATE:
        m_value.date = value.date;
        break;
      case VT_BSTR:
        m_value.bstrVal = ::SysAllocString(value.bstrVal);
        break;
      case VT_DISPATCH:
        m_value.pdispVal = value.pdispVal;
        ADDREF_INTERFACE(m_value.pdispVal);
        break;
      case VT_ERROR:
        m_value.scode = value.scode;
        break;
      case VT_BOOL:
        m_value.boolVal = value.boolVal;
        break;
      case VT_VARIANT:
        break;
      case VT_UNKNOWN:
        m_value.punkVal = value.punkVal;
        ADDREF_INTERFACE(m_value.punkVal);
        break;
      case VT_DECIMAL:
        m_value.pdecVal = value.pdecVal;
        break;
      case VT_RECORD:
        break;
      default:
        break;
      } // of switch

      return *this;
    } // of COPropVariant::operator=()

    /**
     *
     */
    COPropVariant& COPropVariant::operator=(const COVariant& value)
    { const VARIANT& v = value;

      *this = v;

      return *this;
    } // of COPropVariant::operator=()

    /**
     *
     */
    bool COPropVariant::GetBool(bool& b) const
    { bool result = false;

      if( m_value.vt==VT_BOOL || m_value.vt==VT_BSTR )
      { result = true;

        b = (m_value.vt==VT_BOOL && m_value.boolVal==VARIANT_TRUE)
             ||
            (m_value.vt==VT_BSTR && _tcsicmp(_T("true"),m_value.bstrVal)==0)
            ;
      } // of if

      return result; 
    }

    /**
     *
     */
    bool COPropVariant::GetString(TString s) const
    { bool result = false;

      if( m_value.vt==VT_BSTR )
      { s = m_value.bstrVal;
       
        result = true;
      }

      return result;
    }

    /**
     *
     */
    bool COPropVariant::GetString(BSTR* s) const
    { bool result = false;

      if( m_value.vt==VT_BSTR && NULL!=s )
      { *s = ::SysAllocString(m_value.bstrVal);
       
        result = true;
      }

      return result;
    }

    /**
     *
     */
    bool COPropVariant::GetULong(ULONG& v) const
    { bool result = false;

      if( m_value.vt==VT_UI4 )
      { v = m_value.ulVal;
       
        result = true;
      }

      return result;
    }
  } // of namespace COM
} // of namespace bvr20983
/*==========================END-OF-FILE===================================*/
