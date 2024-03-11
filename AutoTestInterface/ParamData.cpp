#include <stdio.h>
#include <iostream>
#include <string>
#include "tinystr.h"
#include "tinyxml.h"
#include <iostream>
#include "ParamData.h"
#include "ICDManager.h"
#include "HardwareManager.h"
#include "HiCommon.h"
#include "HiReport.h"
#include "ICDManager.h"
#include "parseXmlApi.h"
#include "BaseField.h"
using namespace std;
#define SUCCESS 0
#define FAIL -1
CObjectData* CObjectData::m_instance = NULL;
inline  void showPac(char* pData,int length)
{
	printf("=====file:%s,sfunction:%s line:%d,======length:%d\n",__FILE__,__FUNCTION__,__LINE__,length);
	for(int i=0;i<length;i++)
	{
		if(i%16==0)
		{
			printf("%02X\t",i/16);
		}
		printf("%02X ",pData[i]&0xFF);
		if((i+1)%8==0)
		{
			printf("\t");
			if((i+1)%16==0)
			{
				printf("\n");
			}
		}
	}
}
void CSubStep::getValue(string &strStepID, string &strDllName)
{
	strStepID = m_strStepID;
	strDllName = m_strDllName;
}
CSubStep::CSubStep()
{
	m_strStepID = "";
	m_strDllName = "";
    m_msg = "";
    m_attachInfo = "";
    m_stepStatus = 0;
    m_pSubStepElement = NULL;
}
CSubStep::~CSubStep()
{

}



CStep::CStep()
{
	m_strCommand = "";
	m_strStepID = "";
	m_strStepName = "";
	m_strType = "";
	m_strTimes = "";
	m_strPeriodMs = "";
}
CStep::~CStep()
{

}
void CStep::getValue(string &strStepID, string &strStepName, long long &periodMs)
{
	strStepID = m_strStepID;
	strStepName = m_strStepName;
	periodMs = atoi(m_strPeriodMs.c_str());
}

CCaseRoot::CCaseRoot()
{
	m_strCaseID = "";
	m_strName = "";
	m_strStepNum = "";
}
CCaseRoot::~CCaseRoot()
{

}



void CCaseRoot::getValue(string &strCaseID, string &strName, string &strStepNum)
{
	m_strCaseID = "";
	m_strName = "";
	m_strStepNum = "";
}

CObjectData* CObjectData::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new CObjectData();
		REPORT(REPORT_INFO, " new CObjectData\n");
	}
	return m_instance;
}



CObjectData::CObjectData()
{

}
CObjectData::~CObjectData()
{

}
void CObjectData::getParamData(TiXmlElement* paramXml, TY_PARAM_VALUE &param)
{
	if(paramXml == NULL)
	{
		REPORT(REPORT_ERROR, " paramXml is NULL failed\n");
	}
	for (TiXmlAttribute* tmp = paramXml->FirstAttribute(); tmp != NULL; tmp = tmp->Next())
	{
		if (strcmp(tmp->Name(), "Name") == 0)
		{
			param.strParamName = tmp->Value();
		}
		else if (strcmp(tmp->Name(), "DataType") == 0)
		{
			param.strParamType = tmp->Value();
		}
		else if (strcmp(tmp->Name(), "Length") == 0)
		{
			param.uParamLength = atoi(tmp->Value());
		}
		else if (strcmp(tmp->Name(), "Value") == 0)
		{
			setDigitValueByString(&param.value, param.strParamType, tmp->Value());
		}
	}
}

void CObjectData::getInputData(TiXmlElement* pIcdXml, TY_INPUT_VALUE &icdValue)
{
	if(pIcdXml == NULL)
	{
		REPORT(REPORT_ERROR, " pIcdXml is NULL failed\n");
	}
	for (TiXmlAttribute* pIcd = pIcdXml->FirstAttribute(); pIcd != NULL; pIcd = pIcd->Next())
	{
		if (strcmp(pIcd->Name(), "ICDID") == 0)
		{
			icdValue.strIcdID = pIcd->Value();
		}
		else if (strcmp(pIcd->Name(), "CardType") == 0)
		{
			icdValue.strCardType = pIcd->Value();
		}
		else if (strcmp(pIcd->Name(), "CardID") == 0)
		{
			icdValue.cardID = atoi(pIcd->Value());
		}
		else if (strcmp(pIcd->Name(), "ChnID") == 0)
		{
			icdValue.chnID = atoi(pIcd->Value());
		}
	}
	for (TiXmlElement* pIcdField = pIcdXml->FirstChildElement(); pIcdField != NULL; pIcdField = pIcdField->NextSiblingElement())
	{
		if(pIcdField == NULL)
		{
			REPORT(REPORT_ERROR, " pIcdField is NULL failed\n");
		}
		TY_INPUT_FIELD icdFieldValue;
		for (TiXmlAttribute* pTmp = pIcdField->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
		{
			if(pTmp == NULL)
			{
				REPORT(REPORT_ERROR, " pTmp is NULL failed\n");
			}
			if (strcmp(pTmp->Name(), "FieldID") == 0)
			{
				icdFieldValue.strFieldID = pTmp->Value();
			}
			else if (strcmp(pTmp->Name(), "VarName") == 0)
			{
				icdFieldValue.strVarName = pTmp->Value();
			}
		}
		icdValue.vecIcdField.push_back(icdFieldValue);
	}
}

void CObjectData::getOutputData(TiXmlElement* pIcdXml, TY_OUTPUT_VALUE &icdValue)
{

	for (TiXmlAttribute* pIcd = pIcdXml->FirstAttribute(); pIcd != NULL; pIcd = pIcd->Next())
	{
		if(pIcd == NULL)
		{
			REPORT(REPORT_ERROR, " pTmp is NULL failed\n");
		}
		if (strcmp(pIcd->Name(), "ICDID") == 0)
		{
			icdValue.strIcdID = pIcd->Value();
		}
		else if (strcmp(pIcd->Name(), "CardType") == 0)
		{
			icdValue.strCardType = pIcd->Value();
		}
		else if (strcmp(pIcd->Name(), "CardID") == 0)
		{
			icdValue.cardID = atoi(pIcd->Value());
		}
		else if (strcmp(pIcd->Name(), "ChnID") == 0)
		{
			icdValue.chnID = atoi(pIcd->Value());
		}
	}
	for (TiXmlElement* pIcdField = pIcdXml->FirstChildElement(); pIcdField != NULL; pIcdField = pIcdField->NextSiblingElement())
	{
		if(pIcdField == NULL)
		{
			REPORT(REPORT_ERROR, " pIcdField is NULL failed\n");
		}
		TY_OUTPUT_FIELD icdFieldValue;
		for (TiXmlAttribute* pTmp = pIcdField->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
		{
			if(pTmp == NULL)
			{
				REPORT(REPORT_ERROR, " pTmp is NULL failed\n");
			}
			if (strcmp(pTmp->Name(), "FieldID") == 0)
			{
				icdFieldValue.strFieldID = pTmp->Value();
			}
			else if (strcmp(pTmp->Name(), "VarName") == 0)
			{
				icdFieldValue.strVarName = pTmp->Value();
			}
			else if (strcmp(pTmp->Name(), "Value") == 0)
			{
				setDigitValueByString(&icdFieldValue.varValue, icdFieldValue.strVarType, pTmp->Value());
			}
			else if (strcmp(pTmp->Name(), "DataType") == 0)
			{
				icdFieldValue.strVarType = pTmp->Value();
			}
		}
		icdValue.vecIcdField.push_back(icdFieldValue);
	}
}

void CObjectData::parseSubStepXMLData(TiXmlElement* subStep, CSubStep* pSubStep)
{
	TiXmlAttribute* pSubStepValue = subStep->FirstAttribute();
	string strStepId;

	if(pSubStepValue == NULL)
	{
		REPORT(REPORT_ERROR, " pSubStepValue is NULL failed\n");
	}
	if (strcmp(pSubStepValue->Name(), "StepID") == 0)
	{
		strStepId = pSubStepValue->Value();
	}

	for (TiXmlElement* pSubStepTmp = subStep->FirstChildElement(); pSubStepTmp != NULL; pSubStepTmp = pSubStepTmp->NextSiblingElement())
	{
		if(pSubStepTmp == NULL)
		{
			REPORT(REPORT_ERROR, " pSubStepTmp is NULL failed\n");
		}
		if (strcmp(pSubStepTmp->Value(), "parameter") == 0)
		{
			CValueArray valueArray;
			valueArray.m_strValueType = pSubStepTmp->Value();
			for (TiXmlElement* pPara = pSubStepTmp->FirstChildElement(); pPara != NULL; pPara = pPara->NextSiblingElement())
			{
				TY_PARAM_VALUE paramValue;
				getParamData(pPara, paramValue);
				valueArray.m_vecParamValue.push_back(paramValue);
			}
			pSubStep->m_vecValueArray.push_back(valueArray);
		}
		else if (strcmp(pSubStepTmp->Value(), "output") == 0)
		{
			CValueArray valueArray;
			valueArray.m_strValueType = pSubStepTmp->Value();
			for (TiXmlElement* pIcd = pSubStepTmp->FirstChildElement(); pIcd != NULL; pIcd = pIcd->NextSiblingElement())
			{
				if(pIcd == NULL)
				{
					REPORT(REPORT_ERROR, " pSubStepTmp is NULL failed\n");
				}
				TY_OUTPUT_VALUE icdOutputValue;
				getOutputData(pIcd, icdOutputValue);
				valueArray.m_vecIcdOutputValue.push_back(icdOutputValue);
			}
			pSubStep->m_vecValueArray.push_back(valueArray);
		}
		else if (strcmp(pSubStepTmp->Value(), "input") == 0)
		{
			CValueArray valueArray;
			valueArray.m_strValueType = pSubStepTmp->Value();
			for (TiXmlElement* pIcd = pSubStepTmp->FirstChildElement(); pIcd != NULL; pIcd = pIcd->NextSiblingElement())
			{
				if(pIcd == NULL)
				{
					REPORT(REPORT_ERROR, " pSubStepTmp is NULL failed\n");
				}
				TY_INPUT_VALUE icdInputValue;
				getInputData(pIcd, icdInputValue);
				valueArray.m_vecIcdInputValue.push_back(icdInputValue);
			}
			pSubStep->m_vecValueArray.push_back(valueArray);
		}
	}
}


int CObjectData::parseXMLData(string strXmlPath)
{
	/*
	//char filePath[256] = {0};
	//getServerPath(filePath);
	//string strXmlPath(filePath);
	//strXmlPath = strXmlPath + "CaseBinding.xml";
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

	for (TiXmlElement* pCaseStep = pCaseRoot->FirstChildElement(); pCaseRoot != NULL; pCaseRoot = pCaseRoot->NextSiblingElement())
	{
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
				m_caseRoot.m_strCaseID = pTmp->Value();
			}
			else if (strcmp(pTmp->Name(), "Name") == 0)
			{
				m_caseRoot.m_strName = pTmp->Value();
			}
			else if (strcmp(pTmp->Name(), "StepNum") == 0)
			{
				m_caseRoot.m_strStepNum = atoi(pTmp->Value());
			}
		}
		for (TiXmlElement* pStep = pCaseStep->FirstChildElement(); pStep != NULL; pStep = pStep->NextSiblingElement())
		{
			if(pStep == NULL)
			{
				REPORT(REPORT_ERROR, " pStep is NULL failed\n");
			}
			CStep stepTmp;
			for (TiXmlAttribute* pTmp = pStep->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
			{
				if(pTmp == NULL)
				{
					REPORT(REPORT_ERROR, " pStep is NULL failed\n");
				}
				if (strcmp(pTmp->Name(), "Command") == 0)
				{
					stepTmp.m_strCommand = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "StepID") == 0)
				{
					stepTmp.m_strStepID = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "StepName") == 0)
				{
					stepTmp.m_strStepName = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "PeriodMs") == 0)
				{
					stepTmp.m_strPeriodMs = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "Type") == 0)
				{
					stepTmp.m_strType = pTmp->Value();
				}
				else if (strcmp(pTmp->Name(), "Times") == 0)
				{
					stepTmp.m_strTimes = pTmp->Value();
				}
			}

			for (TiXmlElement* pSubStep = pStep->FirstChildElement(); pSubStep != NULL; pSubStep = pSubStep->NextSiblingElement())
			{
				if(pSubStep == NULL)
				{
					REPORT(REPORT_ERROR, " pStep is NULL failed\n");
				}
				CSubStep subStepTmp;
				for (TiXmlAttribute* pTmp = pSubStep->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
				{
					if(pTmp == NULL)
					{
						REPORT(REPORT_ERROR, " pStep is NULL failed\n");
					}
					if (strcmp(pTmp->Name(), "StepID") == 0)
					{
						subStepTmp.m_strStepID = pTmp->Value();
					}
					else if (strcmp(pTmp->Name(), "Dll") == 0)
					{
						subStepTmp.m_strDllName = pTmp->Value();
					}
				}
				parseSubStepXMLData(pSubStep, &subStepTmp);
				stepTmp.m_mapVectSubStep[subStepTmp.m_strStepID] = subStepTmp;
			}
			m_caseRoot.m_vecStep.push_back(stepTmp);
		}
	}
	*/
	return SUCCESS;
}

int CObjectData::parseXMLData(TiXmlElement* pSubStep, TY_TEMPLATE_VALUE* value)
{
	CSubStep subStepTmp;
	subStepTmp.m_pSubStepElement = pSubStep;
	for (TiXmlAttribute* pTmp = pSubStep->FirstAttribute(); pTmp != NULL; pTmp = pTmp->Next())
	{
		if(pTmp == NULL)
		{
			REPORT(REPORT_ERROR, " pStep is NULL failed\n");
		}
		if (strcmp(pTmp->Name(), "StepID") == 0)
		{
			subStepTmp.m_strStepID = pTmp->Value();
			(*value).strSubStepID =  pTmp->Value();
		}
		else if (strcmp(pTmp->Name(), "Dll") == 0)
		{
			subStepTmp.m_strDllName = pTmp->Value();
			(*value).strDllName  =  pTmp->Value();
		}
	}
	parseSubStepXMLData(pSubStep, &subStepTmp);
	vector<CValueArray>::iterator valueArray_it;
	for (valueArray_it = subStepTmp.m_vecValueArray.begin(); valueArray_it != subStepTmp.m_vecValueArray.end(); valueArray_it++)
	{
		if (valueArray_it->m_strValueType == PARAM_TYPE)
		{
			(*value).vecParamValue = valueArray_it->m_vecParamValue;
		}
		else if(valueArray_it->m_strValueType == INPUT_ICD_TYPE)
		{
			(*value).vecInputValue = valueArray_it->m_vecIcdInputValue;
		}
		else if(valueArray_it->m_strValueType == OUTPUT_ICD_TYPE)
		{
			(*value).vecOutputValue = valueArray_it->m_vecIcdOutputValue;
		}
		else
		{


		}
	}
	return SUCCESS;
}


int CObjectData::getInfoByDllName(string strDllName,  string& strStepID, string& strStepName, string& strSubStepID, long long& pPeriodMs, TiXmlElement* m_pSubStepElement)
{
	/*vector<CStep>::iterator step_it = m_caseRoot.m_vecStep.begin();
	for (; step_it != m_caseRoot.m_vecStep.end(); step_it++)
	{
		MAPVectSubStep::iterator subStep_it = step_it->m_mapVectSubStep.begin();
		for (; subStep_it != step_it->m_mapVectSubStep.end(); subStep_it++)
		{
			if (strDllName == subStep_it->second.m_strDllName)
			{
				strSubStepID = subStep_it->second.m_strStepID;
				step_it->getValue(strStepID, strStepName, pPeriodMs);
				return SUCCESS;
			}
		}
	}*/
	return FAIL;
}
int CObjectData::resetICDData()
{
	CICDManager *pICDManager = CICDManager::getInstance();
    if(pICDManager == NULL)
    {
		REPORT(REPORT_ERROR, "Get pICDManager failed\n");
		return FAIL;
    }
	pICDManager->resetBlockBuffer();
}

int CObjectData::setICDData(string icdID, string fieldID, TY_VALUEFIELD value)
{
    CICDManager *pICDManager = CICDManager::getInstance();
    if(pICDManager == NULL)
    {
		REPORT(REPORT_ERROR, "Get pICDManager failed\n");
		return FAIL;
    }
    CBlock* pBlock = pICDManager->getBlock(icdID);
	if(pBlock == NULL)
	{
		REPORT(REPORT_ERROR, "Get getBlock failed icdID:%s\n", icdID.c_str());
		return FAIL;

	}
	BaseField* pField = pBlock->getField(fieldID);
	if(pField == NULL)
	{
		REPORT(REPORT_ERROR, "Get getField failed fieldID:%s\n", fieldID.c_str());
		return FAIL;
	}
	pField->setDoubleData(value.llValue);
	//REPORT(REPORT_ERROR,"***************************     value = %lld     *********************************************************\n", value.llValue );
	//pICDManager->getBlock(icdID)->printFieldValue();
	if(pBlock->packTxICD() == -1)
	{
		REPORT(REPORT_ERROR, "Get packTxICD failed\n");
		return FAIL;
	}
	//pICDManager->getBlock(icdID)->printFieldValue();
	return SUCCESS;
}
int CObjectData::getICDData(string icdID, string fieldID, TY_VALUEFIELD& value)
{
    CICDManager *pICDManager = CICDManager::getInstance();
	if(pICDManager == NULL)
	{
		REPORT(REPORT_ERROR, "Get pICDManager failed\n");
		return FAIL;
	}
	CBlock *pBlock = pICDManager->getBlock(icdID);
	if(pBlock == NULL)
	{
		REPORT(REPORT_ERROR, "Get pBlock failed\n");
		return FAIL;
	}

	BaseField* pField = pBlock->getField(fieldID);
	if(pField == NULL)
	{
		REPORT(REPORT_ERROR, "Get pField failed\n");
		return FAIL;
	}
//	unsigned char rxData[MAX_PAYLOAD_LEN];
//	int length = MAX_PAYLOAD_LEN;
//	pBlock->setRxData(rxData, length);//4<<

	pBlock->unpackRxICD();//5<<

	pField->getData((double*)&value.dValue);

	return SUCCESS;

}

int CObjectData::sendICDChnData(string icdID, string cardType, unsigned int cardID)
{
	unsigned char ret = 0;
	unsigned char txData[MAX_PAYLOAD_LEN];
	int length = MAX_PAYLOAD_LEN;
	int lengthRet = 0;

	CICDManager *pICDManager = CICDManager::getInstance();
	if(pICDManager == NULL)
	{
		REPORT(REPORT_ERROR, "Get pICDManager failed\n");
		return FAIL;
	}
	CBlock *pBlock = pICDManager->getBlock(icdID);
	if(pBlock == NULL)
	{
		REPORT(REPORT_ERROR, "Get pBlock %s failed\n",icdID.c_str());
		return FAIL;
	}

	lengthRet = pBlock->getTxData(txData, length);//3<<
	//REPORT(REPORT_INFO, "=====CSendStep >>>>>field lengthRet:%d\n", lengthRet);
//    showPac((char*)txData, lengthRet);

	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
	if(pHardwareManager == NULL)
	{
		REPORT(REPORT_ERROR, "Get pHardwareManager failed\n");
		return FAIL;
	}
	CCardManager * pCardManager = pHardwareManager->getCardManager(cardType);
	if(pCardManager == NULL)
	{
		REPORT(REPORT_ERROR, "Get CardManager failed:%s, cardId:%d\n",cardType.c_str(),cardID);
		return FAIL;
	}
	CCard* pCard = pCardManager->getCard(cardID);
	if(pCard == NULL)
	{
		REPORT(REPORT_ERROR, "Get pCard failed(id:%d), icdID:%s\n",cardID, icdID.c_str());
		return FAIL;
	}
	ret = pCard->writeData(icdID, (char*)txData, lengthRet);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "ICD:%s write Data failed\n", icdID.c_str());
		return FAIL;
	}
	return SUCCESS;
}
/*
typedef struct _time_compare_{
	struct timeval tStart,tStop;
	int flag;
	struct timeval tOld;
	long long  ttold;
}TY_TIME_COMPARE;
typedef map<string, TY_TIME_COMPARE> REFRESH_MAP;
REFRESH_MAP reFreshCacheMap;
*/
#define PRINTF_VALUE 0

int CObjectData::receiveICDChnData(string icdID, string cardType, unsigned int cardID)
{
	
    unsigned char rxData[MAX_PAYLOAD_LEN];
    unsigned char tmpCompareData[MAX_PAYLOAD_LEN];
    int ret = 0;
    int flushFlag = 1;
    int length =  500;//MAX_PAYLOAD_LEN;//TODO:modyfi
    struct timeval t;
    
    CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
    if(pHardwareManager == NULL)
    {
    	REPORT(REPORT_ERROR, "Get pHardwareManager failed\n");
    	return FAIL;
    }
    CCardManager* pCardManager = pHardwareManager->getCardManager(cardType);
    if(pCardManager == NULL)
    {
    	REPORT(REPORT_ERROR, "Get pCardManager failed\n");
    	return FAIL;
    }

	CCard* pCard = pCardManager->getCard(cardID);
	if(pCard == NULL)
	{
		REPORT(REPORT_ERROR, "Get pCard failed(id:%d)\n",cardID);
		return FAIL;

    }
	ret = pCard->readData(icdID, (char*)rxData, length, &t);
	if( 0 != ret )
	{
		//REPORT(REPORT_ERROR, "uuid %s readData failed.\n",icdID.c_str());
		return FAIL;
	}

	

	if(refreshRecvCache(icdID.c_str(), t))
	{
		return FAIL;
	}


#if 0
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
	
#endif
	//showPac((char*)rxData, length);
	//printf("\nrxData>>345 %d\n", (rxData[345]>>5)&0x01);
    CICDManager *pICDManager = CICDManager::getInstance();
    if(pICDManager == NULL)
    {
    	REPORT(REPORT_ERROR, "Get pICDManager failed\n");
    	return FAIL;
    }

    CBlock *pBlock = pICDManager->getBlock(icdID);
    if(pBlock == NULL)
    {
        REPORT(REPORT_ERROR, "Get m_ICDID failed\n");
    	return FAIL;
    }

    pBlock->setRxData(rxData, length);


	return SUCCESS;
}

void CObjectData::eraseObject(string stepId)
{
	//MAPVect::iterator iter = m_mapVect.find(stepId);
	//if (iter != m_mapVect.end())
	//{
	//	m_mapVect.erase(iter);
	//}
}

void CObjectData::setDigitValueByDouble(TY_VALUEFIELD* valueField,string strType, double value)
{
	if(strType==DOUBLE_TYPE)
	{
		valueField->dValue = value;
		//valueField->dValue = (int)value;
	}
	else if(strType==FLOAT_TYPE)
	{
		valueField->fValue = (float)value;
	}
	else if(strType==LONG_TYPE)
	{
		valueField->lValue=(long)value;
	}
	else if(strType==ULONG_TYPE)
	{
		valueField->uLongValue=(unsigned long)value;
	}
	else if(strType==INT_TYPE)
	{
		valueField->iValue=(int)value;
	}
	else if(strType==UINT_TYPE)
	{
		valueField->uIntValue=(unsigned int)value;
	}
	else if(strType==SHORT_TYPE)
	{
		valueField->sValue=(short)value;
	}
	else if(strType==USHORT_TYPE)
	{
		valueField->uShortValue=(unsigned short)value;
	}
	else if(strType==CHAR_TYPE)
	{
		valueField->cValue=(char)value;
	}
	else if(strType==UCHAR_TYPE)
	{
		valueField->uCharValue=(unsigned char)value;
	}
	else
	{
		valueField->lValue=(long)value;
	}
}

void CObjectData::getDigitValueByDouble(TY_VALUEFIELD valueField,string strType, double *value)
{
	if(strType==DOUBLE_TYPE)
	{
		*value = (double)valueField.dValue;
	}
	else if(strType==FLOAT_TYPE)
	{
		*value = (double)valueField.fValue;
	}
	else if(strType==LONG_TYPE)
	{
		*value = (double)valueField.lValue;
	}
	else if(strType==ULONG_TYPE)
	{
		*value = valueField.uLongValue;
	}
	else if(strType==INT_TYPE)
	{
		*value = (double)valueField.iValue;
	}
	else if(strType==UINT_TYPE)
	{
		*value = (double)valueField.uIntValue;
	}
	else if(strType==SHORT_TYPE)
	{
		*value = (double)valueField.sValue;
	}
	else if(strType==USHORT_TYPE)
	{
		*value = (double)valueField.uShortValue;
	}
	else if(strType==CHAR_TYPE)
	{
		*value = (double)valueField.cValue;
	}
	else if(strType==UCHAR_TYPE)
	{
		*value = (double)valueField.uCharValue;
	}
	else
	{
		*value = (double)valueField.lValue;
	}
}
void CObjectData::setDigitValueByString(TY_VALUEFIELD* valueField,string strType,string strValue)
{
	if(strType==DOUBLE_TYPE)
	{
		valueField->dValue =_ttof(strValue.c_str());
	}
	else if(strType==FLOAT_TYPE)
	{
		valueField->fValue=(float)_ttof(strValue.c_str());
	}
	else if(strType==LONG_TYPE)
	{
		valueField->lValue=(long)strtoul(strValue.c_str(),NULL,0);
	}
	else if(strType==ULONG_TYPE)
	{
		valueField->uLongValue=(unsigned long)strtoul(strValue.c_str(),NULL,0);
	}
	else if(strType==INT_TYPE)
	{
		valueField->iValue=(int)strtoul(strValue.c_str(),NULL,0);
	}
	else if(strType==UINT_TYPE)
	{
		valueField->uIntValue=(unsigned int)strtoul(strValue.c_str(),NULL,0);
	}
	else if(strType==SHORT_TYPE)
	{
		valueField->sValue=(short)strtoul(strValue.c_str(),NULL,0);
	}
	else if(strType==USHORT_TYPE)
	{
		valueField->uShortValue=(unsigned short)strtoul(strValue.c_str(),NULL,0);
	}
	else if(strType==CHAR_TYPE)
	{
		valueField->cValue=(char)strtoul(strValue.c_str(),NULL,0);
	}
	else if(strType==UCHAR_TYPE)
	{
		valueField->uCharValue=(unsigned char)strtoul(strValue.c_str(),NULL,0);
	}
	else
	{
		valueField->lValue=(long)strtoul(strValue.c_str(),NULL,0);
	}
}

