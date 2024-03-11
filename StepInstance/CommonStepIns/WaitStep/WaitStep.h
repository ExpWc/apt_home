
#pragma once
#include "BaseStep.h"

extern "C"  CBaseStep* CreateStep();

class CWaitStep : public CBaseStep
{
public:
    CWaitStep();
    ~CWaitStep();

public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep *pStep);
    virtual int init();
    virtual int run();
    virtual int stop();

public:
    long long m_timeMS;
    long long m_timeUS;
};
