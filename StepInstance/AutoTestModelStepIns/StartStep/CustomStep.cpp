#include "CustomStep.h"
#include "RTCTimer.h"
#include "AutoTestInterface.h"
#include "../HiCurveIncManager/Curve/CCurve.h"
#include <sys/unistd.h>

#define AUTOTEST_MODEL 0
void *threadWaitComplete(void *pParam);

CCustomStep::CCustomStep()
{
	m_rtcStartFlag = 0;
	for (int i = 0; i < MAX_SUB_STEP_NUM; i++)
	{
		m_pDlStep[i] = NULL;
	}
}
CCustomStep::~CCustomStep()
{

}
int CCustomStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
	bool bRet = false;
	int ret = 0;

	ret = CBaseStep::parse(pStepElement, pStep);
    if(ret != 0)
    {
        REPORT(REPORT_ERROR, "parse failed\n");
        return ret;
    }
	
	/*release resource*/
	for (int i = 0; i < MAX_SUB_STEP_NUM; i++)
	{
		if (m_pDlStep[i] != NULL)
		{
			delete m_pDlStep[i];
		}
	}
	/*release resource*/

	string period;

	bRet = GetValue(pStepElement, "StepID", m_stepID);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Get StepID failed\n");
		ret = -1;
	}
	string typeStr;
	bRet = GetValue(pStepElement, "Type", typeStr);//modify by ZR 20230612
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Get Type failed\n");
		ret = -1;
	}
	if(typeStr == "termination")
	{
		m_terminationType = TYPE_TERMINATION;
	}
	else if(typeStr == "continue")
	{
		m_terminationType = TYPE_CONTINUE;
	}
	/*	bRet = GetValue(pStepElement, "PeriodMs", period);
		if (!bRet)
		{
		m_type = TYPE_CURVE;
		m_subStepNum = 0;
		m_periodMs = 100;
		return 0;
		}
		m_type = TYPE_MODEL;
		m_periodMs = atoi(period.c_str());
		*/
	int subStepNo = 0;
	for (TiXmlElement* pSubStepElement = pStepElement->FirstChildElement(); pSubStepElement; pSubStepElement
			= pSubStepElement->NextSiblingElement())
	{
		m_pDlStep[subStepNo] = new CCustomDlStep();
		ret = m_pDlStep[subStepNo]->parse(pSubStepElement, this);
		if (ret != 0)
		{
			REPORT(REPORT_ERROR, "subStepNo %d parse failed\n", subStepNo);
			continue;
		}
		subStepNo++;
	}
	m_subStepNum = subStepNo;
	if(m_subStepNum == 0)
	{
		m_type = TYPE_CURVE;
	}
	else
	{
		m_type = TYPE_MODEL;
	}
	return ret;
}
void* semPostHandler(void* pParam)
{
	int count = 0;
	CCustomStep* pCustomStep = (CCustomStep*) pParam;
	for (int subStepNo = 0; subStepNo < pCustomStep->m_subStepNum; subStepNo++)
	{
		int modelStatus = pCustomStep->m_pDlStep[subStepNo]->getStatus();

		if(modelStatus == STEP_RUN || (modelStatus == STEP_STOP && count < 1))
		{
			//REPORT(REPORT_INFO, "semPostHandler subStepNo %d , m_subStepNum:%d\n", subStepNo, pCustomStep->m_subStepNum);
			sem_post(&pCustomStep->m_pDlStep[subStepNo]->m_sem);
			
		}
	}

	CCurveManager *pCurveManager = CCurveManager::getInstance();
	TY_CURVE_BIND_MAP::iterator iterBind = pCurveManager->m_curveBindMap.find(pCustomStep->m_stepID);
	if(iterBind != pCurveManager->m_curveBindMap.end())
	{
		TY_CURVE_MAP::iterator iterCurve = iterBind->second.begin();
		for( ; iterCurve != iterBind->second.end(); iterCurve++)
		{
			//printf("m_sem addr:%x\n", &(iterCurve->second->m_sem));
			sem_post(&(iterCurve->second->m_sem));
		}
	}
	return 0;
}
int CCustomStep::init()
{
	//When init XML>>
	m_stepStatus = STATUS_READY;

	REPORT(REPORT_DEBUG, "init Step number %d\n", m_subStepNum);

	return 0;
}
#if 0
void* threadWait(void *pParam)
{
	CCustomStep* pCustomStep = (CCustomStep*) pParam;
	while (pCustomStep->m_stepStatus == STATUS_RUNNING)
	{
		int i = 0;
		for (; i < pCustomStep->m_subStepNum; i++)
		{
			if (pCustomStep->m_pDlStep[i]->getStatus() == STEP_STOP)
			{
				i++;
			}
		}
		if (i == pCustomStep->m_subStepNum)
		{
			pCustomStep->m_stepStatus = STATUS_SUCCESS;
		}


		usleep(10000);
	}

}
#endif
int CCustomStep::reset()
{

	return 0;
}
int CCustomStep::setStepValue(int frequency)
{
	int num = 0, ret = 0;

	for(num = 0; num < m_subStepNum; num++)
	{
		m_pDlStep[num]->m_stepValue = m_pDlStep[num]->m_periodMs / frequency;
		REPORT(REPORT_INFO,"uuid:%s set StepValue:%d \n", m_stepID.c_str(), m_pDlStep[num]->m_stepValue);
	}
	return ret;
}
int CCustomStep::run()
{
	int ret = 0;

	setPresentStep((char*)m_stepID.c_str());
	m_stepStatus = STATUS_RUNNING;
	int periodMs = 0;
	ret = getRunCycle(&periodMs, AUTOTEST_MODEL);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "getRunCycle fail\n ");
		return -1;
	}

	setStepValue(periodMs);
	//rtctimer
	if (m_rtcStartFlag == 0)
	{
		m_rtcStartFlag = 1;

		ret = rtcInit((1000*periodMs), semPostHandler, this);
		if (ret != 0)
		{
			rtcStop();
			rtcDispose();
			m_rtcStartFlag = 0;
			REPORT(REPORT_ERROR, "rtcStart error periodMs:%d\n ", periodMs);
			m_stepStatus = STATUS_ERROR;
			return -1;
		}
		REPORT(REPORT_INFO, "rtcStart success periodMs:%d\n ",periodMs);
	}
	// start curve

	CCurveManager *pCurveManager = CCurveManager::getInstance();
	TY_CURVE_BIND_MAP::iterator iterBind = pCurveManager->m_curveBindMap.find(m_stepID);
	if(iterBind != pCurveManager->m_curveBindMap.end())
	{
		TY_CURVE_MAP::iterator iterCurve = iterBind->second.begin();
		for( ; iterCurve != iterBind->second.end(); iterCurve++)
		{
			(iterCurve->second)->setStepValue(periodMs);
			(iterCurve->second)->start();
		}
	}

	//end
	if(m_type == TYPE_CURVE)
	{
		//		m_stepStatus = STATUS_RUNNING;
		ret = rtcStart();
		if (ret != 0)
		{
			rtcStop();
			rtcDispose();
			m_rtcStartFlag = 0;
			REPORT(REPORT_ERROR, "rtcStart error periodMs:%d\n ", periodMs);
			m_stepStatus = STATUS_ERROR;
			return -1;
		}

		while(m_stepStatus == STATUS_RUNNING)
		{
			if(pCurveManager->checkCurveStatus() == 0)
			{
				m_stepStatus = STATUS_SUCCESS;
			}
			usleep(10000);
		}
	}
	else if(m_type == TYPE_MODEL)
	{

		createDataDictionaryXml("/home/V1.0.0/HiAutoTestServer/bin/");
		for (int subStepNo = 0; subStepNo < m_subStepNum; subStepNo++)
		{
			ret = m_pDlStep[subStepNo]->init();
			if (ret != 0)
			{
				//ret = -1;
				
				m_stepStatus = STATUS_ERROR;
				return -1;
			}
		}
		if(m_stepStatus == STATUS_ERROR)
		{
			return -1;
		}

		for (int subStepNo = 0; subStepNo < m_subStepNum; subStepNo++)
		{
			//log>>
			char info[1024] = { 0 };
			sprintf(info, "begin sub step no.%d \n", subStepNo);
			m_msg = string(info);
			STEPLOG(LOG_INFO, m_msg);
			//log>>
			ret = m_pDlStep[subStepNo]->run();
			if (ret != 0)
			{
				REPORT(REPORT_ERROR, "subStepNo %d run failed.\n", subStepNo);
				m_stepStatus = STATUS_ERROR;

				rtcStop();
				rtcDispose();
				return -1;
			}
		}
		sleep(1);
		ret = rtcStart();
		if (ret != 0)
		{
			rtcStop();
			rtcDispose();
			REPORT(REPORT_ERROR, "--------rtcStart error\n ");
			m_stepStatus = STATUS_ERROR;
			return -1;
		}


		if(m_terminationType == TYPE_TERMINATION)//modify by ZR 20230612
		{
			while (m_stepStatus == STATUS_RUNNING)
			{
				int stepIndex = 0 ;
				int stepFinishedCnt = 0;
				for (stepIndex; stepIndex < m_subStepNum; stepIndex++)
				{
					if (m_pDlStep[stepIndex]->getStatus() != STEP_RUN)
					{
						stepFinishedCnt++;
					}
				}
				if (stepFinishedCnt == m_subStepNum)
				{
					//sleep(1);
					m_stepStatus = STATUS_FINISH;
				}
				usleep(100000);
			}

			REPORT(REPORT_INFO, "\n ===========================Step run finsh model close=============================== \n");
			iterBind = pCurveManager->m_curveBindMap.find(m_stepID);
			if(iterBind != pCurveManager->m_curveBindMap.end())
			{
				TY_CURVE_MAP::iterator iterCurve = iterBind->second.begin();
				for( ; iterCurve != iterBind->second.end(); iterCurve++)
				{
					(iterCurve->second)->stop();
				}
			}
		}
		else
		{
			int threadStatus = pthread_create(&m_waitThreadId, NULL, threadWaitComplete, this);
			if (0 != threadStatus)
			{
				REPORT(REPORT_ERROR, "pthread_create failed.\n");
				return -1;
			}
			m_stepStatus = STATUS_RUNNING;
		}
	}
	if(m_terminationType == TYPE_TERMINATION || m_type == TYPE_CURVE)//modify by ZR 20230612
	{
		stop();
		if(m_stepStatus != STATUS_SUCCESS)
		{
			return -1;
		}
	}


	return 0;
}

int CCustomStep::stop()
{
	REPORT(REPORT_INFO, "------ZR--------------------------CCustomStep---------------stop,m_subStepNum:%d,m_stepStatus:%d\n",m_subStepNum,m_stepStatus);
	if(m_stepStatus != STATUS_SUCCESS)
	{
		
		for (int subStepNo = 0; subStepNo < m_subStepNum; subStepNo++)
		{
			if(m_pDlStep[subStepNo]->m_statusFlag == STEP_FAILED)
			{
				m_stepStatus = STATUS_FAILED;
			}
			m_pDlStep[subStepNo]->stop();
			
			
		}
		
   		//REPORT(REPORT_INFO, "------ZR--------------------------CCustomStep--------------actually-stop\n");
	}
		//>>modify by WC 20230618---->Ensure that all substeps have been executed before setting the status to STATUS_SUCCESS and stop RTC.
		rtcStop();	
		rtcDispose();
		sleep(2);
		//clearModelVarMap();
		m_rtcStartFlag = 0;
		m_stepStatus = STATUS_SUCCESS;
		REPORT(REPORT_INFO, "------wc--------------------------CCustomStep--------------actually-stop\n");
		//<<wc

	return 0;
}

CBaseStep* CCustomStep::getSubStep(string subStepID)
{
	if(!subStepID.empty())
	{
		for(int i =0; i < MAX_SUB_STEP_NUM; i++)
		{
			if(m_pDlStep[MAX_SUB_STEP_NUM] != NULL)
			{
				if(m_pDlStep[i]->m_subStepID == subStepID)
				{
					return m_pDlStep[i];
				}
			}
		}
	}
	return NULL;
}
void *threadWaitComplete(void *pParam)
{
	CCustomStep *pCustomStep = (CCustomStep *)pParam;
	/*only model*/
	while (pCustomStep->m_stepStatus == STATUS_RUNNING)
	{
		int stepIndex = 0 ;
		int stepFinishedCnt = 0;
		for (stepIndex; stepIndex < pCustomStep->m_subStepNum; stepIndex++)
		{
			if (pCustomStep->m_pDlStep[stepIndex]->getStatus() != STEP_RUN)
			{
				stepFinishedCnt++;
			}
		}
		if (stepFinishedCnt == pCustomStep->m_subStepNum)
		{
			//sleep(1);
			pCustomStep->m_stepStatus = STATUS_SUCCESS;
		}
		usleep(100000);
	}


	//REPORT(REPORT_DEBUG, "\n Step run finsh \n");
	//REPORT(REPORT_DEBUG, "\n ===========================model close====threadWaitComplete=========================== \n");
	CCurveManager *pCurveManager = CCurveManager::getInstance();
	TY_CURVE_BIND_MAP::iterator iterBind;
	iterBind = pCurveManager->m_curveBindMap.find(pCustomStep->m_stepID);
	if(iterBind != pCurveManager->m_curveBindMap.end())
	{
		TY_CURVE_MAP::iterator iterCurve = iterBind->second.begin();
		for( ; iterCurve != iterBind->second.end(); iterCurve++)
		{
			(iterCurve->second)->stop();
		}
	}
	pCustomStep->stop();
	pCustomStep->m_stepStatus = STATUS_SUCCESS;
	//REPORT(REPORT_INFO,"wait thread run over!--------------->");
}
