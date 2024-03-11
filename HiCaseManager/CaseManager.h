#pragma once
#include "Case.h"

typedef map<string, CCase*> TY_CASEID_MAP;
//typedef map<string, int> TY_CASEID_STATUS_MAP;
class CCaseManager
{
public:
    enum RUNING_FLAG_E
    {
        TEST_STOP = 0, TEST_RUN
    };
public:
    CCaseManager();
    ~CCaseManager();
public:
    static CCaseManager* getInstance();

	int optManualTestOneStep(TY_STEP_STATUS & info);
    int init(string caseXml);
    int start(TY_TEST_EXECUTE testExecute);
    int stop();
    int stopStep(string caseID, string stepID);
    int getCaseStatus(string caseID);
    int getStepStatus(string caseID, string stepID, TY_STEP_STATUS & stepStatus);
    int getSubStepStatus(string caseID, string stepID, string subStepID, TY_STEP_STATUS & stepStatus);
    int getExecutiveInfo(TY_ExecutiveInfo &pInfo);
    CCase* getCase(string caseID);
public:
    static CCaseManager* m_instance;
    TY_ExecutiveInfo m_executiveInfo;
    int m_caseNum;
public:
    //CCase *m_pCase[MAX_CASE_NUM];
    TY_CASEID_MAP m_caseIdMap;
    
    vector<TY_CASE_EXECUTE> m_caseExecuteVec;
    RUNING_FLAG_E m_CaseRunningFlag;
    //TY_CASEID_STATUS_MAP m_caseIDStatus;

private:
    int insertCaseMap(CCase* pCase);
private:
    pthread_t m_caseExecuteThreadId;

};
