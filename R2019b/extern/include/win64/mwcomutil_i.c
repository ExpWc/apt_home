

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for win64\mwcomutil.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=IA64 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IMWUtil,0xC47EA90E,0x56D1,0x11d5,0xB1,0x59,0x00,0xD0,0xB7,0xBA,0x75,0x44);


MIDL_DEFINE_GUID(IID, LIBID_MWComUtil,0xA74B3821,0xF27F,0x463B,0x9B,0x73,0xEF,0x83,0xE5,0xCB,0x1A,0xC5);


MIDL_DEFINE_GUID(CLSID, CLSID_MWField,0x3368D7A9,0x077E,0x454E,0x9F,0xA7,0xFB,0x54,0x34,0xF2,0xEF,0xD9);


MIDL_DEFINE_GUID(CLSID, CLSID_MWStruct,0xF15808C6,0x30DD,0x4577,0xBE,0xDC,0xD9,0xF1,0x24,0xBF,0xDF,0x17);


MIDL_DEFINE_GUID(CLSID, CLSID_MWComplex,0xB6434466,0x3D36,0x4D1E,0xB0,0xD2,0x16,0x46,0x7D,0xE0,0x79,0xEE);


MIDL_DEFINE_GUID(CLSID, CLSID_MWSparse,0x81A021B6,0xB2DF,0x4E01,0xA8,0x2B,0x71,0x45,0x97,0x28,0x08,0x91);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArg,0x676D0AA7,0x2C00,0x4850,0x99,0xDA,0xC1,0x66,0x76,0xCB,0x64,0x3F);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArrayFormatFlags,0xDFC7B53A,0x4730,0x40D3,0x99,0x14,0x93,0xAF,0xD0,0x0B,0xE0,0x8D);


MIDL_DEFINE_GUID(CLSID, CLSID_MWDataConversionFlags,0xA5A6DE68,0x7325,0x4EE3,0x80,0x79,0x31,0x4E,0x11,0x6C,0x17,0x2F);


MIDL_DEFINE_GUID(CLSID, CLSID_MWUtil,0x37EA3D04,0x8C5F,0x4EAF,0x97,0x77,0x39,0x6C,0xBE,0xBF,0xE3,0x62);


MIDL_DEFINE_GUID(CLSID, CLSID_MWFlags,0x425F6133,0x400C,0x45EF,0x82,0x39,0x9D,0x3C,0xF8,0xEE,0x82,0x63);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



