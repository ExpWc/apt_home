/* $Revision: 1.68.4.12 $
* Copyright 1994-2005 The MathWorks, Inc.
*
* File    : grt_main.c
*
* Abstract:
*      A Generic "Real-Time (single tasking or pseudo-multitasking,
*      statically allocated data)" main that runs under most
*      operating systems.
*
*      This file may be a useful starting point when targeting a new
*      processor or microcontroller.
*
*
* Compiler specified defines:
*	RT              - Required.
*      MODEL=modelname - Required.
*	NUMST=#         - Required. Number of sample times.
*	NCSTATES=#      - Required. Number of continuous states.
*      TID01EQ=1 or 0  - Optional. Only define to 1 if sample time task
*                        id's 0 and 1 have equal rates.
*      MULTITASKING    - Optional. (use MT for a synonym).
*	SAVEFILE        - Optional (non-quoted) name of .mat file to create.
*			  Default is <MODEL>.mat
*      BORLAND         - Required if using Borland C/C++
*/
#define _GNU_SOURCE
#include <sched.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "rtwtypes.h"
#include "rtmodel.h"
#include "rt_sim.h"
#include "rt_logging.h"
#ifdef UseMMIDataLogging
#include "rt_logging_mmi.h"
#endif
#include "rt_nonfinite.h"
#include <semaphore.h>
/* Signal Handler header */
#ifdef BORLAND
#include <signal.h>
#include <float.h>
#endif

#include "ext_work.h"
#define LOG 1
#include "HiMParaComLib.h"
#include "HiMGlobalVarLib.h"
#include "HiMVarComLib.h"
#include "HiMSched.h"
#include "AutoTestInterface.h"
#include "CurveInterface.h"
#include "sys/time.h"


#include "libSimulinkOffset.h"


//#include "AutoTestExt.h"

//#include <iostream>




/*=========*
* Defines *
*=========*/

#ifndef TRUE
#define FALSE (0)
#define TRUE  (1)
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE  1
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS  0
#endif

#define QUOTE1(name) #name
#define QUOTE(name) QUOTE1(name)    /* need to expand name    */

#ifndef RT
# error "must define RT"
#endif

#ifndef MODEL
# error "must define MODEL"
#endif

#ifndef NUMST
# error "must define number of sample times, NUMST"
#endif

#ifndef NCSTATES
# error "must define NCSTATES"
#endif

#ifndef SAVEFILE
# define MATFILE2(file) #file ".mat"
# define MATFILE1(file) MATFILE2(file)
# define MATFILE MATFILE1(MODEL)
#else
# define MATFILE QUOTE(SAVEFILE)
#endif

#define RUN_FOREVER -1.0

#define EXPAND_CONCAT(name1,name2) name1 ## name2
#define CONCAT(name1,name2) EXPAND_CONCAT(name1,name2)
#define RT_MODEL            CONCAT(MODEL,_rtModel)
#define RT_BLK              CONCAT(MODEL,_B)
#define RT_PARA             CONCAT(MODEL,_P)
/*====================*
* External functions *
*====================*/
#ifdef __cplusplus

extern "C"
{

#endif

extern RT_MODEL *MODEL(void);

extern void MdlInitializeSizes(void);
extern void MdlInitializeSampleTimes(void);
extern void MdlStart(void);
extern void MdlOutputs(int_T tid);
extern void MdlUpdate(int_T tid);
extern void MdlTerminate(void);

#ifdef __cplusplus

}
#endif

#if NCSTATES > 0
#ifdef __cplusplus

extern "C"
{

#endif
extern void rt_ODECreateIntegrationData(RTWSolverInfo *si);
extern void rt_ODEUpdateContinuousStates(RTWSolverInfo *si);
#ifdef __cplusplus

}
#endif

# define rt_CreateIntegrationData(S) \
rt_ODECreateIntegrationData(rtmGetRTWSolverInfo(S));
# define rt_UpdateContinuousStates(S) \
rt_ODEUpdateContinuousStates(rtmGetRTWSolverInfo(S));
# else
# define rt_CreateIntegrationData(S)  \
  rtsiSetSolverName(rtmGetRTWSolverInfo(S),"FixedStepDiscrete");
# define rt_UpdateContinuousStates(S) /* Do Nothing */
#endif

/*==================================*
* Global data local to this module *
*==================================*/

static struct
{
int_T stopExecutionFlag;
int_T isrOverrun;
int_T overrunFlags[NUMST];
int_T eventFlags[NUMST];
const char_T *errmsg;
} GBLbuf;

#ifdef EXT_MODE
#  define rtExtModeSingleTaskUpload(S)                          \
{                                                            \
	int stIdx;                                              \
	rtExtModeUploadCheckTrigger(rtmGetNumSampleTimes(S));   \
	for (stIdx=0; stIdx<NUMST; stIdx++) {                   \
		if (rtmIsSampleHit(S, stIdx, 0 /*unused*/)) {       \
			rtExtModeUpload(stIdx,rtmGetTaskTime(S,stIdx)); \
		}                                                   \
	}                                                       \
}
#else
#  define rtExtModeSingleTaskUpload(S) /* Do nothing */
#endif

/*=================*
* Local functions *
*=================*/

#ifdef BORLAND
/* Implemented for BC++ only*/

typedef void (*fptr)(int, int);

/* Function: divideByZero =====================================================
*
* Abstract: Traps the error Division by zero and prints a warning
*           Also catches other FP errors, but does not identify them
*           specifically.
*/
void divideByZero(int sigName, int sigType)
{
signal(SIGFPE, (fptr)divideByZero);
if ((sigType == FPE_ZERODIVIDE)||(sigType == FPE_INTDIV0))
{
	printf("*** Warning: Division by zero\n\n");
	return;
}
else
{
	printf("*** Warning: Floating Point error\n\n");
	return;
}
} /* end divideByZero */

#endif /* BORLAND */

#if !defined(MULTITASKING)  /* SINGLETASKING */

/* Function: rtOneStep ========================================================
*
* Abstract:
*      Perform one step of the model. This function is modeled such that
*      it could be called from an interrupt service routine (ISR) with minor
*      modifications.
*/
static void rt_OneStep(RT_MODEL *S)
{
	real_T tnext;

	/***********************************************
	 * Check and see if base step time is too fast *
	 ***********************************************/
	if (GBLbuf.isrOverrun++)
	{
		GBLbuf.stopExecutionFlag = 1;
		return;
	}

	/***********************************************
	 * Check and see if error status has been set  *
	 ***********************************************/
	if (rtmGetErrorStatus(S) != NULL)
	{
		GBLbuf.stopExecutionFlag = 1;
		return;
	}

	/* enable interrupts here */

	/*
	 * In a multi-tasking environment, this would be removed from the base rate
	 * and called as a "background" task.
	 */
	rtExtModeOneStep(rtmGetRTWExtModeInfo(S), rtmGetNumSampleTimes(S),
			(boolean_T *) &rtmGetStopRequested(S));

	tnext = rt_SimGetNextSampleHit();
	rtsiSetSolverStopTime(rtmGetRTWSolverInfo(S), tnext);

	MdlOutputs(0);

	rtExtModeSingleTaskUpload(S);
	#if LOG
	GBLbuf.errmsg = rt_UpdateTXYLogVars(rtmGetRTWLogInfo(S),
			rtmGetTPtr(S));
	if (GBLbuf.errmsg != NULL)
	{
		GBLbuf.stopExecutionFlag = 1;
		return;
	}
	//rt_UpdateSigLogVars(rtmGetRTWLogInfo(S), rtmGetTPtr(S));
	#endif

	MdlUpdate(0);
	rt_SimUpdateDiscreteTaskSampleHits(rtmGetNumSampleTimes(S),
			rtmGetTimingData(S), rtmGetSampleHitPtr(S), rtmGetTPtr(S));

	if (rtmGetSampleTime(S, 0) == CONTINUOUS_SAMPLE_TIME)
	{
		rt_UpdateContinuousStates(S);
	}

	GBLbuf.isrOverrun--;

	rtExtModeCheckEndTrigger();

} /* end rtOneStep */

#else /* MULTITASKING */

# if TID01EQ == 1
#  define FIRST_TID 1
# else
#  define FIRST_TID 0
# endif

/* Function: rtOneStep ========================================================
*
* Abstract:
*      Perform one step of the model. This function is modeled such that
*      it could be called from an interrupt service routine (ISR) with minor
*      modifications.
*
*      This routine is modeled for use in a multitasking environment and
*	therefore needs to be fully re-entrant when it is called from an
*	interrupt service routine.
*
* Note:
*      Error checking is provided which will only be used if this routine
*      is attached to an interrupt.
*
*/
static void rt_OneStep(RT_MODEL *S)
{
int_T i;
real_T tnext;
int_T *sampleHit = rtmGetSampleHitPtr(S);

/***********************************************
 * Check and see if base step time is too fast *
 ***********************************************/
if (GBLbuf.isrOverrun++)
{
	GBLbuf.stopExecutionFlag = 1;
	return;
}

/***********************************************
 * Check and see if error status has been set  *
 ***********************************************/
if (rtmGetErrorStatus(S) != NULL)
{
	GBLbuf.stopExecutionFlag = 1;
	return;
}
/* enable interrupts here */

/*
 * In a multi-tasking environment, this would be removed from the base rate
 * and called as a "background" task.
 */
rtExtModeOneStep(rtmGetRTWExtModeInfo(S),
		rtmGetNumSampleTimes(S),
		(boolean_T *)&rtmGetStopRequested(S));

/***********************************************
 * Update discrete events                      *
 ***********************************************/
tnext = rt_SimUpdateDiscreteEvents(rtmGetNumSampleTimes(S),
		rtmGetTimingData(S),
		rtmGetSampleHitPtr(S),
		rtmGetPerTaskSampleHitsPtr(S));
rtsiSetSolverStopTime(rtmGetRTWSolverInfo(S),tnext);
for (i=FIRST_TID+1; i < NUMST; i++)
{
	if (sampleHit[i] && GBLbuf.eventFlags[i]++)
	{
		GBLbuf.isrOverrun--;
		GBLbuf.overrunFlags[i]++; /* Are we sampling too fast for */
		GBLbuf.stopExecutionFlag=1; /*   sample time "i"?           */
		return;
	}
}
/*******************************************
 * Step the model for the base sample time *
 *******************************************/
MdlOutputs(FIRST_TID);

rtExtModeUploadCheckTrigger(rtmGetNumSampleTimes(S));
rtExtModeUpload(FIRST_TID,rtmGetTaskTime(S, FIRST_TID));

GBLbuf.errmsg = rt_UpdateTXYLogVars(rtmGetRTWLogInfo(S),
		rtmGetTPtr(S));
if (GBLbuf.errmsg != NULL)
{
	GBLbuf.stopExecutionFlag = 1;
	return;
}

//rt_UpdateSigLogVars(rtmGetRTWLogInfo(S), rtmGetTPtr(S));

MdlUpdate(FIRST_TID);

if (rtmGetSampleTime(S,0) == CONTINUOUS_SAMPLE_TIME)
{
	rt_UpdateContinuousStates(S);
}
else
{
	rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S),
			rtmGetTimingData(S), 0);
}

#if FIRST_TID == 1
rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S),
		rtmGetTimingData(S),1);
#endif

/************************************************************************
 * Model step complete for base sample time, now it is okay to          *
 * re-interrupt this ISR.                                               *
 ************************************************************************/

GBLbuf.isrOverrun--;

/*********************************************
 * Step the model for any other sample times *
 *********************************************/
for (i=FIRST_TID+1; i<NUMST; i++)
{
	/* If task "i" is running, don't run any lower priority task */
	if (GBLbuf.overrunFlags[i]) return;

	if (GBLbuf.eventFlags[i])
	{
		GBLbuf.overrunFlags[i]++;

		MdlOutputs(i);

		rtExtModeUpload(i, rtmGetTaskTime(S,i));

		MdlUpdate(i);

		rt_SimUpdateDiscreteTaskTime(rtmGetTPtr(S),
				rtmGetTimingData(S),i);

		/* Indicate task complete for sample time "i" */
		GBLbuf.overrunFlags[i]--;
		GBLbuf.eventFlags[i]--;
	}
}

rtExtModeCheckEndTrigger();

} /* end rtOneStep */

#endif /* MULTITASKING */

static void displayUsage(void)
{
	(void) printf("usage: %s -tf <finaltime> -w -port <TCPport>\n",
			QUOTE(MODEL));
	(void) printf("arguments:\n");
	(void) printf("  -tf <finaltime> - overrides final time specified in "
		"Simulink (inf for no limit).\n");
	(void) printf("  -w              - waits for Simulink to start model "
		"in External Mode.\n");
	(void) printf("  -port <TCPport> - overrides 17725 default port in "
		"External Mode, valid range 256 to 65535.\n");
}
/*========================getModelName===========================>>*/
/*size_t getExeName( char* processname)
{
char* path_end;
char processdir[1024];
memset(processdir,0,sizeof(processdir));
if(readlink("/proc/self/exe", processdir,sizeof(processdir)) <=0)
	return -1;
path_end = strrchr(processdir, '/');
if(path_end == NULL)
	return -1;
++path_end;
strcpy(processname, path_end);
printf("%s\n",processname);
printf("%s\n",path_end);
*path_end = '\0';
return (size_t)(path_end - processdir);
}*/

void getExeName(char* modelName)
{
	char buffer[256] = {0};
	char* l_str = strstr(modelName, "lib");
	int r_str = strcspn(l_str, ".");
	int l_len = strlen("lib");
	int str_len = r_str - l_len;

	strncpy(buffer, modelName + l_len, str_len);
	memset(modelName, 0, sizeof(modelName));
	strcpy(modelName, buffer);
}
/*<<*/
/*===================*
* Visible functions *
*===================*/

/* Function: main =============================================================
*
* Abstract:
*      Execute model on a generic target such as a workstation.
*/
char modelName[256];
char stopFlagName[64] = {"0"};
double stopFlag = 0;
sem_t run_sem;
unsigned int modelRunFlag = RUN_FLAG_NULL;

TY_MODEL_COM_INFO * pModelComHandle = NULL;
TY_MODEL_PARAP_COM_INFO *pParaComHandle = NULL;
struct timeval gLastTime;
void* matlabThread(void *arg)
{
#if 0
	system("shield -a 7");
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(7, &mask);

	pthread_t thread = pthread_self();
 int r = 0;
r = pthread_setaffinity_np(thread, sizeof(mask), &mask);
if(r != 0)
 {
	printf("-----------------------------------------------------------------------------------------------------------------thread binding error!%ld ret:%d err:%s.\n",thread,r,strerror(errno));
 exit(1);
 }
 int cpuID = sched_getcpu();
 printf("-----------------------------------------------------------------------------------------------------------------threadid %ld %d.\n",thread,cpuID);
#endif
	RT_MODEL *S;
	const char *status;
	int ret =0;
	real_T finaltime = -2.0;
	printf("modelName:%s\n", modelName);
	int_T oldStyle_argc;
	const char_T *oldStyle_argv[5];
	//CCurveManager *pCurveManager = CCurveManager::getInstance();

	/******************************
	 * MathError Handling for BC++ *
	 ******************************/
	#ifdef BORLAND
	signal(SIGFPE, (fptr)divideByZero);
	#endif

	long long times = 0;

	if(ret !=0)
	{
		return -1;
	}
	do
	{
		/****************************
		* Initialize global memory *
		****************************/
		(void) memset(&GBLbuf, 0, sizeof(GBLbuf));

		/************************
		* Initialize the model *
		************************/
		initVar("HiSim");
//		TY_MODEL_PARAP_COM_INFO *pParaComHandle = NULL;
		pParaComHandle = openMParaCom(modelName);


		updateParamData(pParaComHandle, modelName);

//		while (modelRunFlag == RUN_FLAG_INIT)
		{
//			usleep(100); 

	#ifdef Parameters
			paraUpate(pParaComHandle, (real_T*)&RT_PARA/*S->ModelData.defaultParam*/);
	#else
			paraUpate(pParaComHandle, NULL/*S->ModelData.defaultParam*/);
	#endif
		}
	//ZR<<
		rt_InitInfAndNaN(sizeof(real_T));
		//printf("------------1------------modelName:%s-------model gointo mdlInitializesizes;\n",modelName);
		S = MODEL();

		rtmSetModelName(S,modelName);

		if (finaltime >= 0.0 || finaltime == RUN_FOREVER)
			rtmSetTFinal(S, finaltime);
		/****************>>*********************/

//		TY_MODEL_COM_INFO * pModelComHandle = NULL;
		pModelComHandle = openMVarCom(rtmGetModelName(S));


		/****************<<*********************/
		MdlInitializeSizes();
		MdlInitializeSampleTimes();
		status = rt_SimInitTimingEngine(rtmGetNumSampleTimes(S),
				rtmGetStepSize(S), rtmGetSampleTimePtr(S), rtmGetOffsetTimePtr(
						S), rtmGetSampleHitPtr(S),
				rtmGetSampleTimeTaskIDPtr(S), rtmGetTStart(S),
				&rtmGetSimTimeStep(S), &rtmGetTimingData(S));

		if (status != NULL)
		{
			(void) fprintf(stderr,
					"Failed to initialize sample time engine: %s\n", status);
			exit(EXIT_FAILURE);
		}
		rt_CreateIntegrationData(S);
	#ifdef UseMMIDataLogging
		rt_FillStateSigInfoFromMMI(rtmGetRTWLogInfo(S),&rtmGetErrorStatus(S));
		//rt_FillSigLogInfoFromMMI(rtmGetRTWLogInfo(S),&rtmGetErrorStatus(S));
	#endif
	#if LOG
		GBLbuf.errmsg = rt_StartDataLogging(rtmGetRTWLogInfo(S),
				rtmGetTFinal(S),
				rtmGetStepSize(S),
				&rtmGetErrorStatus(S));
		if (GBLbuf.errmsg != NULL)
		{
			(void)fprintf(stderr,"Error starting data logging: %s\n",GBLbuf.errmsg);
			return(EXIT_FAILURE);
		}
	#endif
		rtExtModeCheckInit(rtmGetNumSampleTimes(S));
		rtExtModeWaitForStartPkt(rtmGetRTWExtModeInfo(S), rtmGetNumSampleTimes(
				S), (boolean_T *) &rtmGetStopRequested(S));

		(void) printf("\n** starting the model:%s **\n",rtmGetModelName(S));

		MdlStart();
		if (rtmGetErrorStatus(S) != NULL)
		{
			GBLbuf.stopExecutionFlag = 1;
		}

		/*************************************************************************
		 * Execute the model.  You may attach rtOneStep to an ISR, if so replace *
		 * the call to rtOneStep (below) with a call to a background task        *
		 * application.                                                          *
		 *************************************************************************/
		if (rtmGetTFinal(S) == RUN_FOREVER)
		{
			printf("\n**May run forever. Model stop time set to infinity.,modelRunFlag == %d**\n",modelRunFlag);
		}

	

		if (modelRunFlag == RUN_FLAG_RUNNING)
		{
			while (!GBLbuf.stopExecutionFlag && (rtmGetTFinal(S) == RUN_FOREVER
				|| rtmGetTFinal(S) - rtmGetT(S) > rtmGetT(S) * DBL_EPSILON))
			{

				rtExtModePauseIfNeeded(rtmGetRTWExtModeInfo(S),
				rtmGetNumSampleTimes(S),
				(boolean_T *) &rtmGetStopRequested(S));

				if (rtmGetStopRequested(S))
				{	
					break;
				}
        
				//printf("[%s：%s:%d]================+++++++++++====================\n",__FILE__,__FUNCTION__,__LINE__);

	#ifdef Parameters
				paraUpate(pParaComHandle, (real_T*)&RT_PARA/*S->ModelData.defaultParam*/);
	#else
				paraUpate(pParaComHandle, NULL/*S->ModelData.defaultParam*/);
	#endif
				//ret = modelSchedWaitNextFrame(pSchedHandle);
				sem_wait(&run_sem);
				//printf("[%s：%s:%d]================+++++++++++====================\n",__FILE__,__FUNCTION__,__LINE__);
#if 1
        struct timeval timeTmp;
      	gettimeofday(&timeTmp, NULL);
        long long diff = (timeTmp.tv_sec - gLastTime.tv_sec) * 1000000 + (timeTmp.tv_usec - gLastTime.tv_usec);
          if (diff > 10010)
          {
        	 // printf("------------------------------------------->>>>>>autoTestDoOneStep time s:%d us:%d ltime s:%d us:%d num:%d diff %ld.\n", timeTmp.tv_sec, timeTmp.tv_usec ,gLastTime.tv_sec, gLastTime.tv_usec ,run_sem.__align,diff);
           }
           gLastTime.tv_sec = timeTmp.tv_sec;
           gLastTime.tv_usec = timeTmp.tv_usec;
        struct timeval timeStart;
	      gettimeofday(&timeStart, NULL);
#endif 
				updateParamData(pParaComHandle, modelName);
				//pCurveManager->updateCurveInputData(modelName);
				updateCurveData(modelName);
				updateInputData(modelName);
				/*usleep(1000000)*/

				//printf("[%s：%s:%d]================+++++++++++====================\n",__FILE__,__FUNCTION__,__LINE__);
				rt_OneStep(S);
		#ifdef BlockIO
				varUpate(pModelComHandle, gHilVarTablePtr, gSilVarTablePtr,
							(void*)&RT_BLK/*S->ModelData.blockIO*/, times);
		#else
				varUpate(pModelComHandle, gHilVarTablePtr, gSilVarTablePtr,
							NULL/*S->ModelData.blockIO*/, times);
		#endif
				updateOutputData(modelName);
				//storeModelVar(modelName);
				//printf("[%s：%s:%d]================+++++++++++====================\n",__FILE__,__FUNCTION__,__LINE__);
				getVarDataByName(stopFlagName, (char*)&stopFlag);
				//stopFlag = 2;
				if (((modelRunFlag != RUN_FLAG_RUNNING)&&(modelRunFlag != RUN_FLAG_PAUSE)) || (stopFlag != 0))
				{	
					modelRunFlag = RUN_FLAG_NULL;
					//storeModelVar(modelName);
					break;
				}
				times++;
#if 0
        struct timeval timeStop;
	      gettimeofday(&timeStop, NULL);
        diff = (timeStop.tv_sec - timeStart.tv_sec) * 1000000 + (timeStop.tv_usec - timeStart.tv_usec);
        if (diff > 10000)
        {
      	  printf("matlabThread handle diff %ld.\n", diff);
         }
#endif
			}
		}
		else
		{
			/*NULL*/
		}

		//storeModelVar(modelName);
		if (!GBLbuf.stopExecutionFlag && !rtmGetStopRequested(S))
		{
			/* Execute model last time step */
			rt_OneStep(S);
		}

		/********************
		 * Cleanup and exit *
		 ********************/
	#ifdef UseMMIDataLogging
		rt_CleanUpForStateLogWithMMI(rtmGetRTWLogInfo(S));
		//rt_CleanUpForSigLogWithMMI(rtmGetRTWLogInfo(S));
	#endif
	#if LOG
		rt_StopDataLogging(MATFILE,rtmGetRTWLogInfo(S));
	#endif
		rtExtModeShutdown(rtmGetNumSampleTimes(S));

		if (GBLbuf.errmsg)
		{
			(void) fprintf(stderr, "%s\n", GBLbuf.errmsg);
			exit(EXIT_FAILURE);
		}

		if (rtmGetErrorStatus(S) != NULL)
		{
			(void) fprintf(stderr, "ErrorStatus set: \"%s\"\n",
					rtmGetErrorStatus(S));
			exit(EXIT_FAILURE);
		}

		if (GBLbuf.isrOverrun)
		{
			(void) fprintf(stderr,
					"%s: ISR overrun - base sampling rate is too fast\n",
					QUOTE(MODEL));
			exit(EXIT_FAILURE);
		}

	#ifdef MULTITASKING
		else
		{
			int_T i;
			for (i=1; i<NUMST; i++)
			{
				if (GBLbuf.overrunFlags[i])
				{
					(void)fprintf(stderr,
							"%s ISR overrun - sampling rate is too fast for "
							"sample time index %d\n", QUOTE(MODEL), i);
					exit(EXIT_FAILURE);
				}
			}
		}
	#endif
		/*******************************>>*****************************************************/
		//sleep(1);
/*
		storeModelVar(modelName);
		closeMParaCom(pParaComHandle);
		printf("close closeMParaCom\n");
		clearVar();
		closeMVarCom(pModelComHandle);
*/		
		//closeSchedHandle(pSchedHandle);

		/****************************************<<********************************************/

		MdlTerminate();
	} while (modelRunFlag != RUN_FLAG_NULL);
	//closeSchedHandle(pSchedHandle);
	return (EXIT_SUCCESS);

} /* end main */
pthread_t threadHandle;

int autoTestDoInitialization(char* pSubStep)
{
	int ret ;
	double value = 0;
	memset(modelName, 0, sizeof(modelName));
	strcpy(modelName, pSubStep);
	getExeName(modelName);
	strcpy(stopFlagName, modelName);
	strcat(stopFlagName,"/StopFlag");
        clearOffsetMap();
	modelRunFlag = RUN_FLAG_INIT;
	printf("Init c model 1 .\n");

	ret = pthread_create(&threadHandle, NULL, matlabThread, NULL);
//	usleep(100000);
	if(ret != 0)
	{
		printf("pthread_create fail.\n");
		return -1;
	}		
		//valeArray.strAttachInfo = "Init c model\n";
	ret = sem_init(&run_sem, 0, 0);
 	if(ret != 0)
	{
		return -1;
	}
	run_sem.__align = 0;
	modelRunFlag = RUN_FLAG_RUNNING;


	printf("Init c model 2 .\n");
		/*<<*/

}

int autoTestDoOneStep(long long time)
{

	


	if ((modelRunFlag != RUN_FLAG_RUNNING)&&(modelRunFlag != RUN_FLAG_PAUSE) && (modelRunFlag != RUN_FLAG_INIT))
	{	
		//storeModelVar(modelName);
		return (int)stopFlag;
	}
	sem_post(&run_sem);
	if(modelRunFlag == RUN_FLAG_INIT)
	{
		modelRunFlag = RUN_FLAG_RUNNING;
	}

	if(MANUAL_TEST_FLAG == 1)
	{
		//storeModelVar(modelName);
	}
	else /*Automated test store model information interface*/
	{
		storeModelVar(modelName);
	}

	return 0;
}

int autoTestDoDispose()
{

	/*write your code here*/
	/*>>*/
	void *status;
	modelRunFlag = RUN_FLAG_NULL;
	sem_post(&run_sem);
	pthread_join(threadHandle, &status);	

	/*need to add : If the manual test flag is 0, cancel the comment storeModelVar(modelName);*/
	if(MANUAL_TEST_FLAG == 1)
	{
		//storeModelVar(modelName);
	}
	else /*Automated test store model information interface*/
	{
		storeModelVar(modelName);
	}

	closeMParaCom(pParaComHandle);
	printf("close closeMParaCom\n");
	clearVar();
	closeMVarCom(pModelComHandle);

	printf("Dispose c model");
	/*<<*/
	return 0;
}


int autoTestSetSubStepID(char *pSubStepID)
{
	if (pSubStepID != NULL)
	{
		//valeArray.strSubStepID = pSubStepID;
	}
	return -1;
}


int autoTestGetSubStepAttachInfo(char *attachInfo)
{

	//strcpy(attachInfo, valeArray.strAttachInfo.c_str());
	return 0;
}
int main()
{
	char buffer[256];
	int num = 0;

	parseCaseBinding("/home/V1.0.0/HiAutoTestServer/bin/");
	setPresentStep("4c26662e-078f-4c79-ad7a-558193219f62");
	createDataDictionaryXml("/home/V1.0.0/HiAutoTestServer/bin/");
	autoTestDoInitialization("libuntitled.so");
	sleep(1);
	for(num = 0; num < 100; num++)
	{
		autoTestDoOneStep(num);
		sleep(5);
	}
	autoTestDoDispose();
	return 0;


}
/* EOF: grt_main.c */
