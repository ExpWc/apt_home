#include "StepFactory.h"
#include <dlfcn.h>
#include "CustomStep.h"
#include "StopModelStep.h"

CStepFactory::CStepFactory()
{ 

}
CStepFactory::~CStepFactory()
{

}

CBaseStep* CStepFactory::createStep(string type)
{
    string tmpLibName;

    CBaseStep* pStep = NULL;
    if(type == "send")
    {
        tmpLibName ="StepLib/libSendStep.so";
    }
    else if(type == "receive")
    {
		tmpLibName ="StepLib/libReceiveStep.so";
    }
    else if(type == "wait")
    {
		tmpLibName ="StepLib/libWaitStep.so";
        //pStep = new CWaitStep();
    }
    else if(type == "setData")
    {
		tmpLibName ="StepLib/libSetDataStep.so";
        //pStep = new CSetDataStep();
    }
    else if(type == "compare")
    {
		tmpLibName ="StepLib/libCompareStep.so";
       // pStep = new CCompareStep();
    }
    else if(type == "start")
    {
		//tmpLibName ="StepLib/libStartStep.so";
        pStep = new CCustomStep();
		return pStep;
    }
    else if(type == "DeviceCTLStep")
	{
		tmpLibName ="StepLib/libWaitStep.so";
		//pStep = new CBaseDeviceCTLStep();
	}
    else if(type == "stop")
	{
		//tmpLibName ="StepLib/libWaitStep.so";
		pStep = new CStopModelStep();
		return pStep;
	}
    else if(type == "simpole")
	{
		tmpLibName ="StepLib/libSimpoleStep.so";
		//pStep = new CBaseSimpoleInterfaceStep();
	}
    else if(type == "wiring")
	{
		tmpLibName ="StepLib/libWiringStep.so";
		//pStep = new CSwitchWiringStep();
	}
    else
    {
        REPORT(REPORT_ERROR, "create step type is not exist.\n");
    }

#ifdef _WIN32
	HMODULE libHandle = LoadLibrary(tmpLibName.c_str());
#else
	//void* libHandle = dlopen(m_config.cardLibName.c_str(), RTLD_LAZY);
	void* libHandle = dlopen(tmpLibName.c_str(), RTLD_LAZY);
#endif

	if (libHandle == NULL)
	{
		char errMsg[256];
		sprintf(errMsg, "Can't open file:%s", tmpLibName.c_str());
		REPORT(REPORT_ERROR,errMsg);
#ifndef _WIN32
		sprintf(errMsg, "Reason:%s\n", dlerror());
		REPORT(REPORT_ERROR,errMsg);
#endif
	
		return NULL;
	}
	else
	{
		REPORT(REPORT_INFO,"open file:%s success\n",
			tmpLibName.c_str());
	}

	CBaseStep * (*CreateStepFunc)();
	
#ifdef _WIN32
	CreateStepFunc = (CBaseStep *(*)())GetProcAddress(libHandle, "CreateStep");
#else
	CreateStepFunc = (CBaseStep *(*)()) dlsym(libHandle, "CreateStep");
#endif

	if (CreateStepFunc == NULL)
	{
		char errMsg[256];
#ifdef _WIN32
		sprintf(errMsg, "can't CreateStep:%s\n",
			tmpLibName.c_str());
#else
		sprintf(errMsg, "can't CreateStep:%s reason:%s\n",
			tmpLibName.c_str(), dlerror());
#endif
		REPORT(REPORT_INFO, errMsg);
		return NULL;
	}
	
	return CreateStepFunc ();

}
