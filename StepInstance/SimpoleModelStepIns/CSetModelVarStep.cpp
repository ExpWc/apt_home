#include "CSetModelVarStep.h"
#include "ICDManager.h"
#include "ParamData.h"
#include "CStorageManager.h"

//#include "SimpoleInterfaceLinux.h"

#define PASS_LEN_4 4


CSetModelVarStep::CSetModelVarStep()
{

}
CSetModelVarStep::~CSetModelVarStep()
{
    
}

int CSetModelVarStep::parse(TiXmlElement *pStepElement, CBaseStep* pStep)
{

    int ret = 0;
    bool bRet = false;
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

    string tmpValue = "";
    bRet = GetValue(pStepElement, "Value", tmpValue);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get Value failed\n");
        return -1;
    }
    
    m_modelVal = atof(tmpValue.c_str());
    
    for(pVarInfoElement = pStepElement->FirstChildElement(); pVarInfoElement; pVarInfoElement = pVarInfoElement->NextSiblingElement())
    {    
        string tmpVarName = "";
        bRet = GetValue(pVarInfoElement, "Name", tmpName);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get Name failed\n");
            return -1;
        }
        bRet = GetValue(pVarInfoElement, "Referenced", tmpFullPath);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get Referenced failed\n");
            return -1;
        }
	/*
	tmpFullPath = tmpFullPath + "/" + tmpName;
        strcpy(m_pList.fullPath, tmpVarName.c_str());
	*/

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
		strcpy(m_pList.fullPath, tmpFullPath.c_str());
	}
	else if(m_varType == "param")
	{
		tmpFullPath = tmpFullPath + "/" + tmpName;
		strcpy(m_pList.fullPath, tmpFullPath.c_str());
	}

	string tmpDataType = "";
	bRet = GetValue(pVarInfoElement, "DataType", tmpDataType);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Get dataType failed\n");
		return -1;
	}
	m_dataType = tmpDataType;

    }
    REPORT(REPORT_INFO, "parse\n");
    return ret;
}

int CSetModelVarStep::init()
{

	m_stepStatus = STATUS_READY;
	REPORT(REPORT_INFO, "init ok \n");
	return 0;
}

int CSetModelVarStep::setVarValue()
{
	int ret = 0;
	ret = setModelVarInterface(m_pList.fullPath, m_modelVal);
	if (ret != 0)
	{
		printf("setModelVarInterface failed.\n");
	}

	return 0;
}

int packParam(TY_PARA_DES &paramStru, string type, double value)
{
	if(type == "bool")
	{
		paramStru.dataType = M_BOOL;
		memcpy(paramStru.value, &value, 1);
	}
	else if(type == "char")
	{
		paramStru.dataType = M_CHAR;
		memcpy(paramStru.value, &value, 1);
	}
	else if(type == "unsigned char")
	{
		paramStru.dataType = M_UCHAR;
		memcpy(paramStru.value, &value, 1);
	}
	else if(type == "short")
	{
		paramStru.dataType = M_SHORT;
		memcpy(paramStru.value, &value, 2);
	}
	else if(type == "unsigned short")
	{
		paramStru.dataType = M_USHORT;
		memcpy(paramStru.value, &value, 2);
	}
	else if(type == "int")
	{
		paramStru.dataType = M_INT;
		memcpy(paramStru.value, &value, 4);
	}
	else if(type == "unsigned int")
	{
		paramStru.dataType = M_UINT;
		memcpy(paramStru.value, &value, 4);
	}
	else if(type == "float")
	{
		paramStru.dataType = M_FLOAT;
		memcpy(paramStru.value, &value, 4);
	}
	else if(type == "double")
	{
		paramStru.dataType = M_DOUBLE;
		memcpy(paramStru.value, &value, 8);
	}

	return 0;
}

int CSetModelVarStep::setParamValue()
{
	int num = 1;
	TY_PARA_DES paraDes;
	memset(&paraDes, 0, sizeof(TY_PARA_DES));
	//sprintf(paraDes.fullPath, "demo1/Constant1/Constant1_Value");
	strcpy(paraDes.fullPath, m_pList.fullPath);
	packParam(paraDes, m_dataType, m_modelVal);

	int ret= setParameterValue(&paraDes, num, m_addr);
	if (ret != 0)
	{
		printf("Set parameter failed.fullPath:%s, m_dataType:%d\n",paraDes.fullPath , paraDes.dataType);
		return ret;
	}
	printf("Set parameter .fullPath:%s, m_dataType:%d, value:%lf\n",paraDes.fullPath , paraDes.dataType, m_modelVal);

	return 0;
}

int CSetModelVarStep::run()
{
	int ret = 0;
	m_stepStatus = STATUS_RUNNING;
	REPORT(REPORT_INFO, "setParamValue ---> m)varType=%s\n", m_varType.c_str());
	if(m_varType == "var")
	{
		ret = setVarValue();
		if(ret != 0)
		{
			REPORT(REPORT_ERROR, "getVarValue faild\n");
		}
	}
	else if(m_varType == "param")
	{
		ret = setParamValue();
		if(ret != 0)
		{
			REPORT(REPORT_ERROR, "getVarValue faild\n");
		}
		REPORT(REPORT_INFO, "setParamValue success\n");
	}

	m_stepStatus = STATUS_SUCCESS;
	REPORT(REPORT_INFO, "run\n");
	return 0;
}

string CSetModelVarStep::getAttachInfo()
{

	//char data[32];
	//sprintf(data, "%lf", this->m_dataDouble);

	// string retString = string(data);
	string retString = "";
	return retString;
	//return (string)NULL;
}


