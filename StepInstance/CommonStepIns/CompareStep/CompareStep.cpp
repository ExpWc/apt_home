#include "CompareStep.h"
#include "string.h"
#include "AutoTestInterface.h"
#include "HiMGlobalVarLib.h"

CBaseStep *CreateStep()
{
    return new CCompareStep();
}

CCompareStep::CCompareStep()
{
    m_dataDouble = 0;
    m_expectation = 0;
    m_upperLimit = 0;
    m_lowLimit = 0;
}
CCompareStep::~CCompareStep()
{
}

int CCompareStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
    int ret = 0;
    ret = CBaseStep::parse(pStepElement, pStep);
    if (ret != 0)
    {
        return ret;
    }

    bool bRet = false;
    string tmpExpect = "";
    bRet = GetValue(pStepElement, "Expectation", tmpExpect);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get Expectation failed\n");
        return -1;
    }
    // m_expectation = atof(tmpValue.c_str());
    string tmpUpper = "";
    bRet = GetValue(pStepElement, "UpperLimit", tmpUpper);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get UpperLimit failed\n");
        return -1;
    }
    string tmpLow = "";
    bRet = GetValue(pStepElement, "LowLimit", tmpLow);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get LowLimit failed\n");
        return -1;
    }

    m_lowLimit = atof(tmpLow.c_str());
    m_upperLimit = atof(tmpUpper.c_str());
    m_expectation = atof(tmpExpect.c_str());
    /// REPORT(REPORT_ERROR, "=============.m_expectation:%lf, m_upperLimit:%lf, m_lowLimit:%lf\n", m_expectation,m_upperLimit,m_lowLimit);
    TiXmlElement *pFieldElement = NULL;
    pFieldElement = pStepElement->FirstChildElement();
    if (pFieldElement == NULL)
    {
        REPORT(REPORT_ERROR, "Init.xml can't find the node of ICD.\n");
        ret = -1;
    }
    else
    {
        ret = CBaseICDStep::parse(pFieldElement);
        if (ret != 0)
        {
            ret = GetValue(pFieldElement, "Name", m_varName);
            m_type = TYPE_MODEL;
            if (!ret)
            {
                REPORT(REPORT_ERROR, "Get FieldID failed\n");
                ret = -1;
            }
            ret = GetValue(pFieldElement, "Referenced", m_referenced);
            if (!ret)
            {
                REPORT(REPORT_ERROR, "Get Referenced failed\n");
                ret = -1;
            }
            ret = 0;
        }
        else
        {
            m_type = TYPE_ICD;
        }
    }
    //>>>

    REPORT(REPORT_DEBUG, "parse\n");
    return ret;
}

int CCompareStep::init()
{
    int ret = 0;
    ret = CBaseICDStep::init();
    if (ret != 0)
    {
        REPORT(REPORT_ERROR, "init failed\n");
        ret = -1;
        m_stepStatus = STATUS_ERROR;
    }

    m_stepStatus = STATUS_READY;
    REPORT(REPORT_DEBUG, "init\n");
    return ret;
}
inline void stepLog(string stepID, string ICDID, string fieldId, char *resion)
{
    FILE *pFd = fopen("./step.txt", "a+");
    struct timeval recvTime;
    gettimeofday(&recvTime, NULL);
    fprintf(pFd, "caseID:%s stepID:%s time:%d-%d\n", ICDID.c_str(), stepID.c_str(), recvTime.tv_sec, recvTime.tv_usec);
    fprintf(pFd, "%s", resion);
}
int CCompareStep::run()
{
    m_stepStatus = STATUS_RUNNING;

    int ret = 0;
    // double dataDouble = 0;
    char info[128] = {0};
    if (m_type == TYPE_ICD)
    {
        ret = CBaseICDStep::getData(&m_dataDouble);
        printf("------------------------type = icd , m_dataDouble:%lf\n", m_dataDouble);
    }
    else if (m_type == TYPE_MODEL)
    {
        // ret = getModelVar((char*)m_varName.c_str() ,&m_dataDouble);  // ----modefy by wc--20230625
        ret = getModelVarInterface((char *)m_referenced.c_str(), &m_dataDouble); // ----modefy by wc--20230625
                                                                                 // REPORT(REPORT_INFO, "getModelVar Name:%s value:%lf\n", m_varName.c_str(), m_dataDouble);
        // ret = getVarDataByName((char*)m_varName.c_str(), (char*)&m_dataDouble);
        // m_dataDouble = value;
        printf("======get input:%s ,data:%lf,ret %d.\n", m_referenced.c_str(), m_dataDouble, ret);
    }
    if (ret != 0)
    {
        REPORT(REPORT_ERROR, "getData failed\n");

        m_stepStatus = STATUS_ERROR;
        ret = -1;
    }

    REPORT(REPORT_INFO, " CCompareStep::run >>>>>tmpDouble %lf.\n", m_dataDouble);

    m_stepStatus = compare(m_dataDouble);
    REPORT(REPORT_ERROR, " m_stepStatus %d.\n", m_stepStatus);
    if (m_stepStatus == STATUS_FAILED)
    {
        // char info[1024] = {0};
        ret = -1;
        sprintf(info, "  compare failed-actual:%lf expect:%lf \n", m_dataDouble, m_expectation);
        // stepLog(m_stepID, m_ICDID, m_fieldId, tmpStr);
        string varName = string("referenced: ") + string(m_referenced) + string(" ") + string(m_varName);

#ifdef AUTO_LOG
        m_msg = varName + string(info);
        if (m_type == TYPE_ICD)
        {
            ICDSTEPLOG(LOG_ERROR, m_msg);
        }
        else if (m_type == TYPE_MODEL)
        {
            STEPLOG(LOG_ERROR, m_msg);
        }

        // ICDSTEPLOG(LOG_ERROR, m_msg);

#endif
    }
    // m_msg = "Success";

    // REPORT(REPORT_INFO, "run finish\n");
    //printf("ret=%d\n", ret);
    return ret;
}

int CCompareStep::compare(double data)
{
    int ret = 0;

    if ((data >= (m_expectation - m_lowLimit - 0.000001)) && (data <= (m_expectation + m_upperLimit + 0.000001)))
    {
        // REPORT(REPORT_INFO, "Compare success.\n");
        ret = STATUS_SUCCESS;
    }
    else
    {
        REPORT(REPORT_ERROR, "Compare result is inconsistent.actual:%lf expect:%lf.m_lowLimit:%lf,m_upperLimit:%lf\n",
               data, m_expectation, m_lowLimit, m_upperLimit);
        ret = STATUS_FAILED;
    }
    return ret;
}

string CCompareStep::getAttachInfo()
{
    char data[32];
    sprintf(data, "%lf", this->m_dataDouble);
    string retString = string(data);
    return retString;
}
