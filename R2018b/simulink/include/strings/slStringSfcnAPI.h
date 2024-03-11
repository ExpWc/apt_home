/* API for s-functions and Stateflow to support Simulink String */

/* Copyright 2015-2016 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef _sl_string_sfcn_api_h_
#define _sl_string_sfcn_api_h_

#include "simulink_spec.h"

SIMULINK_EXPORT_EXTERN_C size_t ssGetMaxStringLength(SimStruct *S, int dataType);
SIMULINK_EXPORT_EXTERN_C DTypeId ssRegisterStringDataType(SimStruct *S, size_t maxStringLength);
SIMULINK_EXPORT_EXTERN_C bool ssIsStringDataType(SimStruct *S, int dataType);
SIMULINK_EXPORT_EXTERN_C BDErrorValue ssGetStringTruncationDiagnosticSetting(SimStruct *S);
SIMULINK_EXPORT_EXTERN_C int ssGetStringBufferSizeDiagnosticSetting(SimStruct *S);
SIMULINK_EXPORT_EXTERN_C int ssGetTargetLangStandard(SimStruct *S);

/* This needs to be called after EvalDlgPrms() stage, otherwise it won't have effect */
SIMULINK_EXPORT_EXTERN_C void ssSetInputSupportStringDataType(SimStruct *S, int portIdx, bool support);
/* This needs to be called after EvalDlgPrms() stage, otherwise it won't have effect */
SIMULINK_EXPORT_EXTERN_C void ssSetOutputSupportStringDataType(SimStruct *S, int portIdx, bool support);

#endif /* _sl_string_sfcn_api_h_ */
