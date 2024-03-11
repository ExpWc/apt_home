#pragma once
#include "BaseCommunicationStep.h"

// typedef struct _router_
// {
//     string command;
//     string caseID;//
// 	string stepID;
// 	string icdID;
//     string busType;
// 	unsigned int boardNo;
//     unsigned int chnNo;
// } TY_ROUTER;
// typedef map<string, TY_ROUTER> TY_ROUTER_MAP;

extern "C"  CBaseStep* CreateStep();

class CSendStep : public CBaseCommunicationStep
{
public:
    CSendStep();
    ~CSendStep();
public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep* pStep);
    virtual int init();
    virtual int run();
    //virtual int stop();???????????

private:
    //int paserRouter(TiXmlElement *pStepElement);

    //TY_ROUTER m_sendRouter;

};