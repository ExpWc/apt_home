/* Copyright 2015 The MathWorks, Inc. */

#ifdef SUPPORTS_PRAGMA_ONCE
#pragma once
#endif

#ifndef SLSIMTGTINSTRUMENTATIONSLMESSAGEBRIDGE_H
#define SLSIMTGTINSTRUMENTATIONSLMESSAGEBRIDGE_H

#include "sl_simtarget_instrumentation_spec.h"

SL_SIMTARGET_INSTRUMENTATION_EXPORT_EXTERN_C void * slmsg_ssGetMsgMemMgr(SimStruct *S);
SL_SIMTARGET_INSTRUMENTATION_EXPORT_EXTERN_C int slmsg_ssGetMsgQueueId(SimStruct *S, int qId);
SL_SIMTARGET_INSTRUMENTATION_EXPORT_EXTERN_C int slmsg_ssGetExternalQueueForOutput(SimStruct *S, int rootOutportIdx);

#endif
