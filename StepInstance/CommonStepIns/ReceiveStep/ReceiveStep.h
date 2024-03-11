#pragma once
#include "BaseCommunicationStep.h"

extern "C"  CBaseStep* CreateStep();

class CReceiveStep : public CBaseCommunicationStep
{
public:
    CReceiveStep();
    ~CReceiveStep();
public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep* pStep);
    virtual int init();
    virtual int run();

private:
    //int paserRouter(TiXmlElement *pStepElement);

    //TY_ROUTER m_receiveRouter;

};