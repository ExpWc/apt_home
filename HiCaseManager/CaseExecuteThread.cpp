#include "CaseExecuteThread.h"
#include "CaseManager.h"
#include "HardwareManager.h"

#include "AutoTestInterface.h"
void *threadCaseExecute(void *pParam)
{
	//int ret = 0;
	CCaseManager *pCaseManager = (CCaseManager *)pParam;
	REPORT(REPORT_INFO, "pCaseManager->m_caseNum:%d.\n", pCaseManager->m_caseNum);

    CHardwareManager * pHardwareManager = CHardwareManager::getInstance();


	while (pCaseManager->m_CaseRunningFlag == CCaseManager::TEST_RUN)
	{

		pCaseManager->m_executiveInfo.runStatus = STATUS_RUNNING;
	   
		for (unsigned int i = 0; i < pCaseManager->m_caseExecuteVec.size(); i++)
		{
			
			if(pCaseManager->m_CaseRunningFlag != CCaseManager::TEST_RUN)
			{

				break;
			}
			//printf("[%s:%s:%d]--> case start\n", __FILE__, __FUNCTION__, __LINE__);
			pHardwareManager->start();
			resetICDData();
			string caseID(pCaseManager->m_caseExecuteVec[i].caseID);
			strcpy(pCaseManager->m_executiveInfo.preCaseID, caseID.c_str());
			setPresentCase(pCaseManager->m_executiveInfo.preCaseID);
			CCase *pCase = pCaseManager->getCase(caseID);
			if(pCase == NULL)
			{
				REPORT(REPORT_ERROR, "CaseID:%s, %x is not exsit.\n", caseID.c_str(), pCase);
			}
			else
			{
				//REPORT(REPORT_INFO, "=== thread run pCase->m_caseID %s\n", pCase->m_caseID.c_str());
				pCase->run();
			}

			pHardwareManager->stop();
		}
		
		//>>wc modify by 20230915
		
		//<<wc
		pCaseManager->m_CaseRunningFlag = CCaseManager::TEST_STOP;
		pCaseManager->m_executiveInfo.runStatus = STATUS_SUCCESS;
		REPORT(REPORT_INFO, " ==========pCaseManager->m_executiveInfo.runStatus = STATUS_SUCCESS===========\n");
	}




}
