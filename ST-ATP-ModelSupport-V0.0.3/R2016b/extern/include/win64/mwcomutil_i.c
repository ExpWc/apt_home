

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Fri Jul 22 14:18:24 2016
 */
/* Compiler settings for win64\mwcomutil.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=IA64 8.00.0603 
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
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IMWUtil,0xC47EA90E,0x56D1,0x11d5,0xB1,0x59,0x00,0xD0,0xB7,0xBA,0x75,0x44);


MIDL_DEFINE_GUID(IID, LIBID_MWComUtil,0xD84427FA,0x13B9,0x4C9D,0xA4,0xC2,0x23,0x32,0x18,0x61,0xEF,0x6C);


MIDL_DEFINE_GUID(CLSID, CLSID_MWField,0x914AB0CF,0x5A0C,0x4116,0x90,0x7E,0x3D,0x59,0xC2,0xBD,0x4E,0xE7);


MIDL_DEFINE_GUID(CLSID, CLSID_MWStruct,0x572357A7,0x1742,0x4713,0xB4,0xA8,0x39,0x40,0x6C,0x50,0x98,0x46);


MIDL_DEFINE_GUID(CLSID, CLSID_MWComplex,0x4EDDFC31,0x8F87,0x4633,0xB8,0x56,0x39,0x8B,0x33,0x5E,0x28,0x30);


MIDL_DEFINE_GUID(CLSID, CLSID_MWSparse,0xFE151796,0xC2AD,0x4227,0xAB,0x4F,0x2C,0x0B,0x52,0xEF,0x9D,0xE5);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArg,0xE5B82F8B,0xBC84,0x4DF4,0x9E,0xF3,0x6B,0x20,0x73,0x6F,0xE3,0xA6);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArrayFormatFlags,0xE6F597E5,0x326D,0x4297,0x89,0x85,0xD0,0xA5,0xE6,0xB8,0x83,0xE1);


MIDL_DEFINE_GUID(CLSID, CLSID_MWDataConversionFlags,0x93B0F9E8,0x2BC3,0x4C2E,0x90,0xED,0x5E,0x74,0x05,0xA8,0x4D,0x78);


MIDL_DEFINE_GUID(CLSID, CLSID_MWUtil,0x46A76E33,0x5C37,0x4DB4,0xBA,0x11,0xBA,0x20,0x8D,0x13,0xB5,0x49);


MIDL_DEFINE_GUID(CLSID, CLSID_MWFlags,0xDF5F8A59,0x4FDE,0x44F5,0x95,0x08,0xB0,0xBB,0x32,0x84,0x36,0x76);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



