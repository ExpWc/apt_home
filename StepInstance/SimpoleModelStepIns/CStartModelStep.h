#pragma once
#include "CBaseSimpoleInterfaceStep.h"
#include "HardwareManager.h"
#include "CustomDlStep.h"

#define MAX_SUB_STEP_NUM 4
#define MAX_TIME_NAME_S 64

class CStartModelStep : public CBaseSimpoleInterfaceStep
{
public:
    CStartModelStep();
    ~CStartModelStep();

    virtual int parse(TiXmlElement *pStepElement, CBaseStep* pStep);
    virtual int init();
    virtual int run();
    virtual int stop();
    virtual string getAttachInfo();
    int setStepValue(int frequency);

public:
    CCustomDlStep* m_pDlStep[MAX_SUB_STEP_NUM];
    int m_subStepNum;
    int m_type;
    string m_stepID;
    int m_rtcStartFlag;
private:
 
    TY_MODELNAME_VALUE m_modelNameValue;
    pthread_t m_modelRunThreadId;



};
