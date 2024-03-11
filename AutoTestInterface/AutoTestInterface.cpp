#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "tinyxml.h"
#include "AutoTestInterface.h"
#include "HiReport.h"
#include "HiCommon.h"
#include "parseXmlApi.h"
#include "HiMGlobalVarLib.h"
#include "HiMParaComLib.h"
#include "ParamData.h"
#include "tinystr.h"
//#include "../HiCurveIncManager/Curve/CCurve.h"

//#include "../HiCaseManager/CaseManager.h"
//#include "../HiCaseManager/StepInstance/CustomStep.h"
//#include "../HiCaseManager/StepInstance/CustomDlStep.h"

#include "CStorageManager.h"
//#include "DataType.h"
#include "StoreModelVar.h"
//#include "CStartModelStep.h"
//#include "CBaseSimpoleInterfaceStep.h"

//#include "../HiICDManager/Block/Block.h"

using namespace std;
#define SUCCESS 0
#define FAIL -1
#define DATA_TYPE_MODEL	 3
#define AUTO_TEST_0 0
#define MANUAL_TEST_1 1
#define MANUAL_MODEL 0

const char *SimpoleHomePath = "SIMPOLE_HOME";

typedef struct _time_compare_{
	struct timeval tStart,tStop;
	int flag;
	struct timeval tOld;
	long long  ttold;
}TY_TIME_COMPARE;
typedef map<string, TY_TIME_COMPARE> REFRESH_MAP;
REFRESH_MAP reFreshCacheMap;

TY_MODEL_VAR_MAP gModelSetVarMap;
//REFRESH_MAP reFreshCacheMap;
int parseXMLData(char* pSubStep, char* value)
{

	CObjectData *pAutoTest = CObjectData::getInstance();
	//parse XML Data
	if(pAutoTest == NULL)
	{
		REPORT(REPORT_ERROR, "Get CObjectData::getInstance() failed\n");
		return FAIL;
	}
	if(pAutoTest->parseXMLData((TiXmlElement*)pSubStep, (TY_TEMPLATE_VALUE*)value) != 0)
	{
		REPORT(REPORT_ERROR, "parseXMLData failed\n");
	}
	return SUCCESS;
}

int setICDData(char *pIcdID, char *pFieldID, TY_VALUEFIELD value)
{
	string strIcdID = pIcdID;
	string strFieldID = pFieldID;
	CObjectData *pAutoTest = CObjectData::getInstance();
	if(pAutoTest == NULL)
	{
		REPORT(REPORT_ERROR, "Get CObjectData::getInstance() failed\n");
		return FAIL;
	}
	if(pAutoTest->setICDData(strIcdID, strFieldID, value) != 0)
	{
		REPORT(REPORT_ERROR, "setICDData failed\n");
		return FAIL;
	}
	return SUCCESS;
}

int getICDData(char *pIcdID, char *pFieldID, TY_VALUEFIELD* value)
{
	string strIcdID = pIcdID;
	string strFieldID = pFieldID;
	CObjectData *pAutoTest = CObjectData::getInstance();
	if(pAutoTest == NULL)
	{
		REPORT(REPORT_ERROR, "Get CObjectData::getInstance() failed\n");
		return FAIL;
	}
	if(pAutoTest->getICDData(strIcdID, strFieldID, (*value)) != 0)
	{
		REPORT(REPORT_ERROR, "getICDData failed\n");
		return FAIL;
	}
	return SUCCESS;
}
/*
int getCurveData(char *pCurveID, double *value)
{
	string strCurveID(pCurveID);
	CObjectData *pAutoTest = CObjectData::getInstance();
	CCurveManager* pCurveManager =  CCurveManager::getInstance();
	TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	TY_CURVE_BIND_MAP::iterator iterBind = pCurveManager->m_curveBindMap.find(iterCase->second.m_presentStepId);
	if(iterBind != pCurveManager->m_curveBindMap.end())
	{
		TY_CURVE_MAP::iterator iterCurve = iterBind->second.find(strCurveID);
		if(iterCurve != iterBind->second.end())
		{
			(*value) = (iterCurve->second)->m_outputValue;
		}

	}

	return SUCCESS;
}
*/
int getRunCycle(int *PeriodMs, int flag) // 0 ->autotestModel  1->simpoleModel
{

	//CCaseManager *pCaseManager = CCaseManager::getInstance();
	CObjectData *pAutoTest = CObjectData::getInstance();
	//CCurveManager *pCurveManager = CCurveManager::getInstance();

	char stepId[128] = {0};
	getPresentStep(stepId);
	string presentStepID = stepId;

	vector<int> RunCycle;

	TY_CURVE_INFO_MAP::iterator iterCurve = pAutoTest->m_curveInfoMap.find(presentStepID);
	if(iterCurve != pAutoTest->m_curveInfoMap.end())
	{

		TY_CURVE_ATTRIBUTE_MAP::iterator iterAttributeCurve = iterCurve->second.begin();

		for( ; iterAttributeCurve != iterCurve->second.end(); iterAttributeCurve++)
		{
			//printf("m_curveID:%s m_excInteral:%d\n",(iterCurve->second)->m_curveID.c_str(), (iterCurve->second)->m_excInteral);

			RunCycle.push_back(iterAttributeCurve->second.m_excInteral);
	
		}
	}
	else
	{
		REPORT(REPORT_ERROR, "iterCurve != pCurveManager->m_curveInfoMap.end():%s\n ", presentStepID.c_str());
	}



	TY_CASE_MAP::iterator caseIter = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	if(caseIter != pAutoTest->m_caseMap.end())
	{
		TY_STEP_MAP::iterator iterStep = caseIter->second.m_StepMap.find(caseIter->second.m_presentStepId);
		if(iterStep != caseIter->second.m_StepMap.end())
		{
			for(TY_ATTRIBUTE_MAP::iterator iterAttribute = iterStep->second.modelAttributeMap.begin(); 
				iterAttribute != iterStep->second.modelAttributeMap.end();
				iterAttribute++)
			{
				
				int tmpPeriodMs = atoi(iterAttribute->second.strPeriodMs.c_str());
				RunCycle.push_back(tmpPeriodMs);
			}
		}
	}


	//find min
	long temp = 0xffffffff;
	unsigned int num = 0;
	for(num=0; num < RunCycle.size(); num++)
	{

		if(temp > RunCycle[num] && RunCycle[num] != 0)
		{
			temp = RunCycle[num];
			//printf(" temp:%ld\n",temp);
		}
	}
	while(temp>0)
	{

		for(num = 0; num < RunCycle.size(); num++)
		{
			if(RunCycle[num] % temp != 0)
			{
				break;
			}

			if(num == (RunCycle.size()-1))
			{
				*PeriodMs = temp;
				caseIter->second.m_tickNum = temp;
				/*
				if(iterBind != pCurveManager->m_curveBindMap.end())
				{
					TY_CURVE_MAP::iterator iterCurve = iterBind->second.begin();
					for( ; iterCurve != iterBind->second.end(); iterCurve++)
					{
						iterCurve->second->setStepValue(temp);
					}
				}
				if(iterCase != pCaseManager->m_caseIdMap.end())
				{
					if(iterCase != pCaseManager->m_caseIdMap.end())
					{
						TY_STEPID_MAP::iterator iterStep = iterCase->second->m_stepIDMap.find(presentStepID);
						if(iterStep != iterCase->second->m_stepIDMap.end())
						{
							iterStep->second->setStepValue(temp);
						}
					}
				}
				*/
				return SUCCESS;
			}
		}
		temp--;
	}
	return FAIL;
}


int sendICDChnData(char *pIcdID, char *pCardType, unsigned int cardID)
{
	string strIcdID = string(pIcdID);
	string strCardType = string(pCardType);
	CObjectData *pAutoTest = CObjectData::getInstance();
	if(pAutoTest == NULL)
	{
		REPORT(REPORT_ERROR, "Get CObjectData::getInstance() failed\n");
		return FAIL;
	}
 if(strIcdID == "")
 {
 return FAIL;
 }
	if(pAutoTest->sendICDChnData(strIcdID, strCardType, cardID) != 0)
	{
		REPORT(REPORT_ERROR, " sendICDChnData failed %s %s %d\n",strIcdID.c_str(), strCardType.c_str(), cardID);
		return FAIL;
	}
	return SUCCESS;
}

int receiveICDChnData(char *pIcdID, char *pCardType, unsigned int cardID)
{
	string strIcdID = pIcdID;
	string strCardType = pCardType;
	CObjectData *pAutoTest = CObjectData::getInstance();
	if(pAutoTest == NULL)
	{
		REPORT(REPORT_ERROR, "Get CObjectData::getInstance() failed\n");
		return FAIL;
	}
	if(pAutoTest->receiveICDChnData(strIcdID, strCardType, cardID) != 0)
	{

		//REPORT(REPORT_ERROR, " receiveICDChnData failed\n");
		return FAIL;
	}
	return SUCCESS;
}


int sendICDData(char* output)
{
	string strIcdID, strCardType, strFieldID;
	TY_OUTPUT_VALUE* tmp = (TY_OUTPUT_VALUE*)output;
	unsigned int cardId = 0;
	TY_VALUEFIELD value;
	strIcdID = (*tmp).strIcdID;
	strCardType = (*tmp).strCardType;
	cardId = (*tmp).cardID;
	vector<TY_OUTPUT_FIELD>::iterator it = (*tmp).vecIcdField.begin();
	for(; it != (*tmp).vecIcdField.end(); it++)
	{
		strFieldID = it->strFieldID;
		value = it->varValue;
		if(setICDData((char*)(strIcdID.c_str()), (char*)(strFieldID.c_str()), value) != 0)
		{
			REPORT(REPORT_ERROR, " setICDData failed\n");
			return FAIL;
		}
	}
	if(sendICDChnData((char*)(strIcdID.c_str()), (char*)(strCardType.c_str()), cardId) != 0)
	{
		REPORT(REPORT_ERROR, " sendICDChnData failed\n");
		return FAIL;
	}
	return SUCCESS;
}

int receiveICDData(char* input, TY_VALUEFIELD* value)
{
	string strIcdID, strCardType, strFieldID;
	TY_INPUT_VALUE *tmp = (TY_INPUT_VALUE *)input;
	unsigned int cardId = 0;
	strIcdID = (*tmp).strIcdID;
	strCardType = (*tmp).strCardType;
	cardId =  (*tmp).cardID;
	strFieldID = (*tmp).vecIcdField[0].strFieldID;
	if(receiveICDChnData((char*)(strIcdID.c_str()), (char*)(strCardType.c_str()), cardId) != 0)
	{
		REPORT(REPORT_ERROR, " receiveICDChnData failed\n");
		return FAIL;
	}
	if((getICDData((char*)(strIcdID.c_str()), (char*)(strFieldID.c_str()), value) != 0))
	{
		REPORT(REPORT_ERROR, " getICDData failed\n");
		return FAIL;
	}

	return SUCCESS;
}

/*
int getVecValueBySubStepID(char *value)
{
	int ret = 0;
	TY_TEMPLATE_VALUE valueArray;
	CObjectData *pAutoTest = CObjectData::getInstance();
	ret = pAutoTest->getVecValueBysubStepID(valueArray);
	memcpy(value, &valueArray, sizeof(TY_TEMPLATE_VALUE));
	return ret;
}
*/

int setPresentStep(char* stepId)
{
	CObjectData *pAutoTest = CObjectData::getInstance();
	TY_CASE_MAP::iterator caseIter = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	if(caseIter != pAutoTest->m_caseMap.end())
	{
		caseIter->second.m_presentStepId = stepId;
	}
	else
	{
		REPORT(REPORT_ERROR, "pAutoTest->m_caseMap.find(%s) ", pAutoTest->m_preCaseId.c_str());
	}

	return 0;
}
int getPresentStep(char* stepId)
{
	CObjectData *pAutoTest = CObjectData::getInstance();
	TY_CASE_MAP::iterator caseIter = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	if(caseIter != pAutoTest->m_caseMap.end())
	{
		strcpy(stepId, caseIter->second.m_presentStepId.c_str());
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}
}
int setPresentCase(char* caseId)
{
	CObjectData *pAutoTest = CObjectData::getInstance();
	pAutoTest->m_preCaseId = caseId;
	return 0;

}
int getPresentCase(char* caseId)
{
	CObjectData *pAutoTest = CObjectData::getInstance();
	strcpy(caseId, pAutoTest->m_preCaseId.c_str());
	return 0;

}
#define PRINTF_VALUE 0
int gFlag = 0;
int updateOutputData(char *modelName)
{
	char buff[254] = {0};
	sprintf(buff, "%s%s%s", "lib", modelName, ".so");
	string dllName(buff);
	string strModelName(modelName);
	if(MANUAL_TEST_FLAG == 0)
	{
		CStorageManager *pStorageManager = CStorageManager::getInstance();
		TY_VAR_DATA varData;
		memset(&varData, 0, sizeof(TY_VAR_DATA));
		struct timeval recvTime;
		gettimeofday(&recvTime, NULL);
		TY_STORAGE_DATA storageData;
		memset(&storageData, 0, sizeof(TY_STORAGE_DATA));
		storageData.sec = recvTime.tv_sec;
		storageData.usec = recvTime.tv_usec;
		sprintf(storageData.uuid,"%s_%s", modelName, "output");
		storageData.dataType = DATA_TYPE_MODEL;
		char *pData = storageData.payload;

		CObjectData *pAutoTest = CObjectData::getInstance();
		TY_CASE_MAP::iterator caseIter = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
		if(caseIter != pAutoTest->m_caseMap.end())
		{
			TY_STEP_MAP::iterator iterStep = caseIter->second.m_StepMap.find(caseIter->second.m_presentStepId);
			if(iterStep != caseIter->second.m_StepMap.end())
			{
				
				TY_OUTPUT_MAP::iterator iterOutput = iterStep->second.outputMap.find(dllName);
				if(iterOutput != iterStep->second.outputMap.end())
				{
					TY_OUTPUT_VAR_MAP::iterator iter = iterOutput->second.begin();
					while(iter != iterOutput->second.end())
					{
						TY_OUTPUT_VAR var = iter->second;
						
						var.value.llValue = 0;
						double value;
						getVarDataByName(var.strReferenced.c_str(), (char*)&value);
#if 0

						if(strcmp(var.strReferenced.c_str(),"AZZKKstatus_WithDataType_1010/Main_Flight_Control/MDL_with_Control/Controlsystem/SS_TX_to_FSECU/PFC_FSECU2/MIL-1553-Constant_Update_fsecu2" ) == 0)
						{
							printf("[%s:%s:%d]-name:%s-valure::%lf\n", __FILE__,__FUNCTION__,__LINE__,var.strReferenced.c_str(),value);
						}
						if(strcmp(var.strReferenced.c_str(),"Bstatus_WithDataType_0904/Main_Flight_Control/MDL_with_Control/Controlsystem/SS_TX_to_FSECU/PFC_FSECU4/MIL-1553-AG1sta_fsecu4" ) == 0)
						{
							printf("[%s:%s:%d]-----name:%s-valure::%lf\n", __FILE__,__FUNCTION__,__LINE__,var.strReferenced.c_str(),value);
						}

#endif
						//printf("-------------------model_name = %s --------model-value = %lf------------\n",var.strReferenced.c_str(),value);
						pAutoTest->setDigitValueByDouble(&(var.value), var.strVarType, value);
						if(!var.strIcdID.empty())
						{
							setICDData((char*)var.strIcdID.c_str(), (char*)var.strFieldID.c_str(), var.value);
#if(0)      
							if (var.strFieldID == string("ba638875-c9b7-44e9-a965-1910b7ee4aae"))
							{
								printf( "=======setIcdData icdId:%s,value:%lf,var.strVarType:%s\n", (char*)var.strIcdID.c_str(),value,var.strVarType.c_str());
							}
#endif            
#if(PRINTF_VALUE)               
#endif
						}
#if(1)
						if((int)value != 0)
						{
					//		printf("output VarName:%s, value:%lf\n",var.strReferenced.c_str(), value);
						}

#endif
						//<< -->>> dst>>

						if((storageData.length + sizeof(varData)) < PAYLOAD_LEN)
						{
							TY_MODEL_STORAGE_MAP::iterator iterModel = pStorageManager->m_modelStorageMap.find(strModelName);
							if(iterModel != pStorageManager->m_modelStorageMap.end())
							{
								//								printf("[%s:%s:%d]--> strReferenced:%s, \n", __FILE__, __FUNCTION__, __LINE__,var.strReferenced.c_str());
								TY_STORAGE_MODEL_VAR_MAP::iterator iterVar = iterModel->second.outputInfo.find(var.strReferenced);
								if(iterVar != iterModel->second.outputInfo.end())
								{

									//strcpy(varData.fullPath, var.strReferenced.c_str());
									memcpy(varData.value, (char*)&value, sizeof(value));
									memcpy(pData, &varData.value, sizeof(double));
									pData = pData + sizeof(double);
									storageData.length = storageData.length + sizeof(double);	
									//printf("------storage: outputs  VarName:%s, value:%lf\n",var.strReferenced.c_str(), value);
								}
							}
#if 0
							TY_MODEL_STORAGE_MAP::iterator iterModel = pStorageManager->m_modelStorageMap.find(strModelName);
							if(iterModel != pStorageManager->m_modelStorageMap.end())
							{
								for(unsigned int i = 0; i < iterModel->second.outputPath.size(); i++)
								{
									//REPORT(REPORT_ERROR, "---------------outputsize:%d,varData.fullPath:%s,iterModel->second.outputPath[i].c_str():%s", iterModel->second.outputPath.size(), var.strReferenced.c_str(), iterModel->second.outputPath[i].c_str());
									if(strcmp(var.strReferenced.c_str(), iterModel->second.outputPath[i].c_str()) == 0)
									{
										//printf("111111111111111111111111111value :%lf\n", value);
										strcpy(varData.fullPath, var.strReferenced.c_str());
										memcpy(varData.value, (char*)&value, sizeof(value));
										memcpy(pData, &varData, sizeof(varData));
										pData = pData + sizeof(varData);
										storageData.length = storageData.length + sizeof(varData);
										//	pStorageManager->insertModelData(&storageData);

									}

								}

							}
#endif
						}
						//<<end
						iter++;
					}
					//printf("--------------------------wc------out-lenlenlen:%d\n",storageData.length);
					if(storageData.length != 0 && MANUAL_TEST_FLAG == 0)	
					{
						pStorageManager->insertModelData(&storageData);
					}
#if(PRINTF_VALUE) 
					string info;
					struct timeval timeStart;
					gettimeofday(&timeStart, NULL);
					int num = 0;
					long long diffTimeTotal = 0;
#endif
					iter = iterOutput->second.begin();
					map<string, int> icdMap;

					while(iter != iterOutput->second.end())
					{
						TY_OUTPUT_VAR var = iter->second;
						if(!var.strIcdID.empty())
						{
							map<string, int>::iterator iterIcd = icdMap.find(var.strIcdID);
							if(iterIcd == icdMap.end())
							{
								icdMap.insert(pair<string, int>(var.strIcdID, 1));
#if(PRINTF_VALUE) 
								struct timeval tStart,tStop;
								gettimeofday(&tStart, NULL);
#endif

								sendICDChnData((char*)var.strIcdID.c_str(), (char*)var.strCardType.c_str(), var.cardID);
#if(PRINTF_VALUE) 
								gettimeofday(&tStop, NULL);
								long long diffTime = (tStop.tv_sec - tStart.tv_sec) * 1000000 + (tStop.tv_usec - tStart.tv_usec);
								char tmp[256] = {0};
								diffTimeTotal += diffTime;
								sprintf(tmp,"NO.%d icd:%s type:%s cardID:%d diffTime %lld\n",num,var.strIcdID.c_str(), var.strCardType.c_str(), var.cardID, diffTime);
								info += string(tmp);
								num ++;
#endif

							}
						}
						iter++;

					}
#if(PRINTF_VALUE) 
					char tmp[256] = {0};
					sprintf(tmp,"diffTimeTotal %lld\n",diffTimeTotal);
					info += string(tmp);
					struct timeval timeStop;
					gettimeofday(&timeStop, NULL);
					long long diff = (timeStop.tv_sec - timeStart.tv_sec) * 1000000 + (timeStop.tv_usec - timeStart.tv_usec);
					gFlag++;
					if (diff > 8000)
					{
						printf("updateOutputData handle diff %ld.size%d.\n", diff,num);
						printf("%s", info.c_str());
					}
#endif
				}
			}

		}

	}
#if MANUAL_MODEL	
	else if(MANUAL_TEST_FLAG == 1)	
	{
		int i;
		//CStorageManager *pStorageManager = CStorageManager::getInstance();
		TY_VAR_DATA varData;
		memset(&varData, 0, sizeof(TY_VAR_DATA));
		struct timeval recvTime;
		gettimeofday(&recvTime, NULL);

		CObjectData *pAutoTest = CObjectData::getInstance();

		//printf("-----------------------manual test model!----outputUpdate\n");
		TY_MODEL_CARD_MAP::iterator iterModel;
		//	printf("[%s:%s:%d]--> dllName:%s, modelCardsize:%d\n", __FILE__, __FUNCTION__, __LINE__,dllName.c_str(),modelCard.size());
		iterModel = modelCard.find(dllName);
		//printf("[%s:%s:%d]--> -----------------------------------------------------------------\n", __FILE__, __FUNCTION__, __LINE__);
		if( iterModel != modelCard.end())
		{
			int paramNum = 0;
			CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;
			//map<string, int> icdMap;

			for(i = 0; i < pCard->m_subStepInfo.outputVarInfo.size(); ++i)
			{
				pCard->m_subStepInfo.outputVarInfo[i].value.llValue = 0;
				double value;
				getVarDataByName(pCard->m_subStepInfo.outputVarInfo[i].strReferenced.c_str(), (char*)&value);
#if(0) 
				if(strcmp(pCard->m_subStepInfo.outputVarInfo[i].strReferenced.c_str(),"Bstatus_WithDataType_0904/Main_Flight_Control/MDL_with_Control/Controlsystem/SS_TX_to_FSECU/PFC_FSECU4/MIL-1553-AG1sta_fsecu4" ) == 0)
				{
					printf("[%s:%s:%d]------------------------------------------------valure::%lf\n", __FILE__,__FUNCTION__,__LINE__,value);
				}
#endif
				//printf("------------------------------shoudong------------------------------model_name = %s --------model-value = %lf------------\n",pCard->m_subStepInfo.outputVarInfo[i].strReferenced.c_str(),value);
				pAutoTest->setDigitValueByDouble(&(pCard->m_subStepInfo.outputVarInfo[i].value), pCard->m_subStepInfo.outputVarInfo[i].strVarType, value);
				if(!pCard->m_subStepInfo.outputVarInfo[i].strIcdID.empty())
				{
					setICDData((char*)pCard->m_subStepInfo.outputVarInfo[i].strIcdID.c_str(),
							(char*)pCard->m_subStepInfo.outputVarInfo[i].strFieldID.c_str(),
							pCard->m_subStepInfo.outputVarInfo[i].value);
				}

#if(PRINTF_VALUE)
				printf("output VarName:%s, value:%lf\n",pCard->m_subStepInfo.inputVarInfo[i].strReferenced.c_str(), value);
#endif

			}

			string info;
			struct timeval timeStart;
			gettimeofday(&timeStart, NULL);
			int num = 0;
			long long diffTimeTotal = 0;
			map<string, int> icdMap;
			for(i = 0; i < pCard->m_subStepInfo.outputVarInfo.size(); ++i)
			{
				map<string, int>::iterator iterIcd = icdMap.find(pCard->m_subStepInfo.outputVarInfo[i].strIcdID);
				if(iterIcd == icdMap.end())
				{
					if(pCard->m_subStepInfo.outputVarInfo[i].cardID == 10)
					{
						//printf("[%s:%s:%d]--->card = 0, cardType;%s,icdID:%s\n ",__FILE__, __FUNCTION__, __LINE__, pCard->m_subStepInfo.outputVarInfo[i].strCardType.c_str(),pCard->m_subStepInfo.outputVarInfo[i].strIcdID.c_str());
					}
					icdMap.insert(pair<string, int>(pCard->m_subStepInfo.outputVarInfo[i].strIcdID, 1));
					//sendICDChnData((char*)pCard->m_subStepInfo.outputVarInfo[i].strIcdID.c_str(), (char*)pCard->m_subStepInfo.outputVarInfo[i].strCardType.c_str(), pCard->m_subStepInfo.outputVarInfo[i].cardID);
					struct timeval tStart,tStop;
					gettimeofday(&tStart, NULL);
					sendICDChnData((char*)pCard->m_subStepInfo.outputVarInfo[i].strIcdID.c_str(), (char*)pCard->m_subStepInfo.outputVarInfo[i].strCardType.c_str(), pCard->m_subStepInfo.outputVarInfo[i].cardID);
					gettimeofday(&tStop, NULL);
					long long diffTime = (tStop.tv_sec - tStart.tv_sec) * 1000000 + (tStop.tv_usec - tStart.tv_usec);
					char tmp[256] = {0};
					diffTimeTotal += diffTime;
					sprintf(tmp,"NO.%d icd:%s type:%s cardID:%d diffTime %lld\n",num,pCard->m_subStepInfo.outputVarInfo[i].strIcdID.c_str(), 
							pCard->m_subStepInfo.outputVarInfo[i].strCardType.c_str(), pCard->m_subStepInfo.outputVarInfo[i].cardID, diffTime);
					info += string(tmp);
					num ++;
				}
			}
			char tmp[256] = {0};
			sprintf(tmp,"diffTimeTotal %lld\n",diffTimeTotal);
			info += string(tmp);

			struct timeval timeStop;
			gettimeofday(&timeStop, NULL);
			long long diff = (timeStop.tv_sec - timeStart.tv_sec) * 1000000 + (timeStop.tv_usec - timeStart.tv_usec);
			gFlag++;
			if (diff > 8000)
			{
				printf("updateOutputData handle diff %ld.size%d.\n", diff,num);
				printf("%s", info.c_str());
			}
		}

	}
#endif

}

int updateInputData(char *modelName)
{
	char buff[64] = {0};
	sprintf(buff, "%s%s%s", "lib", modelName, ".so");
	string dllName(buff);
	string strModelName(modelName);
	int ret = 0;
	int i;
	int indexRead = 0;
	int rtuReadData[2048] = {0};
	//printf("-----------------------manual test model!----inputUpdate1111\n");
	//	if(MANUAL_TEST_FLAG == 0)
	//	{

	CObjectData *pAutoTest = CObjectData::getInstance();
	TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	//printf("m_presentStepId:%s\n", iterCase->second.m_presentStepId.c_str());
	TY_STEP_MAP::iterator iterStep = iterCase->second.m_StepMap.find(iterCase->second.m_presentStepId);

	CStorageManager *pStorageManager = CStorageManager::getInstance();
	TY_VAR_DATA varData;
	memset(&varData, 0, sizeof(TY_VAR_DATA));
	struct timeval recvTime;
	gettimeofday(&recvTime, NULL);
	TY_STORAGE_DATA storageData;
	memset(&storageData, 0, sizeof(TY_STORAGE_DATA));
	storageData.sec = recvTime.tv_sec;
	storageData.usec = recvTime.tv_usec;
	sprintf(storageData.uuid,"%s_%s", modelName, "input");
	storageData.dataType = DATA_TYPE_MODEL;
	char *pData = storageData.payload;

	if(iterStep != iterCase->second.m_StepMap.end())
	{
		//	printf("[%s:%s:%d]--> ret :%d,var.strBindType:%s\n", __FILE__, __FUNCTION__, __LINE__, ret,dllName.c_str());
		TY_INPUT_MAP::iterator iterInput = iterStep->second.inputMap.find(dllName);
		if(iterInput != iterStep->second.inputMap.end())
		{

			TY_INPUT_VAR_MAP::iterator iter = iterInput->second.begin();
			map<string, int> icdMap;
			while(iter != iterInput->second.end())
			{
				TY_INPUT_VAR var = iter->second;
				//printf("sldfj;saldfsafd------------->Map->value:%lf,name:%s,inputVar.strVarType:%s\n", iter->second.value.dValue, iter->second.strReferenced.c_str(),iter->second.strVarType.c_str());
				if(var.strBindType == "ICD")
				{
					map<string, int>::iterator iterIcd = icdMap.find(var.strIcdID);
					if(iterIcd == icdMap.end())
					{

						rtuReadData[indexRead++] = receiveICDChnData((char*)var.strIcdID.c_str(), (char*)var.strCardType.c_str(), var.cardID);   
						icdMap.insert(pair<string, int>(var.strIcdID,rtuReadData[indexRead - 1]));

					}


				}
				iter++;
			}
			indexRead = 0;
			iter = iterInput->second.begin();
			string tmpStricd = "";
			int flagloop = 0;
			while(iter != iterInput->second.end())
			{
				TY_INPUT_VAR var = iter->second;
				double value = 0;
				if(var.strBindType == "ICD")
				{
					map<string, int>::iterator iterIcd = icdMap.find(var.strIcdID);
					if(iterIcd != icdMap.end())
					{
						if(iterIcd->second != 0)
						{

							value = 0;
							ret = 0;
						}
						else
						{
							ret = getICDData((char*)var.strIcdID.c_str(), (char*)var.strFieldID.c_str(), &(var.value));
							pAutoTest->getDigitValueByDouble(var.value, var.strVarType, &value);

						}	
					}
#if 0
					if (var.strIcdID == string("ba638875-c9b7-44e9-a965-1910b7ee4aae"))
					{
						printf("=======getICDData icdId:%s,value:%lf,var.strVarType:%s\n", (char*)var.strIcdID.c_str(),value,var.strVarType.c_str());
					}

#endif			
				}
				/*
				   else if(var.strBindType == "Curve")
				   {
				   ret = getCurveData((char*)var.strCurveID.c_str(), &value);
#if(PRINTF_VALUE)
printf("VarName:%s, value:%lf\n",var.strReferenced.c_str(), value);
#endif
}
*/
				else if(var.strBindType == "Constant")
				{
					ret = 0;
					value = var.value.dValue;
				#if 1
					//printf("--VarName:%s, value:%lf\n",var.strReferenced.c_str(), value);
				#endif
				}
				else
				{
					ret = 0;
					TY_MODEL_VAR_MAP::iterator iter = gModelSetVarMap.find(var.strReferenced);
					if(iter != gModelSetVarMap.end())
					{
						value = iter->second.value;
					}
				}

if(ret == 0 && var.strBindType != "Curve")
{


#if 0
	printf("[%s:%s:%d]-name:%s-valure::%lf\n", __FILE__,__FUNCTION__,__LINE__,var.strReferenced.c_str(),value);

#endif
	setVarDataByName(var.strReferenced.c_str(), (char*)&value);
}
//>> modefy by chao.wu3 --->src
#if 0
if((storageData.length + sizeof(varData)) < PAYLOAD_LEN)
{
	strcpy(varData.fullPath, var.strReferenced.c_str());

	memcpy(varData.value, (char*)&value, sizeof(value));
	memcpy(pData, &varData, sizeof(varData));

	memcpy((char*)&value, varData.value, sizeof(value));
	//printf("fullPath:%s value:%lf\n", varData.fullPath, value);
	pData = pData + sizeof(varData);
	storageData.length = storageData.length + sizeof(varData);
}
#endif
// << --->dst >>
//printf("---------------,var:%s,StoLen:%d\n", var.strReferenced.c_str(), storageData.length);
if((storageData.length + sizeof(varData)) < PAYLOAD_LEN)
{
	TY_MODEL_STORAGE_MAP::iterator iterModel = pStorageManager->m_modelStorageMap.find(strModelName);
	if(iterModel != pStorageManager->m_modelStorageMap.end())
	{
		TY_STORAGE_MODEL_VAR_MAP::iterator iterVar = iterModel->second.inputInfo.find(var.strReferenced);
		if(iterVar != iterModel->second.inputInfo.end())
		{
		    if(var.strBindType == "Curve")
		    {
		        memcpy(varData.value, (char*)var.value.value, sizeof(double));
		    }
		    else
		    {
		       	//strcpy(varData.fullPath, var.strReferenced.c_str());
			    memcpy(varData.value, (char*)&value, sizeof(value));
		    }

			memcpy(pData, &varData.value, sizeof(double));
			pData = pData + sizeof(double);
			storageData.length = storageData.length + sizeof(double);	
		}
	}

}
//<<end
iter++;
}
//printf("--- storage:len:%d name = %s value = %lf\n",storageData.length, var.strReferenced, varData.value);
if(storageData.length != 0 && MANUAL_TEST_FLAG == 0)	
{
	pStorageManager->insertModelData(&storageData);
}
}

}

//}
#if MANUAL_MODEL
else if(MANUAL_TEST_FLAG == 1)	
{
	CObjectData *pAutoTest = CObjectData::getInstance();
	//CStorageManager *pStorageManager = CStorageManager::getInstance();
	TY_VAR_DATA varData;
	memset(&varData, 0, sizeof(TY_VAR_DATA));
	struct timeval recvTime;
	gettimeofday(&recvTime, NULL);

	TY_MODEL_CARD_MAP::iterator iterModel;
	iterModel = modelCard.find(dllName);
	if( iterModel != modelCard.end())
	{

		int paramNum = 0;
		CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;
		map<string, int> icdMap;
		//printf("====inputVal -------------------modelName:%s,inputVarInfoLen:%d,outLen:%d\n",dllName.c_str(),pCard->m_subStepInfo.inputVarInfo.size(),pCard->m_subStepInfo.outputVarInfo.size());
		for(i = 0; i < pCard->m_subStepInfo.inputVarInfo.size(); ++i)
		{
			//printf("====inputVal loop===\n");
			if(pCard->m_subStepInfo.inputVarInfo[i].strBindType == "ICD")
			{
				map<string, int>::iterator iterIcd = icdMap.find(pCard->m_subStepInfo.inputVarInfo[i].strIcdID);
				if(iterIcd == icdMap.end())
				{
					//icdMap.insert(pair<string, int>(pCard->m_subStepInfo.inputVarInfo[i].strIcdID, 1));
					rtuReadData[i] = receiveICDChnData((char*)pCard->m_subStepInfo.inputVarInfo[i].strIcdID.c_str(),
							(char*)pCard->m_subStepInfo.inputVarInfo[i].strCardType.c_str(),
							pCard->m_subStepInfo.inputVarInfo[i].cardID);
					icdMap.insert(pair<string, int>(pCard->m_subStepInfo.inputVarInfo[i].strIcdID, rtuReadData[indexRead]));
				}
			}

		}
		//indexRead = 0;
		for(i = 0; i < pCard->m_subStepInfo.inputVarInfo.size(); i++)
		{
			//printf("input loop---------------i = %d--\n", i);
			//printf("-----------------------manual test model!----12211::strBindType:%s\n",pCard->m_subStepInfo.inputVarInfo[i].strBindType.c_str());
			double value = 0;
			if(pCard->m_subStepInfo.inputVarInfo[i].strBindType == "ICD")
			{

				map<string, int>::iterator iterIcd = icdMap.find(pCard->m_subStepInfo.inputVarInfo[i].strIcdID);
				if(iterIcd != icdMap.end())
				{
					if(iterIcd->second != 0)
					{

						//REPORT(REPORT_INFO, "------------ret READDATA Value = 0, i = %d\n", i);
						value = 0;
						ret = 0;
						/*
						   if(strcmp("Bstatus_WithDataType_0904/PDU_System_Simulator/ICD_unpack_FSECU2FPDU/Label_202_Label", pCard->m_subStepInfo.inputVarInfo[i].strReferenced.c_str()) == 0)
						   {
						   printf("---------------vaname:%s, value:%lf---------------------------\n",pCard->m_subStepInfo.inputVarInfo[i].strReferenced.c_str(),value);
						   }
						   */
					}
					else
					{
						ret = getICDData((char*)pCard->m_subStepInfo.inputVarInfo[i].strIcdID.c_str(),
								(char*)pCard->m_subStepInfo.inputVarInfo[i].strFieldID.c_str(),
								&(pCard->m_subStepInfo.inputVarInfo[i].value));
						pAutoTest->getDigitValueByDouble(pCard->m_subStepInfo.inputVarInfo[i].value, pCard->m_subStepInfo.inputVarInfo[i].strVarType, &value);

#if 0
						if(strcmp("6a556b06-6a55-4e56-8c58-0c1af01556ac", (char*)pCard->m_subStepInfo.inputVarInfo[i].strIcdID.c_str()) == 0)
						{
							printf("=======getICDData icdId:%s,value:%lf,var.strVarType:%s\n", (char*)pCard->m_subStepInfo.inputVarInfo[i].strIcdID.c_str(),
									value,pCard->m_subStepInfo.inputVarInfo[i].strVarType.c_str());

						}
#endif
					}	
				}

#if(PRINTF_VALUE)
				printf("VarName:%s, value:%lf\n",pCard->m_subStepInfo.inputVarInfo[i].strReferenced.c_str(), value);
#endif
			}
			else if(pCard->m_subStepInfo.inputVarInfo[i].strBindType == "Curve")
			{	
				ret = getCurveData((char*)pCard->m_subStepInfo.inputVarInfo[i].strCurveID.c_str(), &value);
#if(PRINTF_VALUE)
				printf("VarName:%s, value:%lf\n",pCard->m_subStepInfo.inputVarInfo[i].strReferenced.c_str(), value);
#endif
			}
			else if(pCard->m_subStepInfo.inputVarInfo[i].strBindType == "Constant")
			{
				ret = 0;
				value = pCard->m_subStepInfo.inputVarInfo[i].value.dValue;
				//printf("================input Var[%d] = %lf========\n",i, value);
#if(PRINTF_VALUE)
				printf("VarName:%s, value:%lf\n",pCard->m_subStepInfo.inputVarInfo[i].strReferenced.c_str(), value);
#endif
			}
			else
			{
				value = pCard->m_subStepInfo.inputVarInfo[i].value.dValue;
				ret = 0;
				//printf("================input: strReferenced:%s, var[%d] = %lf========\n",pCard->m_subStepInfo.inputVarInfo[i].strReferenced.c_str(), i, value);
			}

			if(ret == 0)
			{

				setVarDataByName(pCard->m_subStepInfo.inputVarInfo[i].strReferenced.c_str(), (char*)&value);

			}

		}
		//	pStorageManager->insertModelData(&storageData);

	}
	else
	{
		printf("[%s:%s:%d]-->Do not found!\n",__FILE__,__FUNCTION__,__LINE__);
		return -1;
	}

}
#endif

return 0;
}

int updateParamData(void* pParamComInfo, char *modelName)
{
	char buff[64] = {0};
	TY_MODEL_PARAP_COM_INFO* handle = (TY_MODEL_PARAP_COM_INFO*)pParamComInfo;
	sprintf(buff, "%s%s%s", "lib", modelName, ".so");
	string dllName(buff);
	int i;
	int count= 0;
	//	if(MANUAL_TEST_FLAG == 0)
	//	{
	CObjectData *pAutoTest = CObjectData::getInstance();
	TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	TY_STEP_MAP::iterator iterStep = iterCase->second.m_StepMap.find(iterCase->second.m_presentStepId);
	if(iterStep != iterCase->second.m_StepMap.end())
	{
		TY_PARAM_MAP::iterator iterParam = iterStep->second.paramMap.find(dllName);
		if(iterParam != iterStep->second.paramMap.end())
		{
			TY_PARAM_VAR_MAP::iterator iter = iterParam->second.begin();
			int paramNum = 0;
			while(iter != iterParam->second.end())
			{
				long length = 0;

				handle->sPararInfos[paramNum].varLength =  iter->second.uParamLength;
				handle->sPararInfos[paramNum].offset = iter->second.offset;
				memcpy(handle->sPararInfos[paramNum].data,  iter->second.value.value,  handle->sPararInfos[paramNum].varLength);

				double value = 0;
				memcpy(&value, handle->sPararInfos[paramNum].data, sizeof(double));
				//printf("----------------------value:%lf %d\n", value,iter->second.offset);
				string fullpath = iter->second.strParamName.c_str();	
				iter++;
				paramNum++;
				//printf("----------------unt = %d,paramNum:%d\n",count,paramNum);
				//printf("count = %d,paramNum:%d, value = %lf iter->second.uParamLength:%d, strParamName:%s\n",count,paramNum,iter->second.value.value,iter->second.uParamLength,iter->second.strParamName.c_str());
				//printf("count = %d,paramNum:%d, value = %lf  strParamName:%s\n",count,paramNum,value,fullpath.c_str());
				count++;
			}
			handle->setParaTabelLen = paramNum;//modify by zr 20230622
			//handle->setParaTabelLen = paramNum;
		}
	}
	//	}
#if MANUAL_MODEL
	else if(MANUAL_TEST_FLAG == 1)
	{
		//printf("-----------------------manual test model!----paramUpdate--modelName:%s-\n",dllName.c_str());
		TY_MODEL_CARD_MAP::iterator iterModel;
		iterModel = modelCard.find(dllName);
		if( iterModel != modelCard.end())
		{
			int paramNum = 0;
			CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;
			for(i = 0; i < pCard->m_subStepInfo.paramInfo.size(); ++i)
			{
				long length = 0;
				handle->setParaTabelLen = 1;
				handle->sPararInfos[paramNum].varLength =  pCard->m_subStepInfo.paramInfo[i].uParamLength;
				handle->sPararInfos[paramNum].offset = pCard->m_subStepInfo.paramInfo[i].offset;
				memcpy(handle->sPararInfos[paramNum].data,  pCard->m_subStepInfo.paramInfo[i].value.value,  handle->sPararInfos[paramNum].varLength);
				//printf("=======================param update data[%d]:%lf\n",i,pCard->m_subStepInfo.paramInfo[i].value.dValue);
				paramNum++;

			}
			handle->setParaTabelLen = paramNum;

		}
		else
		{
			printf("[%s:%s:%d]-->Do not found!\n",__FILE__,__FUNCTION__,__LINE__);
			return -1;
		}

	}	
#endif
	return 0;
}


int parseCaseBinding(char* xmlPath)
{
	CObjectData *pAutoTest = CObjectData::getInstance();
	pAutoTest->m_caseMap.clear();
	gModelSetVarMap.clear();
	char filePath[256] = {0};
	getServerPath(filePath);
	string strXmlPath(filePath);
	string dictionaryXmlPath;
	if(MANUAL_TEST_FLAG == 1)
	{
		printf("--------------parseCaseBinding---ModelPara.xml\n");
		strXmlPath = strXmlPath + "ModelPara.xml";
	}
	else
	{
		strXmlPath = strXmlPath + "CaseInfo.xml";
	}

	//REPORT(REPORT_INFO, " --------pasecaseInfo.xmlPath:%s\n", strXmlPath.c_str());
	TiXmlDocument mydoc(strXmlPath.c_str());
	bool loadOk = mydoc.LoadFile();
	if (!loadOk)
	{
		std::string errStr = mydoc.ErrorDesc();
		REPORT(REPORT_ERROR, " parse XML Data error:%s\n", errStr.c_str());
		return -1;
	}
	TiXmlElement* rootElement = mydoc.RootElement();
	TiXmlElement* pRootXML = rootElement;
	TiXmlElement* pCaseRoot = pRootXML->FirstChildElement();

	TY_INPUT_VAR_MAP inPutVarMap;
	inPutVarMap.clear();
	//<<clear map
	//>>clear map

	for (TiXmlElement* pCaseStep = pCaseRoot->FirstChildElement(); pCaseStep != NULL; pCaseStep = pCaseStep->NextSiblingElement())
	{
		CCaseRoot caseRoot;
		if(pCaseStep == NULL)
		{
			REPORT(REPORT_ERROR, " pCaseStep is NULL failed\n");
		}
		for (TiXmlAttribute* pTmp = pCaseStep->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
		{
			if(pTmp == NULL)
			{
				REPORT(REPORT_ERROR, " pTmp is NULL failed\n");
			}
			if (strcmp(pTmp->Name(), "CaseID") == 0)
			{
				caseRoot.m_strCaseID = pTmp->Value();
			}
			else if (strcmp(pTmp->Name(), "Name") == 0)
			{
				caseRoot.m_strName = pTmp->Value();
			}
			else if (strcmp(pTmp->Name(), "StepNum") == 0)
			{
				caseRoot.m_strStepNum = atoi(pTmp->Value());
			}
		}
		for (TiXmlElement* pStep = pCaseStep->FirstChildElement(); pStep != NULL; pStep = pStep->NextSiblingElement())
		{
			TY_STEP stepTmp;
			if(pStep == NULL)
			{
				REPORT(REPORT_ERROR, " pStep is NULL failed\n");
			}

			for (TiXmlAttribute* pTmp = pStep->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
			{
				if(pTmp == NULL)
				{
					REPORT(REPORT_ERROR, " pStep is NULL failed\n");
				}
				if (strcmp(pTmp->Name(), "Command") == 0)
				{
					stepTmp.strCommand = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "CtlCmd") == 0)
				{
					stepTmp.strSubCmd = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "StepID") == 0)
				{
					stepTmp.strStepID = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "StepName") == 0)
				{
					stepTmp.strStepName = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "PeriodMs") == 0)
				{
					stepTmp.strPeriodMs = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "Type") == 0)
				{
					stepTmp.strType = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "Times") == 0)
				{
					stepTmp.strTimes = pTmp->Value();
				}
			}
			if(strcmp(stepTmp.strCommand.c_str(), "start") == 0 || (strcmp(stepTmp.strCommand.c_str(), "simpole") == 0 && strcmp(stepTmp.strSubCmd.c_str(), "startModel") == 0))
			{
				for (TiXmlElement* pSubStep = pStep->FirstChildElement(); pSubStep != NULL; pSubStep = pSubStep->NextSiblingElement())
				{
					if(pSubStep == NULL)
					{
						REPORT(REPORT_ERROR, " pStep is NULL failed\n");
					}
					CSubStep subStepTmp;
					TY_ATTRIBUTE_MODEL modelAttributeTmp;
					for (TiXmlAttribute* pTmp = pSubStep->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
					{
						if(pTmp == NULL)
						{
							REPORT(REPORT_ERROR, " pStep is NULL failed\n");
						}
						if (strcmp(pTmp->Name(), "SubStepID") == 0)
						{
							subStepTmp.m_strStepID = pTmp->Value();
						}
						else if (strcmp(pTmp->Name(), "Dll") == 0)
						{
							subStepTmp.m_strDllName = pTmp->Value();
							modelAttributeTmp.strDllName = pTmp->Value();
						}
						else if (strcmp(pTmp->Name(), "ModelName") == 0)
						{
							subStepTmp.m_strDllName = pTmp->Value();
							subStepTmp.m_strDllName = "lib" + subStepTmp.m_strDllName + ".so";
							modelAttributeTmp.strDllName = "lib" + subStepTmp.m_strDllName + ".so";
						}
						else if (strcmp(pTmp->Name(), "PeriodMs") == 0)
						{
							modelAttributeTmp.strPeriodMs = pTmp->Value();
						}
						else if (strcmp(pTmp->Name(), "Times") == 0)
						{
							modelAttributeTmp.strTimes = pTmp->Value();
						}
						else if (strcmp(pTmp->Name(), "Type") == 0)
						{
							modelAttributeTmp.strTerminalType = pTmp->Value();
						}
					}

					stepTmp.modelAttributeMap.insert(pair<string, TY_ATTRIBUTE_MODEL>(subStepTmp.m_strDllName, modelAttributeTmp));

					TiXmlElement *pParaElement = pSubStep->FirstChildElement("parameter");
					TiXmlElement *pOutputElement = pSubStep->FirstChildElement("output");
					TiXmlElement *pInputElement = pSubStep->FirstChildElement("input");

					//parse pOutputElement
					TY_OUTPUT_VAR_MAP outputVarMap;
					for(TiXmlElement *pTmpElement = pOutputElement->FirstChildElement(); pTmpElement != NULL; pTmpElement=pTmpElement->NextSiblingElement())
					{
						TY_OUTPUT_VAR outputVar;
						for (TiXmlAttribute* pIcd = pTmpElement->FirstAttribute(); pIcd != NULL; pIcd = pIcd->Next())
						{
							if (strcmp(pIcd->Name(), "ICDID") == 0)
							{
								outputVar.strIcdID = pIcd->Value();
							}
							else if (strcmp(pIcd->Name(), "CardType") == 0)
							{
								outputVar.strCardType = pIcd->Value();
							}
							else if (strcmp(pIcd->Name(), "CardID") == 0)
							{
								outputVar.cardID = atoi(pIcd->Value());
							}
							else if (strcmp(pIcd->Name(), "ChnID") == 0)
							{
								outputVar.chnID = atoi(pIcd->Value());
							}
							else if (strcmp(pIcd->Name(), "FieldID") == 0)
							{
								outputVar.strFieldID = pIcd->Value();
							}
							/*
							   else if (strcmp(pIcd->Name(), "VarName") == 0)
							   {
							   outputVar.strReferenced = "untitled1/" + string(pIcd->Value());  //等待修改。。。
							   printf("--------uparse------vaname:%s\n", outputVar.strReferenced.c_str());
							   }
							   */
							else if (strcmp(pIcd->Name(), "Referenced") == 0) //modify by wc 20231219
							{
								outputVar.strReferenced = string(pIcd->Value());  
								printf("--------uparse------vaname:%s\n", outputVar.strReferenced.c_str());
							}
							else if (strcmp(pIcd->Name(), "DataType") == 0)
							{
								outputVar.strVarType = pIcd->Value();
							}
						}
						outputVarMap.insert(pair<string, TY_OUTPUT_VAR>(outputVar.strReferenced, outputVar));
					}
					stepTmp.outputMap.insert(pair<string, TY_OUTPUT_VAR_MAP>(subStepTmp.m_strDllName, outputVarMap));


					for(TiXmlElement *pTmpElement = pInputElement->FirstChildElement(); pTmpElement != NULL; pTmpElement=pTmpElement->NextSiblingElement())
					{

						TY_INPUT_VAR inputVar;
						GetValue(pTmpElement, "BindType", inputVar.strBindType);
						if(inputVar.strBindType == "Curve")
						{
							GetValue(pTmpElement, "CurveID", inputVar.strCurveID);
							//GetValue(pTmpElement, "VarName", inputVar.strReferenced);
							GetValue(pTmpElement, "Referenced", inputVar.strReferenced);//modify by wc 20231219

						}
						else if(inputVar.strBindType == "Constant")
						{
							//GetValue(pTmpElement, "VarName", inputVar.strReferenced);
							GetValue(pTmpElement, "Referenced", inputVar.strReferenced);//modify by wc 20231219
							GetValue(pTmpElement, "DataType", inputVar.strVarType);
							string value;
							GetValue(pTmpElement, "Value", value);
							inputVar.value.dValue = atof(value.c_str());
							printf("==========================Parse referenced:%s, Value:%lf,inputVar.value.dValue:%lf \n",inputVar.strReferenced.c_str(),  atof(value.c_str()),inputVar.value.dValue);

						}
						else if(inputVar.strBindType == "ICD")
						{
							for (TiXmlAttribute* pIcd = pTmpElement->FirstAttribute(); pIcd != NULL; pIcd = pIcd->Next())
							{
								if(pIcd == NULL)
								{
									REPORT(REPORT_ERROR, " pTmp is NULL failed\n");
								}
								if (strcmp(pIcd->Name(), "ICDID") == 0)
								{
									inputVar.strIcdID = pIcd->Value();
								}
								else if (strcmp(pIcd->Name(), "CardType") == 0)
								{
									inputVar.strCardType = pIcd->Value();
								}
								else if (strcmp(pIcd->Name(), "CardID") == 0)
								{
									inputVar.cardID = atoi(pIcd->Value());
								}
								else if (strcmp(pIcd->Name(), "ChnID") == 0)
								{
									inputVar.chnID = atoi(pIcd->Value());
								}
								else if (strcmp(pIcd->Name(), "FieldID") == 0)
								{
									inputVar.strFieldID = pIcd->Value();
								}
								/*
								   else if (strcmp(pIcd->Name(), "VarName") == 0)
								   {
								   inputVar.strReferenced = "untitled1/" + string(pIcd->Value());
								   printf("--------input------vaname:%s\n", inputVar.strReferenced.c_str());
								   }
								   */
								else if (strcmp(pIcd->Name(), "Referenced") == 0) //modify by wc 20231219
								{
									inputVar.strReferenced = string(pIcd->Value());  
									printf("--------input------vaname:%s\n", inputVar.strReferenced.c_str());
								}
								else if (strcmp(pIcd->Name(), "DataType") == 0)
								{
									inputVar.strVarType = pIcd->Value();
								}

								else if (strcmp(pIcd->Name(), "CurveID") == 0)
								{
									inputVar.strCurveID = pIcd->Value();
								}
							}
						}
						else
						{
							GetValue(pTmpElement, "Referenced", inputVar.strReferenced);
							GetValue(pTmpElement, "DataType", inputVar.strVarType);
							string value;
							GetValue(pTmpElement, "Value", value);
							inputVar.value.dValue = atof(value.c_str());
							//inputVar.value.dValue = 0.0;

							TY_MODEL_VAR_MAP::iterator iterTmp = gModelSetVarMap.find(inputVar.strReferenced);
							if(iterTmp == gModelSetVarMap.end())
							{
								TY_MODEL_VAR modelVar;
								modelVar.value = inputVar.value.dValue;
								modelVar.strName = inputVar.strReferenced;
								gModelSetVarMap.insert(pair<string, TY_MODEL_VAR>(inputVar.strReferenced, modelVar));
								//gModelSetVarMap.insert(pair<string, TY_MODEL_VAR>(inputVar.strReferenced, modelVar));
								printf("===2323=====Parse referenced:%s, inputVar.value.dValue:%lf \n",inputVar.strReferenced.c_str(),  modelVar.value);
							}

						}

						inPutVarMap.insert(pair<string, TY_INPUT_VAR>(inputVar.strReferenced, inputVar));

						for(TY_INPUT_VAR_MAP::iterator iterTmp = inPutVarMap.begin(); iterTmp != inPutVarMap.end();iterTmp++)
						{
							printf("==========map->value:%lf,name:%s\n", iterTmp->second.value.dValue, iterTmp->second.strReferenced.c_str());
						}

					}
					stepTmp.inputMap.insert(pair<string, TY_INPUT_VAR_MAP>(subStepTmp.m_strDllName, inPutVarMap));



					int count = 0;
					TY_PARAM_VAR_MAP paramVarMap;
					for(TiXmlElement *pTmpElement = pParaElement->FirstChildElement(); pTmpElement != NULL; pTmpElement=pTmpElement->NextSiblingElement())
					{
						TY_PARAM_VALUE paramVar;
						string tmpName;
						for (TiXmlAttribute* tmp = pTmpElement->FirstAttribute(); tmp != NULL; tmp = tmp->Next())
						{
							if (strcmp(tmp->Name(), "Referenced") == 0) //modyfiy by zr 20230617
							{
								paramVar.strParamName = tmp->Value();
							}
							else if (strcmp(tmp->Name(), "Name") == 0)
							{
								tmpName = tmp->Value();
							}
							else if (strcmp(tmp->Name(), "DataType") == 0)
							{
								paramVar.strParamType = tmp->Value();
							}
							else if (strcmp(tmp->Name(), "Length") == 0)
							{
								paramVar.uParamLength = atoi(tmp->Value());
							}
							else if (strcmp(tmp->Name(), "Offset") == 0)
							{
								paramVar.offset = atoi(tmp->Value());
							}
							else if (strcmp(tmp->Name(), "Value") == 0)
							{
								pAutoTest->setDigitValueByString(&paramVar.value, paramVar.strParamType, tmp->Value());
							}

						}
						paramVar.strParamName = paramVar.strParamName + "/" + tmpName;

						printf("---------parsecasebinding--- count:%d, strname:%s\n",count++, paramVar.strParamName.c_str());
						paramVarMap.insert(pair<string, TY_PARAM_VALUE>(paramVar.strParamName, paramVar));
					}
					stepTmp.paramMap.insert(pair<string, TY_PARAM_VAR_MAP>(subStepTmp.m_strDllName, paramVarMap));
				}
				caseRoot.m_StepMap.insert(pair<string, TY_STEP>(stepTmp.strStepID, stepTmp));

				//REPORT(REPORT_ERROR, "==================insert stop map success=====stepTmp.strStepID:%s\n",stepTmp.strStepID.c_str());
			}

		}
		pAutoTest->m_caseMap.insert(pair<string, CCaseRoot>(caseRoot.m_strCaseID, caseRoot));
		//REPORT(REPORT_ERROR, "==================insert case map success===========caseRoot.m_strCaseID:%s,pAutoTestAdd:%x\n",caseRoot.m_strCaseID.c_str(),pAutoTest);


	}
	return SUCCESS;
}

int creatDataDictionaryManualXml(char * testPath, const char *modelName, int modeSelect)
{
	char* pathSimpole = NULL;
	int ret = 0;
	char filePath[256] = {0};
	int offset = 0;
	int pos1 = 0;
	string dllName(modelName);
	string simpoleModelName(modelName);
	//pos1 = dllName.find_last_of(".");
	//dllName = dllName.substr(3, pos1-3);
	string modelXmlPath;
	getServerPath(filePath);
	string strXmlPath(filePath);
	string uuidPath(testPath);
	string dictionaryXmlPath;
	//string strName; //000000000000000000000
	//if(pathSimpole == NULL)
	//{
	pathSimpole = getenv(SimpoleHomePath);//"SIMPOLE_HOME"
	//}
	string strSimpoleXmlPath(pathSimpole);

	TiXmlDocument *writeXml = new TiXmlDocument;
	TiXmlElement *caseElement = new TiXmlElement("project");
	TiXmlElement *modelElement = new TiXmlElement("model");

	caseElement->SetAttribute("name", "Model Data Dictionary");
	caseElement->SetAttribute("simVersion", "2.0");
	caseElement->SetAttribute("type", "dataDictionary.xml");	
	//modelElement->SetAttribute("name", dllName.c_str());

	if(modeSelect == 1)
	{
		char cmd[256];
		/*
		   modelElement->SetAttribute("name", simpoleModelName.c_str());
		   modelXmlPath = strSimpoleXmlPath + "/projects/RTW.Sources/" + uuidPath +"/"+simpoleModelName +"/" + simpoleModelName +"_simpole/"+ simpoleModelName + ".xml";
		   */
		/*The offset of simpole variables in version 2.1.4 does not match that of autotest,So the autotest datadirction file is directly multiplexed from the simpole file*/
		// modify by wc 20231226
		modelXmlPath = strSimpoleXmlPath + "/projects/RTW/" + uuidPath +"/dataDictionary.xml";
		sprintf(cmd, "cp -rf \"%s\" ..",modelXmlPath.c_str());
		printf("[%s:%s:%d]--> cmd:%s\n", __FILE__, __FUNCTION__, __LINE__,cmd);
		system(cmd);
		return 0;

	}
	else
	{
		modelElement->SetAttribute("name", dllName.c_str());
		modelXmlPath = strXmlPath + "../../" + "matlabModel/"+dllName +"_simpole/"+ dllName + ".xml";
	}


	TiXmlDocument modelXmlDoc(modelXmlPath.c_str());
	bool loadOk = modelXmlDoc.LoadFile();//�����ĵ�
	if (!loadOk)
	{
		REPORT(REPORT_ERROR, "createDataDictionaryXml open xml :%s failed\n", modelXmlPath.c_str());
		return -1;
		//cout << "error" << endl;
	}

	TiXmlElement* pModelRootElement1 = modelXmlDoc.RootElement();
	TiXmlElement* pModelElement1 = pModelRootElement1->FirstChildElement();
	TiXmlElement* pGoToElement1 = pModelElement1->FirstChildElement("ExternalGoTo");
	TiXmlElement* pFromElement1 = pModelElement1->FirstChildElement("ExternalFrom");

	for(TiXmlElement* pModule = pFromElement1->FirstChildElement(); pModule != NULL; pModule = pModule->NextSiblingElement())
	{
		TiXmlElement *variableElement = new TiXmlElement("variable");
		for (TiXmlAttribute* pTmp = pModule->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
		{

			if (strcmp(pTmp->Name(), "name") == 0)
			{
				//strName = pTmp->Value();//000000000000000000000
				//variableElement->SetAttribute("name", (char*)strName.c_str());
				//printf("[%s:%s:%d]--> ----------------strName:%s, name:%s\n", __FILE__, __FUNCTION__, __LINE__,strName.c_str(),pTmp->Value());
				variableElement->SetAttribute("name", pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "offset") == 0)
			{
				char offsetBuf[8] = {0};
				sprintf(offsetBuf, "%d", offset);
				variableElement->SetAttribute("offset", offsetBuf);
				offset+=13;
			}
			else if (strcmp(pTmp->Name(), "length") == 0)
			{
				variableElement->SetAttribute("length", pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "dataType") == 0)
			{
				variableElement->SetAttribute("dataType", pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "Referenced") == 0)
			{
				//string fullpath(pTmp->Value());//000000000000000000000
				//fullpath = fullpath + "/" + strName; //000000000000000000000
				//variableElement->SetAttribute("referenced", (char*)fullpath.c_str());//000000000000000000000

				variableElement->SetAttribute("Referenced", pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "mode") == 0)
			{
				variableElement->SetAttribute("mode", pTmp->Value());
			}
		}
		modelElement->LinkEndChild(variableElement);
	}
	for(TiXmlElement* pModule = pGoToElement1->FirstChildElement(); pModule != NULL; pModule = pModule->NextSiblingElement())
	{
		TiXmlElement *variableElement = new TiXmlElement("variable");
		for (TiXmlAttribute* pTmp = pModule->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
		{
			if (strcmp(pTmp->Name(), "name") == 0)
			{
				variableElement->SetAttribute("name", pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "offset") == 0)
			{
				char offsetBuf[8] = {0};
				sprintf(offsetBuf, "%d", offset);
				variableElement->SetAttribute("offset", offsetBuf);
				offset+=13;
			}
			else if (strcmp(pTmp->Name(), "length") == 0)
			{
				variableElement->SetAttribute("length", pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "dataType") == 0)
			{
				variableElement->SetAttribute("dataType", pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "Referenced") == 0)
			{
				variableElement->SetAttribute("Referenced", pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "mode") == 0)
			{
				variableElement->SetAttribute("mode", pTmp->Value());
			}
		}
		modelElement->LinkEndChild(variableElement);
	}
	caseElement->LinkEndChild(modelElement);

	writeXml->LinkEndChild(caseElement);
	dictionaryXmlPath = strXmlPath + "../" + "dataDictionary.xml";
	writeXml->SaveFile(dictionaryXmlPath.c_str());

	return 0;

}
int createDataDictionaryXml(char * testPath)
{
	int ret = 0;
	char filePath[256] = {0};

	getServerPath(filePath);
	string strXmlPath(filePath);
	string dictionaryXmlPath;

	string caseBindPath = strXmlPath + "CaseInfo.xml";
	TiXmlDocument mydoc(caseBindPath.c_str());
	TiXmlDocument *writeXml = new TiXmlDocument;
	//增加case
	TiXmlElement *caseElement = new TiXmlElement("project");

	int offset = 0;
	bool loadOk = mydoc.LoadFile();//�����ĵ�
	if (!loadOk)
	{
		/*std::string errStr = mydoc.ErrorDesc();
		  int rowID = mydoc.ErrorRow();
		  int colID = mydoc.ErrorCol();*/
		return -1;
		//cout << "error" << endl;
	}
	TiXmlElement* rootElement = mydoc.RootElement();
	TiXmlElement* pRootXML = rootElement;
	TiXmlElement* pCaseRoot = pRootXML->FirstChildElement();

	for (TiXmlElement* pCaseStep = pCaseRoot->FirstChildElement(); pCaseStep != NULL; pCaseStep = pCaseStep->NextSiblingElement())
	{
		string caseID, caseName, presentCase;
		ret = GetValue(pCaseStep, "CaseID", caseID);
		if(ret == false)
		{
			return -1;
		}
		ret = GetValue(pCaseStep, "Name", caseName);
		if(ret == false)
		{
			return -1;
		}
		caseElement->SetAttribute("name", caseName.c_str());
		caseElement->SetAttribute("simVersion", "2.0");
		caseElement->SetAttribute("type", "dataDictionary.xml");
		char caseId[128] = {0};
		getPresentCase(caseId);
		presentCase = caseId;
		if(presentCase == caseID)
		{
			for (TiXmlElement* pStep = pCaseStep->FirstChildElement(); pStep != NULL; pStep = pStep->NextSiblingElement())
			{
				string strStepId, strStepName;
				char stepId[128] = {0};
				getPresentStep(stepId);
				string presentStepID = stepId;
				//新建model
				ret = GetValue(pStep, "StepID", strStepId);
				if(ret == false)
				{
					return -1;
				}

				ret = GetValue(pStep, "StepName", strStepName);
				if(ret == false)
				{
					return -1;
				}

				if(strStepId == presentStepID)
				{
					for (TiXmlElement* pSubStep = pStep->FirstChildElement(); pSubStep != NULL; pSubStep = pSubStep->NextSiblingElement())
					{
						string subStepId, dllName;
						int pos1 = 0, pos2 = 0;
						TiXmlElement *modelElement = new TiXmlElement("model");

						ret = GetValue(pSubStep, "SubStepID", subStepId);
						if(ret == false)
						{
							return -1;
						}
						modelElement->SetAttribute("id", subStepId.c_str());
						ret = GetValue(pSubStep, "Dll", dllName);
						if(ret == false)
						{
							return -1;
						}
						//插入subStep
						//subStepElement->SetAttribute("subStepID", subStepId.c_str());
						string modelXmlPath;
						pos1 = dllName.find_last_of(".");
						//				pos2 = dllName.find_first_of("lib");
						dllName = dllName.substr(3, pos1-3);
						modelElement->SetAttribute("name", dllName.c_str());
						modelXmlPath = strXmlPath + "../../" + "matlabModel/"+dllName +"_simpole/"+ dllName + ".xml";
						TiXmlDocument modelXmlDoc(modelXmlPath.c_str());
						loadOk = modelXmlDoc.LoadFile();//�����ĵ�
						if (!loadOk)
						{
							std::string errStr = mydoc.ErrorDesc();
							int rowID = mydoc.ErrorRow();
							int colID = mydoc.ErrorCol();
							REPORT(REPORT_ERROR, "createDataDictionaryXml open xml :%s failed\n", modelXmlPath.c_str());
							return -1;
							//cout << "error" << endl;
						}
						TiXmlElement* pModelRootElement = modelXmlDoc.RootElement();
						TiXmlElement* pModelElement = pModelRootElement->FirstChildElement();
						TiXmlElement* pGoToElement = pModelElement->FirstChildElement("ExternalGoTo");
						TiXmlElement* pFromElement = pModelElement->FirstChildElement("ExternalFrom");
						for(TiXmlElement* pModule = pFromElement->FirstChildElement(); pModule != NULL; pModule = pModule->NextSiblingElement())
						{
							TiXmlElement *variableElement = new TiXmlElement("variable");
							for (TiXmlAttribute* pTmp = pModule->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
							{
								if (strcmp(pTmp->Name(), "name") == 0)
								{
									variableElement->SetAttribute("name", pTmp->Value());
								}
								else if (strcmp(pTmp->Name(), "offset") == 0)
								{
									char offsetBuf[8] = {0};
									sprintf(offsetBuf, "%d", offset);
									variableElement->SetAttribute("offset", offsetBuf);
									offset+=13;
								}
								else if (strcmp(pTmp->Name(), "length") == 0)
								{
									variableElement->SetAttribute("length", pTmp->Value());
								}
								else if (strcmp(pTmp->Name(), "dataType") == 0)
								{
									variableElement->SetAttribute("dataType", pTmp->Value());
								}
								else if (strcmp(pTmp->Name(), "referenced") == 0)
								{
									variableElement->SetAttribute("referenced", pTmp->Value());
								}
								else if (strcmp(pTmp->Name(), "mode") == 0)
								{
									variableElement->SetAttribute("mode", pTmp->Value());
								}
							}
							modelElement->LinkEndChild(variableElement);
						}
						for(TiXmlElement* pModule = pGoToElement->FirstChildElement(); pModule != NULL; pModule = pModule->NextSiblingElement())
						{
							TiXmlElement *variableElement = new TiXmlElement("variable");
							for (TiXmlAttribute* pTmp = pModule->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
							{
								if (strcmp(pTmp->Name(), "name") == 0)
								{
									variableElement->SetAttribute("name", pTmp->Value());
								}
								else if (strcmp(pTmp->Name(), "offset") == 0)
								{
									char offsetBuf[8] = {0};
									sprintf(offsetBuf, "%d", offset);
									variableElement->SetAttribute("offset", offsetBuf);
									offset+=13;
								}
								else if (strcmp(pTmp->Name(), "length") == 0)
								{
									variableElement->SetAttribute("length", pTmp->Value());
								}
								else if (strcmp(pTmp->Name(), "dataType") == 0)
								{
									variableElement->SetAttribute("dataType", pTmp->Value());
								}
								else if (strcmp(pTmp->Name(), "referenced") == 0)
								{
									variableElement->SetAttribute("referenced", pTmp->Value());
								}
								else if (strcmp(pTmp->Name(), "mode") == 0)
								{
									variableElement->SetAttribute("mode", pTmp->Value());
								}
							}
							modelElement->LinkEndChild(variableElement);
						}
						caseElement->LinkEndChild(modelElement);
					}
				}
				//caseElement->LinkEndChild(modelElement);
			}
		}
	}
	writeXml->LinkEndChild(caseElement);
	dictionaryXmlPath = strXmlPath + "../" + "dataDictionary.xml";
	writeXml->SaveFile(dictionaryXmlPath.c_str());
	/*	dictionaryXmlPath = strXmlPath + "../../" + "dataDictionary.xml";
		writeXml->SaveFile(dictionaryXmlPath.c_str());
		*/	
	delete writeXml;
	writeXml = NULL;

	return 0;
}
int parseCurveConfig(char* xmlPath);
/*
   int parseCurveConfig(char* xmlPath)
   {

   CCurveManager *pCurveManager = CCurveManager::getInstance();
   pCurveManager->loadConfig(xmlPath);

   }
   */
//add by zr 20230617>>
int setModelVarInterface(char *varName, double value)
{
	string strReferenced = string(varName);
	TY_MODEL_VAR_MAP::iterator iter = gModelSetVarMap.find(strReferenced);

	if(iter != gModelSetVarMap.end())
	{
		iter->second.value = value;
		printf("=========ZR=========setModelVarInterface=====param update data:%lf\n",iter->second.value);
	}
	else
	{
		TY_MODEL_VAR modelVar;
		modelVar.value = value;
		modelVar.strName = strReferenced;
		gModelSetVarMap.insert(pair<string, TY_MODEL_VAR>(strReferenced, modelVar));
		printf("=========ZR=========setModelVarInterface+1=====param update data:%lf %s\n",value,strReferenced.c_str());
	}
	return 0;
}

TY_MODEL_PARAP_COM_INFO *pParaComHandle = NULL;
int getParamVal(char*modelName,TY_PARAM_VAL *pParamVal)
{
	int ret = 0;
	char buff[64] = {0};
	if(pParaComHandle == NULL)
	{
		printf("=========ZR=========A:getParamVal====t %s %p\n",modelName,pParaComHandle);
		pParaComHandle = openMParaCom(modelName);

	}
	int paramNum = 0;
	pParaComHandle->getParaTabelLen = 1;
	pParaComHandle->gPararInfos[paramNum].varLength =  pParamVal->uParamLength;
	pParaComHandle->gPararInfos[paramNum].offset = pParamVal->offset;
	int timeCount = 0;
	for (timeCount = 0; timeCount < 50; timeCount++)
	{
		if (pParaComHandle->getParaTabelLen == 0)
		{
			break;
		}
		else
		{
			usleep(100000);
		}
	}

	if (timeCount == 50)
	{
		ret = -1;
		printf("=========ZR=========A:getParamVal=====param update time out \n");
	}
	else
	{
		memcpy(pParamVal->value.value, pParaComHandle->gPararInfos[paramNum].data ,  pParaComHandle->sPararInfos[paramNum].varLength);
		//printf("=========ZR=========A:getParamVal=====param update data:%lf\n",pParamVal->value.dValue);
	}
	return ret;

}

int setModelParamInterface(char *varName, double value)
{
	string varNameStr(varName);

	string modelName = varNameStr.substr(0,varNameStr.find_first_of("/"));
	char buff[256] = {0};
	sprintf(buff, "%s%s%s", "lib", modelName.c_str(), ".so");
	printf("%s\n", buff);

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

				int paramNum = 0;
				//printf("===========ZR======set Model Param=========dllName %s,fullpath:%s, var:%lf\n", dllName.c_str(),varNameStr.c_str(), value);
				iter = pParamVarMap->find((char*)varNameStr.c_str());
				if(iter != pParamVarMap->end())
				{
					memcpy(iter->second.value.value,  &value, 8);
					//printf("======ZR====set modelvar:%lf %d %lf\n", value,iter->second.offset,iter->second.value.dValue);
				}

			}

		}
	}
	return 0;

}

int refreshRecvCache(const char*pIcdID, struct timeval t)
{

	string icdID(pIcdID);
	TY_TIME_COMPARE icdRefreshInfo; 
	icdRefreshInfo.flag = 1;
	long long tt = t.tv_sec*1000000+t.tv_usec;
	//>>modify by wc 20230915
	REFRESH_MAP::iterator pos =  reFreshCacheMap.find(icdID);
	if(pos!=reFreshCacheMap.end())
	{
		if(pos->second.ttold == tt)
		{
			if(pos->second.flag == 1)
			{
				gettimeofday(&pos->second.tStart, NULL);
				pos->second.flag = 0;
#if(PRINTF_VALUE)

				///if(icdID == "5b53e4c8-2359-4177-a974-bc6615c5e411")
				//printf("icdID:%s======start===goto map =time:--------->%d\n", icdID.c_str(), pos->second.tStart.tv_usec);
#endif

			}
			gettimeofday(&pos->second.tStop, NULL);
#if(PRINTF_VALUE)

			//if(icdID == "5b53e4c8-2359-4177-a974-bc6615c5e411")
			//printf("icdID:%s======stop===goto map =time:>%d\n", icdID.c_str(), pos->second.tStop.tv_usec );
#endif

			if((pos->second.tStop.tv_sec - pos->second.tStart.tv_sec) * 1000000 + (pos->second.tStop.tv_usec - pos->second.tStart.tv_usec) > 200000)
			{
				//pos->second.flag = 1;
				//REPORT(REPORT_ERROR, "-----no recv data,pIcdID:%s , time:%lld\n", pIcdID, t);
				return FAIL;
			}
#if(PRINTF_VALUE)

			///if(icdID == "5b53e4c8-2359-4177-a974-bc6615c5e411")
			//printf("icdID:%s============time:--------->%lld\n", icdID.c_str(), (pos->second.tStop.tv_sec - pos->second.tStart.tv_sec) * 1000000 + (pos->second.tStop.tv_usec - pos->second.tStart.tv_usec));
#endif

		}
		else
		{
			pos->second.ttold = tt;
			pos->second.flag = 1;
#if(PRINTF_VALUE)

			//if(icdID == "5b53e4c8-2359-4177-a974-bc6615c5e411")
			//printf("icdID:%s=======11=1111===goto map =time:--------->%lld\n", icdID.c_str(), tt);
#endif

		}

	}
	else
	{
		reFreshCacheMap.insert(make_pair(icdID, icdRefreshInfo));
	}

	return 0;
}

int resetICDData()
{

	CObjectData *pAutoTest = CObjectData::getInstance();
	if(pAutoTest == NULL)
	{
		REPORT(REPORT_ERROR, "Get CObjectData::getInstance() failed\n");
		return FAIL;
	}
	if(pAutoTest->resetICDData() != 0)
	{

		//REPORT(REPORT_ERROR, " resetICDData failed\n");
		return FAIL;
	}

	return SUCCESS;
}
