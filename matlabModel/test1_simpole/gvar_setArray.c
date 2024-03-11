/*
 * sfuntmpl_basic.c: Basic 'C' template for a level 2 S-function.
 *
 *  -------------------------------------------------------------------------
 *  | See matlabroot/simulink/src/sfuntmpl_doc.c for a more detailed template |
 *  -------------------------------------------------------------------------
 *
 * Copyright 1990-2002 The MathWorks, Inc.
 * $Revision: 1.27.4.2 $
 */


/*
 * You must specify the S_FUNCTION_NAME as the name of your S-function
 * (i.e. replace sfuntmpl_basic with the name of your S-function).
 */

#define S_FUNCTION_NAME  gvar_setArray
#define S_FUNCTION_LEVEL 2

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"
#ifndef MATLAB_MEX_FILE
#include "libSimulinkOffset.h"
#include "HiMGlobalVarLib.h"
#endif

#define NUMBER_PARAMETER 2
#define OFFSET_ARG              ssGetSFcnParam(S,0)
#define LENGTH_ARG              ssGetSFcnParam(S,1)


#define NO_I_WORKS              (3)/*0:offsetFlag ,1��offset��2��varLength*/
#define OFFSET_FLAG_POS 0
#define OFFSET_POS      1
#define VAR_LENGTH_POS  2
/* Error handling
 * --------------
 *
 * You should use the following technique to report errors encountered within
 * an S-function:
 *
 *       ssSetErrorStatus(S,"Error encountered due to ...");
 *       return;
 *
 * Note that the 2nd argument to ssSetErrorStatus must be persistent memory.
 * It cannot be a local variable. For example the following will cause
 * unpredictable errors:
 *
 *      mdlOutputs()
 *      {
 *         char msg[256];         {ILLEGAL: to fix use "static char msg[256];"}
 *         sprintf(msg,"Error due to %s", string);
 *         ssSetErrorStatus(S,msg);
 *         return;
 *      }
 *
 * See matlabroot/simulink/src/sfuntmpl_doc.c for more details.
 */

/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    /* See sfuntmpl_doc.c for more details on the macros below */
    int ii;
    char  path[1024];
    char modelName[1024];
    char* path_end;
    int offsetFlag;
    int offset;
	int varLength;
    int i;
	int varMode;


    ssSetNumSFcnParams(S, NUMBER_PARAMETER);  /* Number of expected parameters */



    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

    if (!ssSetNumInputPorts(S, 1)) return;
    //ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED); 
    //ssSetInputPortDataType(S, 0, DYNAMICALLY_SIZED);
     
    ssSetInputPortDimensionInfo( S, 0, DYNAMIC_DIMENSION);   //TST
    ssSetInputPortDataType(S, 0, SS_UINT8);
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortRequiredContiguous(S, 0, 1);

    //ssSetInputPortRequiredContiguous(S, 0, true); /*direct input signal access*/
    /*
     * Set direct feedthrough flag (1=yes, 0=no).
     * A port has direct feedthrough if the input is used in either
     * the mdlOutputs or mdlGetTimeOfNextVarHit functions.
     * See matlabroot/simulink/src/sfuntmpl_directfeed.txt.
     */
    //ssSetInputPortDirectFeedThrough(S, 0, 1);

    if (!ssSetNumOutputPorts(S, 0)) return;
   /* ssSetOutputPortWidth(S, 0, 1);*/

    ssSetNumSampleTimes(S, 1);
    ssSetNumRWork(S, 0);
    ssSetNumIWork(S, NO_I_WORKS);
    ssSetNumPWork(S, 0);
    ssSetNumModes(S, 0);
    ssSetNumNonsampledZCs(S, 0);

    /* Specify the sim state compliance to be same as a built-in block */
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);

    ssSetOptions(S, 0);

#if 1  //TST
#ifndef MATLAB_MEX_FILE
    strcpy(modelName, S->path);
    path_end = strchr(modelName, '/');
    *path_end = '\0';
#ifndef VAR_MODE
    offsetFlag = getVarOffsetConfig(modelName, S->path, &offset, &varLength);
#else
	offsetFlag = getVarModeConfig(modelName, S->path, &offset, &varLength, &varMode);
	//printf("--gvar_get-->offset%d varMode%d\n",offset,varMode);
	setVarMode(offset, varMode);
#endif
#ifdef MODEL_UPDATE
	setSMMode(offset,0);
    double initVar = 0;
    setVarDataP(offset,(char *)&initVar,varLength,1);
#else
	setVarData(offset,(char *)&initVar,varLength);
#endif    
    ssSetIWorkValue(S, OFFSET_FLAG_POS, offsetFlag);
    ssSetIWorkValue(S, OFFSET_POS, offset);
	ssSetIWorkValue(S, VAR_LENGTH_POS, varLength);

#endif
    
   

    printf("====>>gvar_set path:%s\n", S->path);
    printf("====>>gvar_set modelName:%s\n", modelName);
    printf("====>>gvar_set offset:%d ret:%d varlen:%d\n", offset,offsetFlag,varLength);
    // add by dzy for read xml    
 #endif //TST
}



/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);

}



#define MDL_INITIALIZE_CONDITIONS   /* Change to #undef to remove function */
#if defined(MDL_INITIALIZE_CONDITIONS)
  /* Function: mdlInitializeConditions ========================================
   * Abstract:
   *    In this function, you should initialize the continuous and discrete
   *    states for your S-function block.  The initial states are placed
   *    in the state vector, ssGetContStates(S) or ssGetRealDiscStates(S).
   *    You can also perform any other initialization activities that your
   *    S-function may require. Note, this routine will be called at the
   *    start of simulation and if it is present in an enabled subsystem
   *    configured to reset states, it will be call when the enabled subsystem
   *    restarts execution to reset the states.
   */
  static void mdlInitializeConditions(SimStruct *S)
  {
  }
#endif /* MDL_INITIALIZE_CONDITIONS */



#define MDL_START  /* Change to #undef to remove function */
#if defined(MDL_START) 
  /* Function: mdlStart =======================================================
   * Abstract:
   *    This function is called once at start of model execution. If you
   *    have states that should be initialized once, this is the place
   *    to do it.
   */
  static void mdlStart(SimStruct *S)
  {
  	
  }
#endif /*  MDL_START */



/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block.
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    int offsetFlag;
    int varLength =8;
    int offset  = 0;
    int i;
    
    //const uint8_T *u = (const uint8_T*) ssGetInputPortSignal(S,0);
    unsigned char *data;
    data = (unsigned char*) ssGetInputPortSignal(S,0);
    varLength = strlen(data);
   
    varLength = ssGetInputPortWidth(S,0);
#if 0
   for( i = 0; i < varLength; i++)
   {
        printf("%02x  ", data[i]);
   }
   printf("\n");
#endif
   
    if(varLength<1)
    {
       varLength = 1;
    }
   // printf("====>>gvar_get varlen:%d  \n",varLength);
    
    #ifndef MATLAB_MEX_FILE
    
    offsetFlag = ssGetIWorkValue(S, OFFSET_FLAG_POS);

    if(offsetFlag == OFFSET_CONFIG_OK)
     {
     	 offset = ssGetIWorkValue(S, OFFSET_POS);
		 varLength = ssGetIWorkValue(S, VAR_LENGTH_POS);
#ifdef MODEL_UPDATE
         setVarDataP(offset,(char *)data,varLength,1);
#else
		 setVarData(offset,(char *)data,varLength);
#endif
		 
     }
      else if((offsetFlag== VAR_CONFIG_NOT_FOUND)||(offsetFlag == VAR_CONFIG_OFFSET_NOT_FOUND)||(offsetFlag ==VAR_OFFSET_NOT_CONFIG))
     {

     }
     else
     {
#ifdef MODEL_UPDATE	     
         setVarDataP(offset,(char *)data,varLength,1);
#else
		 setVarData(offset,(char *)data,varLength);
#endif
     }
    
    #endif
    
}



#define MDL_UPDATE  /* Change to #undef to remove function */
#if defined(MDL_UPDATE)
  /* Function: mdlUpdate ======================================================
   * Abstract:
   *    This function is called once for every major integration time step.
   *    Discrete states are typically updated here, but this function is useful
   *    for performing any tasks that should only take place once per
   *    integration step.
   */
  static void mdlUpdate(SimStruct *S, int_T tid)
  {
  }
#endif /* MDL_UPDATE */



#define MDL_DERIVATIVES  /* Change to #undef to remove function */
#if defined(MDL_DERIVATIVES)
  /* Function: mdlDerivatives =================================================
   * Abstract:
   *    In this function, you compute the S-function block's derivatives.
   *    The derivatives are placed in the derivative vector, ssGetdX(S).
   */
  static void mdlDerivatives(SimStruct *S)
  {
  }
#endif /* MDL_DERIVATIVES */


#if defined(MATLAB_MEX_FILE)
#define MDL_SET_INPUT_PORT_DIMENSION_INFO
/* Function: mdlSetInputPortDimensionInfo ====================================
 * Abstract:
 *    This routine is called with the candidate dimensions for an input port
 *    with unknown dimensions. If the proposed dimensions are acceptable, the
 *    routine should go ahead and set the actual port dimensions.
 *    If they are unacceptable an error should be generated via
 *    ssSetErrorStatus.
 *    Note that any other input or output ports whose dimensions are
 *    implicitly defined by virtue of knowing the dimensions of the given port
 *    can also have their dimensions set.
 */
static void mdlSetInputPortDimensionInfo(SimStruct        *S,
int_T            port,
const DimsInfo_T *dimsInfo)
{
    /* Set input port dimension */
    if(!ssSetInputPortDimensionInfo(S, port, dimsInfo)) return;
} /* end mdlSetInputPortDimensionInfo */

# define MDL_SET_OUTPUT_PORT_DIMENSION_INFO
/* Function: mdlSetOutputPortDimensionInfo ===================================
 * Abstract:
 *    This routine is called with the candidate dimensions for an output port
 *    with unknown dimensions. If the proposed dimensions are acceptable, the
 *    routine should go ahead and set the actual port dimensions.
 *    If they are unacceptable an error should be generated via
 *    ssSetErrorStatus.
 *    Note that any other input or output ports whose dimensions are
 *    implicitly defined by virtue of knowing the dimensions of the given
 *    port can also have their dimensions set.
 */
static void mdlSetOutputPortDimensionInfo(SimStruct        *S,
int_T            port,
const DimsInfo_T *dimsInfo)
{

} /* end mdlSetOutputPortDimensionInfo */

# define MDL_SET_DEFAULT_PORT_DIMENSION_INFO
/* Function: mdlSetDefaultPortDimensionInfo ====================================
 *    This routine is called when Simulink is not able to find dimension
 *    candidates for ports with unknown dimensions. This function must set the
 *    dimensions of all ports with unknown dimensions.
 */
static void mdlSetDefaultPortDimensionInfo(SimStruct *S)
{
    ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
} /* end mdlSetDefaultPortDimensionInfo */
#endif


/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
}


/*======================================================*
 * See sfuntmpl_doc.c for the optional S-function methods *
 *======================================================*/

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
