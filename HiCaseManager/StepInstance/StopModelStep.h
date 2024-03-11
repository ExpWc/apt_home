#pragma once
#include "BaseStep.h"

class CStopModelStep : public CBaseStep
{
public:
    CStopModelStep();
    ~CStopModelStep();

public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep *pStep);
    virtual int init();
    virtual int run();
    virtual int stop();

public:
    string m_dllName;
};
