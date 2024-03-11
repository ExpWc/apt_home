#pragma once
#include "CBaseSimpoleInterfaceStep.h"
#include "HardwareManager.h"

#define MAX_TIME_NAME_S 64


class CloadModelStep : public CBaseSimpoleInterfaceStep
{
public:
    CloadModelStep();
    ~CloadModelStep();

    virtual int parse(TiXmlElement *pStepElement, CBaseStep* pStep);
    virtual int init();
    virtual int run();
    virtual string getAttachInfo();

public:
    int getStorageVar();
    int AddMonitorVar();
    int GetCurrentProject();

private:
   // int m_cpuIndex;
   // int m_period;
   // char m_timeResName[MAX_TIME_NAME_S];
    TY_SIM_CONFIG m_simConfig;
    string m_uuid;


};
