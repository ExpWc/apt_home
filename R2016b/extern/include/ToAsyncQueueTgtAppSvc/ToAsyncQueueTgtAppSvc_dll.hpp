/* Copyright 2013 The MathWorks, Inc. */

#ifndef ToAsyncQueueTgtAppSvc_dll_h
#define ToAsyncQueueTgtAppSvc_dll_h

#ifdef __cplusplus
  #define TOASYNCQUEUETGTAPPSVC_API_EXTERN_C extern "C"
#else
  #define TOASYNCQUEUETGTAPPSVC_API_EXTERN_C
#endif

#ifdef SL_INTERNAL

  #include "version.h"

  #if defined(BUILDING_LIBMWCODER_TOASYNCQUEUETGTAPPSVC)
      #define TOASYNCQUEUETGTAPPSVC_API DLL_EXPORT_SYM
      #define TOASYNCQUEUETGTAPPSVC_API_C TOASYNCQUEUETGTAPPSVC_API_EXTERN_C DLL_EXPORT_SYM
  #else
      #define TOASYNCQUEUETGTAPPSVC_API DLL_IMPORT_SYM
      #define TOASYNCQUEUETGTAPPSVC_API_C TOASYNCQUEUETGTAPPSVC_API_EXTERN_C DLL_IMPORT_SYM
  #endif

#else

  #define TOASYNCQUEUETGTAPPSVC_API
  #define TOASYNCQUEUETGTAPPSVC_API_C TOASYNCQUEUETGTAPPSVC_API_EXTERN_C

#endif

#endif