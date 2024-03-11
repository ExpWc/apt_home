/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-08 01:26:59
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-09 01:59:20
 * @FilePath: /ATP_V1.0.0/ManaTestInterface/ManualTestInterface.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tinyxml.h"
#include "AutoTestInterface.h"
#include "ManualTestInterface.h"
#include "HiReport.h"
#include "HiCommon.h"
#include "parseXmlApi.h"

//>>wc
#include "CustomDlStep.h"
#include "../HiCaseManager/RTCTimer"
//<<wc

//#include "HiMGlobalVarLib.h"
//#include "HiMParaComLib.h"
//#include "ParamData.h"
#include "tinystr.h"
//#include "../HiCurveIncManager/Curve/CCurve.h"
//#include "../HiCaseManager/CaseManager.h"
//#include "../HiCaseManager/StepInstance/CustomStep.h"
//#include "../HiCaseManager/StepInstance/CustomDlStep.h"

//#include "CStorageManager.h"
//#include "DataType.h"
//#include "StoreModelVar.h"
//#include "CStartModelStep.h"
//#include "CBaseSimpoleInterfaceStep.h"

//#include "../HiICDManager/Block/Block.h"

using namespace std;
#define SUCCESS 0
#define FAIL -1
#define MANUAL_PERIOD_MS 10

#define unlikely(x) __builtin_expect(!!(x), 0)
#define likely(x) __builtin_expect(!!(x), 1)

const char *gSimpoleHomePath = "SIMPOLE_HOME";
int MANUAL_TEST_FLAG;
int gModelStatus;
int gRtcStartFlag;

int startManualTest ()
{
	int ret = 0;
	int periodMs = MANUAL_PERIOD_MS;
	MANUAL_TEST_FLAG = 1;
	gModelStatus = STATUS_RUNNING;
	
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
	if (gRtcStartFlag == 0)
	{
		gRtcStartFlag = 1;

		ret = rtcInit((1000*periodMs), modelStartHandler, this);
		sleep(1);
		if (ret != 0)
		{
			REPORT(REPORT_ERROR, "rtcInit error periodMs:%d\n ", periodMs);
			gModelStatus = STATUS_ERROR;
			return FAIL;
		}

		rtcStart();
		REPORT(REPORT_INFO, "start rtc\n");	
	}

	pHardwareManager->start();
	return SUCCESS;
}

int stopManualTest ()
{
	int ret = 0;

	rtcStop();
	rtcDispose();
	sleep(2);
	gRtcStartFlag = 0;
	gModelStatus = STATUS_SUCCESS;
	TY_MODEL_CARD_MAP::iterator iterModel;

	for(iterModel = modelCard.begin(); iterModel != modelCard.end(); iterModel++)
	{
		CCustomDlStep* pCustomDlStep = (CCustomDlStep*)iterModel->second;
		pCustomDlStep->stop();
	}
	
	MANUAL_TEST_FLAG = 0;
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
	pHardwareManager->stop();

	return SUCCESS;
}

int optManualTestOneStep(const char *pOneStep, TY_STEP_STATUS *pInfo)
{
	if(unlikely(pOneStep == NULL || pInfo == NULL))
	{
		REPORT(REPORT_ERROR, "Parameter passing error!");
		return FAIL;
	}

	int ret = 0;
    string CaseID;
    string StepID;
    CCase *pCase = NULL;
    CBaseStep* pStep = NULL;
    string attachInfo; // 步骤返回值

	//-->1 获取xml文件放入string变量
	string str(pOneStep);

	//-->2 获取CaseManager实例/初始化CaseInfo 数据
	CCaseManager * pCaseManager = CCaseManager::getInstance();
	ret = pCaseManager->init(str);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, " init error");
		return FAIL;
	}

	//-->3 获取CaseID及其对象地址
	TY_CASEID_MAP::iterator pos = pCaseManager->m_caseIdMap.begin();
    if (pos != pCaseManager->m_caseIdMap.end())
    {
        CaseID = pos->first;
        pCase = pos->second;
        //REPORT(REPORT_INFO, "m_caseId is \n", CaseID.c_str());
    }
    else
    {
        REPORT(REPORT_ERROR, "m_caseId is no find\n");
        return FAIL;
    }
    //-->4   
    TY_CASE_EXECUTE caseExecute;
    memcpy(caseExecute.caseID, CaseID.c_str(), CaseID.length());
    caseExecute.interval = 0;
    pCaseManager->m_caseExecuteVec.push_back(caseExecute);
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
        return FAIL;
    }

    //-->7 获取步骤返回信息
    attachInfo = pStep->getAttachInfo();
    pInfo->attachInfo = attachInfo;
    //printf("-----[%s:%s:%d]-->value:%s\n",__FILE__,__FUNCTION__,__LINE__,info.attachInfo.c_str());
    pInfo->stepID = StepID;
    pInfo->msg = "success";
    pInfo->caseStepStatus = STATUS_SUCCESS;

	return SUCCESS;
}

int startModel (const char* pData)
{
	if(unlikely(pData == NULL))
	{
		REPORT(REPORT_ERROR, "Parameter passing error!");
		return FAIL;
	}
	
	int ret = 0;
	CCustomDlStep* pCustomDlStep = new CCustomDlStep();
	TiXmlNode* rootNode = NULL;
	TiXmlElement *pModelElement = NULL;
    TiXmlDocument * pInitXmlManager = NULL;

	pInitXmlManager = load_xml(pData, FROM_DATA);
	if (pInitXmlManager == NULL)
	{
		REPORT(REPORT_ERROR, "Xml is not valid\n");
		return FAIL;
	}
	else
	{
		rootNode = pInitXmlManager->RootElement();
	}
	pModelElement = rootNode->FirstChildElement("model");

	ret = parseCaseBinding("/home/V1.0.0/HiAutoTestServer/bin/");
	if (ret != 0)
	{
		REPORT(REPORT_ERROR, "parseCaseBinding faild\n");
	}

	ret = pCustomDlStep->parse(pModelElement, NULL);
	if (ret != 0)
	{
		//REPORT(REPORT_ERROR, "subStepNo %d parse failed\n", subStepNo);
		REPORT(REPORT_ERROR, "parsr faild\n");

	}

	ret = creatDataDictionaryManualXml("/home/V1.0.0/HiAutoTestServer/bin/", pCustomDlStep->m_dlName.c_str(), 0);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "creat dataDictionary  faild\n");
		return FAIL;
	}

	ret = pCustomDlStep->init();
	if (ret != 0)
	{
		gModelStatus = STATUS_ERROR;
		return FAIL;
	}

	ret = pCustomDlStep->run();
	if (ret != 0)
	{
		REPORT(REPORT_ERROR, "mode run faild\n");
		gModelStatus = STATUS_ERROR;
		rtcDispose();
		return FAIL;
	}

	return SUCCESS;
}

int stopModel (const char* pModelName)
{
	if(unlikely(pModelName == NULL))
	{
		REPORT(REPORT_ERROR, "Parameter passing error!");
		return FAIL;
	}

	int ret = 0;
	char buff[64] = {0};
	sprintf(buff, "%s%s%s", "lib", pModelName, ".so");
	string strModelName(buff);
	cout << "============stop Model cout << model: "<< strModelName << endl;

	TY_MODEL_CARD_MAP::iterator iterModel;
	iterModel = modelCard.find(strModelName);
	if( iterModel != modelCard.end())
	{
		CCustomDlStep* pCustomDlStep = (CCustomDlStep* )iterModel->second;
		pCustomDlStep->stop();
		modelCard.erase(iterModel);
	}
	else
	{
		REPORT(REPORT_ERROR, "Do not found modelName:%s!",strModelName.c_str());
		return FAIL;
	}

	return SUCCESS;
}

int getModelRunStatus (const char* pModelName, int *pModelStatus)
{
	if(unlikely(pModelName == NULL || pModelStatus == NULL))
	{
		REPORT(REPORT_ERROR, "Parameter passing error!");
		return FAIL;
	}

	int ret = 0;
	char buff[64] = {0};
	sprintf(buff, "%s%s%s", "lib", pModelName, ".so");
	string strModelName(buff);

	TY_MODEL_CARD_MAP::iterator iterModel = modelCard.find(strModelName);
	if( iterModel != modelCard.end())
	{
		//REPORT(REPORT_INFO, "-------------------------------------get model status\n");
		CCustomDlStep* pCustomDlStep = (CCustomDlStep* )iterModel->second;
		int ModelStatus = pCustomDlStep->getStatus();
		*pModelStatus = ModelStatus;
	}
	else
	{
		for(TY_MODEL_CARD_MAP::iterator iter = modelCard.begin(); iter!=modelCard.end(); iter++)
		{
			printf("[%s:%s:%d]-->modelMap have:%s\n",__FILE__,__FUNCTION__,__LINE__, iter->first.c_str());
		}
		printf("[%s:%s:%d]-->Do not found modelName:%s!,map len:%d\n",__FILE__,__FUNCTION__,__LINE__,strModelName.c_str(),modelCard.size());
		return FAIL;
	}

	return SUCCESS;
}

int getModelParam(TY_MODEL_INFO *  pModelInfo, int modelNum)
{
	if(unlikely(pModelInfo == NULL || modelNum <= 0))
	{
		REPORT(REPORT_ERROR, "Parameter passing error!");
		return FAIL;
	}

	int ret = 0;
	char buff[256] = {0};

	if(MANUAL_TEST_FLAG == 1)
	{
		TY_MODEL_CARD_MAP::iterator iterModel;
		vector<TY_PARAM_INFO>::iterator iter;
		
		for(int i = 0; i < modelNum; i++)
		{
			sprintf(buff, "%s%s%s", "lib", pModelInfo[i]->modelName, ".so");
			string strModelName(buff);

			iterModel = modelCard.find(strModelName);
			if( iterModel != modelCard.end())
			{
				CCustomDlStep* pCustomDlStep = (CCustomDlStep* )iterModel->second;
				for(int j = 0; j < pModelInfo[i]->num; j++)
				{
					string fullPath(pModelInfo[i]->fullPath[j]);
					//printf("=============get ModelParam===========fullpath:%s\n",fullPath.c_str());
					for(iter = pCustomDlStep->m_subStepInfo.paramInfo.begin(); iter != pCustomDlStep->m_subStepInfo.paramInfo.end(); iter++)
					{
						//	printf("=============get ModelParam===========Map->fullpath:%s\n",iter->strParamName.c_str());
						if(strcmp(iter->strParamName.c_str(), fullPath.c_str()) == 0)
						{
							memcpy(&pModelInfo[i]->data[j], iter->value.value, 8);
							//printf("==========get model param var: %lf\n",pModelInfo[i]->data[j]);
						}
						else
						{
							continue;
						}
					}
				}
			}
			else
			{
				REPORT(REPORT_ERROR,"------Do not find model :pModelInfo[%d]->modelName:%s ",i, pModelInfo[i]->modelName);
				return FAIL;
			}
		}
	}
	else
	{
		for(int i = 0; i < modelNum; i++)
		{
			string modelName = pModelInfo[i]->modelName;
			sprintf(buff, "%s%s%s", "lib", modelName.c_str(), ".so");
			string dllName(buff);

			CObjectData *pAutoTest = CObjectData::getInstance();
			TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);

			if(iterCase != pAutoTest->m_caseMap.end())
			{
				CCaseRoot* pCaseRoot = &iterCase->second;
				TY_STEP_MAP::iterator iterStep = pCaseRoot->m_StepMap.find(pCaseRoot->m_presentStepId);
				if(iterStep != pCaseRoot->m_StepMap.end())
				{

					TY_STEP* pStep = &iterStep->second;
					TY_PARAM_MAP::iterator iterParam = pStep->paramMap.find(dllName);
					if(iterParam != pStep->paramMap.end())
					{
						TY_PARAM_VAR_MAP* pParamVarMap = &iterParam->second;
						TY_PARAM_VAR_MAP::iterator iter = pParamVarMap->begin();	
						for(int j = 0; j <  pModelInfo[i]->num; j++)
						{
							int paramNum = 0;
							string strFullpath(pModelInfo[i]->fullPath[j]);
							iter = pParamVarMap->find(strFullpath);
							if(iter != pParamVarMap->end())
							{
								TY_PARAM_VAL paramVal;
								memcpy(&pModelInfo[i]->data[j], iter->second.value.value, 8);
									
								//printf("==========get model param var: %lf\n",pModelInfo[i]->data[j]);
								//}
								/*
								sprintf(paramVal.paramName,"%s",iter->second.strParamName.c_str());
								sprintf(paramVal.paraReferenced,"%s",iter->second.strReferenced.c_str());
								paramVal.uParamLength = iter->second.uParamLength;
								paramVal.offset = iter->second.offset;
								getParamVal((char*)modelName.c_str(),&paramVal);
								//modelInfo[i].fullPath[j] = CORBA::string_dup(pModelInfo[i]->fullPath[j]);
								memcpy(&modelInfo[i].data[j], paramVal.value.value , 8);
								//printf("======ZR====get modelvar%d %d:%lf\n",i, j, modelInfo[i].data[j]);
								*/
							}
						}

					}
				}
			}
		}

	}

	return SUCCESS;
}

int setModelParam( TY_MODEL_INFO *  pModelInfo, int modelNum)
{
	if(unlikely(pModelInfo == NULL || modelNum <= 0))
	{
		REPORT(REPORT_ERROR, "Parameter passing error!");
		return FAIL;
	}

	int ret = 0;

	if(MANUAL_TEST_FLAG == 1)
	{
		TY_MODEL_CARD_MAP::iterator iterModel;
		vector<TY_PARAM_INFO>::iterator iter;
		char buff[64] = {0};
		
		for(int i = 0; i < modelNum; i++)
		{
			sprintf(buff, "%s%s%s", "lib", pModelInfo[i]->modelName, ".so");
			string modelName(buff);

			iterModel = modelCard.find(modelName);
			if( iterModel != modelCard.end())
			{
				CCustomDlStep* pCustomDlStep = (CCustomDlStep* )iterModel->second;

				for(int j = 0; j < pModelInfo[i]->num; j++)
				{

					string fullPath(pModelInfo[i]->fullPath[j]);
					for(	iter = pCustomDlStep->m_subStepInfo.paramInfo.begin(); 
							iter != pCustomDlStep->m_subStepInfo.paramInfo.end(); 
							iter++)
					{
						if(strcmp(iter->strParamName.c_str(), fullPath.c_str()) == 0)
						{
							memcpy(iter->value.value,  &pModelInfo[i]->data[j], 8);
						}
						else
						{
							continue;
						}
					}

				}
			}
			else
			{
				REPORT(REPORT_ERROR,"Do not find model :pModelInfo[%d]->modelName :%s",i, pModelInfo[i]->modelName);
				return FAIL;
			}
		}
	}
	else
	{

		for(int i = 0; i < modelNum; i++)
		{
    	    string modelName(pModelInfo[i]->modelName);
			char buff[256] = {0};
			sprintf(buff, "%s%s%s", "lib", modelName.c_str(), ".so");
			string dllName(buff);

			CObjectData *pAutoTest = CObjectData::getInstance();
			TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);

			if(iterCase != pAutoTest->m_caseMap.end())
			{
				CCaseRoot* pCaseRoot = &iterCase->second;
				TY_STEP_MAP::iterator iterStep = pCaseRoot->m_StepMap.find(pCaseRoot->m_presentStepId);
				if(iterStep != pCaseRoot->m_StepMap.end())
				{

					TY_STEP* pStep = &iterStep->second;
					TY_PARAM_MAP::iterator iterParam = pStep->paramMap.find(dllName);
					if(iterParam != pStep->paramMap.end())
					{
						TY_PARAM_VAR_MAP* pParamVarMap = &iterParam->second;
						TY_PARAM_VAR_MAP::iterator iter = pParamVarMap->begin();				
           				for(int j = 0; j <  pModelInfo[i]->num; j++)
						{
							int paramNum = 0;
							//printf("===========ZR======set Model Param=========dllName %s,fullpath:%s, var:%lf\n", dllName.c_str(),modelInfo[i].fullPath[j].in(), modelInfo[i].data[j]);
							iter = pParamVarMap->find(pModelInfo[i]->fullPath[j].in());
							if(iter != pParamVarMap->end())
							{
								memcpy(iter->second.value.value,  &pModelInfo[i]->data[j], 8);
								//printf("======ZR====set modelvar%d %d:%lf %d %lf\n",i, j, modelInfo[i].data[j],iter->second.offset,iter->second.value.dValue);
							}
						}
					}

				}
			}
		}
	}

	return SUCCESS;
}

int getModelVar(TY_MODEL_INFO *  pModelInfo, int modelNum){}
int setModelVar(TY_MODEL_INFO *  pModelInfo, int modelNum){}

int setICDData (TY_SET_ICD_INFO *pIcdInfo, int icdNum){}
int sendICDData(TY_SEND_ICD_INFO *pIcdInfo, int icdNum){}
int getICDData(TY_GET_ICD_INFO *pIcdInfoIn, int icdNum){}