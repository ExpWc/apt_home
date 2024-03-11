#pragma once
#include "CBaseSimpoleInterfaceStep.h"
#include "HardwareManager.h"

#define MAX_TIME_NAME_S 64

class CCompareModelVarStep : public CBaseSimpoleInterfaceStep
{
public:
    CCompareModelVarStep();
    ~CCompareModelVarStep();

    virtual int parse(TiXmlElement *pStepElement, CBaseStep* pStep);
    virtual int init();
    virtual int run();
    virtual string getAttachInfo();

public:
    int GetParamValue();
    int GetVarValue();

public:
    double m_simpoleExpectation;
    double m_simpoleActualValue;
    double m_simpoleUpperLimit;
    double m_simpoleLowLimit;
    string m_varName;
    string m_varType;
private:
   // int m_cpuIndex;
   // int m_period;
   // char m_timeResName[MAX_TIME_NAME_S];
   TY_VAR_VALUE m_pList;



};
