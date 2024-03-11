#pragma once
#include "CBaseSimpoleInterfaceStep.h"
#include "HardwareManager.h"

#define MAX_TIME_NAME_S 64

class CPauseModelStep : public CBaseSimpoleInterfaceStep
{
public:
    CPauseModelStep();
    ~CPauseModelStep();

    virtual int parse(TiXmlElement *pStepElement, CBaseStep* pStep);
    virtual int init();
    virtual int run();
    virtual string getAttachInfo();

    
private:
   // int m_cpuIndex;
   // int m_period;
   // char m_timeResName[MAX_TIME_NAME_S];
    TY_MODELNAME_VALUE m_modelNameValue;



};
