/* Copyright 1997-2016 The MathWorks, Inc. */

#ifndef _debugger_codegen_interface_h
#define _debugger_codegen_interface_h

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#if defined(S_FUNCTION_NAME)
#include "mwmathutil.h"
#endif

#include "sf_runtime/sf_runtime_spec.h"
#ifndef _SIMSTRUCT
#  define _SIMSTRUCT
typedef struct SimStruct_tag SimStruct;

#endif

LIBMWSF_RUNTIME_API void* sfListenerInitializeUsingSimStruct(SimStruct* S);
LIBMWSF_RUNTIME_API void* sfListenerInitializeUsingBlockPath(char* blkPath);

LIBMWSF_RUNTIME_API void sfListenerTerminate(void* rt);
LIBMWSF_RUNTIME_API void sfListenerInitializeRuntimeVars(void* rt, int* areListenersActive, bool isJitCompliantDebuggerOn, bool isSequenceViewerVisible, int* lineNumber);
LIBMWSF_RUNTIME_API void sfListenerInitializeIsStmtByStmtModeVar(void* rt, int* isStmtByStmtMode);
LIBMWSF_RUNTIME_API void sfListenerInitializeRuntimeFcnVars(void* rt, int* numFcnVars, char* dataNames[], void* dataPtrs[], char* marshallingOutFcns[], char* marshallingInFcns[], int statuses[]);

LIBMWSF_RUNTIME_API void sfListenerReportScriptName(void* rt, char* aScriptName, char* className);

LIBMWSF_RUNTIME_API void sfListenerReportStartingSection(void* rt, unsigned int aSSIDNumber, int aSectionId);
LIBMWSF_RUNTIME_API void sfListenerReportEndingSection(void* rt, unsigned int aSSIDNumber, int aSectionId);

LIBMWSF_RUNTIME_API void sfListenerPushScope(void* rt, unsigned int aSSIDNumber, unsigned int N, char* aVarNames[], void* aDataPtr[], char* aMarchallingOutFunctions[], char* aMarshallingInFunctions[], int aStatus[]);
LIBMWSF_RUNTIME_API void sfListenerPushScopeForSingleVar(void* rt, unsigned int aSSIDNumber, char* aVarNames, void* aDataPtr, char* aMarchallingOutFunction, char* aMarshallingInFunction, int aStatus);
LIBMWSF_RUNTIME_API void sfListenerPopScope(void* rt, unsigned int aSSIDNumber);

LIBMWSF_RUNTIME_API void sfListenerPushScopeForDynamicMatrices(void* rt, unsigned int aSSIDNumber, char* aVarName, void* aDataPtrOrig, char* aMarshallingOutFunction, char* aMarshallingInFunction, int aStatus);
LIBMWSF_RUNTIME_API void sfListenerPushScopeForChart(void* rt, unsigned int aSSIDNumber, unsigned int N, char* aVarNames[], void* aDataPtr[], char* aMarshallingOutFunctions[], char* aMarshallingInFunctions[], int aStatus[]);
LIBMWSF_RUNTIME_API void sfListenerPushScopeForChartForDynamicMatrices(void* rt, unsigned int aSSIDNumber, char* aVarName, void* aDataPtr, void* sizeFirstPtr, void* sizeSecondPtr, char* aMarshallingOutFunction, char* aMarshallingInFunction, int aStatus);
LIBMWSF_RUNTIME_API void sfListenerPushScopeForEMChartForDynamicMatrices(void* rt, unsigned int aSSIDNumber, char* aVarName, void* aDataPtrOrig, char* aMarshallingOutFunction, char* aMarshallingInFunction, int aStatus);
LIBMWSF_RUNTIME_API void sfListenerPopScopeForChart(void* rt, unsigned int aSSIDNumber);

LIBMWSF_RUNTIME_API void sfListenerReportStartingEventBroadcast(void* rt, unsigned int aEventSSIDNumber, int aSectionId);
LIBMWSF_RUNTIME_API void sfListenerReportEndingEventBroadcast(void* rt, unsigned int aEventSSIDNumber, int aSectionId);
LIBMWSF_RUNTIME_API void sfListenerReportLineNumber(void* rt, unsigned int aSSIDNumber,int aLineNumber);
LIBMWSF_RUNTIME_API void sfListenerReportOffsetLength(void* rt, unsigned int aSSIDNumber, int aOffset, int aLength);

LIBMWSF_RUNTIME_API void sfListenerRegisterExtendedHoverDispatchFcn(void* rt, void* fcnPtr);
LIBMWSF_RUNTIME_API void sfAppendHoverData(mxArray** target, mxArray* rawDataToAppend, unsigned int ssid, int labelStringStartPosition, int labelStringEndPosition);

LIBMWSF_RUNTIME_API void* sfListenerMarshallInSLString(void* slString, char* varName);
LIBMWSF_RUNTIME_API void* sfListenerMarshallOutSLString(void* slString);

#endif
