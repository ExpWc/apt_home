/******************************************************************
 *
 *  File: raccel_sfcn_utils.c
 *
 *
 *  Abstract:
 *      - functions for dynamically loading s-function mex files
 *
 * Copyright 2007-2015 The MathWorks, Inc.
 ******************************************************************/

/* INCLUDES */
#include  <stdio.h>
#include  <stdlib.h>

#include  <string.h>
#include  <math.h>
#include  <float.h>
#include  <ctype.h>

#ifdef _WIN32
#include <windows.h>
#else
#include  <dlfcn.h>
#endif

/*
 * We want access to the real mx* routines in this file and not their RTW
 * variants in rt_matrx.h, the defines below prior to including simstruc.h
 * accomplish this.
 */
#include  "mat.h"
#define TYPEDEF_MX_ARRAY
#define rt_matrx_h
#include "simstruc.h"
#undef rt_matrx_h
#undef TYPEDEF_MX_ARRAY

#include "dt_info.h"
#include "raccel_sfcn_utils.h"

typedef struct SFcnInfo_T {
    const char* mexPath;
    mxArray** dialogPrms;
    void* mexHandle;
} SFcnInfo;

extern const char* gblSFcnInfoFileName;
static SFcnInfo* gblSFcnInfo = NULL;
extern size_t gblNumMexFileSFcns;

void raccelReadSFcnInfoFile(SimStruct *S)
{
    const char* errStr = NULL;
    MATFile *pmat = NULL;
    mxArray *mxFeatureValue = NULL;
    int_T featureValue;
    mxArray *sFcnInfoStruct = NULL;
    size_t numSFcns;


    if ((pmat = matOpen(gblSFcnInfoFileName, "r")) == NULL) {
        errStr = "could not find MAT-file containing s-function info";
        goto EXIT_POINT;
    }

    if ((mxFeatureValue = matGetNextVariable(pmat, NULL)) == NULL) {
        errStr = "error reading feature value from s-function info MAT-file";
        goto EXIT_POINT;
    }

    if (!mxIsDouble(mxFeatureValue) || !mxIsScalar(mxFeatureValue)) {
        errStr = "feature value must be a double scalar";
        goto EXIT_POINT;
    }        

    featureValue = (int)((mxGetPr(mxFeatureValue))[0]);

    if (featureValue != 1 && featureValue != 2) {
        errStr = "invalid feature value";
        goto EXIT_POINT;
    }

    if ((sFcnInfoStruct = matGetNextVariable(pmat, NULL)) == NULL) {
        errStr = "error reading s-function info structure from s-function info MAT-file";
        goto EXIT_POINT;
    }

    if (!mxIsStruct(sFcnInfoStruct) || mxGetM(sFcnInfoStruct) != 1) 
    {
        errStr = "RTP must be a structure whose first dimension is 1";
        goto EXIT_POINT;
    }

    gblNumMexFileSFcns = 0;
    numSFcns = mxGetN(sFcnInfoStruct);
    gblSFcnInfo = (SFcnInfo*) calloc(numSFcns, sizeof(SFcnInfo));

    {
        mxArray *temp = NULL;
        boolean_T sourceCodeExists;
        size_t sfIdx;

        if (featureValue == 1)
        {
            /* use mex-file only when source code is not available */
            for (sfIdx = 0; sfIdx < numSFcns; ++sfIdx) {
                temp = mxGetField(sFcnInfoStruct, sfIdx, "sourceCodeExists");
                sourceCodeExists = mxIsLogicalScalarTrue(temp);

                if (!sourceCodeExists) {
                    gblNumMexFileSFcns++;
                    temp = mxGetField(sFcnInfoStruct, sfIdx, "mexPath");
                    gblSFcnInfo[sfIdx].mexPath = mxArrayToString(temp);
                }
            }
        } else if (featureValue == 2) {
            /* always use mex-file */
            gblNumMexFileSFcns = numSFcns;
            for (sfIdx = 0; sfIdx < numSFcns; ++sfIdx) {
                temp = mxGetField(sFcnInfoStruct, sfIdx, "mexPath");
                gblSFcnInfo[sfIdx].mexPath = mxArrayToString(temp);
            }
        }
    }

  EXIT_POINT:
    mxDestroyArray(mxFeatureValue);
    mxDestroyArray(sFcnInfoStruct);

    if (pmat!= NULL) {
        matClose(pmat);
        pmat = NULL;
    }
    if (errStr != NULL) {
        ssSetErrorStatus(S, errStr);
    }
    return;
}

void raccelInitializeGblDialogPrms(SimStruct* child, size_t childIdx)
{
    size_t nPrms = ssGetSFcnParamsCount(child);
    real_T** rtpPtr = (real_T**) ssGetSFcnParamsPtr(child);

    if (nPrms == 0) return;

    gblSFcnInfo[childIdx].dialogPrms = (mxArray**) mxMalloc(sizeof(mxArray*) * nPrms);
    
    {
        size_t prmIdx;
        for (prmIdx = 0; prmIdx < nPrms; ++prmIdx)
        {
            real_T* rtpParam = rtpPtr[prmIdx];
            size_t m = (size_t) rtpParam[0];
            size_t n = (size_t) rtpParam[1];
            real_T* prmData = &rtpParam[2];
            
            gblSFcnInfo[childIdx].dialogPrms[prmIdx] = mxCreateDoubleMatrix(m, n, mxREAL);

            memcpy((real_T*) mxGetData(gblSFcnInfo[childIdx].dialogPrms[prmIdx]),
                   prmData,
                   sizeof(real_T) * m * n);
        }
    }
}

void raccelUnloadSFcnLibrariesAndFreeAssociatedGbls(SimStruct* parent) 
{
    size_t sfIdx;
    for (sfIdx = 0; sfIdx < gblNumMexFileSFcns; ++sfIdx) {
        if (gblSFcnInfo[sfIdx].mexHandle != NULL) {
#ifdef _WIN32
            FreeLibrary(gblSFcnInfo[sfIdx].mexHandle);
#else
            dlclose(gblSFcnInfo[sfIdx].mexHandle);
#endif      
        }

        {
            if (gblSFcnInfo[sfIdx].dialogPrms != NULL) {
                SimStruct* child = ssGetSFunction(parent, sfIdx);
                size_t nPrms = ssGetSFcnParamsCount(child);
                if (nPrms != 0) {
                    mxFree(gblSFcnInfo[sfIdx].dialogPrms);
                }
            }
        }
    }

    free(gblSFcnInfo);
}

void raccelSetSFcnParamPtrToGblAndCopyParamData(SimStruct* child, size_t childIdx, const double** rtpPtr)
{
    size_t nPrms = ssGetSFcnParamsCount(child);
    {
        size_t prmIdx;
        for (prmIdx = 0; prmIdx < nPrms; ++prmIdx) {
            real_T m = rtpPtr[prmIdx][0];
            real_T n = rtpPtr[prmIdx][1];
            const real_T *data = &rtpPtr[prmIdx][2];
            real_T *gblPrmData = (real_T*) mxGetData(gblSFcnInfo[childIdx].dialogPrms[prmIdx]);
            memcpy(gblPrmData, data, sizeof(real_T) * m * n);
        }
    }
    ssSetSFcnParamsPtr(child, gblSFcnInfo[childIdx].dialogPrms);
}

/* Below this point: mex-file loading and simstruct setup ===================================== */

int_T raccelSetRegNumInputPortsFcn(void* arg1, int_T nInputPorts) 
{
    SimStruct* child = (SimStruct*) arg1;
    child->sizes.in.numInputPorts = nInputPorts;
    return 1;
}

int_T raccelSetRegNumOutputPortsFcn(void* arg1, int_T nOutputPorts) 
{
    SimStruct* child = (SimStruct*) arg1;
    child->sizes.out.numOutputPorts = nOutputPorts;
    return 1;
}

void setupPortInfo(SimStruct* child)
{
    child->portInfo.regNumInputPortsFcn = raccelSetRegNumInputPortsFcn;
    child->portInfo.regNumInputPortsFcnArg = child;
    child->portInfo.regNumOutputPortsFcn = raccelSetRegNumOutputPortsFcn;
    child->portInfo.regNumOutputPortsFcnArg = child;
}

#define RACCEL_SFCNPARAM_TUNABLE         (1 << 0x1)
void setupParamInfo(SimStruct* child)
{
    size_t nParams = ssGetNumSFcnParams(child);
    child->sfcnParams.dlgAttribs = calloc(nParams, sizeof(uint_T));
    {
        size_t loopIdx;
        for (loopIdx=0; loopIdx < nParams; ++loopIdx) {
            child->sfcnParams.dlgAttribs[loopIdx] = RACCEL_SFCNPARAM_TUNABLE;
        }
    }
}

void setupChildSimstruct(SimStruct* child) 
{
    setupPortInfo(child);
    setupParamInfo(child);
}

/* see dynload.cpp for these defines */
#ifndef RACCEL_RTLD_NOW
#define RACCEL_RTLD_NOW 1
#endif

#ifndef RACCEL_RTLD_GLOBAL
#define RACCEL_RTLD_GLOBAL 8
#endif

#define RACCEL_DLOPEN_MODE ((RACCEL_RTLD_NOW) | (RACCEL_RTLD_GLOBAL))

#define RACCEL_SFCN_NLHS  5
#define RACCEL_RHS_T 0
#define RACCEL_RHS_X 1
#define RACCEL_RHS_U 2
#define RACCEL_RHS_FLAG 3
#define RACCEL_RHS_P1 4

#define RACCEL_SFCNPARAM_TUNABLE (1 << 0x1)

typedef void (*MexFunctionPtr)(
    int_T, 
    mxArray*[],
    int_T,
    const mxArray *[]
    );

void callSFcnMexFunction(SimStruct* parent,
                         SimStruct** childPtr, 
                         MexFunctionPtr mexFunctionHandle) 
{
    const mxArray** prhs;
    double *pr;
    size_t nEl;
    size_t nPrms = ssGetSFcnParamsCount(*childPtr);
    size_t nrhs = 4 + nPrms;
    mxArray* plhs[RACCEL_SFCN_NLHS] = {NULL}; 

    /* mimicking CreateSFcnRHSArgs */
    if ((prhs = (const mxArray **) calloc(nrhs, sizeof(mxArray*))) == NULL) { 
        ssSetErrorStatus(parent, "error during memory allocation for mexFunction return arguments");
        return;
    }

    if ((prhs[RACCEL_RHS_T] = mxCreateDoubleMatrix(0,0,0)) == NULL) { /* T = [] */
        ssSetErrorStatus(parent, "error during memory allocation for mexFunction return arguments");
        return;
    }

    nEl = (sizeof(SimStruct*)/sizeof(int)+1);
    if ((prhs[RACCEL_RHS_X] = mxCreateDoubleMatrix(nEl, 1, 0)) == NULL) {
        ssSetErrorStatus(parent, "error during memory allocation for mexFunction return arguments");
        return;
    }   
    pr = mxGetPr(prhs[RACCEL_RHS_X]);
     
    {
        size_t loopIdx;
        void *voidChildPtr = (void *) (*childPtr);
        int *intPtr = (int*) &voidChildPtr;

        for (loopIdx = 0; loopIdx < nEl-1; loopIdx++) {
            pr[loopIdx] = (double)(intPtr[loopIdx]);
        }
    }
    pr[nEl-1] = (double) SIMSTRUCT_VERSION_LEVEL2;

    if ((prhs[RACCEL_RHS_U] = mxCreateDoubleMatrix(0,0,0)) == NULL) { /* U */
        ssSetErrorStatus(parent, "error during memory allocation for mexFunction return arguments");
        return;
    }

    if ((prhs[RACCEL_RHS_FLAG] = mxCreateDoubleMatrix(1,1,0)) == NULL) {
        ssSetErrorStatus(parent, "error during memory allocation for mexFunction return arguments");
        return;
    }

    {
        size_t prmIdx;
        for (prmIdx = 0; prmIdx < nPrms; ++prmIdx) 
        {
            prhs[RACCEL_RHS_P1 + prmIdx] = ssGetSFcnParam(*childPtr, prmIdx);
        }
    }

    mexFunctionHandle(1, plhs, nrhs, prhs);
}

void raccelLoadSFcnMexFile(SimStruct* parent,
                           SimStruct* child,
                           size_t childIdx)
{
#ifdef _WIN32
    HINSTANCE libraryHandle;
#else
    void *libraryHandle;
#endif

    MexFunctionPtr mexFunctionHandle;
    const char* mexFilePath = gblSFcnInfo[childIdx].mexPath;

#ifdef _WIN32
    libraryHandle = LoadLibrary(mexFilePath);
#else
    libraryHandle = dlopen(mexFilePath, RACCEL_DLOPEN_MODE);
#endif

    if (libraryHandle == NULL) {
        ssSetErrorStatus(parent, "error loading s-function mex file");
        return;
    }

    gblSFcnInfo[childIdx].mexHandle = libraryHandle;

    /* the casts circumvent a warning: ISO C forbids conversion of object pointer to 
       function pointer type */
#ifdef _WIN32
    mexFunctionHandle = (MexFunctionPtr) GetProcAddress(libraryHandle, "mexFunction");
#else
    *(void **)(&mexFunctionHandle) = dlsym(libraryHandle, "mexFunction");
#endif

    if (mexFunctionHandle == NULL) {
        ssSetErrorStatus(parent, "error finding mexFunction symbol in s-function mex file");
        return;
    }

    setupChildSimstruct(child);

    callSFcnMexFunction(parent, &child, mexFunctionHandle);
}


/* EOF raccel_sfcn_utils.c  */

/* LocalWords:  raccel_sfcn_utils.c
 */

