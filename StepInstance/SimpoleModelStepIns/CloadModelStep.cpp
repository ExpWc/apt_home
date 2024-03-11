#include "CloadModelStep.h"
#include "ICDManager.h"
#include "ParamData.h"
#include "CStorageManager.h"
#include "libSimulinkOffset.h"
#include "AutoTestInterface.h"
//#include "SimpoleInterfaceLinux.h"

#define PASS_LEN_4 4
#define SIMPOLE_MODE 1


CloadModelStep::CloadModelStep()
{

}
CloadModelStep::~CloadModelStep()
{
    
}

int CloadModelStep::parse(TiXmlElement *pStepElement, CBaseStep* pStep)
{

    int ret = 0;
    bool bRet = false;
    int modelIndex = 0;
    TiXmlElement *pMdlInfoElement = NULL;
    TiXmlElement *pParamElement = NULL;

    /*parse router*/
    ret = CBaseSimpoleInterfaceStep::parseRouter(pStepElement);
    if(ret != 0)
    {
        return ret;
    }

    string tmpTimeResName = "";
	bRet = GetValue(pStepElement, "timeResName", tmpTimeResName);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Get timeResName failed\n");
		return -1;
	}
    //printf("tmpTimeResName:%s\n", tmpTimeResName.c_str());
    memset(&m_simConfig, 0, sizeof(TY_SIM_CONFIG));
    memcpy(m_simConfig.timeResName, tmpTimeResName.c_str(), tmpTimeResName.length()); 
    sprintf(m_simConfig.storageName, "%s", "test");

    string tmpModelNum = "";
	bRet = GetValue(pStepElement, "modelNum", tmpModelNum);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Get modelNum failed\n");
		return -1;
	}
    //printf("modelNum:%s\n", tmpModelNum.c_str());
    m_simConfig.modelNum  = atoi(tmpModelNum.c_str());
    m_simConfig.pModelConfigs = new TY_MODEL_CONFIG[m_simConfig.modelNum];

    for(pMdlInfoElement = pStepElement->FirstChildElement(); pMdlInfoElement; pMdlInfoElement = pMdlInfoElement->NextSiblingElement())
    {
        string tmpModelName = "";
        bRet = GetValue(pMdlInfoElement, "modelName", tmpModelName);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get modelName failed\n");
            return -1;
        }
    	//printf("modelName:%s\n", tmpModelName.c_str());
        sprintf(m_simConfig.pModelConfigs[modelIndex].modelName, "%s", tmpModelName.c_str());

        string tmpPeriod = "";
        bRet = GetValue(pMdlInfoElement, "period", tmpPeriod);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get period failed\n");
            return -1;
        }
    	//printf("period:%s\n", tmpPeriod.c_str());
        m_simConfig.pModelConfigs[modelIndex].period = atoi(tmpPeriod.c_str());

        string tmpCpuIndex = "";
        bRet = GetValue(pMdlInfoElement, "cpuIndex", tmpCpuIndex);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get cpuIndex failed\n");
            return -1;
        }
    	//printf("cpuIndex:%s\n", tmpCpuIndex.c_str());
        m_simConfig.pModelConfigs[modelIndex].cpuIndex = atoi(tmpCpuIndex.c_str());

        modelIndex++;
    }
    REPORT(REPORT_INFO, "parse\n");
    return ret;
}

int CloadModelStep::init()
{

    for(int i = 0; i < m_simConfig.modelNum; i++)
    {
     
        m_simConfig.pModelConfigs[i].storageVarNum = 0;
		    m_simConfig.pModelConfigs[i].storageSignalNum = 0;
    }

    m_stepStatus = STATUS_READY;
    REPORT(REPORT_INFO, "init ok \n");
    return 0;
}


int CloadModelStep::getStorageVar()
{
    int ret = 0;
    int modelIndex = 0;

    char pPreCaseID[64] = {"0"};
	getPresentCase(pPreCaseID);	

    CStorageManager *pStorageManager = CStorageManager::getInstance();
    ret = pStorageManager->getParseInfo(pPreCaseID);
    if(ret != 0)
	{
		REPORT(REPORT_ERROR, "pasreConfigXml error\n");
		return -1;
	}
    for(int i = 0; i < m_simConfig.modelNum; i++)
    {
        //printf("m_simConfig.pModelConfigs[i].modelName:%s\n", m_simConfig.pModelConfigs[i].modelName);
        //printf("pStorageManager->m_modelStorageMap.empty():%d\n", pStorageManager->m_modelStorageMap.empty());
       /*if(pStorageManager->m_modelStorageMap.empty() == 0)
        {
            printf("if(pStorageManager->m_modelStorageMap.empty() == 0)\n");
            REPORT(REPORT_INFO, "conrtol simpole\n");
        }
        else
        {
            printf("else\n");*/
            TY_MODEL_STORAGE_MAP::iterator iter = pStorageManager->m_modelStorageMap.find(m_simConfig.pModelConfigs[i].modelName);
            if(iter != pStorageManager->m_modelStorageMap.end())
            {
                int j = 0;
                int varIndex = 0;
                for(TY_STORAGE_MODEL_VAR_MAP::iterator iterVar = iter->second.inputInfo.begin(); iterVar != iter->second.inputInfo.end(); iterVar++ )
                {
                    strcpy(m_simConfig.pModelConfigs[varIndex].storageVar[varIndex], iterVar->first.c_str());
                    varIndex++;
                }
                for(TY_STORAGE_MODEL_VAR_MAP::iterator iterVar = iter->second.outputInfo.begin(); iterVar != iter->second.outputInfo.end(); iterVar++ )
                {
                    strcpy(m_simConfig.pModelConfigs[varIndex].storageVar[varIndex], iterVar->first.c_str());
                    varIndex++;
                }
                /*
                int inputVarLen = iter->second.inputPath.size();
                int outputVarLen = iter->second.outputPath.size();
                for(j; j < inputVarLen; j++)
                {
                    sprintf(m_simConfig.pModelConfigs[i].storageVar[j], "%s", iter->second.inputPath[j].c_str()); 
                }
                varIndex = j;

                for(j = 0; j < outputVarLen; j++)
                {
                    sprintf(m_simConfig.pModelConfigs[i].storageVar[j + varIndex], "%s", iter->second.outputPath[j].c_str()); 
                }
                */
                m_simConfig.pModelConfigs[i].storageVarNum = varIndex;
                m_simConfig.pModelConfigs[i].storageSignalNum = 0;
            }
        //}

        if ((m_simConfig.pModelConfigs[i].storageVarNum | m_simConfig.pModelConfigs[i].storageSignalNum) != 0)
		    m_simConfig.stored = true;
    }
    return 0;
}

int CloadModelStep::AddMonitorVar()
{
	TY_MODELNAME_VALUE modelNameValue;

	int modelNum;
	int ret = getModelListNum(modelNum, m_addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
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
	ret = 0;
	ret = getVariableListNum(modelNameValue, num, m_addr);
	if (ret != 0)
	{
		printf("Get variables num failed.\n");
		return ret;
	}

	TY_VAR_DES *paraDes = new TY_VAR_DES[num];
	ret = getVariableList(modelNameValue, paraDes, num, m_addr);
	if (ret != 0)
	{
		printf("Get variables failed.\n");
		delete[]paraDes;
		return ret;
	}

	ret = addVarMonitor(paraDes, num, m_addr);
	if (ret != 0)
	{
		printf("Add variables failed.\n");
	}
	else
	{
		printf("Add variables success.\n");
	}

	delete[]paraDes;
	return ret;
}

int CloadModelStep::GetCurrentProject()
{
	TY_PROJECT_VALUE projectValue;
	int ret = getCurrentProject(&projectValue, m_addr);
	if (ret == 0)
	{
		printf("projectName:%s\nprojectUuid:%s\n", projectValue.name, projectValue.uuid);
	}
	else
	{
		printf("get project fail\n");
	}
	
    m_uuid = string(projectValue.uuid);
	return ret;
}

int CloadModelStep::run()
{
    int ret = 0;
    m_stepStatus = STATUS_RUNNING;
   
    /*ret = getStorageVar();
    if(ret != 0)
    {
        REPORT(REPORT_ERROR, "There are no variables to store\n");   
    }*/

    ret = loadModel(m_simConfig, m_addr);
    if(ret != 0)
    {
        REPORT(REPORT_ERROR, "loadModel faild, m_simConfig->timeResName:%s, modelNum:%d, storageName:%s, stored:%d, modelName:%s, period:%d, cupIndex:%d, storageSignalNum:%d, storageVarNum:%d, m_addr:%s\n", 
                            m_simConfig.timeResName, m_simConfig.modelNum, m_simConfig.storageName, m_simConfig.stored, m_simConfig.pModelConfigs[0].modelName,
                            m_simConfig.pModelConfigs[0].period,m_simConfig.pModelConfigs[0].cpuIndex, m_simConfig.pModelConfigs[0].storageSignalNum, m_simConfig.pModelConfigs[0].storageVarNum,m_addr);
        return -1;
    }
    //>> Initialize the shared memory
    ret = GetCurrentProject();
    if(ret != 0)
    {
        REPORT(REPORT_ERROR, "GetCurrentProject faild\n");
        return -1;
    }
	printf("---------num:%d, uuid:%s\n",m_simConfig.modelNum,(char*)m_uuid.c_str());
    for(int i = 0 ; i < m_simConfig.modelNum; i++)
    {
        ret = creatDataDictionaryManualXml((char*)m_uuid.c_str(), (const char*)m_simConfig.pModelConfigs[i].modelName, SIMPOLE_MODE);
        if(ret != 0)
        {
            REPORT(REPORT_ERROR, "loadModel faild\n");
            return -1;
        }

        ret = loadVarOffsetConfig((const char*)m_simConfig.pModelConfigs[i].modelName);
        if(ret != 0)
        {
            REPORT(REPORT_ERROR, "loadVarOffsetConfig faild, ret  = %d\n", ret);
            return -1;
        }

    }
     
    //<<
    ret = AddMonitorVar();
    if(ret != 0)
    {
        REPORT(REPORT_ERROR, "loadModel faild\n");
    }

    delete[] m_simConfig.pModelConfigs; 
    m_simConfig.pModelConfigs = NULL;

    m_stepStatus = STATUS_SUCCESS;
    REPORT(REPORT_INFO, "run\n");
    return 0;
}

string CloadModelStep::getAttachInfo()
{
	
	//char data[32];
	//sprintf(data, "%lf", this->m_dataDouble);
	
   // string retString = string(data);
    string retString = "";
	return retString;
	//return (string)NULL;
}


