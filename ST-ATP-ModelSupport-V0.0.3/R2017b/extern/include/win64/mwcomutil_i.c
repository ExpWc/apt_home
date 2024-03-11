

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Mon Jul 24 21:49:27 2017
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


MIDL_DEFINE_GUID(IID, LIBID_MWComUtil,0xA9CFE9B5,0xB63C,0x4A37,0x9D,0x79,0x9A,0x24,0x79,0xBA,0x39,0xA5);


MIDL_DEFINE_GUID(CLSID, CLSID_MWField,0xCEABFF31,0x3809,0x4F5B,0xBD,0x23,0x54,0x71,0x53,0x18,0x74,0xDF);


MIDL_DEFINE_GUID(CLSID, CLSID_MWStruct,0xC04108B7,0xC68B,0x4993,0x8E,0x4F,0xE1,0xF7,0x47,0x3F,0xFE,0x2C);


MIDL_DEFINE_GUID(CLSID, CLSID_MWComplex,0xCEF60203,0xB747,0x4608,0x84,0x96,0xC5,0x04,0x1C,0xD5,0x82,0x16);


MIDL_DEFINE_GUID(CLSID, CLSID_MWSparse,0xF7A2CCCE,0x553E,0x42A1,0xA3,0x5E,0xF4,0x47,0x58,0x24,0xBE,0x1F);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArg,0xB26E68AE,0x0BB2,0x47C4,0xB5,0x6C,0xDF,0x12,0x78,0x06,0xDD,0xEB);


MIDL_DEFINE_GUID(CLSID, CLSID_MWArrayFormatFlags,0xB0BBDD3F,0x77BD,0x47B3,0xB2,0xB0,0x97,0xDC,0x67,0x57,0x32,0x2D);


MIDL_DEFINE_GUID(CLSID, CLSID_MWDataConversionFlags,0x42EA1843,0xEAB0,0x4343,0x8C,0xE7,0x24,0x6E,0x4F,0x46,0x40,0xD3);


MIDL_DEFINE_GUID(CLSID, CLSID_MWUtil,0x7A4F5C44,0x0612,0x4D92,0xB2,0xA0,0x82,0x25,0x83,0x1F,0x50,0xAF);


MIDL_DEFINE_GUID(CLSID, CLSID_MWFlags,0xC85224B1,0x75BE,0x44F1,0x82,0x6E,0x72,0x5E,0xCA,0x9B,0x75,0x80);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



