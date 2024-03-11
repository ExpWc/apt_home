#pragma once
#include "CBaseSimpoleInterfaceStep.h"
#include "HardwareManager.h"


class CLoginStep : public CBaseSimpoleInterfaceStep
{
public:
    CLoginStep();
    ~CLoginStep();
public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep* pStep);
    virtual int init();
    virtual int run();
    virtual string getAttachInfo();

public:

private:
    //double  m_dataDouble;

};
