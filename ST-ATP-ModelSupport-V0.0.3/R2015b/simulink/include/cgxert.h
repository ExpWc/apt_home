/*
 * PUBLISHED header for cgxert, the runtime library for CGXE
 *
 * Copyright 2014-2015 The MathWorks, Inc.
 *
 */

#ifndef cgxert_h
#define cgxert_h

#if defined(_MSC_VER)
# pragma once
#endif
#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 3))
# pragma once
#endif

/*
 * Only define EXTERN_C if it hasn't been defined already. This allows
 * individual modules to have more control over managing their exports.
 */
#ifndef EXTERN_C

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C extern
#endif

#endif

#ifndef LIBCGXERT_API
#define LIBCGXERT_API
#endif

#define CGXE_SIM_RUNNING 0
#define CGXE_SIM_STOPPED 1
#define CGXE_SIM_PAUSED 2

#include "emlrt.h"
#include "covrt.h"

typedef struct SimStruct_tag SimStruct;

/*
 *  MATLAB INTERNAL USE ONLY :: macro wrappers for jit mode
 */ 
EXTERN_C LIBCGXERT_API const void* cgxertGetInputPortSignal(SimStruct *S, int32_T/*Sint32*/index);
EXTERN_C LIBCGXERT_API void* cgxertGetOutputPortSignal(SimStruct *S, int32_T index);
EXTERN_C LIBCGXERT_API void* cgxertGetDWork(SimStruct *S, int32_T index);
EXTERN_C LIBCGXERT_API void* cgxertGetRunTimeParamInfoData(SimStruct *S, int32_T index);
/* Get varsize input port dimensions array address */
EXTERN_C LIBCGXERT_API int* cgxertGetCurrentInputPortDimensions(SimStruct *S, int32_T portNumber);
/* Get varsize output port dimensions array address */
EXTERN_C LIBCGXERT_API int* cgxertGetCurrentOutputPortDimensions(SimStruct *S, int32_T portNumber);
EXTERN_C LIBCGXERT_API void cgxertSetCurrentOutputPortDimensions(SimStruct *S, int32_T pIdx, int32_T dIdx, int32_T val);
EXTERN_C LIBCGXERT_API bool cgxertIsMajorTimeStep(SimStruct *S);
EXTERN_C LIBCGXERT_API void cgxertSetSolverNeedsReset(SimStruct *S);
EXTERN_C LIBCGXERT_API double cgxertGetT(SimStruct *S);
EXTERN_C LIBCGXERT_API double cgxertGetTaskTime(SimStruct *S, int32_T sti);
EXTERN_C LIBCGXERT_API bool cgxertIsSampleHit(SimStruct *S, int32_T sti, int32_T tid);
EXTERN_C LIBCGXERT_API void* cgxertGetPrevZCSigState(SimStruct *S);
EXTERN_C LIBCGXERT_API void cgxertCallAccelRunBlock(SimStruct *S, int32_T sysIdx, int32_T blkIdx, int32_T method);

/*
 * MATLAB INTERNAL USE ONLY :: Runtime info access functions
 */
EXTERN_C LIBCGXERT_API void* cgxertGetRuntimeInstance(SimStruct* S);
EXTERN_C LIBCGXERT_API void cgxertSetRuntimeInstance(SimStruct* S, void* instance);
EXTERN_C LIBCGXERT_API bool cgxertIsJITEnabled(SimStruct* S);
EXTERN_C LIBCGXERT_API void* cgxertGetEMLRTCtx(SimStruct *S, int32_T index);
EXTERN_C LIBCGXERT_API covrtInstance* cgxertGetCovrtInstance(SimStruct *S, int32_T);
EXTERN_C LIBCGXERT_API void cgxertAllocateRuntimeInfo(SimStruct *S, bool isJIT, void *ri, uint32_T aCovId);
EXTERN_C LIBCGXERT_API void cgxertReleaseRuntimeInfo(SimStruct *S);

/*
 * MATLAB INTERNAL USE ONLY :: Get runtime error status
 */
EXTERN_C LIBCGXERT_API int cgxertGetErrorStatus(SimStruct *S);

/*
 * MATLAB INTERNAL USE ONLY :: Check if model has stopped simulation
 */
EXTERN_C LIBCGXERT_API unsigned int cgxertListenForCtrlC(SimStruct *S);

/*
* MATLAB INTERNAL USE ONLY :: Register runtime error with Simulink if there is any
*/
EXTERN_C LIBCGXERT_API void cgxertRegisterRuntimeError(SimStruct *S);


#endif /* cgxert_h */

