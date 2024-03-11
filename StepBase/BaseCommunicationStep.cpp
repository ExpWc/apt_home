#include "BaseCommunicationStep.h"


CBaseCommunicationStep::CBaseCommunicationStep()
{

}
CBaseCommunicationStep::~CBaseCommunicationStep()
{

}

int CBaseCommunicationStep::parseRouter(TiXmlElement *pStepElement)
{
    bool bRet = false;
    TiXmlElement *pICDElement = NULL;
    TiXmlElement *pChnInfoElement = NULL;
    pICDElement = pStepElement->FirstChildElement();
    if (pICDElement == NULL)
    {
        REPORT(REPORT_ERROR, "Init.xml can't find the node of ICD.\n");
    }
    for (pICDElement = pStepElement->FirstChildElement(); pICDElement; 
        pICDElement = pICDElement->NextSiblingElement())
    {
        //TY_ROUTER sendRouter;
        m_router.command = m_command;
        m_router.stepID = m_stepID;

        bRet = GetValue(pICDElement, "ICDID", m_router.icdID);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get ICDID failed\n");
            return -1;
        }
        string tmpType = "";
        bRet = GetValue(pICDElement, "Type", tmpType);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get Type failed\n");
            return -1;
        }

        pChnInfoElement = pICDElement->FirstChildElement(); 
        if (pChnInfoElement == NULL)
        {
            REPORT(REPORT_ERROR, "Init.xml can't find the node of chnInfo.\n");
        }
        for(pChnInfoElement = pICDElement->FirstChildElement(); pChnInfoElement;
            pChnInfoElement = pChnInfoElement->NextSiblingElement())
        {
            bRet = GetValue(pChnInfoElement, "CardType", m_router.CardType);
            if (!bRet)
            {
                REPORT(REPORT_ERROR, "Get CardType:%s failed\n", m_router.CardType.c_str());
                return -1;
            }
            string tmpCardID = "";
            bRet = GetValue(pChnInfoElement, "CardID", tmpCardID);
            if (!bRet)
            {
                REPORT(REPORT_ERROR, "Get CardID failed\n");
                return -1;
            }
            m_router.CardID = atoi(tmpCardID.c_str());

            string tmpChnID = "";
            bRet = GetValue(pChnInfoElement, "ChnID", tmpChnID);
            if (!bRet)
            {
                REPORT(REPORT_ERROR, "Get ChnID failed\n");
                return -1;
            }
            m_router.ChnID = atoi(tmpChnID.c_str());
        }
    }
    REPORT(REPORT_DEBUG, "parse router\n");
    return 0;
}

int CBaseCommunicationStep::log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
		int lineNo, string msg)
{
	char tmp[1024];
	sprintf(tmp, "CaseID:%s icdID:%s type:%s card:%d-%d\n",
			m_router.caseID.c_str(),
			m_router.icdID.c_str(),
			m_router.CardType.c_str(),
			m_router.CardID,
			m_router.ChnID);
	string msgTmp = string(tmp) + msg;
	CBaseStep::log(logType, fileName, funName, lineNo, msgTmp);
}

