#include "SetDataStep.h"
#include "AutoTestInterface.h"

CBaseStep *CreateStep()
{
    return new CSetDataStep();
}

CSetDataStep::CSetDataStep()
{
    m_varName = "";
}
CSetDataStep::~CSetDataStep()
{
}
int CSetDataStep::stringConvertType(string value, string type, unsigned long long &setValue)
{

    int varLen = 8;
    memset(&setValue, 0, varLen);
    if ((type == "double") || (type == "float"))
    {
        double data = atof(value.c_str());
        varLen = 8;

        memcpy(&setValue, &data, varLen);
        // REPORT(REPORT_INFO, "data %lf value.c_str() %s m_setValue 0x%llx\n",
        //		data, value.c_str(), setValue);
    }
    else if (type == "float")
    {
        float data = (float)atof(value.c_str());
        varLen = 4;
        memcpy(&setValue, &data, varLen);
    }
    else if ((type == "int") || (type == "unsigned int"))
    {
        int data = atoi(value.c_str());
        varLen = 4;
        memcpy(&setValue, &data, varLen);
    }
    else if ((type == "short") || (type == "unsigned short"))
    {
        short data = (short)atoi(value.c_str());
        varLen = 2;
        memcpy(&setValue, &data, varLen);
    }
    else if ((type == "char") || (type == "unsigned char"))
    {
        char data = (char)atoi(value.c_str());
        varLen = 1;
        memcpy(&setValue, &data, varLen);
    }
    else if (type == "bool")
    {
        bool data = (char)atoi(value.c_str());
        varLen = 1;
        memcpy(&setValue, &data, varLen);
    }
    else if ((type == "long") || (type == "unsigned long"))
    {
        long data = atol(value.c_str());
        varLen = 8;
        memcpy(&setValue, &data, varLen);
    }
    else if ((type == "long long") || (type == "unsigned long long"))
    {
        long long data = atol(value.c_str());
        varLen = 8;
        memcpy(&setValue, &data, varLen);
    }
    else
    {
        REPORT(REPORT_ERROR, "Type is inconsistent\n");
        return -1;
    }

    return 0;
}

int CSetDataStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
    int ret = 0;
    ret = CBaseStep::parse(pStepElement, pStep);
    if (ret != 0)
    {
        return ret;
    }
    m_logFlag = ICDSTEPLOGFLAG;
    bool bRet = false;
    string tmpValue = "";
    bRet = GetValue(pStepElement, "Value", tmpValue);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get Value failed\n");
        return -1;
    }
    //    bRet = GetValue(pStepElement, "Type", m_type);
    //    if (!bRet)
    //    {
    //        REPORT(REPORT_ERROR, "Get Type failed\n");
    //        return -1;
    //    }
    m_type = "double";
    ret = stringConvertType(tmpValue, m_type, m_setValue);
    if (ret != 0)
    {
        return ret;
    }

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
            m_logFlag = STEPLOGFLAG;
            // REPORT(REPORT_ERROR, "parse failed\n");
            ret = -1;

            bool bRet = false;
            string tmpValue = "";
            bRet = GetValue(pFieldElement, "Name", tmpValue);
            if (!bRet)
            {
                REPORT(REPORT_ERROR, "Get VarName failed\n");
                ret = -1;
            }
            else
            {
                m_varName = tmpValue;
                ret = 0;
            }
            bRet = GetValue(pFieldElement, "Referenced", tmpValue);
            if (!bRet)
            {
                REPORT(REPORT_ERROR, "Get Referenced failed\n");
                ret = -1;
            }
            else
            {
                m_referenced = tmpValue;
                ret = 0;
            }

            bRet = GetValue(pFieldElement, "Type", m_varType);
            if (!bRet)
            {

                REPORT(REPORT_ERROR, "Get m_VarType failed\n");
                ret = -1;
            }
            else
            {
                ret = 0;
            }
        }
    }
    return ret;
}

int CSetDataStep::init()
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

int CSetDataStep::run()
{
    m_stepStatus = STATUS_RUNNING;

    int ret = 0;
    double tmpDouble = 0;
    memcpy(&tmpDouble, &m_setValue, 8);
    if (m_varName == "")
    {
        REPORT(REPORT_INFO, "set value:%lf\n", tmpDouble);
        // REPORT(REPORT_ERROR, "ICDID:%s  setValue %lld \n", m_ICDID.c_str(), m_setValue);
        ret = CBaseICDStep::setData(m_setValue);
        if (ret != 0)
        {
            REPORT(REPORT_ERROR, "setData failed\n");
            ret = -1;
            m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
            char tmp[32] = {0};
            sprintf(tmp, "%lf", tmpDouble);
            string varName = string("referenced: ") + string(m_referenced) + string(" ") + string(m_varName);
            m_msg = varName + string(", SetData value is:") + string(tmp);
            if (m_logFlag = STEPLOGFLAG)
            {
                STEPLOG(LOG_ERROR, m_msg);
            }
            else
            {
                ICDSTEPLOG(LOG_ERROR, m_msg);
            }
#endif
            // case excuteErrNum++;ZRs
            return ret;
        }
    }
    else
    {
        if (m_varType == "var")
        {

            setModelVarInterface((char *)m_referenced.c_str(), tmpDouble);
        }
        else if (m_varType == "param")
        {
            string tmpFullPath = m_referenced + "/" + m_varName;
            setModelParamInterface((char *)tmpFullPath.c_str(), tmpDouble);
        }
    }

    REPORT(REPORT_ERROR, "--ZR--------goto set model Value,fullPath:%s, data:%lf\n",
           m_referenced.c_str(), tmpDouble);
    m_stepStatus = STATUS_SUCCESS;
    m_msg = "Success";


#ifdef AUTO_LOG
    // log>>

    memcpy(&tmpDouble, &m_setValue, 8);
    char tmp[32] = {0};
    sprintf(tmp, "%lf", tmpDouble);
    string varName = string("referenced: ") + string(m_referenced) + string(" ") + string(m_varName);
    m_msg = varName + string(", SetData value is:") + string(tmp);
    if (m_logFlag = STEPLOGFLAG)
    {
        STEPLOG(LOG_INFO, m_msg);
    }
    else
    {
        ICDSTEPLOG(LOG_INFO, m_msg);
    }

    // log<<

#endif
    // REPORT(REPORT_INFO, "run\n");
    return ret;
}
