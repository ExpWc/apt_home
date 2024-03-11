#pragma once
#include "PublicInclude.h"
#include "BaseStep.h"
#include "CustomDlStep.h"

#define TYPE_MODEL 0
#define TYPE_CURVE 1
#define TYPE_TERMINATION 2
#define TYPE_CONTINUE 3

//#include <pthread.h>
#define MAX_SUB_STEP_NUM 4
void* threadWait(void *pParam);

class CCustomStep : public CBaseStep
{
public:
    CCustomStep();
    ~CCustomStep();

public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep *pStep);
    virtual int init();
    virtual int run();
    virtual int stop();
    virtual int reset();
    virtual int setStepValue(int frequency);
    virtual CBaseStep* getSubStep(string subStepID);
public:

    CCustomDlStep* m_pDlStep[MAX_SUB_STEP_NUM];
    int m_subStepNum;
    int m_rtcStartFlag;
    int m_type;
    int m_terminationType;
private:
    pthread_t m_waitThreadId;

};
