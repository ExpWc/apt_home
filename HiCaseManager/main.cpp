#include <string>
#include "CaseManager.h"
#include "parseXmlApi.h"
#include "ICDManager.h"

const char xmlFileName[] = "CaseBinding.xml";

#define TEST_MAIN
#ifdef TEST_MAIN
int main()
{
    int ret = 0;
    int caseStatus = -1;
    int stepStatus = -1;
    string str;
    str = string(GetXmlData(xmlFileName));
#if 1
    CICDManager *pICDManager = CICDManager::getInstance();//<<
    pICDManager->init(str);
#endif

    CCaseManager * pCaseManager = new CCaseManager();//<<
    ret = pCaseManager->init(str);//<<
    if(ret != 0)
    {
        printf("main init failed\n");
    }
    else
    {
        printf("main init success\n");
    }

    TY_TEST_EXECUTE testExecute;
    testExecute.testExecuteNum = 1;
    testExecute.interval = 0;
    memset(testExecute.caseExecute[0].caseID, 0, 64);
    sprintf(testExecute.caseExecute[0].caseID, "68c74cee-958d-4e7e-a87e-f0f699c0bbca");
    testExecute.caseExecute[0].caseExecuteTimes = 1;
    testExecute.caseExecute[0].interval = 0;
    caseStatus = pCaseManager->getCaseStatus(testExecute.caseExecute[0].caseID);
    printf("--->>>main caseStatus:%d\n", caseStatus);

    ret = pCaseManager->start(testExecute);//<<
    if(ret != 0)
    {
        printf("main run failed\n");
    }
    else
    {
        printf("main run success\n");
    }
#if 0
    ret = pCaseManager->start(testExecute);//<<
    if(ret != 0)
    {
        printf("main run failed\n");
    }
    else
    {
        printf("main run success\n");
    }
#endif
#if 1
    caseStatus = pCaseManager->getCaseStatus(testExecute.caseExecute[0].caseID);
    printf("--->>>main caseStatus:%d\n\n", caseStatus);
    int targetTimes = 5;
    int times = 0;
    int runFlag = 1;
    while(runFlag)
    {
        if(times == targetTimes)
        {
            runFlag = 0;
        }
#if 0
        string stepID1("77777777-4a81-4ccc-bebb-f0625d096263");
        string stepID2("77777777-4a81-4ccc-bebb-f0625d096264");//send
        string stepID3("77777777-4a81-4ccc-bebb-f0625d096266");//wait
        // stepStatus = pCaseManager->getStepStatus(testExecute.caseExecute[0].caseID, stepID1);
        // printf("--->>>main stepStatus1:%d\n", stepStatus);
        //stepStatus = pCaseManager->getStepStatus(testExecute.caseExecute[0].caseID, stepID2);
        printf("--->>>main stepStatus2:%d\n", stepStatus);
        //stepStatus = pCaseManager->getStepStatus(testExecute.caseExecute[0].caseID, stepID3);
        printf("--->>>main stepStatus3:%d\n", stepStatus);
        caseStatus = pCaseManager->getCaseStatus(testExecute.caseExecute[0].caseID);
        printf("--->>>main caseStatus:%d\n\n", caseStatus);
#endif
        sleep(1);
        times++;
    }
#endif
    printf("main excute finish.\n"); 

    //pICDManager->init(str);
    return 0;
}
#endif

