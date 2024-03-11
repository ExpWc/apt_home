#include"Case.h"
#include "StepFactory.h"
#include "AutoTestInterface.h"
#include "CStorageManager.h"
int MANUAL_TEST_FLAG;

CCase::CCase(string caseID, int caseNo)
{
	m_caseID = caseID;
	m_caseNo = caseNo;
}
CCase::~CCase()
{
	/*release resource*/
	TY_STEPID_MAP::iterator it;
	for(it = m_stepIDMap.begin(); it != m_stepIDMap.end(); it++)
	{
		CBaseStep *pBaseStep = it->second;
		if(pBaseStep != NULL)
		{
			delete pBaseStep;
		}
	}
	m_stepIDMap.clear();
	m_stepExecuteVec.clear();
	/*
	if(MANUAL_TEST_FLAG == 0)
	{
		modelCard.clear();
	}
	*/
	/*release resource*/
}

int CCase::init(TiXmlElement *pCaseElement)
{
	//init para>>
	m_caseStatus = STATUS_READY;
	//init para>>
	m_stepExecuteNo = 0;
	/*release resource*/
	TY_STEPID_MAP::iterator it;
	for(it = m_stepIDMap.begin(); it != m_stepIDMap.end(); it++)
	{
		CBaseStep *pBaseStep = it->second;
		if(pBaseStep != NULL)
		{
			delete pBaseStep;
		}
	}
	m_stepIDMap.clear();
	m_stepExecuteVec.clear();
	/*
	if(MANUAL_TEST_FLAG == 0)
	{
		modelCard.clear();
	}
	*/
	/*release resource*/

	bool bRet = false;
	TiXmlElement *pStepElement = NULL;
	string tmpId = "";
	string tmpCommand = "";
	/*get case Name*/
	bRet = GetValue(pCaseElement, "CaseID", m_caseID);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Node is not valid\n");
	}
	/*get case Name*/
	bRet = GetValue(pCaseElement, "Name", m_caseName);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Node is not valid\n");
	}
	/*get step StepNum*/
	string tmpStepNum = "";
	bRet = GetValue(pCaseElement, "StepNum", tmpStepNum);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Node is not valid\n");
	}
	m_stepNum = atoi(tmpStepNum.c_str());
	//REPORT(REPORT_INFO, "%d %s %s\n", m_stepNum, m_caseName.c_str(), m_caseID.c_str());

	pStepElement = pCaseElement->FirstChildElement();
	if (pStepElement == NULL)
	{
		REPORT(REPORT_ERROR, "Init.xml can't find the node of step\n");
	}
	for (pStepElement = pCaseElement->FirstChildElement(); pStepElement; pStepElement
			= pStepElement->NextSiblingElement())
	{
		tmpId = "";
		/*get step id*/
		bRet = GetValue(pStepElement, "StepID", tmpId);
		if (!bRet)
		{
			REPORT(REPORT_ERROR, "Get StepID failed\n");
			continue;
		}
		bRet = GetValue(pStepElement, "Command", tmpCommand);
		if (!bRet)
		{
			REPORT(REPORT_ERROR, "Get Command failed\n");
			continue;
		}
		CBaseStep* pStep = CStepFactory::createStep(tmpCommand);
		//CBaseStep* pStep = CStepFactory::createStep(tmpCommand);
		//REPORT(REPORT_ERROR, "CStepFactory create %s success.\n", tmpCommand.c_str());
		if (pStep == NULL)
		{
			REPORT(REPORT_ERROR, "Create command:%s faild.\n", tmpCommand.c_str());
			continue;
		}
		else
		{
			int ret = pStep->parse(pStepElement, pStep);
			if(ret != 0)
			{

#ifdef AUTO_LOG
				//log>>
				LOG(LOG_INFO, "Parse step:%s failed.\n", tmpId.c_str());
				//log<<
#endif
				continue;
			}
			else
			{
				insertStepMap(pStep);
				m_stepExecuteVec.push_back(pStep);
				ret = pStep->init();
				if(ret != 0)
				{
					REPORT(REPORT_ERROR, "Init step failed.\n");
					continue;
				}   
			}
		}
	}
	parseCaseBinding("/home/V1.0.0/HiAutoTestServer/bin/");

	//    parseCurveConfig("/home/V1.0.0/HiAutoTestServer/bin/CurveConfig.xml");
	REPORT(REPORT_INFO, "Case init.\n");
	return 0;
}
int CCase::run()
{
	int ret = 0;

	char pPreCaseID[64] = {"0"};
	getPresentCase(pPreCaseID);	

	CStorageManager *pStorageManager = CStorageManager::getInstance();
	if(MANUAL_TEST_FLAG == 0)
	{
		pStorageManager->startStorage(pPreCaseID);
	}
	m_caseStatus = STATUS_RUNNING;
	for (unsigned int stepNo = 0; stepNo < m_stepExecuteVec.size(); stepNo++)
	{
		ret = m_stepExecuteVec[stepNo]->reset();
	}

	REPORT(REPORT_INFO, "m_stepExecuteVec.size() %d\n", m_stepExecuteVec.size());
	for (unsigned int stepNo = 0; stepNo < m_stepExecuteVec.size(); stepNo++)
	{
		preStepID = m_stepExecuteVec[stepNo]->m_stepID;
		printf("=======================stepNo %d=========size:%d==================\n", stepNo,m_stepExecuteVec.size());
		m_stepExecuteNo = stepNo + 1;


		ret = m_stepExecuteVec[stepNo]->run();
		if (ret != 0)
		{
			REPORT(REPORT_ERROR, "Step execute failed.\n");
			m_caseStatus = STATUS_FAILED;
		}
		/*
		   if(stepNo == 20 || stepNo == 22)
		   {
		   REPORT(REPORT_ERROR, "Step execute status:%d.\n", m_caseStatus);
		   }
		   */

		if(m_caseStatus == STATUS_READY)
		{
			REPORT(REPORT_ERROR, "Step execute stop.\n");
			//log>>
			LOG(LOG_INFO, "Step execute stop.stepNo:%d\n", m_stepExecuteNo);
			//log<<
			break;
		}
	}
	if(MANUAL_TEST_FLAG == 0)
	{
		pStorageManager->stopStorage();
	}

	if(m_caseStatus != STATUS_FAILED)
	{
		m_caseStatus = STATUS_SUCCESS;
	}  
	REPORT(REPORT_INFO, "Case run finish.m_caseStatus:%d\n",m_caseStatus);
	return 0;
}
int CCase::stop()
{
	int ret =0;


	//>> Added close step implementation by WC
	/* */
	for(unsigned int stepNo = 0; stepNo < m_stepExecuteNo; stepNo++)
	{
		if( m_stepExecuteVec[stepNo]->m_stepStatus == STATUS_RUNNING)
		{
			ret = m_stepExecuteVec[stepNo]->stop();
			if(ret != 0)
			{
				REPORT(REPORT_ERROR, "model stop error ret：%d.\n", ret);
			}
		}
	}
	//<<
	/*
	   for(vector<CBaseStep*>::iterator it = m_stepExecuteVec.begin(); it != m_stepExecuteVec.end(); it++ )
	   {
	   if(NULL != *it)
	   {
	   delete *it;
	 *it = NULL;
	 }
	 }
	 */
	/*  -------modify by wc 20240305
	TY_STEPID_MAP::iterator it;
	for(it = m_stepIDMap.begin(); it != m_stepIDMap.end(); it++)
	{
		CBaseStep *pBaseStep = it->second;
		if(pBaseStep->m_command == "start")
		{
			CCustomStep *pCustomStep = (CCustomStep*)pBaseStep;
			for (int i = 0; i < MAX_SUB_STEP_NUM; i++)
			{
				
				if (pCustomStep->m_pDlStep[i] != NULL)
				{
					delete pCustomStep->m_pDlStep[i];
					pCustomStep->m_pDlStep[i] = NULL;
				}
			}
		}
		if(pBaseStep->m_command == "DeviceCTLStep")
		{
			CBaseDeviceCTLStep *pBaseDeviceCTLStep = (CBaseDeviceCTLStep *)pBaseStep;
			if(pBaseDeviceCTLStep->m_pSubStep != NULL)
			{
				delete pBaseDeviceCTLStep->m_pSubStep;
				pBaseDeviceCTLStep->m_pSubStep = NULL;
			}
		}

		if(pBaseStep != NULL)
		{
			delete pBaseStep;
		}
	}
	*/
	m_stepIDMap.clear();
	m_stepExecuteVec.clear();
	/*
	if(MANUAL_TEST_FLAG == 0)
	{
		modelCard.clear();
	}
	*/
	m_caseStatus = STATUS_READY;


	return 0;
}
int CCase::insertStepMap(CBaseStep* pStep)
{
	TY_STEPID_MAP::iterator pos = m_stepIDMap.find(pStep->m_stepID);
	if(pos == m_stepIDMap.end())
	{
		m_stepIDMap.insert(std::make_pair(pStep->m_stepID, pStep));
		return 0;
	}
	return -1;
}
CBaseStep* CCase::getStep(string stepID)
{
	CBaseStep* pBaseStep = NULL;
	TY_STEPID_MAP::iterator pos = m_stepIDMap.find(stepID);
	if(pos == m_stepIDMap.end())
	{
		REPORT(REPORT_ERROR, "stepID:%s is not exsit.\n", stepID.c_str());
	}
	else
	{
		pBaseStep = pos->second;
	}
	return pBaseStep;
}
int CCase::getStatus()
{
	return m_caseStatus;
}
