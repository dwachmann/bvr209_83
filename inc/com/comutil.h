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
#ifndef COMUTIL_H
#define COMUTIL_H


//Macros for setting DISPPARAMS structures
#define SETDISPPARAMS(dp, numArgs, pvArgs, numNamed, pNamed) \
    {\
    (dp).cArgs=numArgs;\
    (dp).rgvarg=pvArgs;\
    (dp).cNamedArgs=numNamed;\
    (dp).rgdispidNamedArgs=pNamed;\
    }

#define SETNOPARAMS(dp) SETDISPPARAMS(dp, 0, NULL, 0, NULL)

//Macros for setting EXCEPINFO structures
#define SETEXCEPINFO(ei, excode, src, desc, file, ctx, func, scd) \
    {\
    (ei).wCode=excode;\
    (ei).wReserved=0;\
    (ei).bstrSource=src;\
    (ei).bstrDescription=desc;\
    (ei).bstrHelpFile=file;\
    (ei).dwHelpContext=ctx;\
    (ei).pvReserved=NULL;\
    (ei).pfnDeferredFillIn=func;\
    (ei).scode=scd;\
    }


#define INITEXCEPINFO(ei) SETEXCEPINFO(ei,0,NULL,NULL,NULL,0L,NULL,S_OK)

// An enumeration of persistent storage states for use in the
// implementation of IPersistStorage.
typedef enum
{
  PERS_UNINIT = 0,  // Uninitialized.
  PERS_SCRIBBLE,    // Write     permitted; Read     permitted.
  PERS_NOSCRIBBLE,  // Write NOT permitted; Read     permitted.
  PERS_HANDSOFF     // Write NOT permitted; Read NOT permitted.
} PERSTGSTATE;

#define CKHR(msg,hr) \
{\
  if( FAILED(hr) )\
  {\
    LOGGER_ERROR<<msg<<setHR<<CHResult(hr)<<endl; \
  }\
}


// for the types of sinks that the COControl class has.  you shouldn't ever
// need to use these
//
#define SINK_TYPE_EVENT      0
#define SINK_TYPE_PROPNOTIFY 1

// superclass window support.  you can pass this in to DoSuperClassPaint
//
#define DRAW_SENDERASEBACKGROUND        1

//=--------------------------------------------------------------------------=
// Various Hosts don't handle OLEIVERB_PROPERTIES correctly, so we can't use
// that as our Properties verb number.  Instead, we're going to define
// CTLIVERB_PROPERTIES as 1, and return that one in IOleObject::EnumVerbs,
// but we'll still handle OLEIVERB_PROPERTIES correctly in DoVerb.
//
#define CTLIVERB_PROPERTIES     1


//=--------------------------------------------------------------------------=
// QueryInterface Optimizations
//=--------------------------------------------------------------------------=
// for optimizing QI's
//
#define DO_GUIDS_MATCH(riid1, riid2) ((riid1.Data1 == riid2.Data1) && (riid1 == riid2))

// Data1_*
//
// the first dword of GUIDs for most of the interesting interfaces.  these are
// used by speed critical versions of QueryInterface
//
#define Data1_IProxyManager                0x00000008
#define Data1_IClientSecurity              0x0000013D
#define Data1_IAdviseSink                  0x0000010f
#define Data1_IAdviseSink2                 0x00000125
#define Data1_IAdviseSinkEx                0x3af24290
#define Data1_IActiveScript                0xBB1A2AE1
#define Data1_IBindCtx                     0x0000000e
#define Data1_ICDataDoc                    0xF413E4C0
#define Data1_IClassFactory                0x00000001
#define Data1_IClassFactory2               0xb196b28f
#define Data1_IConnectionPoint             0xb196b286
#define Data1_IConnectionPointContainer    0xb196b284
#define Data1_IControl_95                  0x9a4bbfb5
#define Data1_IControl                     0xa7fddba0
#define Data1_ICreateErrorInfo             0x22f03340
#define Data1_ICreateTypeInfo              0x00020405
#define Data1_ICreateTypeLib               0x00020406
#define Data1_IDataAdviseHolder            0x00000110
#define Data1_IDataFrame                   0x97F254E0
#define Data1_IDataFrameExpert             0x73687490
#define Data1_IDataObject                  0x0000010e
#define Data1_IDispatch                    0x00020400
#define Data1_IDispatchEx                  0xA6EF9860
#define Data1_IDropSource                  0x00000121
#define Data1_IDropTarget                  0x00000122
#define Data1_IEnumCallback                0x00000108
#define Data1_IEnumConnectionPoints        0xb196b285
#define Data1_IEnumConnections             0xb196b287
#define Data1_IEnumFORMATETC               0x00000103
#define Data1_IEnumGeneric                 0x00000106
#define Data1_IEnumHolder                  0x00000107
#define Data1_IEnumMoniker                 0x00000102
#define Data1_IEnumOLEVERB                 0x00000104
#define Data1_IEnumSTATDATA                0x00000105
#define Data1_IEnumSTATSTG                 0x0000000d
#define Data1_IEnumString                  0x00000101
#define Data1_IEnumOleUndoActions          0xb3e7c340
#define Data1_IEnumUnknown                 0x00000100
#define Data1_IEnumVARIANT                 0x00020404
#define Data1_IErrorInfo                   0x1cf2b120
#define Data1_IExternalConnection          0x00000019
#define Data1_IFont                        0xbef6e002
#define Data1_IFontDisp                    0xbef6e003
#define Data1_IFormExpert                  0x5aac7f70
#define Data1_IGangConnectWithDefault      0x6d5140c0
#define Data1_IHTMLInputTextElement        0x3050f2a6
#define Data1_IInternalMoniker             0x00000011
#define Data1_ILockBytes                   0x0000000a
#define Data1_IMalloc                      0x00000002
#define Data1_IMarshal                     0x00000003
#define Data1_IMessageFilter               0x00000016
#define Data1_IMoniker                     0x0000000f
#define Data1_IMsoCommandTarget            0xb722bccb
#define Data1_IMsoDocument                 0xb722bcc5
#define Data1_IMsoView                     0xb722bcc6
#define Data1_IObjectSafety                0xCB5BDC81
#define Data1_IOleInPlaceComponent         0x5efc7970
#define Data1_IOleAdviseHolder             0x00000111
#define Data1_IOleCache                    0x0000011e
#define Data1_IOleCache2                   0x00000128
#define Data1_IOleCacheControl             0x00000129
#define Data1_IOleClientSite               0x00000118
#define Data1_IOleCompoundUndoAction       0xa1faf330
#define Data1_IOleContainer                0x0000011b
#define Data1_IOleControl                  0xb196b288
#define Data1_IOleControlSite              0xb196b289
#define Data1_IOleInPlaceActiveObject      0x00000117
#define Data1_IOleInPlaceFrame             0x00000116
#define Data1_IOleInPlaceObject            0x00000113
#define Data1_IOleInPlaceObjectWindowless  0x1c2056cc
#define Data1_IOleInPlaceSite              0x00000119
#define Data1_IOleInPlaceSiteEx            0x9c2cad80
#define Data1_IOleInPlaceSiteWindowless    0x922eada0
#define Data1_IOleInPlaceUIWindow          0x00000115
#define Data1_IOleItemContainer            0x0000011c
#define Data1_IOleLink                     0x0000011d
#define Data1_IOleManager                  0x0000011f
#define Data1_IOleObject                   0x00000112
#define Data1_IOlePresObj                  0x00000120
#define Data1_IOlePropertyFrame            0xb83bb801
#define Data1_IOleStandardTool             0xd97877c4
#define Data1_IOleUndoAction               0x894ad3b0
#define Data1_IOleUndoActionManager        0xd001f200
#define Data1_IOleWindow                   0x00000114
#define Data1_IPSFactory                   0x00000009
#define Data1_IPSFactoryBuffer             0xd5f569d0
#define Data1_IParseDisplayName            0x0000011a
#define Data1_IPerPropertyBrowsing         0x376bd3aa
#define Data1_IPersist                     0x0000010c
#define Data1_IPersistFile                 0x0000010b
#define Data1_IPersistPropertyBag          0x37D84F60
#define Data1_IPersistPropertyBag2         0x22F55881
#define Data1_IPersistStorage              0x0000010a
#define Data1_IPersistStream               0x00000109
#define Data1_IPersistStreamInit           0x7fd52380
#define Data1_IPicture                     0x7bf80980
#define Data1_IPictureDisp                 0x7bf80981
#define Data1_IPointerInactive             0x55980ba0
#define Data1_IPropertyBag                 0x55272A00
#define Data1_IPropertyBag2                0x22F55882
#define Data1_IPropertyNotifySink          0x9bfbbc02
#define Data1_IPropertyPage                0xb196b28d
#define Data1_IPropertyPage2               0x01e44665
#define Data1_IPropertyPage3               0xb83bb803
#define Data1_IPropertyPageInPlace         0xb83bb802
#define Data1_IPropertyPageSite            0xb196b28c
#define Data1_IPropertyPageSite2           0xb83bb804
#define Data1_IProvideClassInfo            0xb196b283
#define Data1_IProvideDynamicClassInfo     0x6d5140d1
#define Data1_IQuickActivate               0xcf51ed10
#define Data1_IRequireClasses              0x6d5140d0
#define Data1_IRootStorage                 0x00000012
#define Data1_IRunnableObject              0x00000126
#define Data1_IRunningObjectTable          0x00000010
#define Data1_ISelectionContainer          0x6d5140c6
#define Data1_IServiceProvider             0x6d5140c1
#define Data1_ISimpleFrameSite             0x742b0e01
#define Data1_ISpecifyPropertyPages        0xb196b28b
#define Data1_IStdMarshalInfo              0x00000018
#define Data1_IStorage                     0x0000000b
#define Data1_IStream                      0x0000000c
#define Data1_ISupportErrorInfo            0xdf0b3d60
#define Data1_ITypeComp                    0x00020403
#define Data1_ITypeInfo                    0x00020401
#define Data1_ITypeLib                     0x00020402
#define Data1_IUnknown                     0x00000000
#define Data1_IViewObject                  0x0000010d
#define Data1_IViewObject2                 0x00000127
#define Data1_IViewObjectEx                0x3af24292
#define Data1_IWeakRef                     0x0000001a
#define Data1_ICategorizeProperties        0x4d07fc10


#define QI_INHERITS(pObj, itf)              \
    case Data1_##itf:                       \
      if( DO_GUIDS_MATCH(riid, IID_##itf) ) \
      {                                     \
        *ppv = (void *)(itf *)pObj;         \
      }                                     \
      break;

#endif //COMUTIL_H

#define CALLCOM_0(i,m)                 { if( NULL!=i ) i->m(); }
#define CALLCOMEX_0(i,m)               { if( NULL!=i ) THROW_COMEXCEPTION( i->m() ); }

#define CALLCOM_1(i,m,a1)              { if( NULL!=i ) i->m(a1); }
#define CALLCOMEX_1(i,m,a1)            { if( NULL!=i ) THROW_COMEXCEPTION( i->m(a1) ); }

#define CALLCOM_2(i,m,a1,a2)           { if( NULL!=i ) i->m(a1,a2); }
#define CALLCOMEX_2(i,m,a1,a2)         { if( NULL!=i ) THROW_COMEXCEPTION( i->m(a1,a2) ); }

#define CALLCOM_3(i,m,a1,a2,a3)        { if( NULL!=i ) i->m(a1,a2,a3); }
#define CALLCOMEX_3(i,m,a1,a2,a3)      { if( NULL!=i ) THROW_COMEXCEPTION( i->m(a1,a2,a3) ); }

#define CALLCOM_4(i,m,a1,a2,a3,a4)     { if( NULL!=i ) i->m(a1,a2,a3,a4); }
#define CALLCOMEX_4(i,m,a1,a2,a3,a4)   { if( NULL!=i ) THROW_COMEXCEPTION( i->m(a1,a2,a3,a4) ); }

#define CALLCOM_5(i,m,a1,a2,a3,a4,a5)     { if( NULL!=i ) i->m(a1,a2,a3,a4,a5); }
#define CALLCOMEX_5(i,m,a1,a2,a3,a4,a5)   { if( NULL!=i ) THROW_COMEXCEPTION( i->m(a1,a2,a3,a4,a5) ); }
