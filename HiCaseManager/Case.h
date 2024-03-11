#pragma once
#include "PublicInclude.h"
#include "CommonCase.h"
#include "BaseStep.h"
#include "ErrorCode.h"
#include "HiLog.h"

typedef map<string, CBaseStep *> TY_STEPID_MAP;

class CCase
{
public:
    CCase(string caseID, int caseNo);
    ~CCase();
public:
    int init(TiXmlElement *pCaseElement);
    int run();
    int stop();
    int getStatus();
    CBaseStep* getStep(string stepID);
private:
    int insertStepMap(CBaseStep* pStep);
public:
    int m_caseStatus;
    string m_caseID;
    string m_caseName;
    int m_caseNo;
    int m_stepNum;
    string preStepID;
    TY_STEPID_MAP m_stepIDMap;
    vector<CBaseStep*> m_stepExecuteVec;
    int m_stepExecuteNo;
};
