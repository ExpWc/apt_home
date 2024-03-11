#include "BaseStep.h"
#include "dlfcn.h"

CBaseStep::CBaseStep()
{
    m_msg = "";
}

CBaseStep::~CBaseStep()
{
	m_msg = "";
}

int CBaseStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
    //init para>>
    m_stepStatus = STATUS_READY;
    //init para>>
    
    int ret = 0;
    bool bRet = false;
    bRet = GetValue(pStepElement, "Command", m_command);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get Command failed\n");
        ret = -1;
    }
    bRet = GetValue(pStepElement, "StepID", m_stepID);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get StepID failed\n");
        ret = -1;
    }
    pStep->m_stepID = m_stepID;
    bRet = GetValue(pStepElement, "StepName", m_stepName);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get StepName failed\n");
        ret = -1;
    }
    //REPORT(REPORT_INFO, "StepID:%s m_command:%s m_stepName:%s\n", m_stepID.c_str(), m_command.c_str(), m_stepName.c_str());
    
    return ret;
}

int CBaseStep::init()
{
    REPORT(REPORT_INFO, "Step init\n");
    return 0;
}

int CBaseStep::run()
{
    REPORT(REPORT_INFO, "Step run\n");
    return 0;
}

int CBaseStep::stop()
{
    REPORT(REPORT_INFO, "Step stop\n");
    return 0;
}

int CBaseStep::getStatus()
{
    //REPORT(REPORT_INFO, "Step run m_stepStatus %d\n", m_stepStatus);
    return m_stepStatus;
}

CBaseStep* CBaseStep::getSubStep(string subStepID)
{
	CBaseStep * step = NULL;
	return step;
}

string CBaseStep::getMsg()
{
    return m_msg;
}

string CBaseStep::getAttachInfo()
{
	string retString = "";
	return retString;
}

int CBaseStep::reset()
{
	m_stepStatus = STATUS_READY;
	return 0;
}
int CBaseStep::setStepValue(int frequency)
{
	printf("StepID:%s, frequency:%d\n", m_stepID.c_str(), frequency);
	return 0;
}

int CBaseStep::log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
		int lineNo, string msg)
{
	CLog *pLog = CLog::getInstance();
	char tmp[1024];
	sprintf(tmp, "[%s]Run Step... StepID:%s name:%s\n%s\n",
			m_command.c_str(), m_stepID.c_str(), m_stepName.c_str(), msg.c_str());
	pLog->log(logType, fileName, funName, lineNo, tmp);
	return 0;
}

