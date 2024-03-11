#include "StopModelStep.h"
#include "AutoTestInterface.h"
#include "ParamData.h"
#include "HiCommon.h"
//#include "CustomDlStep.h"
#include "CustomStep.h"
#include "CaseManager.h"
CStopModelStep::CStopModelStep()
{
}
CStopModelStep::~CStopModelStep()
{
}
int CStopModelStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
    int ret = 0;
    ret = CBaseStep::parse(pStepElement, pStep);
    if(ret != 0)
    {
        REPORT(REPORT_ERROR, "parse failed\n");
        return ret;
    }
    bool bRet = false;
    TiXmlElement *pModelElement = NULL;
    pModelElement = pStepElement->FirstChildElement();
    if (pModelElement == NULL)
    {
        REPORT(REPORT_ERROR, "Init.xml can't find the node of model.\n");
        ret = -1;
    }
    else
    {
      bRet = GetValue(pModelElement, "Dll", m_dllName);
      if (!bRet)
      {
          REPORT(REPORT_ERROR, "Get m_dllName failed\n");
          return -1;
      }
      REPORT(REPORT_INFO, "parse m_dllName:%s\n", m_dllName.c_str());
    }
    return 0;
}
int CStopModelStep::init()
{
    m_stepStatus = STATUS_READY;
    REPORT(REPORT_DEBUG, "init\n");
    return 0;
}

int CStopModelStep::run()
{
    int ret = 0;
    char caseID[128] = {0};
    m_stepStatus = STATUS_RUNNING;
#if 0
   // REPORT(REPORT_INFO, "run-waitStep-----------------------------------------------------------------------ZR %d %s.\n",modelCard.size(),m_dllName.c_str());
   	TY_MODEL_CARD_MAP::iterator iterModel;
  	iterModel = modelCard.find(m_dllName);
  	if( iterModel != modelCard.end())
  	{
  		CCustomDlStep* pCustomDlStep = (CCustomDlStep* )iterModel->second;
        REPORT(REPORT_INFO, "run-------------------------------------------------stopmodeladdR:%x, %p-\n", pCustomDlStep, pCustomDlStep);
  		ret = pCustomDlStep->stop();
  		if(ret == -1)
  		{
			m_stepStatus = STATUS_ERROR;	
  		}
  		modelCard.erase(iterModel);
  	}
  	else
  	{
  		REPORT(REPORT_INFO, "Do not found!\n");
  		ret = -1;
    	m_stepStatus = STATUS_ERROR;
  	}
#endif

    CCaseManager * pCaseManager = CCaseManager::getInstance();
	getPresentCase(caseID);
	string strCaseID = caseID;
	CCase *pCase = pCaseManager->getCase(strCaseID);
	if(pCase == NULL)
	{
		return -1;
	}

    for (unsigned int stepNo = 0; stepNo < pCase->m_stepExecuteVec.size(); stepNo++)
	{   
       // REPORT(REPORT_WARNING, "--------stepNo:%d, m_command:%s\n", stepNo, pCase->m_stepExecuteVec[stepNo]->m_command.c_str());
		if(pCase->m_stepExecuteVec[stepNo]->m_stepStatus == STATUS_RUNNING
            && pCase->m_stepExecuteVec[stepNo]->m_command == "start")
		{
            CCustomStep *pCustomStep = (CCustomStep* )pCase->m_stepExecuteVec[stepNo];
            for (int subStepNo = 0; subStepNo < pCustomStep->m_subStepNum; subStepNo++)
            {
                if(m_dllName == pCustomStep->m_pDlStep[subStepNo]->m_dlName)
                ret = pCustomStep->m_pDlStep[subStepNo]->stop();
                if (ret != 0)
                {
                    m_stepStatus = STATUS_ERROR;
                }
            }
          
		}
	}

    m_stepStatus = STATUS_SUCCESS;
//#endif
  
    return ret;
}
int CStopModelStep::stop()
{
    m_stepStatus = STATUS_READY;
    REPORT(REPORT_INFO, "stop\n");
    return 0;
}
