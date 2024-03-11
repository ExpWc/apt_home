#include "CaseManager.h"
#include "CaseExecuteThread.h"
#include <stddef.h>
#include <unistd.h>
#include "HiCommon.h"
#include "AutoTestInterface.h"


CCaseManager *CCaseManager::m_instance = NULL;
CCaseManager *CCaseManager::getInstance()
{
    if (m_instance == NULL)
    {
        m_instance = new CCaseManager();
    }
    return m_instance;
}
CCaseManager::CCaseManager()
{
    m_CaseRunningFlag = TEST_STOP;
}
CCaseManager::~CCaseManager()
{
    /*release resource*/
    TY_CASEID_MAP::iterator it;
    for(it = m_caseIdMap.begin(); it != m_caseIdMap.end(); it++)
    {
        CCase *pCase = it->second;
        if(pCase != NULL)
        {
            delete pCase;
        }
    }
    m_caseIdMap.clear();
	m_caseExecuteVec.clear();
    /*release resource*/
}

int CCaseManager::optManualTestOneStep(TY_STEP_STATUS & info)
{
     int ret = 0;
    string CaseID;
    string StepID;
    CCase *pCase = NULL;
    CBaseStep* pStep = NULL;
    string attachInfo; // 步骤返回值

#if 0
    //-->1 获取xml文件放入string变量
    string str;
    str = string(GetXmlData("xmlwc/ManualOneStepInfo.xml"));
    //-->2 获取CaseManager实例
    CCaseManager * pCaseManager = CCaseManager::getInstance();
    ret = pCaseManager->init(str);
#endif    
    //-->3 获取CaseID及其对象地址
    TY_CASEID_MAP::iterator pos = m_caseIdMap.begin();
    if (pos != m_caseIdMap.end())
    {
        CaseID = pos->first;
        pCase = pos->second;
        //REPORT(REPORT_INFO, "m_caseId is \n", CaseID.c_str());
    }
    else
    {
        REPORT(REPORT_ERROR, "m_caseId is no find\n");
        return -1;
    }
    //-->4   
    TY_CASE_EXECUTE caseExecute;
    memcpy(caseExecute.caseID, CaseID.c_str(), CaseID.length());
    caseExecute.interval = 0;
    m_caseExecuteVec.push_back(caseExecute);
    //-->5 执行case
    pCase->run();
    //-->6 获取StepID及其对象地址
     TY_STEPID_MAP::iterator Steppos = pCase->m_stepIDMap.begin();
    if (Steppos != pCase->m_stepIDMap.end())
    {
        StepID = Steppos->first;
        pStep = Steppos->second;
        //REPORT(REPORT_INFO, "m_stepId is %s \n", StepID.c_str());
    }
    else
    {
        REPORT(REPORT_ERROR, "m_stepId is no find\n");
        return -1;
    }
    //-->7 获取步骤返回信息
    attachInfo = pStep->getAttachInfo();
    info.attachInfo = attachInfo;
    //printf("-----[%s:%s:%d]-->value:%s\n",__FILE__,__FUNCTION__,__LINE__,info.attachInfo.c_str());
    info.stepID = StepID;
    info.msg = "success";
    info.caseStepStatus = STATUS_SUCCESS;

    return 0;
}

int CCaseManager::init(string caseXml)
{
    /*release resource*/
    TY_CASEID_MAP::iterator it;
    for(it = m_caseIdMap.begin(); it != m_caseIdMap.end(); it++)
    {
        CCase *pCase = it->second;
        if(pCase != NULL)
        {
            delete pCase;
        }
    }
    m_caseIdMap.clear();
	m_caseExecuteVec.clear();
    /*release resource*/


    bool bRet = false;
    int ret = 0;
    string tmpId = "";
    string tmpName = "";
    string tmpStepNum = "";
    int caseNo = 0;


    TiXmlNode *projectNode = NULL;
    TiXmlNode *rootNode = NULL;
    TiXmlElement *pCaseElement = NULL;
    TiXmlDocument *pInitXmlManager = NULL;
    pInitXmlManager = load_xml((const char *)caseXml.c_str(), FROM_DATA);
    if (pInitXmlManager == NULL)
    {
        REPORT(REPORT_ERROR, "Xml is not valid\n");
        return -1;
    }
    else
    {
        rootNode = pInitXmlManager->RootElement();
    }
    if (rootNode == NULL)
    {
        REPORT(REPORT_ERROR, "Init.xml can't find the node\n");
        return -1;
    }
    else
    {
        projectNode = rootNode->FirstChild("caseRoot");
    }

    if (projectNode == NULL)
    {
        REPORT(REPORT_ERROR, "Init.xml can't find the node of init.\n");
        return -1;
    }
    else
    {
        pCaseElement = projectNode->FirstChildElement();
        if (pCaseElement == NULL)
        {
            REPORT(REPORT_ERROR, "Init.xml can't find the node of card.\n");
        }
        for (pCaseElement = projectNode->FirstChildElement(); pCaseElement; pCaseElement = pCaseElement->NextSiblingElement())
        {
            tmpId = "";
            /*get case id*/
            bRet = GetValue(pCaseElement, "CaseID", tmpId);

            if (!bRet)
            {
                REPORT(REPORT_ERROR, "Init.xml can't find the node of %s.\n", tmpId.c_str());
                continue;
            }
            /*get case Name*/
            tmpName = "";
            bRet = GetValue(pCaseElement, "Name", tmpName);
            if (!bRet)
            {
                REPORT(REPORT_ERROR, "Init.xml can't find the node of %s.\n", tmpName.c_str());
                continue;
            }
            REPORT(REPORT_INFO, "caseNo %d\n", caseNo);
            CCase *pCase = new CCase(tmpId, caseNo);
            //m_pCase[caseNo] = pCase;
            ret = pCase->init(pCaseElement);
            if(ret != 0)
            {
                REPORT(REPORT_ERROR, "Init case failed.\n");
                continue;
            }
            insertCaseMap(pCase);
            caseNo++;
        }
        m_caseNum = caseNo;
        REPORT(REPORT_INFO, "m_caseNum %d\n", m_caseNum);
    }
//    CLog *pLog = CLog::getInstance();
//    char filePath[256];
//	getServerPath(filePath);
//	string fullPath(filePath);
//	fullPath.append("caseLog.log");
//    pLog->setLogFileName((char*)fullPath.c_str());
    delete pInitXmlManager;
    pInitXmlManager = NULL;
    REPORT(REPORT_INFO, "=================case init caseNum:%d=====================\n", m_caseNum);
    LOG(LOG_INFO, "=================case init caseNum:%d=====================\n", m_caseNum);

    return 0;
}

int CCaseManager::start(TY_TEST_EXECUTE testExecute)
{
    if(m_CaseRunningFlag == TEST_RUN)
    {
        REPORT(REPORT_ERROR, "has start.\n");
    }
    else
    {
    	m_caseExecuteVec.clear();//modify by zr 20211122
    	char tmpStr[2048];
    	char tmp[1024];
    	sprintf(tmp, "=================case start testExecuteNum:%d=====================\n",
    			testExecute.testExecuteNum);
    	strcat(tmpStr, tmp);
        for (int num = 0; num < testExecute.testExecuteNum; num++)
        {
            m_caseExecuteVec.push_back(testExecute.caseExecute[num]);

        }

        //log>>
        LOG(LOG_INFO, tmpStr);
        //log<<


        m_CaseRunningFlag = TEST_RUN;
        int threadStatus = pthread_create(&m_caseExecuteThreadId, NULL, threadCaseExecute, this);
        if (0 != threadStatus)
        {
            REPORT(REPORT_ERROR, "pthread_create failed.\n");
            return -1;
        }
    }
    
    REPORT(REPORT_INFO, "CCaseManager start.\n");
    return 0;
}
int CCaseManager::stop()
{
    int ret = 0;
    m_CaseRunningFlag = TEST_STOP;

    for (unsigned int i = 0; i < m_caseExecuteVec.size(); i++)
	{
		string caseID(m_caseExecuteVec[i].caseID);
		CCase *pCase = getCase(caseID);
		if(pCase == NULL)
		{
			REPORT(REPORT_ERROR, "CaseID:%s, %x is not exsit.\n", caseID.c_str(), pCase);
		}
		else
		{
			pCase->stop();
		}
	}

	//clearModelMap();
    //log>>
    LOG(LOG_INFO, "CCaseManager stop.\n");
    //log<<

    REPORT(REPORT_INFO, "CCaseManager stop.\n");
    return ret;
}

int CCaseManager::stopStep(string caseID, string stepID)
{
    int ret = 0;
    CCase* pCase = getCase(caseID);
    if(pCase != NULL)
    {
    	CBaseStep* pStep = pCase->getStep(stepID);
        if(pStep != NULL)
        {
        	ret = pStep->stop();

        }
        else
        {
            ret = NOT_FOUND_STEP;
        }
    }
    else
    {
        ret = NOT_FOUND_CASE;
    }
    return ret;
}

int CCaseManager::getCaseStatus(string caseID)
{
    int status = -1;
    if(getCase(caseID) != NULL)
    {
        status = getCase(caseID)->getStatus();
    }
    else
    {
        status = NOT_FOUND_CASE;
    }
    return status;

}
int CCaseManager::getStepStatus(string caseID, string stepID, TY_STEP_STATUS & stepStatus)
{
    int status = -1;
    string attachInfo = "";
	CCase* pCase = getCase(caseID);
    if(pCase != NULL)
    {
    	CBaseStep* pStep = pCase->getStep(stepID);
        if(pStep != NULL)
        {
        	status = pStep->getStatus();
        	attachInfo = pStep->getAttachInfo();

        	stepStatus.attachInfo = attachInfo;

            //stepStatus.attachInfo = pStep->getAttachInfo();
            stepStatus.caseStepStatus = (CaseStepStatusE)status;
            if(status == STATUS_ERROR)
			{
            	stepStatus.msg = pStep->getMsg();
			}
			else
			{
				stepStatus.msg = "success";
			}
        }
        else
        {
        	stepStatus.msg = "step not found";
            status = NOT_FOUND_STEP;
        }
    }
    else
    {
    	stepStatus.msg = "case not found";
        status = NOT_FOUND_CASE;
    }
    return status;
}

int CCaseManager::getSubStepStatus(string caseID, string stepID, string subStepID, TY_STEP_STATUS & stepStatus)
{
    int status = -1;
    string attachInfo = "";
	CCase* pCase = getCase(caseID);
    if(pCase != NULL)
    {
       	CBaseStep* pStep = pCase->getStep(stepID);
        if(pStep != NULL)
        {
        	CBaseStep* pSubStep = pStep->getSubStep(subStepID);
        	if(pSubStep != NULL)
        	{
				status = pSubStep->getStatus();
				stepStatus.attachInfo = pSubStep->getAttachInfo();
				stepStatus.caseStepStatus = (CaseStepStatusE)status;
				if(status == STATUS_ERROR)
				{
					stepStatus.msg = pSubStep->getMsg();
				}
				else
				{
					stepStatus.msg = "success";
				}
        	}
        }
      else
        {
        	stepStatus.msg = "step not found";
            status = NOT_FOUND_STEP;
        }
    }
    else
    {
    	stepStatus.msg = "case not found";
        status = NOT_FOUND_CASE;
    }
    return status;
}
int CCaseManager::getExecutiveInfo(TY_ExecutiveInfo &pInfo)
{

	char caseID[128] = {0};
	getPresentCase(caseID);
	string strCaseID = caseID;
	CCase *pCase = getCase(strCaseID);
	if(pCase == NULL)
	{
		return -1;
	}


	memcpy(&pInfo, &m_executiveInfo, sizeof(TY_ExecutiveInfo));
	strcpy(pInfo.preCaseID, caseID);
	strcpy(pInfo.preStepID, pCase->preStepID.c_str());
	pInfo.failNum = 0;
	pInfo.successNum = 1;


	return 0;
}
CCase* CCaseManager::getCase(string caseID)
{
    CCase *pCase = NULL;
    // TY_CASEID_MAP::iterator it;
    // for(it = m_caseIdMap.begin(); it != m_caseIdMap.end(); it++)
    // {
    //     CCase *pCase = it->second;
    //     //REPORT(REPORT_INFO, "it->first ,%s,%x\n", it->first.c_str(), it->second);
    // }
    TY_CASEID_MAP::iterator pos = m_caseIdMap.find(caseID);
    if (pos == m_caseIdMap.end())
    {
        REPORT(REPORT_ERROR, "CaseID:%s, is not exsit.\n", caseID.c_str());
    }
    else
    {
        pCase = pos->second;
    }
    return pCase;
}

int CCaseManager::insertCaseMap(CCase *pCase)
{
    int ret = 0;
    TY_CASEID_MAP::iterator pos = m_caseIdMap.find(pCase->m_caseID);
    if (pos != m_caseIdMap.end())
    {
        REPORT(REPORT_ERROR, "m_caseId is exist in map.\n");
        ret = -1;
    }
    else
    {
        m_caseIdMap.insert(std::make_pair(pCase->m_caseID, pCase));
        //REPORT(REPORT_INFO, "pCase->m_caseID %s\n", pCase->m_caseID.c_str());
    }
    return ret;
    
}
