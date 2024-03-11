#include "CCompareModelVarStep.h"
#include "ICDManager.h"
#include "ParamData.h"
#include "CStorageManager.h"
//#include "SimpoleInterfaceLinux.h"

#define PASS_LEN_4 4


CCompareModelVarStep::CCompareModelVarStep()
{

}
CCompareModelVarStep::~CCompareModelVarStep()
{
    
}

int CCompareModelVarStep::parse(TiXmlElement *pStepElement, CBaseStep* pStep)
{

    int ret = 0;
    bool bRet = false;
    int modelIndex = 0;
    string tmpName;
    string tmpFullPath;	
    TiXmlElement *pVarInfoElement = NULL;
    TiXmlElement *pParamElement = NULL;

    /*parse router*/
    ret = CBaseSimpoleInterfaceStep::parseRouter(pStepElement);
    if(ret != 0)
    {
        return ret;
    }

    string tmpExpect = "";
    bRet = GetValue(pStepElement, "Expectation", tmpExpect);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get Expectation failed\n");
        return -1;
    }
    //m_expectation = atof(tmpValue.c_str());
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
    
    m_simpoleLowLimit = atof(tmpLow.c_str());
    m_simpoleUpperLimit = atof(tmpUpper.c_str());
    m_simpoleExpectation = atof(tmpExpect.c_str());

    for(pVarInfoElement = pStepElement->FirstChildElement(); pVarInfoElement; pVarInfoElement = pVarInfoElement->NextSiblingElement())
    {
        
        string tmpVarName = "";
        bRet = GetValue(pVarInfoElement, "Name", tmpName);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get VarName failed\n");
            return -1;
        }
        bRet = GetValue(pVarInfoElement, "Referenced", tmpFullPath);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get Referenced failed\n");
            return -1;
        }
        //strcpy(m_pList.varPath, m_varName.c_str());

        string tmpVarType = "";
        bRet = GetValue(pVarInfoElement, "VarType", tmpVarType);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get VarType failed\n");
            return -1;
        }
        
        m_varType = tmpVarType;
	if(m_varType == "var")
	{
		strcpy(m_pList.varPath, tmpFullPath.c_str());
	}
	else if(m_varType == "param")
	{
		tmpFullPath = tmpFullPath + "/" + tmpName;
		strcpy(m_pList.varPath, tmpFullPath.c_str());
	}
	modelIndex++;
    }
    REPORT(REPORT_INFO, "parse\n");
    return ret;
}

int CCompareModelVarStep::init()
{

	m_stepStatus = STATUS_READY;
	m_simpoleActualValue = 0;
	REPORT(REPORT_INFO, "init ok \n");
	return 0;
}

int CCompareModelVarStep::GetVarValue()
{
	int ret = 0, num = 1;
#if 0
	ret = getRamSampleInTimeWindow(&m_pList, num, m_addr);
	if (ret != 0)
	{
		printf("AGetRamSampleInTimeWindow failed.varPath:%s,m_addr:%s \n", m_pList.varPath, m_addr);
	}
	else
	{
		printf("11varPath:%s value:%s\n", m_pList.varPath, m_pList.queueValues[0].value);	
		//strcpy((char*)&m_simpoleActualValue, m_pList.queueValues[0].value);
		m_simpoleActualValue = atof(m_pList.queueValues[0].value);
		printf("222m_simpoleActualValue:%lf\n",m_simpoleActualValue);
	}
#endif
#if 1
	ret = getModelVarInterface(m_pList.varPath, &m_simpoleActualValue);  // ----modefy by wc--20230625
	//ret = getModelVarInterface((char*)m_varName.c_str() ,&m_simpoleActualValue);  // ----modefy by wc--20230625
	if (ret != 0)
	{
		printf("getModelVarInterface  failed.varPath:%s \n", m_varName.c_str());
	}
	else
	{
		printf("11varPath:%s value:%lf\n", m_pList.varPath, m_simpoleActualValue);	
		//strcpy((char*)&m_simpoleActualValue, m_pList.queueValues[0].value);
		// m_simpoleActualValue = atof(m_pList.queueValues[0].value);
		//  printf("222m_simpoleActualValue:%lf\n",m_simpoleActualValue);
	}
#endif
	return 0;
}

int CCompareModelVarStep::GetParamValue()
{
	TY_MODELNAME_VALUE modelNameValue;
	memset(&modelNameValue, 0 , sizeof(TY_MODELNAME_VALUE));

	int modelNum;
	int ret = getModelListNum(modelNum, m_addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	memset(&modelList, 0 , sizeof(TY_MODEL_LIST));
	getModelList(&modelList, m_addr);//2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}
	modelNameValue.modelNumber = modelNum;

	for (int i = 0; i < modelNum; i++)
	{
		sprintf(modelNameValue.modelName[i], "%s", modelList.modelName[i]);
	}
	int num = 0;
	ret = getParameterListNum(modelNameValue, num, m_addr);
	if (ret != 0)
	{
		printf("Get parameter num failed.\n");
		return ret;
	}

	TY_PARA_DES *paraDes = new TY_PARA_DES[num];
	modelNameValue.modelNumber = modelNum;
	ret = getParameterList(modelNameValue, paraDes, num, m_addr);
	if (ret != 0)
	{
		printf("Get parameter failed.\n");
		return ret;
	}
	ret = getParameterValue(paraDes, num, m_addr);
	if (ret != 0)
	{
		printf("Get parameter value failed.num:%d\n",num);
		return ret;
	}

	for (int i = 0; i < num; i++)
	{
		if(strcmp(m_pList.varPath, paraDes[i].fullPath) == 0)
		{
			if (paraDes[i].dataType == M_DOUBLE)
			{
				memcpy(&m_simpoleActualValue, paraDes[i].value, 8);
			}
			else
			{
				memcpy(&m_simpoleActualValue, paraDes[i].value, 1);
			}
		}
		/*
		   if (paraDes[i].dataType == M_DOUBLE)
		   {
		   double t = 0;
		   memcpy(&t, paraDes[i].value, 8);
		   printf("%d %d fullPath:%s Type:%d Value:%f\n", num, i + 1, paraDes[i].fullPath, paraDes[i].dataType, t);
		   }
		   */

	}
	delete[]paraDes;
	paraDes = NULL;
	return ret;
}

int CCompareModelVarStep::run()
{
	int ret = 0;
	m_stepStatus = STATUS_RUNNING;

	if(m_varType == "var")
	{
		m_pList.varType = 1;
		ret = GetVarValue();
		if(ret != 0)
		{
			REPORT(REPORT_ERROR, "getVarValue faild\n");
		}
	}
	else if(m_varType == "param")
	{
		m_pList.varType = 1;
		ret = GetParamValue();
		if(ret != 0)
		{
			REPORT(REPORT_ERROR, "getVarValue faild\n");
		}
	}

	if((m_simpoleActualValue >= (m_simpoleExpectation - m_simpoleLowLimit - 0.000001)) && (m_simpoleActualValue <= (m_simpoleExpectation + m_simpoleUpperLimit + 0.000001)))
	{
		REPORT(REPORT_INFO, "Compare success.\n");
		m_stepStatus = STATUS_SUCCESS;
	}
	else
	{
		REPORT(REPORT_ERROR, "Compare result is inconsistent.actual:%lf expect:%lf.m_lowLimit:%lf,m_upperLimit:%lf\n",
				m_simpoleActualValue, m_simpoleExpectation,m_simpoleLowLimit,m_simpoleUpperLimit);
		m_stepStatus = STATUS_FAILED;
	}

	//m_stepStatus = STATUS_SUCCESS;
	printf("m_stepStatus:%d\n", m_stepStatus);
	REPORT(REPORT_INFO, "run\n");
	return 0;
}

string CCompareModelVarStep::getAttachInfo()
{

	//char data[32];
	//sprintf(data, "%lf", this->m_dataDouble);
	char data[32];
	sprintf(data, "%lf", this->m_simpoleActualValue);
    printf("data:%s\n", data);
	// string retString = string(data);
	//string retString = "";
	string retString = string(data);
    printf("retString:%s\n", retString.c_str());
	return retString;
	//return (string)NULL;
}


