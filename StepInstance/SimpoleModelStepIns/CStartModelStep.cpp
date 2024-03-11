#include "CStartModelStep.h"
#include "AutoTestInterface.h"
#include "ICDManager.h"
#include "../HiCurveIncManager/Curve/CCurve.h"
#include "ParamData.h"
#include "RTCTimer.h"

#define PASS_LEN_4 4
#define TYPE_MODEL 0
#define TYPE_CURVE 1
#define SIMPOLE_MODEL 1
CStartModelStep::CStartModelStep()
{
	m_rtcStartFlag = 0;
	for (int i = 0; i < MAX_SUB_STEP_NUM; i++)
	{
		if (m_pDlStep[i] != NULL)
		{
			m_pDlStep[i] = NULL;
		}
	}
}
CStartModelStep::~CStartModelStep()
{
    for (int i = 0; i < MAX_SUB_STEP_NUM; i++)
	{
		delete m_pDlStep[i];
		m_pDlStep[i] = NULL;
	}
}

int CStartModelStep::parse(TiXmlElement *pStepElement, CBaseStep* pStep)
{

    int ret = 0;
    bool bRet = false;
    int modelIndex = 0;
    TiXmlElement *pMdlInfoElement = NULL;

    /*release resource*/

    /*parse router*/
    ret = CBaseSimpoleInterfaceStep::parseRouter(pStepElement);
    if(ret != 0)
    {
        return ret;
    }

    bRet = GetValue(pStepElement, "StepID", m_stepID);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Get StepID failed\n");
		ret = -1;
	}

    for(pMdlInfoElement = pStepElement->FirstChildElement(); pMdlInfoElement; pMdlInfoElement = pMdlInfoElement->NextSiblingElement())
    {
        string tmpModelName = "";
        bRet = GetValue(pMdlInfoElement, "ModelName", tmpModelName);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get modelName failed\n");
            return -1;
        }
        strcpy(m_modelNameValue.modelName[modelIndex], tmpModelName.c_str());

        m_pDlStep[modelIndex] = new CCustomDlStep();
		ret = m_pDlStep[modelIndex]->parse(pMdlInfoElement, this);
		if (ret != 0)
		{
			REPORT(REPORT_ERROR, "modelIndex %d parse failed\n", modelIndex);
			continue;
		}

        modelIndex++;

    }
    m_modelNameValue.modelNumber = modelIndex;
	m_subStepNum = modelIndex;
    	REPORT(REPORT_INFO, "---------------------------------------m_subStepNum %d \n", m_subStepNum);
	if(m_subStepNum == 0)
	{
		m_type = TYPE_CURVE;
	}
	else
	{
		m_type = TYPE_MODEL;
	}

    REPORT(REPORT_INFO, "parse\n");
    return ret;
}

void* semPostSimpoleHandler(void* pParam)
{
	CStartModelStep* pCStartModelStep = (CStartModelStep*) pParam;
	for (int subStepNo = 0; subStepNo < pCStartModelStep->m_subStepNum; subStepNo++)
	{
		//int modelStatus = pCStartModelStep->m_pDlStep[subStepNo]->getStatus();

		//if(modelStatus == STEP_RUN)
		//{
			//REPORT(REPORT_INFO, "semPostHandler subStepNo %d , m_subStepNum:%d,addr:%x\n", subStepNo, pCStartModelStep->m_subStepNum,pCStartModelStep->m_pDlStep[subStepNo]);
			sem_post(&pCStartModelStep->m_pDlStep[subStepNo]->m_sem);
		//}
	}

	CCurveManager *pCurveManager = CCurveManager::getInstance();
	TY_CURVE_BIND_MAP::iterator iterBind = pCurveManager->m_curveBindMap.find(pCStartModelStep->m_stepID);
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


void *threadWaitModelRun(void *pParam)
{
	CStartModelStep *pCStartModelStep = (CStartModelStep *)pParam;
	/*only model*/
	//printf("[%s:%s:%d], ---addr:0x%x\n",__FILE__, __FUNCTION__, __LINE__, pCStartModelStep);
	while (pCStartModelStep->m_stepStatus == STATUS_RUNNING )
	{
		int stepIndex = 0 ;
		int stepFinishedCnt = 0;
		if(gSimpoleModelStatus == 1)
		{
			for (stepIndex; stepIndex < pCStartModelStep->m_subStepNum; stepIndex++)
			{
				pCStartModelStep->m_pDlStep[stepIndex]->stop();
				printf("[%s:%s:%d]--> -------change success status\n", __FILE__, __FUNCTION__, __LINE__);
				pCStartModelStep->m_stepStatus = STATUS_SUCCESS;
			}
		}
		else
		{
			for (stepIndex; stepIndex < pCStartModelStep->m_subStepNum; stepIndex++)
			{	
				if (pCStartModelStep->m_pDlStep[stepIndex]->getStatus() != STEP_RUN)
				{
					stepFinishedCnt++;
				}
			}
			if (stepFinishedCnt == pCStartModelStep->m_subStepNum)
			{
				//sleep(1);
				pCStartModelStep->m_stepStatus = STATUS_SUCCESS;
			}
			//printf("stepFinishedCnt:%d, pCStartModelStep->m_subStepNum:%d\n", stepFinishedCnt, pCStartModelStep->m_subStepNum);
		}
		usleep(100000);
	}
	//REPORT(REPORT_DEBUG, "\n Step run finsh \n");

	//REPORT(REPORT_DEBUG, "\n ===========================model close====threadWaitComplete=========================== \n");
	CCurveManager *pCurveManager = CCurveManager::getInstance();
	TY_CURVE_BIND_MAP::iterator iterBind;
	iterBind = pCurveManager->m_curveBindMap.find(pCStartModelStep->m_stepID);
	if(iterBind != pCurveManager->m_curveBindMap.end())
	{
		TY_CURVE_MAP::iterator iterCurve = iterBind->second.begin();
		for( ; iterCurve != iterBind->second.end(); iterCurve++)
		{
			(iterCurve->second)->stop();
		}
	}
	//printf("[%s:%s:%d]--> model not run, m_stepStatus:%d, addr:0x%x\n", __FILE__, __FUNCTION__, __LINE__,pCStartModelStep->m_stepStatus, pCStartModelStep);
	pCStartModelStep->stop();
	pCStartModelStep->m_stepStatus = STATUS_SUCCESS;
	//sleep(1);
	return NULL;
}

int CStartModelStep::init()
{

    m_stepStatus = STATUS_READY;
    REPORT(REPORT_INFO, "init ok \n");
    return 0;
}

int CStartModelStep::setStepValue(int frequency)
{
	int num = 0, ret = 0;

	for(num = 0; num < m_subStepNum; num++)
	{
		m_pDlStep[num]->m_stepValue = m_pDlStep[num]->m_periodMs / frequency;
		REPORT(REPORT_INFO,"uuid:%s set StepValue:%d \n", m_stepID.c_str(), m_pDlStep[num]->m_stepValue);
	}
	return ret;
}

int CStartModelStep::run()
{
    int ret = 0;
    m_stepStatus = STATUS_RUNNING;
   
    setPresentStep((char*)m_stepID.c_str());
    int periodMs = 0;
	ret = getRunCycle(&periodMs, SIMPOLE_MODEL);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "getRunCycle fail\n ");
		return -1;
	}

//	setStepValue(periodMs);
    //init rtcTimer
    if (m_rtcStartFlag == 0)
	{
		m_rtcStartFlag = 1;
		
		ret = rtcInit((1000*periodMs), semPostSimpoleHandler, this);
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
    // 
    ret = startModel(m_modelNameValue, m_addr);
    if(ret != 0)
    {
	    REPORT(REPORT_ERROR, "startModel faild\n");
    }

    //Start the data forwarding service
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

    for (int subStepNo = 0; subStepNo < m_subStepNum; subStepNo++)
    {
	    //log>>
	    char info[1024] = { 0 };
	    sprintf(info, "begin sub step no.%d \n", subStepNo);
	    m_msg = string(info);
	    STEPLOG(LOG_INFO, m_msg);
	    //log>>
	    ret = this->m_pDlStep[subStepNo]->simpoleRun();
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
	    REPORT(REPORT_ERROR, "rtcStart error\n ");
	    m_stepStatus = STATUS_ERROR;
	    return -1;
    }
    REPORT(REPORT_INFO, "start rtc, startModelStepAddr0x%x\n", this);
    m_stepStatus = STATUS_RUNNING;
    gSimpoleModelStatus = 0;
    int threadStatus = pthread_create(&m_modelRunThreadId, NULL, threadWaitModelRun, this);
    if (0 != threadStatus)
    {
	    REPORT(REPORT_ERROR, "pthread_create failed.\n");
	    return -1;
    }

    m_msg = "Success";
    //m_stepStatus = STATUS_SUCCESS;
    REPORT(REPORT_INFO, "run\n");
    return 0;
}


int CStartModelStep::stop()
{
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

	}
	//>>modify by WC 20230915
	rtcStop();	
	rtcDispose();
	//sleep(2);
	m_rtcStartFlag = 0;
	m_stepStatus = STATUS_SUCCESS;
	printf("m_stepStatus = STATUS_SUCCESS;\n");
	REPORT(REPORT_INFO, "------wc--------------------------CStartModelStep--------------actually-stop, addr:0x%x\n",this);
	//<<wc
	gStartModelFlag = 1;
	return 0;
}

string CStartModelStep::getAttachInfo()
{

	//char data[32];
	//sprintf(data, "%lf", this->m_dataDouble);

	// string retString = string(data);
	string retString = "";
	return retString;
	//return (string)NULL;
}


