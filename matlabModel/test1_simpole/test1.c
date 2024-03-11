/*
 * test1.c
 *
 * Code generation for model "test1".
 *
 * Model version              : 1.12
 * Simulink Coder version : 8.13 (R2017b) 24-Jul-2017
 * C source code generated on : Mon Mar  4 11:01:49 2024
 *
 * Target selection: AutoTest_grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Generic->Custom
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "test1.h"
#include "test1_private.h"

/* Block signals (auto storage) */
B_test1_T test1_B;

/* Block states (auto storage) */
DW_test1_T test1_DW;

/* Real-time model */
RT_MODEL_test1_T test1_M_;
RT_MODEL_test1_T *const test1_M = &test1_M_;

/* Model output function */
static void test1_output(void)
{
  /* S-Function (gvar_get): '<Root>/Shared Memory Get' */

  /* Level2 S-Function Block: '<Root>/Shared Memory Get' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[0];
    sfcnOutputs(rts,0);
  }

  /* S-Function (gvar_set): '<Root>/Shared Memory Set' */

  /* Level2 S-Function Block: '<Root>/Shared Memory Set' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[1];
    sfcnOutputs(rts,0);
  }

  /* S-Function (gvar_get): '<Root>/Shared Memory Get1' */

  /* Level2 S-Function Block: '<Root>/Shared Memory Get1' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[2];
    sfcnOutputs(rts,0);
  }

  /* S-Function (gvar_set): '<Root>/Shared Memory Set1' */

  /* Level2 S-Function Block: '<Root>/Shared Memory Set1' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[3];
    sfcnOutputs(rts,0);
  }
}

/* Model update function */
static void test1_update(void)
{
  /* Update for S-Function (gvar_get): '<Root>/Shared Memory Get' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Get' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[0];
    sfcnUpdate(rts,0);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* Update for S-Function (gvar_set): '<Root>/Shared Memory Set' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Set' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[1];
    sfcnUpdate(rts,0);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* Update for S-Function (gvar_get): '<Root>/Shared Memory Get1' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Get1' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[2];
    sfcnUpdate(rts,0);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* Update for S-Function (gvar_set): '<Root>/Shared Memory Set1' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Set1' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[3];
    sfcnUpdate(rts,0);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* Update absolute time for base rate */
  /* The "clockTick0" counts the number of times the code of this task has
   * been executed. The absolute time is the multiplication of "clockTick0"
   * and "Timing.stepSize0". Size of "clockTick0" ensures timer will not
   * overflow during the application lifespan selected.
   * Timer of this task consists of two 32 bit unsigned integers.
   * The two integers represent the low bits Timing.clockTick0 and the high bits
   * Timing.clockTickH0. When the low bit overflows to 0, the high bits increment.
   */
  if (!(++test1_M->Timing.clockTick0)) {
    ++test1_M->Timing.clockTickH0;
  }

  test1_M->Timing.t[0] = test1_M->Timing.clockTick0 * test1_M->Timing.stepSize0
    + test1_M->Timing.clockTickH0 * test1_M->Timing.stepSize0 * 4294967296.0;

  {
    /* Update absolute timer for sample time: [0.2s, 0.0s] */
    /* The "clockTick1" counts the number of times the code of this task has
     * been executed. The absolute time is the multiplication of "clockTick1"
     * and "Timing.stepSize1". Size of "clockTick1" ensures timer will not
     * overflow during the application lifespan selected.
     * Timer of this task consists of two 32 bit unsigned integers.
     * The two integers represent the low bits Timing.clockTick1 and the high bits
     * Timing.clockTickH1. When the low bit overflows to 0, the high bits increment.
     */
    if (!(++test1_M->Timing.clockTick1)) {
      ++test1_M->Timing.clockTickH1;
    }

    test1_M->Timing.t[1] = test1_M->Timing.clockTick1 *
      test1_M->Timing.stepSize1 + test1_M->Timing.clockTickH1 *
      test1_M->Timing.stepSize1 * 4294967296.0;
  }
}

/* Model initialize function */
static void test1_initialize(void)
{
  /* Start for S-Function (gvar_get): '<Root>/Shared Memory Get' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Get' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[0];
    sfcnStart(rts);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* Start for S-Function (gvar_set): '<Root>/Shared Memory Set' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Set' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[1];
    sfcnStart(rts);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* Start for S-Function (gvar_get): '<Root>/Shared Memory Get1' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Get1' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[2];
    sfcnStart(rts);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* Start for S-Function (gvar_set): '<Root>/Shared Memory Set1' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Set1' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[3];
    sfcnStart(rts);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* InitializeConditions for S-Function (gvar_get): '<Root>/Shared Memory Get' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Get' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[0];
    sfcnInitializeConditions(rts);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* InitializeConditions for S-Function (gvar_set): '<Root>/Shared Memory Set' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Set' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[1];
    sfcnInitializeConditions(rts);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* InitializeConditions for S-Function (gvar_get): '<Root>/Shared Memory Get1' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Get1' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[2];
    sfcnInitializeConditions(rts);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }

  /* InitializeConditions for S-Function (gvar_set): '<Root>/Shared Memory Set1' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Set1' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[3];
    sfcnInitializeConditions(rts);
    if (ssGetErrorStatus(rts) != (NULL))
      return;
  }
}

/* Model terminate function */
static void test1_terminate(void)
{
  /* Terminate for S-Function (gvar_get): '<Root>/Shared Memory Get' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Get' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[0];
    sfcnTerminate(rts);
  }

  /* Terminate for S-Function (gvar_set): '<Root>/Shared Memory Set' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Set' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[1];
    sfcnTerminate(rts);
  }

  /* Terminate for S-Function (gvar_get): '<Root>/Shared Memory Get1' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Get1' (gvar_get) */
  {
    SimStruct *rts = test1_M->childSfunctions[2];
    sfcnTerminate(rts);
  }

  /* Terminate for S-Function (gvar_set): '<Root>/Shared Memory Set1' */
  /* Level2 S-Function Block: '<Root>/Shared Memory Set1' (gvar_set) */
  {
    SimStruct *rts = test1_M->childSfunctions[3];
    sfcnTerminate(rts);
  }
}

/*========================================================================*
 * Start of Classic call interface                                        *
 *========================================================================*/
void MdlOutputs(int_T tid)
{
  test1_output();
  UNUSED_PARAMETER(tid);
}

void MdlUpdate(int_T tid)
{
  test1_update();
  UNUSED_PARAMETER(tid);
}

void MdlInitializeSizes(void)
{
}

void MdlInitializeSampleTimes(void)
{
}

void MdlInitialize(void)
{
}

void MdlStart(void)
{
  test1_initialize();
}

void MdlTerminate(void)
{
  test1_terminate();
}

/* Registration function */
RT_MODEL_test1_T *test1(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)test1_M, 0,
                sizeof(RT_MODEL_test1_T));

  {
    /* Setup solver object */
    rtsiSetSimTimeStepPtr(&test1_M->solverInfo, &test1_M->Timing.simTimeStep);
    rtsiSetTPtr(&test1_M->solverInfo, &rtmGetTPtr(test1_M));
    rtsiSetStepSizePtr(&test1_M->solverInfo, &test1_M->Timing.stepSize0);
    rtsiSetErrorStatusPtr(&test1_M->solverInfo, (&rtmGetErrorStatus(test1_M)));
    rtsiSetRTModelPtr(&test1_M->solverInfo, test1_M);
  }

  rtsiSetSimTimeStep(&test1_M->solverInfo, MAJOR_TIME_STEP);
  rtsiSetSolverName(&test1_M->solverInfo,"FixedStepDiscrete");
  test1_M->solverInfoPtr = (&test1_M->solverInfo);

  /* Initialize timing info */
  {
    int_T *mdlTsMap = test1_M->Timing.sampleTimeTaskIDArray;
    mdlTsMap[0] = 0;
    mdlTsMap[1] = 1;
    test1_M->Timing.sampleTimeTaskIDPtr = (&mdlTsMap[0]);
    test1_M->Timing.sampleTimes = (&test1_M->Timing.sampleTimesArray[0]);
    test1_M->Timing.offsetTimes = (&test1_M->Timing.offsetTimesArray[0]);

    /* task periods */
    test1_M->Timing.sampleTimes[0] = (0.0);
    test1_M->Timing.sampleTimes[1] = (0.2);

    /* task offsets */
    test1_M->Timing.offsetTimes[0] = (0.0);
    test1_M->Timing.offsetTimes[1] = (0.0);
  }

  rtmSetTPtr(test1_M, &test1_M->Timing.tArray[0]);

  {
    int_T *mdlSampleHits = test1_M->Timing.sampleHitArray;
    mdlSampleHits[0] = 1;
    mdlSampleHits[1] = 1;
    test1_M->Timing.sampleHits = (&mdlSampleHits[0]);
  }

  rtmSetTFinal(test1_M, -1);
  test1_M->Timing.stepSize0 = 0.2;
  test1_M->Timing.stepSize1 = 0.2;

  /* Setup for data logging */
  {
    static RTWLogInfo rt_DataLoggingInfo;
    rt_DataLoggingInfo.loggingInterval = NULL;
    test1_M->rtwLogInfo = &rt_DataLoggingInfo;
  }

  /* Setup for data logging */
  {
    rtliSetLogXSignalInfo(test1_M->rtwLogInfo, (NULL));
    rtliSetLogXSignalPtrs(test1_M->rtwLogInfo, (NULL));
    rtliSetLogT(test1_M->rtwLogInfo, "tout");
    rtliSetLogX(test1_M->rtwLogInfo, "");
    rtliSetLogXFinal(test1_M->rtwLogInfo, "");
    rtliSetLogVarNameModifier(test1_M->rtwLogInfo, "rt_");
    rtliSetLogFormat(test1_M->rtwLogInfo, 4);
    rtliSetLogMaxRows(test1_M->rtwLogInfo, 0);
    rtliSetLogDecimation(test1_M->rtwLogInfo, 1);
    rtliSetLogY(test1_M->rtwLogInfo, "");
    rtliSetLogYSignalInfo(test1_M->rtwLogInfo, (NULL));
    rtliSetLogYSignalPtrs(test1_M->rtwLogInfo, (NULL));
  }

  test1_M->solverInfoPtr = (&test1_M->solverInfo);
  test1_M->Timing.stepSize = (0.2);
  rtsiSetFixedStepSize(&test1_M->solverInfo, 0.2);
  rtsiSetSolverMode(&test1_M->solverInfo, SOLVER_MODE_SINGLETASKING);

  /* block I/O */
  test1_M->blockIO = ((void *) &test1_B);
  (void) memset(((void *) &test1_B), 0,
                sizeof(B_test1_T));

  /* states (dwork) */
  test1_M->dwork = ((void *) &test1_DW);
  (void) memset((void *)&test1_DW, 0,
                sizeof(DW_test1_T));

  /* child S-Function registration */
  {
    RTWSfcnInfo *sfcnInfo = &test1_M->NonInlinedSFcns.sfcnInfo;
    test1_M->sfcnInfo = (sfcnInfo);
    rtssSetErrorStatusPtr(sfcnInfo, (&rtmGetErrorStatus(test1_M)));
    rtssSetNumRootSampTimesPtr(sfcnInfo, &test1_M->Sizes.numSampTimes);
    test1_M->NonInlinedSFcns.taskTimePtrs[0] = &(rtmGetTPtr(test1_M)[0]);
    test1_M->NonInlinedSFcns.taskTimePtrs[1] = &(rtmGetTPtr(test1_M)[1]);
    rtssSetTPtrPtr(sfcnInfo,test1_M->NonInlinedSFcns.taskTimePtrs);
    rtssSetTStartPtr(sfcnInfo, &rtmGetTStart(test1_M));
    rtssSetTFinalPtr(sfcnInfo, &rtmGetTFinal(test1_M));
    rtssSetTimeOfLastOutputPtr(sfcnInfo, &rtmGetTimeOfLastOutput(test1_M));
    rtssSetStepSizePtr(sfcnInfo, &test1_M->Timing.stepSize);
    rtssSetStopRequestedPtr(sfcnInfo, &rtmGetStopRequested(test1_M));
    rtssSetDerivCacheNeedsResetPtr(sfcnInfo, &test1_M->derivCacheNeedsReset);
    rtssSetZCCacheNeedsResetPtr(sfcnInfo, &test1_M->zCCacheNeedsReset);
    rtssSetContTimeOutputInconsistentWithStateAtMajorStepPtr(sfcnInfo,
      &test1_M->CTOutputIncnstWithState);
    rtssSetSampleHitsPtr(sfcnInfo, &test1_M->Timing.sampleHits);
    rtssSetPerTaskSampleHitsPtr(sfcnInfo, &test1_M->Timing.perTaskSampleHits);
    rtssSetSimModePtr(sfcnInfo, &test1_M->simMode);
    rtssSetSolverInfoPtr(sfcnInfo, &test1_M->solverInfoPtr);
  }

  test1_M->Sizes.numSFcns = (4);

  /* register each child */
  {
    (void) memset((void *)&test1_M->NonInlinedSFcns.childSFunctions[0], 0,
                  4*sizeof(SimStruct));
    test1_M->childSfunctions = (&test1_M->NonInlinedSFcns.childSFunctionPtrs[0]);
    test1_M->childSfunctions[0] = (&test1_M->NonInlinedSFcns.childSFunctions[0]);
    test1_M->childSfunctions[1] = (&test1_M->NonInlinedSFcns.childSFunctions[1]);
    test1_M->childSfunctions[2] = (&test1_M->NonInlinedSFcns.childSFunctions[2]);
    test1_M->childSfunctions[3] = (&test1_M->NonInlinedSFcns.childSFunctions[3]);

    /* Level2 S-Function Block: test1/<Root>/Shared Memory Get (gvar_get) */
    {
      SimStruct *rts = test1_M->childSfunctions[0];

      /* timing info */
      time_T *sfcnPeriod = test1_M->NonInlinedSFcns.Sfcn0.sfcnPeriod;
      time_T *sfcnOffset = test1_M->NonInlinedSFcns.Sfcn0.sfcnOffset;
      int_T *sfcnTsMap = test1_M->NonInlinedSFcns.Sfcn0.sfcnTsMap;
      (void) memset((void*)sfcnPeriod, 0,
                    sizeof(time_T)*1);
      (void) memset((void*)sfcnOffset, 0,
                    sizeof(time_T)*1);
      ssSetSampleTimePtr(rts, &sfcnPeriod[0]);
      ssSetOffsetTimePtr(rts, &sfcnOffset[0]);
      ssSetSampleTimeTaskIDPtr(rts, sfcnTsMap);

      {
        ssSetBlkInfo2Ptr(rts, &test1_M->NonInlinedSFcns.blkInfo2[0]);
      }

      _ssSetBlkInfo2PortInfo2Ptr(rts,
        &test1_M->NonInlinedSFcns.inputOutputPortInfo2[0]);

      /* Set up the mdlInfo pointer */
      ssSetRTWSfcnInfo(rts, test1_M->sfcnInfo);

      /* Allocate memory of model methods 2 */
      {
        ssSetModelMethods2(rts, &test1_M->NonInlinedSFcns.methods2[0]);
      }

      /* Allocate memory of model methods 3 */
      {
        ssSetModelMethods3(rts, &test1_M->NonInlinedSFcns.methods3[0]);
      }

      /* Allocate memory of model methods 4 */
      {
        ssSetModelMethods4(rts, &test1_M->NonInlinedSFcns.methods4[0]);
      }

      /* Allocate memory for states auxilliary information */
      {
        ssSetStatesInfo2(rts, &test1_M->NonInlinedSFcns.statesInfo2[0]);
        ssSetPeriodicStatesInfo(rts,
          &test1_M->NonInlinedSFcns.periodicStatesInfo[0]);
      }

      /* outputs */
      {
        ssSetPortInfoForOutputs(rts,
          &test1_M->NonInlinedSFcns.Sfcn0.outputPortInfo[0]);
        _ssSetNumOutputPorts(rts, 1);
        _ssSetPortInfo2ForOutputUnits(rts,
          &test1_M->NonInlinedSFcns.Sfcn0.outputPortUnits[0]);
        ssSetOutputPortUnit(rts, 0, 0);

        /* port 0 */
        {
          _ssSetOutputPortNumDimensions(rts, 0, 1);
          ssSetOutputPortWidth(rts, 0, 1);
          ssSetOutputPortSignal(rts, 0, ((real_T *) &test1_B.SharedMemoryGet));
        }
      }

      /* path info */
      ssSetModelName(rts, "Shared Memory Get");
      ssSetPath(rts, "test1/Shared Memory Get");
      ssSetRTModel(rts,test1_M);
      ssSetParentSS(rts, (NULL));
      ssSetRootSS(rts, rts);
      ssSetVersion(rts, SIMSTRUCT_VERSION_LEVEL2);

      /* work vectors */
      ssSetIWork(rts, (int_T *) &test1_DW.SharedMemoryGet_IWORK[0]);

      {
        struct _ssDWorkRecord *dWorkRecord = (struct _ssDWorkRecord *)
          &test1_M->NonInlinedSFcns.Sfcn0.dWork;
        struct _ssDWorkAuxRecord *dWorkAuxRecord = (struct _ssDWorkAuxRecord *)
          &test1_M->NonInlinedSFcns.Sfcn0.dWorkAux;
        ssSetSFcnDWork(rts, dWorkRecord);
        ssSetSFcnDWorkAux(rts, dWorkAuxRecord);
        _ssSetNumDWork(rts, 1);

        /* IWORK */
        ssSetDWorkWidth(rts, 0, 3);
        ssSetDWorkDataType(rts, 0,SS_INTEGER);
        ssSetDWorkComplexSignal(rts, 0, 0);
        ssSetDWork(rts, 0, &test1_DW.SharedMemoryGet_IWORK[0]);
      }

      /* registration */
      gvar_get(rts);
      sfcnInitializeSizes(rts);
      sfcnInitializeSampleTimes(rts);

      /* adjust sample time */
      ssSetSampleTime(rts, 0, 0.0);
      ssSetOffsetTime(rts, 0, 0.0);
      sfcnTsMap[0] = 0;

      /* set compiled values of dynamic vector attributes */
      ssSetOutputPortWidth(rts, 0, 1);
      ssSetOutputPortDataType(rts, 0, SS_DOUBLE);
      ssSetOutputPortComplexSignal(rts, 0, 0);
      ssSetOutputPortFrameData(rts, 0, 0);
      ssSetOutputPortUnit(rts, 0, 0);
      ssSetNumNonsampledZCs(rts, 0);

      /* Update connectivity flags for each port */
      _ssSetOutputPortConnected(rts, 0, 1);
      _ssSetOutputPortBeingMerged(rts, 0, 0);

      /* Update the BufferDstPort flags for each input port */
    }

    /* Level2 S-Function Block: test1/<Root>/Shared Memory Set (gvar_set) */
    {
      SimStruct *rts = test1_M->childSfunctions[1];

      /* timing info */
      time_T *sfcnPeriod = test1_M->NonInlinedSFcns.Sfcn1.sfcnPeriod;
      time_T *sfcnOffset = test1_M->NonInlinedSFcns.Sfcn1.sfcnOffset;
      int_T *sfcnTsMap = test1_M->NonInlinedSFcns.Sfcn1.sfcnTsMap;
      (void) memset((void*)sfcnPeriod, 0,
                    sizeof(time_T)*1);
      (void) memset((void*)sfcnOffset, 0,
                    sizeof(time_T)*1);
      ssSetSampleTimePtr(rts, &sfcnPeriod[0]);
      ssSetOffsetTimePtr(rts, &sfcnOffset[0]);
      ssSetSampleTimeTaskIDPtr(rts, sfcnTsMap);

      {
        ssSetBlkInfo2Ptr(rts, &test1_M->NonInlinedSFcns.blkInfo2[1]);
      }

      _ssSetBlkInfo2PortInfo2Ptr(rts,
        &test1_M->NonInlinedSFcns.inputOutputPortInfo2[1]);

      /* Set up the mdlInfo pointer */
      ssSetRTWSfcnInfo(rts, test1_M->sfcnInfo);

      /* Allocate memory of model methods 2 */
      {
        ssSetModelMethods2(rts, &test1_M->NonInlinedSFcns.methods2[1]);
      }

      /* Allocate memory of model methods 3 */
      {
        ssSetModelMethods3(rts, &test1_M->NonInlinedSFcns.methods3[1]);
      }

      /* Allocate memory of model methods 4 */
      {
        ssSetModelMethods4(rts, &test1_M->NonInlinedSFcns.methods4[1]);
      }

      /* Allocate memory for states auxilliary information */
      {
        ssSetStatesInfo2(rts, &test1_M->NonInlinedSFcns.statesInfo2[1]);
        ssSetPeriodicStatesInfo(rts,
          &test1_M->NonInlinedSFcns.periodicStatesInfo[1]);
      }

      /* inputs */
      {
        _ssSetNumInputPorts(rts, 1);
        ssSetPortInfoForInputs(rts,
          &test1_M->NonInlinedSFcns.Sfcn1.inputPortInfo[0]);
        _ssSetPortInfo2ForInputUnits(rts,
          &test1_M->NonInlinedSFcns.Sfcn1.inputPortUnits[0]);
        ssSetInputPortUnit(rts, 0, 0);

        /* port 0 */
        {
          ssSetInputPortRequiredContiguous(rts, 0, 1);
          ssSetInputPortSignal(rts, 0, &test1_B.SharedMemoryGet);
          _ssSetInputPortNumDimensions(rts, 0, 1);
          ssSetInputPortWidth(rts, 0, 1);
        }
      }

      /* path info */
      ssSetModelName(rts, "Shared Memory Set");
      ssSetPath(rts, "test1/Shared Memory Set");
      ssSetRTModel(rts,test1_M);
      ssSetParentSS(rts, (NULL));
      ssSetRootSS(rts, rts);
      ssSetVersion(rts, SIMSTRUCT_VERSION_LEVEL2);

      /* work vectors */
      ssSetIWork(rts, (int_T *) &test1_DW.SharedMemorySet_IWORK[0]);

      {
        struct _ssDWorkRecord *dWorkRecord = (struct _ssDWorkRecord *)
          &test1_M->NonInlinedSFcns.Sfcn1.dWork;
        struct _ssDWorkAuxRecord *dWorkAuxRecord = (struct _ssDWorkAuxRecord *)
          &test1_M->NonInlinedSFcns.Sfcn1.dWorkAux;
        ssSetSFcnDWork(rts, dWorkRecord);
        ssSetSFcnDWorkAux(rts, dWorkAuxRecord);
        _ssSetNumDWork(rts, 1);

        /* IWORK */
        ssSetDWorkWidth(rts, 0, 3);
        ssSetDWorkDataType(rts, 0,SS_INTEGER);
        ssSetDWorkComplexSignal(rts, 0, 0);
        ssSetDWork(rts, 0, &test1_DW.SharedMemorySet_IWORK[0]);
      }

      /* registration */
      gvar_set(rts);
      sfcnInitializeSizes(rts);
      sfcnInitializeSampleTimes(rts);

      /* adjust sample time */
      ssSetSampleTime(rts, 0, 0.0);
      ssSetOffsetTime(rts, 0, 0.0);
      sfcnTsMap[0] = 0;

      /* set compiled values of dynamic vector attributes */
      ssSetInputPortWidth(rts, 0, 1);
      ssSetInputPortDataType(rts, 0, SS_DOUBLE);
      ssSetInputPortComplexSignal(rts, 0, 0);
      ssSetInputPortFrameData(rts, 0, 0);
      ssSetInputPortUnit(rts, 0, 0);
      ssSetNumNonsampledZCs(rts, 0);

      /* Update connectivity flags for each port */
      _ssSetInputPortConnected(rts, 0, 1);

      /* Update the BufferDstPort flags for each input port */
      ssSetInputPortBufferDstPort(rts, 0, -1);
    }

    /* Level2 S-Function Block: test1/<Root>/Shared Memory Get1 (gvar_get) */
    {
      SimStruct *rts = test1_M->childSfunctions[2];

      /* timing info */
      time_T *sfcnPeriod = test1_M->NonInlinedSFcns.Sfcn2.sfcnPeriod;
      time_T *sfcnOffset = test1_M->NonInlinedSFcns.Sfcn2.sfcnOffset;
      int_T *sfcnTsMap = test1_M->NonInlinedSFcns.Sfcn2.sfcnTsMap;
      (void) memset((void*)sfcnPeriod, 0,
                    sizeof(time_T)*1);
      (void) memset((void*)sfcnOffset, 0,
                    sizeof(time_T)*1);
      ssSetSampleTimePtr(rts, &sfcnPeriod[0]);
      ssSetOffsetTimePtr(rts, &sfcnOffset[0]);
      ssSetSampleTimeTaskIDPtr(rts, sfcnTsMap);

      {
        ssSetBlkInfo2Ptr(rts, &test1_M->NonInlinedSFcns.blkInfo2[2]);
      }

      _ssSetBlkInfo2PortInfo2Ptr(rts,
        &test1_M->NonInlinedSFcns.inputOutputPortInfo2[2]);

      /* Set up the mdlInfo pointer */
      ssSetRTWSfcnInfo(rts, test1_M->sfcnInfo);

      /* Allocate memory of model methods 2 */
      {
        ssSetModelMethods2(rts, &test1_M->NonInlinedSFcns.methods2[2]);
      }

      /* Allocate memory of model methods 3 */
      {
        ssSetModelMethods3(rts, &test1_M->NonInlinedSFcns.methods3[2]);
      }

      /* Allocate memory of model methods 4 */
      {
        ssSetModelMethods4(rts, &test1_M->NonInlinedSFcns.methods4[2]);
      }

      /* Allocate memory for states auxilliary information */
      {
        ssSetStatesInfo2(rts, &test1_M->NonInlinedSFcns.statesInfo2[2]);
        ssSetPeriodicStatesInfo(rts,
          &test1_M->NonInlinedSFcns.periodicStatesInfo[2]);
      }

      /* outputs */
      {
        ssSetPortInfoForOutputs(rts,
          &test1_M->NonInlinedSFcns.Sfcn2.outputPortInfo[0]);
        _ssSetNumOutputPorts(rts, 1);
        _ssSetPortInfo2ForOutputUnits(rts,
          &test1_M->NonInlinedSFcns.Sfcn2.outputPortUnits[0]);
        ssSetOutputPortUnit(rts, 0, 0);

        /* port 0 */
        {
          _ssSetOutputPortNumDimensions(rts, 0, 1);
          ssSetOutputPortWidth(rts, 0, 1);
          ssSetOutputPortSignal(rts, 0, ((real_T *) &test1_B.SharedMemoryGet1));
        }
      }

      /* path info */
      ssSetModelName(rts, "Shared Memory Get1");
      ssSetPath(rts, "test1/Shared Memory Get1");
      ssSetRTModel(rts,test1_M);
      ssSetParentSS(rts, (NULL));
      ssSetRootSS(rts, rts);
      ssSetVersion(rts, SIMSTRUCT_VERSION_LEVEL2);

      /* work vectors */
      ssSetIWork(rts, (int_T *) &test1_DW.SharedMemoryGet1_IWORK[0]);

      {
        struct _ssDWorkRecord *dWorkRecord = (struct _ssDWorkRecord *)
          &test1_M->NonInlinedSFcns.Sfcn2.dWork;
        struct _ssDWorkAuxRecord *dWorkAuxRecord = (struct _ssDWorkAuxRecord *)
          &test1_M->NonInlinedSFcns.Sfcn2.dWorkAux;
        ssSetSFcnDWork(rts, dWorkRecord);
        ssSetSFcnDWorkAux(rts, dWorkAuxRecord);
        _ssSetNumDWork(rts, 1);

        /* IWORK */
        ssSetDWorkWidth(rts, 0, 3);
        ssSetDWorkDataType(rts, 0,SS_INTEGER);
        ssSetDWorkComplexSignal(rts, 0, 0);
        ssSetDWork(rts, 0, &test1_DW.SharedMemoryGet1_IWORK[0]);
      }

      /* registration */
      gvar_get(rts);
      sfcnInitializeSizes(rts);
      sfcnInitializeSampleTimes(rts);

      /* adjust sample time */
      ssSetSampleTime(rts, 0, 0.0);
      ssSetOffsetTime(rts, 0, 0.0);
      sfcnTsMap[0] = 0;

      /* set compiled values of dynamic vector attributes */
      ssSetOutputPortWidth(rts, 0, 1);
      ssSetOutputPortDataType(rts, 0, SS_DOUBLE);
      ssSetOutputPortComplexSignal(rts, 0, 0);
      ssSetOutputPortFrameData(rts, 0, 0);
      ssSetOutputPortUnit(rts, 0, 0);
      ssSetNumNonsampledZCs(rts, 0);

      /* Update connectivity flags for each port */
      _ssSetOutputPortConnected(rts, 0, 1);
      _ssSetOutputPortBeingMerged(rts, 0, 0);

      /* Update the BufferDstPort flags for each input port */
    }

    /* Level2 S-Function Block: test1/<Root>/Shared Memory Set1 (gvar_set) */
    {
      SimStruct *rts = test1_M->childSfunctions[3];

      /* timing info */
      time_T *sfcnPeriod = test1_M->NonInlinedSFcns.Sfcn3.sfcnPeriod;
      time_T *sfcnOffset = test1_M->NonInlinedSFcns.Sfcn3.sfcnOffset;
      int_T *sfcnTsMap = test1_M->NonInlinedSFcns.Sfcn3.sfcnTsMap;
      (void) memset((void*)sfcnPeriod, 0,
                    sizeof(time_T)*1);
      (void) memset((void*)sfcnOffset, 0,
                    sizeof(time_T)*1);
      ssSetSampleTimePtr(rts, &sfcnPeriod[0]);
      ssSetOffsetTimePtr(rts, &sfcnOffset[0]);
      ssSetSampleTimeTaskIDPtr(rts, sfcnTsMap);

      {
        ssSetBlkInfo2Ptr(rts, &test1_M->NonInlinedSFcns.blkInfo2[3]);
      }

      _ssSetBlkInfo2PortInfo2Ptr(rts,
        &test1_M->NonInlinedSFcns.inputOutputPortInfo2[3]);

      /* Set up the mdlInfo pointer */
      ssSetRTWSfcnInfo(rts, test1_M->sfcnInfo);

      /* Allocate memory of model methods 2 */
      {
        ssSetModelMethods2(rts, &test1_M->NonInlinedSFcns.methods2[3]);
      }

      /* Allocate memory of model methods 3 */
      {
        ssSetModelMethods3(rts, &test1_M->NonInlinedSFcns.methods3[3]);
      }

      /* Allocate memory of model methods 4 */
      {
        ssSetModelMethods4(rts, &test1_M->NonInlinedSFcns.methods4[3]);
      }

      /* Allocate memory for states auxilliary information */
      {
        ssSetStatesInfo2(rts, &test1_M->NonInlinedSFcns.statesInfo2[3]);
        ssSetPeriodicStatesInfo(rts,
          &test1_M->NonInlinedSFcns.periodicStatesInfo[3]);
      }

      /* inputs */
      {
        _ssSetNumInputPorts(rts, 1);
        ssSetPortInfoForInputs(rts,
          &test1_M->NonInlinedSFcns.Sfcn3.inputPortInfo[0]);
        _ssSetPortInfo2ForInputUnits(rts,
          &test1_M->NonInlinedSFcns.Sfcn3.inputPortUnits[0]);
        ssSetInputPortUnit(rts, 0, 0);

        /* port 0 */
        {
          ssSetInputPortRequiredContiguous(rts, 0, 1);
          ssSetInputPortSignal(rts, 0, &test1_B.SharedMemoryGet1);
          _ssSetInputPortNumDimensions(rts, 0, 1);
          ssSetInputPortWidth(rts, 0, 1);
        }
      }

      /* path info */
      ssSetModelName(rts, "Shared Memory Set1");
      ssSetPath(rts, "test1/Shared Memory Set1");
      ssSetRTModel(rts,test1_M);
      ssSetParentSS(rts, (NULL));
      ssSetRootSS(rts, rts);
      ssSetVersion(rts, SIMSTRUCT_VERSION_LEVEL2);

      /* work vectors */
      ssSetIWork(rts, (int_T *) &test1_DW.SharedMemorySet1_IWORK[0]);

      {
        struct _ssDWorkRecord *dWorkRecord = (struct _ssDWorkRecord *)
          &test1_M->NonInlinedSFcns.Sfcn3.dWork;
        struct _ssDWorkAuxRecord *dWorkAuxRecord = (struct _ssDWorkAuxRecord *)
          &test1_M->NonInlinedSFcns.Sfcn3.dWorkAux;
        ssSetSFcnDWork(rts, dWorkRecord);
        ssSetSFcnDWorkAux(rts, dWorkAuxRecord);
        _ssSetNumDWork(rts, 1);

        /* IWORK */
        ssSetDWorkWidth(rts, 0, 3);
        ssSetDWorkDataType(rts, 0,SS_INTEGER);
        ssSetDWorkComplexSignal(rts, 0, 0);
        ssSetDWork(rts, 0, &test1_DW.SharedMemorySet1_IWORK[0]);
      }

      /* registration */
      gvar_set(rts);
      sfcnInitializeSizes(rts);
      sfcnInitializeSampleTimes(rts);

      /* adjust sample time */
      ssSetSampleTime(rts, 0, 0.0);
      ssSetOffsetTime(rts, 0, 0.0);
      sfcnTsMap[0] = 0;

      /* set compiled values of dynamic vector attributes */
      ssSetInputPortWidth(rts, 0, 1);
      ssSetInputPortDataType(rts, 0, SS_DOUBLE);
      ssSetInputPortComplexSignal(rts, 0, 0);
      ssSetInputPortFrameData(rts, 0, 0);
      ssSetInputPortUnit(rts, 0, 0);
      ssSetNumNonsampledZCs(rts, 0);

      /* Update connectivity flags for each port */
      _ssSetInputPortConnected(rts, 0, 1);

      /* Update the BufferDstPort flags for each input port */
      ssSetInputPortBufferDstPort(rts, 0, -1);
    }
  }

  /* Initialize Sizes */
  test1_M->Sizes.numContStates = (0);  /* Number of continuous states */
  test1_M->Sizes.numY = (0);           /* Number of model outputs */
  test1_M->Sizes.numU = (0);           /* Number of model inputs */
  test1_M->Sizes.sysDirFeedThru = (0); /* The model is not direct feedthrough */
  test1_M->Sizes.numSampTimes = (2);   /* Number of sample times */
  test1_M->Sizes.numBlocks = (4);      /* Number of blocks */
  test1_M->Sizes.numBlockIO = (2);     /* Number of block outputs */
  return test1_M;
}

/*========================================================================*
 * End of Classic call interface                                          *
 *========================================================================*/
