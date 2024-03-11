#include "CLoginStep.h"
#include "CloadModelStep.h"
#include "CBaseSimpoleInterfaceStep.h"
#include "CUnLoadModelStep.h"
#include "CStartModelStep.h"
#include "CStopSimpoleModelStep.h"
#include "CPauseModelStep.h"
#include "CSetModelVarStep.h"
#include "CCompareModelVarStep.h"

int gStartModelFlag;

CBaseStep* CreateStep()
{
	return new CBaseSimpoleInterfaceStep();
}

CBaseSimpoleInterfaceStep::CBaseSimpoleInterfaceStep()
{

}
CBaseSimpoleInterfaceStep::~CBaseSimpoleInterfaceStep()
{	
	if(m_pSubStep != NULL)
	{
		delete m_pSubStep;
		m_pSubStep = NULL;
	}
}

void *threadStartModelRun(void *pParam)
{
	CBaseSimpoleInterfaceStep *pCBaseSimpoleInterfaceStep = (CBaseSimpoleInterfaceStep *)pParam;
	while (pCBaseSimpoleInterfaceStep->m_stepStatus == STATUS_RUNNING )
	{
		if(gStartModelFlag == 1)
		{
			pCBaseSimpoleInterfaceStep->m_stepStatus = STATUS_SUCCESS;
			break;
		}
		usleep(10000);
	}
	return NULL;
}

int CBaseSimpoleInterfaceStep::parseRouter(TiXmlElement *pStepElement)
{
    bool bRet = false;
    //TY_ROUTER sendRouter;

    string tmpIp = "";
	bRet = GetValue(pStepElement, "ip", tmpIp);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Get ip failed\n");
		return -1;
	}
	/*if(strcmp(tmpIp.c_str(), "192.168.1.121") == 0)
	{
        	sprintf(m_addr, "http://%s:10033",tmpIp.c_str());
	}
	else if(strcmp(tmpIp.c_str(), "192.168.1.123") == 0)
	{
        	sprintf(m_addr, "http://%s:10031",tmpIp.c_str());
	}*/

	sprintf(m_addr, "http://%s:10031",tmpIp.c_str());;
	m_simpoleIp = tmpIp;

    REPORT(REPORT_INFO, "parse router addr:%s\n", m_addr);
    return 0;
}

int CBaseSimpoleInterfaceStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
		int ret = 0;
	    ret = CBaseStep::parse(pStepElement, pStep);
	    if(ret != 0)
	    {
	        return ret;
	    }

	    /*parse info*/
	    bool bRet = false;

	    string tmpCtlCmd = "";
	    bRet = GetValue(pStepElement, "CtlCmd", tmpCtlCmd);
	    if (!bRet)
	    {
	        REPORT(REPORT_ERROR, "Get CtlCmd failed\n");
	        return -1;
	    }
		m_controlModelStepName = tmpCtlCmd;
	    if(strcmp(tmpCtlCmd.c_str(), "login") == 0)
	    {
	    	m_pSubStep = new CLoginStep();
	    }
	    else if(strcmp(tmpCtlCmd.c_str(), "loadModel") == 0)
		{
	    	m_pSubStep = new CloadModelStep();
		}
		else if(strcmp(tmpCtlCmd.c_str(), "unLoadModel") == 0)
		{
	    	m_pSubStep = new CUnLoadModelStep();
		}
		else if(strcmp(tmpCtlCmd.c_str(), "startModel") == 0)
		{
	    	m_pSubStep = new CStartModelStep();
		}
		else if(strcmp(tmpCtlCmd.c_str(), "stopModel") == 0)
		{
	    	m_pSubStep = new CStopSimpoleModelStep();
		}
		else if(strcmp(tmpCtlCmd.c_str(), "pauseModel") == 0)
		{
	    	m_pSubStep = new CPauseModelStep();
		}
		else if(strcmp(tmpCtlCmd.c_str(), "setModelVar") == 0)
		{
	    	m_pSubStep = new CSetModelVarStep();
		}
		else if(strcmp(tmpCtlCmd.c_str(), "compareModelVar") == 0)
		{
	    	m_pSubStep = new CCompareModelVarStep();
		}
		else
		{
			REPORT(REPORT_ERROR, "create Simpole step type is not exist.\n");
			return -1;
		}
		printf("[%s:%s:%d]000》》》ctmcmd:%s,  m_pSubStep:0x%x\n",__FILE__,__FUNCTION__,__LINE__,tmpCtlCmd.c_str(),this);
	    ret = m_pSubStep->parse(pStepElement,pStep);
	    if(ret != 0)
	    {
	    	 REPORT(REPORT_ERROR, "device parse faild\n");
	    	 return -1;
	    }

	    return 0;
}

int	 CBaseSimpoleInterfaceStep::init()
{
	gStartModelFlag = 0;
	m_stepStatus = STATUS_READY;
	int ret = 0;
	ret = m_pSubStep->init();
	if(ret != 0)
	{
		m_stepStatus = STATUS_ERROR;
		REPORT(REPORT_ERROR, "device init faild\n");
		return -1;
	}
	REPORT(REPORT_INFO, "device init \n");
	return 0;
}

int	 CBaseSimpoleInterfaceStep::run()
{
	m_stepStatus = STATUS_RUNNING;
	if(strcmp(m_controlModelStepName.c_str(), "startModel") == 0)
	{
		int threadStatus = pthread_create(&m_startModelRunThreadId, NULL, threadStartModelRun, this);
		if (0 != threadStatus)
		{
			REPORT(REPORT_ERROR, "pthread_create failed.\n");
			return -1;
		}
	}
	int ret = 0;
	ret = m_pSubStep->run();
	if(ret != 0)
	{
		m_stepStatus = STATUS_ERROR;
		REPORT(REPORT_ERROR, "device run faild\n");
		return -1;
	}
	REPORT(REPORT_INFO, "device run \n");
	
	//m_stepStatus = STATUS_SUCCESS;
	/*modify by wzc 20240223*/
	m_stepStatus = m_pSubStep->m_stepStatus;
	return 0;
}

int CBaseSimpoleInterfaceStep::log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
		int lineNo, string msg)
{
	char tmp[1024];
	sprintf(tmp, "CaseID:%s card:%d-%d\n",
			m_router.caseID.c_str(),
			m_router.CardID,
			m_router.ChnID); 
	string msgTmp = string(tmp) + msg;
	CBaseStep::log(logType, fileName, funName, lineNo, msgTmp);
}

string CBaseSimpoleInterfaceStep::getAttachInfo()
{
	string ret;
//	REPORT(REPORT_ERROR, "device getAttachInfo start, m_pSubStep:%p\n",m_pSubStep);
	ret = m_pSubStep->getAttachInfo();
	if(ret.c_str() == NULL)
	{
		REPORT(REPORT_ERROR, "device getAttachInfo faild\n");
		return NULL;
	}
	//REPORT(REPORT_INFO, "device getAttachInfo SUccess\n");
	return ret;

}
