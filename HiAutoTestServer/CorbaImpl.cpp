#include "CorbaImpl.h"
#include "HiAutoTestServerC.h"
#include "CaseManager.h"
#include "ICDManager.h"
#include "HardwareManager.h"
#include "CommonCase.h"
#include "HiLog.h"
#include "CCurve.h"
#include "CStorageManager.h"
#include "BaseField.h"
//manual test
#include "BaseICDStep.h" //--> modify by wc 20240305
#include "BaseStep.h"
#include "CustomDlStep.h" //-->modify by wc 20240305
#include "RTCTimer.h"
#include "HiMGlobalVarLib.h"
#include "ParamData.h"
#include "AutoTestInterface.h"
#include "HiReport.h"

#include "HiCommon.h"

const char *setAutoPath = "AUTO_HOME";
const char *caseXmlPath = "/HiAutoTestServer/bin/CaseInfo.xml";
const char *cardXmlPath = "/HiAutoTestServer/bin/CardInfo.xml";
const char *ICDXmlPath = "/HiAutoTestServer/bin/ICDInfo.xml";
const char *curveXmlPath = "/HiAutoTestServer/bin/CurveInfo.xml";
const char *ModelXmlPath = "/HiAutoTestServer/bin/ModelPara.xml";
const char *StorageXmlPath = "/HiAutoTestServer/bin/StorageInfo.xml";
CorbaImpl::CorbaImpl()
{

}
CorbaImpl::~CorbaImpl()
{

}
const char* caseXmlFileName = "/home/V1.0.0/HiAutoTestServer/bin/CaseInfo.xml";
const char* channelXmlFileName = "/home/V1.0.0/HiAutoTestServer/bin/CardInfo.xml";
const char* ICDXmlFileName = "/home/V1.0.0/HiAutoTestServer/bin/ICDInfo.xml";
const char* storageXmlFileName = "/home/V1.0.0/HiAutoTestServer/bin/StorageInfo.xml";
const char* curveXmlFileName = "/home/V1.0.0/HiAutoTestServer/bin/CurveInfo.xml";
const char* ModelXmlFileName = "/home/V1.0.0/HiAutoTestServer/bin/ModelPara.xml";

// manual test
int modelStatus;
int rtcStartFlag;
static char* pathXml;

typedef struct _get_icd_info
	{
		char icdID[128];
		char fieldID[1024][128];
		char cardType[64];
		int cardID;
		int fieldNum;
		double data[1024];
	}TY_GET_ICD_INFO;

typedef struct _send_icd_info
{
	char icdID[128];
	char cardType[64];
	int cardID;
}TY_SEND_ICD_INFO;

typedef struct _model_info
{
	char modelName[128];
	char fullPath[1024][256];
	double data[1024];
	int num;
}TY_MODEL_INFO;

typedef struct _set_icd_info
{
	char icdID[128];
	char fieldID[1024][128];
	double data[1024];
	int fieldNum;
}TY_SET_ICD_INFO;

typedef std::queue <CCustomDlStep> Model_Queue;
Model_Queue model_Queue;

int MANUAL_TEST_FLAG;


//>>
FILE *fp;
#define PRINTLOG 0
#define PRINTFSTORAGE 0
//<<

//TY_MODEL_CARD_MAP modelCard;   -
void* modelStartHandler(void* pParam)
{
	int count = 0;
	int times = 0;

	TY_MODEL_CARD_MAP::iterator iterModel;
	for(iterModel = modelCard.begin(); iterModel != modelCard.end(); iterModel++)
	{
		CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;
		times = pCard->m_periodMs;
		if(0 == count % times)
		{
			sem_post(&pCard->m_sem);
		}
	}
	count++;

}

autoTest::ReturnInfo CorbaImpl::startManualTest ()
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
#if(PRINTLOG)
	fp = fopen("manuallog.log", "w+");
	if(fp == NULL)
	{
		printf("-------open error\n");
	}
#endif

	MANUAL_TEST_FLAG = 1;
	int periodMs = 10;

	modelStatus = STATUS_RUNNING;

	if (rtcStartFlag == 0)
	{
		rtcStartFlag = 1;

		ret = rtcInit((1000*periodMs), modelStartHandler, this);
		sleep(1);
		if (ret != 0)
		{
			REPORT(REPORT_ERROR, "rtcInit error periodMs:%d\n ", periodMs);
			modelStatus = STATUS_ERROR;

			ret = -1;
			rInfo.status = ret;
			return rInfo;
		}

		rtcStart();
		REPORT(REPORT_DEBUG, "start rtc\n");

		
	}
	pHardwareManager->start();

	rInfo.status = ret;
	//printf("------ret=  %d\n",ret);
	return rInfo;
}

autoTest::ReturnInfo CorbaImpl::stopManualTest ()
{
	printf("=====================stop ManualTest========================\n");
	int ret = 0;
	autoTest::ReturnInfo rInfo;
    struct timeval ccurTime;

	REPORT(REPORT_INFO, "stop\n");
	rtcStop();
	rtcDispose();
	sleep(2);
	rtcStartFlag = 0;
	modelStatus = STATUS_SUCCESS;
	TY_MODEL_CARD_MAP::iterator iterModel;

	for(iterModel = modelCard.begin(); iterModel != modelCard.end(); iterModel++)
	{
		CCustomDlStep* pCard = (CCustomDlStep*)iterModel->second;
		pCard->stop();

	}
	
	MANUAL_TEST_FLAG = 0;
    //	CStorageManager *pStorageManager = CStorageManager::getInstance();
//	pStorageManager->stopStorage();
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
	pHardwareManager->stop();
#if(PRINTLOG)
	fclose(fp);
	fp = NULL;
#endif

	gettimeofday(&ccurTime, NULL);
	rInfo.sec = ccurTime.tv_sec;
	rInfo.usec = ccurTime.tv_usec;
	rInfo.status = ret;
	return rInfo;
}

autoTest::ReturnInfo CorbaImpl::startModel (const char* pData)
{
	printf("===================start to start Model====================\n");
	int ret = 0;
	char pathModel[64] = {0};
	autoTest::ReturnInfo rInfo;
	TiXmlDocument * pModelXmlManager = NULL;

	printf("%s", pData);
	pModelXmlManager = load_xml(pData, FROM_DATA);
	printf("%s", pModelXmlManager);
	if(pathXml == NULL)
	{
		pathXml = getenv(setAutoPath);//"AUTO_HOME"
	}
	memcpy(pathModel,pathXml, strlen(pathXml));
	strcat(pathModel, ModelXmlPath);
	ret = save_xml(pModelXmlManager, pathModel);
	CCustomDlStep* pCard = new CCustomDlStep();

	TiXmlNode* rootNode = NULL;
	TiXmlElement *pModelElement = NULL;
    TiXmlDocument * pInitXmlManager = NULL;

	pInitXmlManager = load_xml(pData, FROM_DATA);
	if (pInitXmlManager == NULL)
	{
		REPORT(REPORT_ERROR, "Xml is not valid\n");
		ret = -1;
		rInfo.status = ret;
		return rInfo;
	}
	else
	{
		rootNode = pInitXmlManager->RootElement();
	}
	pModelElement = rootNode->FirstChildElement("model");

	ret = parseCaseBinding("/home/V1.0.0/HiAutoTestServer/bin/");
	if (ret != 0)
	{
		printf("parseCaseBinding faild\n");
	}

	ret = pCard->parse(pModelElement, NULL);
	if (ret != 0)
	{
		//REPORT(REPORT_ERROR, "subStepNo %d parse failed\n", subStepNo);
		printf("parsr faild\n");

	}

	ret = creatDataDictionaryManualXml("/home/V1.0.0/HiAutoTestServer/bin/", pCard->m_dlName.c_str(), 0);
	if(ret != 0)
	{
		printf("creat dataDictionary err\n");
		ret = -1;
		rInfo.status = ret;
		return rInfo;
	}

	ret = pCard->init();
	if (ret != 0)
	{

		modelStatus = STATUS_ERROR;
		ret = -1;
		rInfo.status = ret;
		return rInfo;
	}

	ret = pCard->run();
	if (ret != 0)
	{
		printf("mode run faild\n");
		modelStatus = STATUS_ERROR;
		rtcDispose();
		ret = -1;
		rInfo.status = ret;
		return rInfo;
	}

	rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::stopModel (const char* modelName)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	if(modelName == NULL)
	{
		printf("=====goto COrbaImpl::getModelRunStatus_API, modelName=NULL\n");
		ret = -1;
		rInfo.status = ret;
		return rInfo;
	}

	//	printf("===================start to stop Model=======modelName:%s,strlen(modelName):%d=============\n",modelName,strlen(modelName));
	char buff[64] = {0};
	sprintf(buff, "%s%s%s", "lib", modelName, ".so");
	string pModelName(buff);
	cout << "============stop Model cout << model: "<< pModelName << endl;

	TY_MODEL_CARD_MAP::iterator iterModel;
	iterModel = modelCard.find(pModelName);
	if( iterModel != modelCard.end())
	{
		CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;
		pCard->stop();
		modelCard.erase(iterModel);
	}
	else
	{
		printf("[%s:%s:%d]-->Do not found modelName:%s!\n",__FILE__,__FUNCTION__,__LINE__,pModelName.c_str());
		ret = -1;
	}

	rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::getModelRunStatus (const char* modelName, ::CORBA::Long_out modelStatus)
{
	if(modelName == NULL)
	{
		printf("=========================goto COrbaImpl::getModelRunStatus_API, modelName=NULL\n");
	}

	//printf("========================================goto COrbaImpl::getModelRunStatus_API, modelName:%s\n",modelName);
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	char buff[64] = {0};
	sprintf(buff, "%s%s%s", "lib", modelName, ".so");
	string pModelName(buff);

	TY_MODEL_CARD_MAP::iterator iterModel = modelCard.find(pModelName);
	if( iterModel != modelCard.end())
	{
		//REPORT(REPORT_INFO, "-------------------------------------get model status\n");
		CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;
		int ModelStatus = pCard->getStatus();
		modelStatus = ModelStatus;
	}
	else
	{
		for(TY_MODEL_CARD_MAP::iterator iter = modelCard.begin(); iter!=modelCard.end(); iter++)
		{
			printf("[%s:%s:%d]-->modelMap have:%s\n",__FILE__,__FUNCTION__,__LINE__, iter->first.c_str());
		}
		printf("[%s:%s:%d]-->Do not found modelName:%s!,map len:%d\n",__FILE__,__FUNCTION__,__LINE__,pModelName.c_str(),modelCard.size());
		ret = -1;
	}

	rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::setICDData (const ::autoTest::TySetIcdInfo & icdInfo, ::CORBA::Long icdNum)
{
	int ret = 0;
	int index = 0;
	unsigned long long tmpData;
	autoTest::ReturnInfo rInfo;

	TY_SET_ICD_INFO tySetIcdInfo[icdNum];
	CICDManager *pICDManager = CICDManager::getInstance();

	memset(tySetIcdInfo, 0, sizeof(tySetIcdInfo));
	for(int i = 0; i < icdNum; i++)
	{
		
	//printf("loop--------set icd \n");
			//memcpy(tySetIcdInfo[i].icdID, icdInfo[i].icdID, 128);
			strcpy(tySetIcdInfo[i].icdID, icdInfo[i].icdID);
			int  count = 0;
			for(int j = 0; j < icdInfo[i].fieldNum; j++)
			{
					tySetIcdInfo[i].fieldNum = icdInfo[i].fieldNum;
					//memcpy(tySetIcdInfo[i].fieldID[j], icdInfo[i].fieldID[j], 128);
					strcpy(tySetIcdInfo[i].fieldID[j], icdInfo[i].fieldID[j]);
					tySetIcdInfo[i].data[j] = icdInfo[i].data[j];
						count++;
			//cout << "set ICDdata ICDID = " << tySetIcdInfo[i].icdID << "  fieldNum = "<< tySetIcdInfo[i].fieldNum <<  " fieldID = " << tySetIcdInfo[0].fieldID[0]  << " data = " << tySetIcdInfo[i].data[j] << "count = " << count << endl;
			}
			count = 0;
	}

	for(int i = 0; i < icdNum; i++)
	{
		string icd(tySetIcdInfo[i].icdID);
		CBlock *pBlock = pICDManager->getBlock(icd);

		if(pBlock == NULL)
		{
			REPORT(REPORT_ERROR, "Get block failed\n");
			//m_stepStatus = STATUS_ERROR;

			ret = -1;
			rInfo.status = ret;
			return rInfo;
		}

		BaseField* pField;

		for(index; index < tySetIcdInfo[i].fieldNum; index++)
		{
			string field(tySetIcdInfo[i].fieldID[index]);
			pField = pBlock->getField(field);
			if(pField == NULL)
			{
				cout << "fieldID = " << field << endl;
				REPORT(REPORT_ERROR, "Get field failed\n");

				ret = -1;
				rInfo.status = ret;
				return rInfo;
			}

			//cout << " ---------------------------set data = " << tySetIcdInfo[i].data[index] << endl; 
			//printf("-------------fieldID = pField->uuid = %s\n", pField->m_uuid.c_str());
			memcpy(&tmpData, &tySetIcdInfo[i].data[index], 8);
			pField->setDoubleData(tmpData);
			//pField->setDoubleData(tyseticdinfo[i].data[index]);

			pBlock->packTxICD();

		}//

	}
	//`printf("------------------------------------------set icd \n");
	rInfo.status = ret;
	return rInfo;

}

autoTest::ReturnInfo CorbaImpl::sendICDData(const ::autoTest::TySendIcdInfo & icdInfo, ::CORBA::Long icdNum)
{
	int ret = 0;
	int chnID;
	autoTest::ReturnInfo rInfo;
	//TY_SEND_ICD_INFO tySendInfo;
	TY_SEND_ICD_INFO tySendInfo[icdNum];
	unsigned char txData[MAX_PAYLOAD_LEN];
	int length = MAX_PAYLOAD_LEN;
	int lengthRet = 0;

	//cout  << "start send " << endl;

	for(int i= 0; i < icdNum; i++)
	{

		tySendInfo[i].cardID = (int)icdInfo[i].cardID;

		string tmp = icdInfo[i].icdID.in();
		//memcpy(tySendInfo[i].icdID,tmp.c_str(), 38);
		strcpy(tySendInfo[i].icdID,tmp.c_str());
		tmp = icdInfo[i].cardType.in();
		//memcpy(tySendInfo[i].cardType,tmp.c_str(), 14);
		strcpy(tySendInfo[i].cardType,tmp.c_str());
		//printf("loop--------cardID[%d] = %d\n ",i, tySendInfo[i].cardID);
	}

	//printf("222222222222222222222-----cardType = %s, icdID = %s\n",tySendInfo[0].cardType,tySendInfo[0].icdID );
	CICDManager *pICDManager = CICDManager::getInstance();
	for(int i = 0; i < icdNum; i++)
	{
		//double x = 9;
		CBlock *pBlock = pICDManager->getBlock(tySendInfo[i].icdID);
		if(pBlock == NULL)
		{
			ret = -1;
		}
		else
		{
			lengthRet = pBlock->getTxData(txData, length);//3<<
		}

		CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
		CCardManager * pCardManager = pHardwareManager->getCardManager(tySendInfo[i].cardType);
		if(pCardManager == NULL)
		{
			ret = -1;
			rInfo.status = ret;
			return rInfo;
		}

		CCard* pCard = pCardManager->getCard(tySendInfo[i].cardID);
		if(pCard == NULL)
		{
			ret = -1;
			rInfo.status = ret;
			return rInfo;
		}

		//lengthRet = 8;
		//	cout << "send size = " << lengthRet << endl;
		//ret = pCard->writeData(tySendInfo[i].icdID, (char*)&x, lengthRet);
		ret = pCard->writeData(tySendInfo[i].icdID, (char*)txData, lengthRet);
		if(ret != 0)
		{
			cout << "send faild --- icdID = " << tySendInfo[i].icdID << endl;
			ret = -1;
			rInfo.status = ret;
			return rInfo;
		}

	}

	//printf("icd  send success--catd id = %d--- -------------- len = %d, icd ID = %s\n",tySendInfo[0].cardID, lengthRet, tySendInfo[0].icdID);
	rInfo.status = ret;
	return rInfo;
} 

autoTest::ReturnInfo CorbaImpl::getICDData(const ::autoTest::TyGetIcdInfo & icdInfoIn, ::autoTest::TyGetIcdInfo_out icdInfo, ::CORBA::Long icdNum)
{
	int ret = 0;
	int count = 0;
	double x;
	int errFlag[1024] = {0};
	memset(errFlag, 0, sizeof(errFlag));
	//printf("start get icd data\n");
	autoTest::ReturnInfo rInfo;
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
	unsigned char rxData[MAX_PAYLOAD_LEN];
	int length =  500;//MAX_PAYLOAD_LEN;//TODO:modyfi
	int num = icdInfoIn.length();
	icdInfo = new autoTest::TyGetIcdInfo();
	icdInfo->length(num);
	struct timeval t;
	TY_GET_ICD_INFO tyGetIcdInfo[icdNum];

	for(int i = 0; i < icdNum; i++)
	{
		//printf("-------------- cardID = %d -- \n",icdInfoIn[i].cardID);
    
		tyGetIcdInfo[i].cardID = icdInfoIn[i].cardID;
		string cardType = icdInfoIn[i].cardType.in();
		sprintf(tyGetIcdInfo[i].cardType, "%s", cardType.c_str());
		string icdID = icdInfoIn[i].icdID.in();
		sprintf(tyGetIcdInfo[i].icdID, "%s", icdID.c_str());

		tyGetIcdInfo[i].fieldNum = icdInfoIn[i].fieldNum;
		icdInfo[i].fieldID.length(icdInfoIn[i].fieldNum);
		icdInfo[i].data.length(icdInfoIn[i].fieldNum);
		//fprintf(fp, "[%s:%s:%d]>>>>>icdID:%s, icdnum:%d, fieldNum:%d\n",__FILE__,__FUNCTION__,__LINE__,icdID.c_str(),icdNum, icdInfoIn[i].fieldNum);
		//fprintf(fp, "\n");
		for(int j = 0; j < tyGetIcdInfo[i].fieldNum; j++)
		{
			string fieldID = icdInfoIn[i].fieldID[j].in();
			sprintf(tyGetIcdInfo[i].fieldID[j], "%s", fieldID.c_str());

		}
		//cout << " GetICDData  ICDID = " << tyGetIcdInfo[i].icdID << "  fieldNum = "<< icdInfoIn[i].fieldNum <<  " fieldID = " <<  icdInfoIn[i].fieldID[0] << endl;
	}
	//REPORT(REPORT_ERROR,"===icdNum:%d, fieldNum:%d, icd1Id:%s, icd2Id:%s",icdNum, tyGetIcdInfo[0].fieldNum+tyGetIcdInfo[1].fieldNum,tyGetIcdInfo[0].icdID,tyGetIcdInfo[1].icdID );
	for(int i = 0; i < icdNum; i++)
	{
		//memset(&tyGetIcdInfo[i].data, 0, sizeof(yGetIcdInfo[i].data));
		string cardType(tyGetIcdInfo[i].cardType);
		CCardManager* pCardManager = pHardwareManager->getCardManager(cardType);
		if(pCardManager == NULL)
		{
			printf("---error: cardType:%s\n", cardType.c_str());
			//ret = -1;
			rInfo.status = ret;
			return rInfo;
		}
		else
		{
			CCard* pCard = pCardManager->getCard(tyGetIcdInfo[i].cardID);
			if(pCard == NULL)
			{
				printf("------error: cardID: %d, icdiD:%s, faild :%d, icdNum:%d\n",tyGetIcdInfo[i].cardID,tyGetIcdInfo[i].icdID,i, icdNum );
			//	ret = -1;
				rInfo.status = ret;
				return rInfo;
			}
			else
			{
				//ret = pCard->readData(tyGetIcdInfo[i].icdID , (char*)&x, length, &t);
				//	printf("-------------------x = %lf\n",x);
				int tmpVal = 0;
				memset(rxData, 0, sizeof(rxData));
				length = 500;
				ret = pCard->readData(tyGetIcdInfo[i].icdID , (char*)rxData, length, &t);
				if(0 != ret)
				{
					//printf("-------------- ret = %d -- icdID = %s\n",ret, tyGetIcdInfo[i].icdID);
					if(ret == -3)
					{
						//for(int i = 0; i < icdNum; i++)
						//{
						//printf("-------------  -3\n");
						for(int j = 0; j < tyGetIcdInfo[i].fieldNum; j++)
						{
							//icdInfo[i].data[j] = 0.0;
							tyGetIcdInfo[i].data[j] = 0.0;
						}
						//}
						errFlag[i] = 1;
						continue;
					}
					else 
					{
						printf("-------------- ret = %d -- icdID = %s\n",ret, tyGetIcdInfo[i].icdID);
						errFlag[i] = 1;
						continue;

					}
				}

#if 0 // add printf to find bug!!! for 429 bug, ok
				REPORT(REPORT_ERROR, "======readData  ==tyGetIcdInfo[i].cardID:%d\n",tyGetIcdInfo[i].cardID );
				memcpy(&tmpVal, rxData, 4);
				tmpVal =  (tmpVal >> 13) & 0xfffe;
				REPORT(REPORT_ERROR, "======readData yu uuid:%s ==tmpVal:%d, data[0]:0x%0x, data[1]:0x%0x\n", tyGetIcdInfo[i].icdID, tmpVal,rxData[0],rxData[1] );
#endif

			}
			count++;
		}

		if(errFlag[i] == 1)
		{
			printf("errFlag[%d] = 1", i);
			//goto getICD;
			continue;
		}
		//>> modify by 20231017

		if(refreshRecvCache((const char*)tyGetIcdInfo[i].icdID, t) != 0)
		{
			for(int j = 0; j < tyGetIcdInfo[i].fieldNum; j++)
			{
				tyGetIcdInfo[i].data[j] = 0.0;

			}	

			continue;
			//goto getICD;
		}
		//<< end modify
		string icdID( tyGetIcdInfo[i].icdID);
		CICDManager *pICDManager = CICDManager::getInstance();
		CBlock *pBlock = pICDManager->getBlock(icdID);
		if(pBlock == NULL)
		{
			REPORT(REPORT_ERROR, "---------------------------------Get m_ICDID failed\n");
			ret = -1;
		}
		else
		{
			length = 500;
			pBlock->setRxData(rxData, length);

		}
		for(int j = 0; j < tyGetIcdInfo[i].fieldNum; j++)
		{
			string field(tyGetIcdInfo[i].fieldID[j]);
			BaseField* pField = pBlock->getField(field);
			if(pField == NULL)
			{
				REPORT(REPORT_ERROR, "----------------------------------Get field failed\n");
			//	ret = -1;
				rInfo.status = ret;
				return rInfo;
			}
			double tmpVal = 0;
			pBlock->unpackRxICD();
			pField->getData(&tmpVal);
			pField->getData(&tyGetIcdInfo[i].data[j]);
#if(0)
			if(icdID == "e1fda781-24eb-40ce-bb5a-34bb84e62ad8" && field == "b1bb0b4d-4713-4d60-a258-efa014506c83")
			{
				fprintf(fp, "[%s:%s:%d]>>>>>1111icdID:%s, field:%s, value:%lf\n",__FILE__,__FUNCTION__,__LINE__,icdID.c_str(),tyGetIcdInfo[i].data[j],tmpVal);
				fprintf(fp, "\n");
				//printf("[%s:%s:%d]----==========field:%s, data:%lf, tmpVal:%lf\n",__FILE__,__FUNCTION__,__LINE__,tyGetIcdInfo[i].fieldID[j], tyGetIcdInfo[i].data[j],tmpVal);
			}

			if(icdID == "d37462ae-1fe7-40a1-9c83-a09c5c5607eb" && field == "e65b8b4c-e750-4e1b-9cfd-7e0d2119d6fa")
			{
				fprintf(fp, "[%s:%s:%d]>>>>>2222icdID:%s, field:%s, value:%lf\n",__FILE__,__FUNCTION__,__LINE__,icdID.c_str(),field.c_str(),tmpVal);
				fprintf(fp, "\n");
				//printf("[%s:%s:%d]----==========field:%s, data:%lf, tmpVal:%lf\n",__FILE__,__FUNCTION__,__LINE__,tyGetIcdInfo[i].fieldID[j], tyGetIcdInfo[i].data[j],tmpVal);
			}
			if(icdID == "fd043c9b-1e08-427e-8970-1b519c09acbb" && field == "4828a5b6-eecd-4b26-92e7-fde00fced240")
			{
				fprintf(fp, "[%s:%s:%d]>>>>>3333icdID:%s, field:%s, value:%lf\n",__FILE__,__FUNCTION__,__LINE__,icdID.c_str(),field.c_str(),tmpVal);
				fprintf(fp, "\n");
				//printf("[%s:%s:%d]----==========field:%s, data:%lf, tmpVal:%lf\n",__FILE__,__FUNCTION__,__LINE__,tyGetIcdInfo[i].fieldID[j], tyGetIcdInfo[i].data[j],tmpVal);
			}

#endif

		}
	}

	for(int i = 0; i < icdNum; i++)
	{
		for(int j = 0; j < icdInfoIn[i].fieldNum; j++)
		{
			//icdInfo[i].data[j] = x;
			icdInfo[i].data[j] = tyGetIcdInfo[i].data[j];
#if(0)
				fprintf(fp, "[%s:%s:%d]>>>>>3333icdID:%s, field:%s, value:%lf, time:%lld\n",__FILE__,__FUNCTION__,__LINE__, 
							tyGetIcdInfo[i].icdID,tyGetIcdInfo[i].fieldID[j],tyGetIcdInfo[i].data[j], t);
				fprintf(fp, "\n");
#endif
			//REPORT(REPORT_ERROR,"===field:%s, data:%lf",tyGetIcdInfo[i].fieldID[j], icdInfo[i].data[i]);
		}
	}
getICD:
	rInfo.status = 0;
	return rInfo;
}

autoTest::ReturnInfo CorbaImpl::getModelParam(const ::autoTest::TyModelInfo & modelInfoIn, ::autoTest::TyModelInfo_out modelInfo, ::CORBA::Long modelNum)
{

	int ret = 0;
	int count = 0;
	char buff[64] = {0};
	autoTest::ReturnInfo rInfo;

	if(MANUAL_TEST_FLAG == 1)
	{
		TY_MODEL_CARD_MAP::iterator iterModel;
		vector<TY_PARAM_INFO>::iterator iter;
		int num = modelInfoIn.length();
		modelInfo = new autoTest::TyModelInfo();
		modelInfo->length(num);
		TY_MODEL_INFO tyModelInfo[modelNum];
		modelInfo = new autoTest::TyModelInfo();
		modelInfo->length(modelNum);

		for(int i = 0; i < modelNum; i++)
		{
      		modelInfo[i].fullPath.length(modelInfoIn[i].num);
			modelInfo[i].data.length(modelInfoIn[i].num);
			string modelName = modelInfoIn[i].modelName.in();
			sprintf(tyModelInfo[i].modelName, "%s", modelName.c_str());
			tyModelInfo[i].num = modelInfoIn[i].num;
      
			for(int j = 0; j <  modelInfoIn[i].num; j++)
			{
				string fullPath = modelInfoIn[i].fullPath[j].in();
				sprintf(tyModelInfo[i].fullPath[j], "%s", fullPath.c_str());
				//memcpy(tyModelInfo[i].fullPath[j], modelInfoIn[i].fullPath[j], 128);
			}
		}


		for(int i = 0; i < modelNum; i++)
		{
			sprintf(buff, "%s%s%s", "lib", tyModelInfo[i].modelName, ".so");
			string pModelName(buff);

			//string pModelName(tyModelInfo[i].modelName);
			iterModel = modelCard.find(pModelName);
			if( iterModel != modelCard.end())
			{
				CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;
				//printf("=============get ModelParam===========num:%d\n",tyModelInfo[i].num);
				//CCustomDlStep* pCard = new CCustomDlStep();
				for(int j = 0; j < tyModelInfo[i].num; j++)
				{

					string fullPath(tyModelInfo[i].fullPath[j]);
					//printf("=============get ModelParam===========fullpath:%s\n",fullPath.c_str());
					for(iter = pCard->m_subStepInfo.paramInfo.begin(); iter != pCard->m_subStepInfo.paramInfo.end(); iter++)
					{
						//	printf("=============get ModelParam===========Map->fullpath:%s\n",iter->strParamName.c_str());
						if(strcmp(iter->strParamName.c_str(), fullPath.c_str()) == 0)
						{
							memcpy(&tyModelInfo[i].data[j], iter->value.value, 8);
							//printf("==========get model param var: %lf\n",tyModelInfo[i].data[j]);
						}
						else
						{
							continue;
						}
					}
					/*
					iter = find(pCard->m_subStepInfo.paramInfo.begin(), pCard->m_subStepInfo.paramInfo.end(), fullPath);
					if(iter !=	pCard->m_subStepInfo.paramInfo.end())
					{
					memcpy(&tyModelInfo[i].data[j], iter->value.value, 8);
					cout << "fullPath find!" << endl;
					}
					else
					{
					cout << "full Path not find!" << endl;
					ret = -1;
					}
					*/

				}

				/*
				for(vector<TY_PARAM_INFO>::iterator iter = pCard->m_subStepInfo.inputVarInfo.begin(); iter != pCard->m_subStepInfo.inputVarInfo.end();iter++)
				{
				memcpy(tyModelInfo[i].fullPath[count], iter->name.c_str(), 128);
				count++;
				}
				*/

			}
			else
			{
				REPORT(REPORT_ERROR,"------Do not find model :tyModelInfo[%d].modelName:%s ",i, tyModelInfo[i].modelName);
				ret = -1;
			}
		}

		for(int i = 0; i < modelNum; i++)
		{
			modelInfo[i].modelName = CORBA::string_dup(tyModelInfo[i].modelName);
			for(int j = 0; j < tyModelInfo[i].num; j++)
			{
				modelInfo[i].fullPath[j] = CORBA::string_dup(tyModelInfo[i].fullPath[j]);
				modelInfo[i].data[j] = tyModelInfo[i].data[j];
			}
		}
	}
	else
	{
		int num = modelInfoIn.length();
		modelInfo = new autoTest::TyModelInfo();
		modelInfo->length(num);

		for(int i = 0; i < modelNum; i++)
		{
			modelInfo[i].fullPath.length(modelInfoIn[i].num);
			modelInfo[i].data.length(modelInfoIn[i].num);
			/*for(int j = 0; j <  modelInfoIn[i].num; j++)
			{
				modelInfo[i].fullPath[j] = modelInfoIn[i].fullPath[j];
			}*/

			string modelName = modelInfoIn[i].modelName.in();
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
						for(int j = 0; j <  modelInfoIn[i].num; j++)
						{
							int paramNum = 0;
							modelInfo[i].fullPath[j] = modelInfoIn[i].fullPath[j];
							//printf("===========ZR======get Model Param=========fullpath:%s, var:%lf\n",
									//modelInfo[i].fullPath[j].in(), modelInfo[i].data[j]);
							iter = pParamVarMap->find(modelInfoIn[i].fullPath[j].in());
							if(iter != pParamVarMap->end())
							{
								TY_PARAM_VAL paramVal;
							
									memcpy(&modelInfo[i].data[j], iter->second.value.value, 8);
									
									//printf("==========get model param var: %lf\n",tyModelInfo[i].data[j]);
								//}
								/*
								sprintf(paramVal.paramName,"%s",iter->second.strParamName.c_str());
								sprintf(paramVal.paraReferenced,"%s",iter->second.strReferenced.c_str());
								paramVal.uParamLength = iter->second.uParamLength;
								paramVal.offset = iter->second.offset;
								getParamVal((char*)modelName.c_str(),&paramVal);
								//modelInfo[i].fullPath[j] = CORBA::string_dup(tyModelInfo[i].fullPath[j]);
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

	rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::setModelParam(const ::autoTest::TyModelInfo & modelInfo, ::CORBA::Long modelNum)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;

	if(MANUAL_TEST_FLAG == 1)
	{
		TY_MODEL_INFO tyModelInfo[modelNum];
		TY_MODEL_CARD_MAP::iterator iterModel;
		vector<TY_PARAM_INFO>::iterator iter;
		char buff[64] = {0};
		for(int i = 0; i < modelNum; i++)
		{
			strcpy(tyModelInfo[i].modelName, modelInfo[i].modelName);
			tyModelInfo[i].num = modelInfo[i].num;
			for(int j = 0; j <  modelInfo[i].num; j++)
			{
				strcpy(tyModelInfo[i].fullPath[j], modelInfo[i].fullPath[j]);
				tyModelInfo[i].data[j] = modelInfo[i].data[j];
			}
		}
		for(int i = 0; i < modelNum; i++)
		{
			sprintf(buff, "%s%s%s", "lib", tyModelInfo[i].modelName, ".so");
			string modelName(buff);

			

			iterModel = modelCard.find(modelName);
			if( iterModel != modelCard.end())
			{
				CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;

				for(int j = 0; j < tyModelInfo[i].num; j++)
				{

					string fullPath(tyModelInfo[i].fullPath[j]);
#if(0)
					fprintf(fp, "[%s:%s:%d]>>>>>3333modelPara:%s value:%lf\n",__FILE__,__FUNCTION__,__LINE__, 
							tyModelInfo[i].fullPath[j], tyModelInfo[i].data[j]);
					fprintf(fp, "\n");
#endif
					for(iter = pCard->m_subStepInfo.paramInfo.begin(); iter != pCard->m_subStepInfo.paramInfo.end(); iter++)
					{
						if(strcmp(iter->strParamName.c_str(), fullPath.c_str()) == 0)
						{
							memcpy(iter->value.value,  &tyModelInfo[i].data[j], 8);

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
				REPORT(REPORT_ERROR,"Do not find model :tyModelInfo[%d].modelName :%s",i, tyModelInfo[i].modelName);
				ret = -1;
			}
		}
	}
	else
	{

		for(int i = 0; i < modelNum; i++)
		{
    	    string modelName = modelInfo[i].modelName.in();
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
           				for(int j = 0; j <  modelInfo[i].num; j++)
						{
							int paramNum = 0;
							//printf("===========ZR======set Model Param=========dllName %s,fullpath:%s, var:%lf\n", dllName.c_str(),modelInfo[i].fullPath[j].in(), modelInfo[i].data[j]);
							iter = pParamVarMap->find(modelInfo[i].fullPath[j].in());
							if(iter != pParamVarMap->end())
							{
								memcpy(iter->second.value.value,  &modelInfo[i].data[j], 8);
								//printf("======ZR====set modelvar%d %d:%lf %d %lf\n",i, j, modelInfo[i].data[j],iter->second.offset,iter->second.value.dValue);
							}
						}
					}

				}
			}

		}

	}


	rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::getModelVar(const ::autoTest::TyModelInfo & modelInfoIn, ::autoTest::TyModelInfo_out modelInfo, ::CORBA::Long modelNum)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;

	if(MANUAL_TEST_FLAG == 1)
	{
		int count = 0;
		char buff[64] = {0};

		TY_MODEL_INFO tyModelInfo[modelNum];
		int num = modelInfoIn.length();
		modelInfo = new autoTest::TyModelInfo();
		modelInfo->length(num);

		vector<TY_VAR_INFO>::iterator iter;
		CObjectData *pAutoTest = CObjectData::getInstance();
		CCurveManager *pCurveManager = CCurveManager::getInstance();

		//REPORT(REPORT_ERROR,"======modelnum:%d,fieldnum:%d\n",modelNum,modelInfoIn[0].num);

		for(int i = 0; i < modelNum; i++)
		{
			memset(&tyModelInfo[i], 0, sizeof(TY_MODEL_INFO));
			tyModelInfo[i].num = modelInfoIn[i].num;
			//memcpy(tyModelInfo[i].modelName, modelInfoIn[i].modelName, 128);
			strcpy(tyModelInfo[i].modelName, modelInfoIn[i].modelName);
			modelInfo[i].fullPath.length(modelInfoIn[i].num);
			modelInfo[i].data.length(modelInfoIn[i].num);
			count = 0;
			for(int j = 0; j <	tyModelInfo[i].num; j++)
			{

			//	memcpy(tyModelInfo[i].fullPath[j], modelInfoIn[i].fullPath[j],256);
				strcpy(tyModelInfo[i].fullPath[j], modelInfoIn[i].fullPath[j]);
				//printf("count:%d, fieldnum:%d,tyModelInfo[%d].fullPath[%d]:%s\n",count++,tyModelInfo[i].num,i,j,tyModelInfo[i].fullPath[j]);
				//tyModelInfo[i].data[j] = modelInfoIn[i].data[j];
			}
		}

		for(int i = 0; i < modelNum; i++)
		{

			sprintf(buff, "%s%s%s", "lib", tyModelInfo[i].modelName, ".so");
			string modelName(buff);
				//cout << "========model name: " << modelName << endl;
			TY_MODEL_CARD_MAP::iterator iterModel = modelCard.find(modelName);
			if( iterModel != modelCard.end())
			{
				//printf("=======goto modelVal, modelFullpathNum[%d]:%d==========modelNum:%d\n",i,tyModelInfo[i].num,modelNum);
				CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;

				for(int j = 0; j < tyModelInfo[i].num; j++)
				{
					string fullPath(tyModelInfo[i].fullPath[j]);
					//cout << "=========fullPath: " << fullPath<< endl;
					for(iter = pCard->m_subStepInfo.outputVarInfo.begin(); iter != pCard->m_subStepInfo.outputVarInfo.end(); iter++)
					{
						if(strcmp(iter->strVarName.c_str(), fullPath.c_str()) == 0)
						{
							//memcpy(&tyModelInfo[i].data[j], iter->value.value, 8);
							//memcpy(iter->value.value,  &tyModelInfo[i].data[j], 8);
							//TY_VAR_INFO var;
							iter->value.llValue = 0;
							double value;
							getVarDataByName(fullPath.c_str(), (char*)&value);

							pAutoTest->setDigitValueByDouble(&(iter->value), iter->strVarType, value);

							tyModelInfo[i].data[j] = iter->value.dValue;
						//	printf("=======out put:%s data = %lf\n",tyModelInfo[i].fullPath[j],value);
							ret = 0;
							break;
						}
						else
						{

							continue;
						}
					}
					if(iter == pCard->m_subStepInfo.outputVarInfo.end())
					{
						//printf("===OutputVarInfo not found !\n");
						for(iter = pCard->m_subStepInfo.inputVarInfo.begin(); iter != pCard->m_subStepInfo.inputVarInfo.end(); iter++)
						{
							if(strcmp(iter->strVarName.c_str(), fullPath.c_str()) == 0)
							{

								double value;
								if(strcmp(iter->strBindType.c_str(), "ICD") == 0)
								{
									ret = ::getICDData((char*)iter->strIcdID.c_str(), (char*)iter->strFieldID.c_str(), &(iter->value));
									pAutoTest->getDigitValueByDouble(iter->value, iter->strVarType, &value);
									tyModelInfo[i].data[j] = iter->value.dValue;
#if(0)
									if(fullPath == "Astatus_WithDataType_setCmd/PDU_System_Simulator/ICD_unpack_FSECU2SPDU_slave/label_202_S_PosDiff__fsecu2spdu")
									{
										fprintf(fp, "[%s:%s:%d]>>>>>1111FullPath:%s, value:%lf\n",__FILE__,__FUNCTION__,__LINE__,fullPath.c_str(),tyModelInfo[i].data[j]);
										fprintf(fp, "\n");
									}
#endif
								}
								else if(strcmp(iter->strBindType.c_str(), "Curve") == 0)
								{
									ret = pCurveManager->getCurveData((char*)iter->strCurveID.c_str(), &value);
									pAutoTest->getDigitValueByDouble(iter->value, iter->strVarType, &value);
									tyModelInfo[i].data[j] = iter->value.dValue;
								}
								else if(strcmp(iter->strBindType.c_str(), "Constant") == 0)
								{

									ret = 0;
									tyModelInfo[i].data[j] = iter->value.dValue;
									//printf("======get input data:%lf\n", tyModelInfo[i].data[j]);
								}
								break;
							}
							else
							{

								continue;
							}
						}

					}

				}

			}
			else
			{
				REPORT(REPORT_ERROR,"Do not find model :tyModelInfo[%d].modelName :%s",i, tyModelInfo[i].modelName);
				ret = -1;
			}
		}


		for(int i = 0; i < modelNum; i++)
		{
			modelInfo[i].modelName = CORBA::string_dup(tyModelInfo[i].modelName);
			for(int j = 0; j < tyModelInfo[i].num; j++)
			{
				modelInfo[i].fullPath[j] = CORBA::string_dup(tyModelInfo[i].fullPath[j]);
				modelInfo[i].data[j] = tyModelInfo[i].data[j];
			}
		}
	}
	else
	{
		//add by ZR 20230617>>
    	int num = modelInfoIn.length();
		modelInfo = new autoTest::TyModelInfo();
		modelInfo->length(num);
		for(int i = 0; i < modelNum; i++)
		{
			modelInfo[i].fullPath.length(modelInfoIn[i].num);
			modelInfo[i].data.length(modelInfoIn[i].num);
			for(int j = 0; j < modelInfoIn[i].num; j++)
			{
				string fullPath;
				fullPath = modelInfoIn[i].fullPath[j];
				modelInfo[i].fullPath[j] = modelInfoIn[i].fullPath[j];
				double value = 0;
				ret = getModelVarInterface((char*)fullPath.c_str() ,&value);  // ----modefy by wc--20230625
#if PRINTLOG
				if((strcmp((char*)fullPath.c_str() , "Bstatus_WithDataType_0904/Main_Flight_Control/MDL_with_Control/Controlsystem/SS_TX_to_FSECU/PFC_FSECU4/MIL-1553-AG1sta_fsecu4") == 0
					||strcmp((char*)fullPath.c_str() , "Bstatus_WithDataType_0904/Main_Flight_Control/MDL_with_Control/Controlsystem/SS_TX_to_FSECU/PFC_FSECU4/MIL-1553-AG4sta_fsecu4") == 0))
				{
					fprintf("varName:%s,value:%lf,ret:%d\n",(char*)fullPath.c_str() ,value, ret);
				}
#endif
				//ret = getVarDataByName(fullPath.c_str(), (char*)&value);
				modelInfo[i].data[j] = value;
				//printf("======get input:%s data:%lf,data:%lf,ret %d.\n", fullPath.c_str(),value,modelInfo[i].data[j],ret);
			}
		}
	}


	rInfo.status = 0;
	//rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::setModelVar(const ::autoTest::TyModelInfo & modelInfo, ::CORBA::Long modelNum)
{
	int ret = 0;

	if(MANUAL_TEST_FLAG == 1)
	{
		char buff[64] = {0};
		autoTest::ReturnInfo rInfo;
		TY_MODEL_INFO tyModelInfo[modelNum];
		TY_MODEL_CARD_MAP::iterator iterModel;
		vector<TY_VAR_INFO>::iterator iter;
		printf("-----moelNum:%d,\n");
		for(int i = 0; i < modelNum; i++)
		{
			
			strcpy(tyModelInfo[i].modelName, modelInfo[i].modelName);
			tyModelInfo[i].num = modelInfo[i].num;
			for(int j = 0; j <  modelInfo[i].num; j++)
			{
				strcpy(tyModelInfo[i].fullPath[j], modelInfo[i].fullPath[j]);
				tyModelInfo[i].data[j] = modelInfo[i].data[j];
				//REPORT(REPORT_ERROR, "goto set model Value,val[%d],tyModelInfo[%d].fullPath[%d]:%s, tyModelInfo[%d].data[%d]:%lf\n", modelInfo[i].num, i,j, tyModelInfo[i].fullPath[j], i,j,tyModelInfo[i].data[j]);
			}
		}
		for(int i = 0; i < modelNum; i++)
		{
			sprintf(buff, "%s%s%s", "lib", tyModelInfo[i].modelName, ".so");
			string modelName(buff);
			iterModel = modelCard.find(modelName);
			if( iterModel != modelCard.end())
			{
				CCustomDlStep* pCard = (CCustomDlStep* )iterModel->second;
				for(int j = 0; j < tyModelInfo[i].num; j++)
				{
					REPORT(REPORT_ERROR, "goto set model Value\n");
					string fullPath(tyModelInfo[i].fullPath[j]);
					for(iter = pCard->m_subStepInfo.inputVarInfo.begin(); iter != pCard->m_subStepInfo.inputVarInfo.end(); iter++)
					{
						if(strcmp(iter->strVarName.c_str(), tyModelInfo[i].fullPath[j]) == 0)
						{
#if(0)
							if(strcmp("Bstatus_WithDataType_0904/Joystick_position_sensor/SuperCtrChannel_3", iter->strVarName.c_str()) == 0)
							{
								fprintf(fp, "[%s:%s:%d]>>>>>strVarName:%s, value:%lf\n",__FILE__,__FUNCTION__,__LINE__,iter->strVarName.c_str(),tyModelInfo[i].data[j]);
								fprintf(fp, "\n");
							}
#endif
							memcpy(iter->value.value,  &tyModelInfo[i].data[j], sizeof(double));
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
				REPORT(REPORT_ERROR,"Do not find model :tyModelInfo[%d].modelName :%s",i, tyModelInfo[i].modelName);
				ret = -1;
			}
		}
	}
	else
	{
		//add by ZR 20230617>>
		for(int i = 0; i < modelNum; i++)
		{
			for(int j = 0; j <  modelInfo[i].num; j++)
			{
				string fullPath;
				fullPath = modelInfo[i].fullPath[j].in();
				setModelVarInterface((char*)fullPath.c_str(), modelInfo[i].data[j]);
				REPORT(REPORT_ERROR, "--ZR--------goto set model Value,val[%d],modelInfo[%d].fullPath[%d]:%s, modelInfo[%d].data[%d]:%lf\n",
						modelInfo[i].num, i,j, fullPath.c_str(), i,j,modelInfo[i].data[j]);
			}
		}

	}

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
/**/
/**/
autoTest::ReturnInfo CorbaImpl::optManualTestOneStep(const char * OneStep, ::autoTest::StepStatusSeq_out info)
{
	
	int ret = 0;
	int num = 1;
    TY_STEP_STATUS  stepStatus;
	char pathCase[64] = {0};

	TiXmlDocument * pCaseXmlManager = NULL;
	printf("%s", OneStep);

	pCaseXmlManager = load_xml(OneStep, FROM_DATA);
	//printf("%s", pCaseXmlManager);
	if(pathXml == NULL)
	{
		pathXml = getenv(setAutoPath);//"AUTO_HOME"
	}
	memcpy(pathCase, pathXml, strlen(pathXml));
	//pathCase = getenv(setAutoPath);//"AUTO_HOME"
	strcat(pathCase, caseXmlPath);
	
	ret = save_xml(pCaseXmlManager, pathCase);

	info = new autoTest::StepStatusSeq();
	info->length(num);
	//-->1 获取xml文件放入string变量
	string str(OneStep);
	//printf("%s\n",OneStep);
	//REPORT(REPORT_INFO, "-->%s",OneStep);
	//-->2 获取CaseManager实例
	CCaseManager * pCaseManager = CCaseManager::getInstance();
	ret = pCaseManager->init(str);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, " init error");
	}

	//-->3 获取CaseID及其对象地址
	ret = pCaseManager->optManualTestOneStep(stepStatus);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, " init error");
	}
	//-->7 获取步骤返回信息
	info[0].msg = CORBA::string_dup(stepStatus.msg.c_str());
	info[0].attachInfo = CORBA::string_dup(stepStatus.attachInfo.c_str());
	info[0].caseStepStatus = (autoTest::CaseStepStatusE)stepStatus.caseStepStatus;

	//modify by  20231108 release memory
	delete pCaseXmlManager;
	pCaseXmlManager = NULL;

	autoTest::ReturnInfo Rtinfo;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	Rtinfo.sec = ccurTime.tv_sec;
	Rtinfo.usec = ccurTime.tv_usec;
	Rtinfo.status = ret;
	return Rtinfo;
}

autoTest::ReturnInfo CorbaImpl::setCase (const char * caseData)
{
	int ret = 0;
	char pathCase[64] = {0};
	TiXmlDocument * pCaseXmlManager = NULL;
	//printf("%s", caseData);

	pCaseXmlManager = load_xml(caseData, FROM_DATA);
	//printf("%s", pCaseXmlManager);
	if(pathXml == NULL)
	{
		pathXml = getenv(setAutoPath);//"AUTO_HOME"
	}
	memcpy(pathCase, pathXml, strlen(pathXml));
	//pathCase = getenv(setAutoPath);//"AUTO_HOME"
	strcat(pathCase, caseXmlPath);

	ret = save_xml(pCaseXmlManager, pathCase);

	CCaseManager * pCaseManager = CCaseManager::getInstance();
	string caseDataStr = string(caseData);

	ret = pCaseManager->init(caseDataStr);
	//REPORT(REPORT_INFO,"--->>>>init case success\n");
	if(ret != 0)
	{
		REPORT(REPORT_ERROR,"setCase init case failed\n");
	}

	//modify by  20231108 release memory
	delete pCaseXmlManager;
	pCaseXmlManager = NULL;

	REPORT(REPORT_INFO,"CorbaImpl::setCasefinish\n");
	//printf("%s\n", caseData);
	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::setICD (const char * ICDInfo)
{
	int ret = 0;
	char pathICD[64] = {0};
	TiXmlDocument * pICDXmlManager = NULL;
	pICDXmlManager = load_xml(ICDInfo, FROM_DATA);
	if(pathXml == NULL)
	{
		pathXml = getenv(setAutoPath);//"AUTO_HOME"
	}
	memcpy(pathICD, pathXml, strlen(pathXml));
	//pathICD = getenv(setAutoPath);//"AUTO_HOME"
	strcat(pathICD, ICDXmlPath);
	ret = save_xml(pICDXmlManager, pathICD);
	REPORT(REPORT_ERROR," ICD path:%s\n",pathICD);


	//ICD First>>
	string ICDInfoStr = string(ICDInfo);
	
	CICDManager *pICDManager = CICDManager::getInstance();//<<
	ret = pICDManager->init(ICDInfoStr);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR,"setCase init ICD failed\n");
	}

	REPORT(REPORT_INFO,"setICDInfo finish\n");
	//printf("%s\n", ICDInfo);

	//modify  wc 20231108 release memory
	delete pICDXmlManager;
	pICDXmlManager = NULL;

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::setChannelConfig (const char * channelConfig)
{
	
	int ret = 0;
	char pathCard[64] = {0};
	TiXmlDocument * pChannelXmlManager = NULL;
	pChannelXmlManager = load_xml(channelConfig, FROM_DATA);
	if(pathXml == NULL)
	{
		pathXml = getenv(setAutoPath);//"AUTO_HOME"
	}
	memcpy(pathCard, pathXml, strlen(pathXml));
	//pathCard = getenv(setAutoPath);//"AUTO_HOME"

	strcat(pathCard, cardXmlPath);
	REPORT(REPORT_ERROR," Card path:%s\n",pathCard);
	ret = save_xml(pChannelXmlManager, pathCard);
	//add....>>

	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
	pHardwareManager->loadConfig(channelConfig);//"/home/V1.0.0/HardwareManager/ChannelConfig.xml"
	REPORT(REPORT_INFO,"setChannelConfig finish\n");
	//printf("%s\n", channelConfig);
	
	delete pChannelXmlManager;
	pChannelXmlManager = NULL;

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}

autoTest::ReturnInfo CorbaImpl::setStorageConfig (const char * storageConfig)
{
	int ret = 0;
	char pathStorage[128] = {0};
	TiXmlDocument * pStorageXmlManager = NULL;
	pStorageXmlManager = load_xml(storageConfig, FROM_DATA);
	if(pathXml == NULL)
	{
		pathXml = getenv(setAutoPath);//"AUTO_HOME"
	}
	memcpy(pathStorage, pathXml, strlen(pathXml));
	//pathStorage = getenv(setAutoPath);//"AUTO_HOME"
	strcat(pathStorage, StorageXmlPath);
	ret = save_xml(pStorageXmlManager, pathStorage);


	CStorageManager* pStorageManager = CStorageManager::getInstance();
	pStorageManager->initStorage(storageConfig);
	REPORT(REPORT_INFO,"storageConfig finish\n");

	//modify  20231108 release memory
	delete pStorageXmlManager;
	pStorageXmlManager = NULL;

	//printf("%s\n", storageConfig);
	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::setCurveConfig (const char * curveConfig)
{
	int ret = 0;
	char pathCurve[64] = {0};
	TiXmlDocument * pCurveXmlManager = NULL;
	pCurveXmlManager = load_xml(curveConfig, FROM_DATA);
	if(pathXml == NULL)
	{
		pathXml = getenv(setAutoPath);//"AUTO_HOME"
	}
	memcpy(pathCurve, pathXml, strlen(pathXml));
	//pathCurve = getenv(setAutoPath);//"AUTO_HOME"
	strcat(pathCurve, curveXmlPath);
	ret = save_xml(pCurveXmlManager, pathCurve);
	//printf("%s\n", curveConfig);
	CCurveManager* pCurveManager = CCurveManager::getInstance();

	pCurveManager->loadConfig(curveConfig);
	REPORT(REPORT_INFO,"curveConfig finish\n");

	//modify by 20231108 release memory
	delete pCurveXmlManager;
	pCurveXmlManager = NULL;

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::start (const ::autoTest::TestExecute & testExecute)
{
#if(PRINTLOG)
	fp = fopen("manuallog.log", "w+");
	if(fp == NULL)
	{
		printf("-------open error\n");
	}
#endif
	int ret = 0;
	// m_caseExcuteRunFlag = RUNNING;
	// cardmanager->
	MANUAL_TEST_FLAG = 0;
	if(ret == 0)
	{
		// caseManager->
		//printf("interval:%d\n", testExecute.interval);
		int num = testExecute.caseExecute.length();
		//printf("testExecuteNum:%d num:%d\n", testExecute.testExecuteNum, num);
		if(num < testExecute.testExecuteNum)
		{
			REPORT(REPORT_INFO,"Actually num is less than execute num.\n");
			ret = -1;
		}
		else
		{
			TY_TEST_EXECUTE testExecuteInfo;
			testExecuteInfo.testExecuteNum = testExecute.testExecuteNum;
			testExecuteInfo.interval = testExecute.interval;
			for(int i = 0; i < testExecuteInfo.testExecuteNum; i++)
			{
				testExecuteInfo.caseExecute[i].caseExecuteTimes = testExecute.caseExecute[i].caseExecuteTimes;
				testExecuteInfo.caseExecute[i].interval =  testExecute.caseExecute[i].interval;

				string caseID = testExecute.caseExecute[i].caseID.in();
				sprintf(testExecuteInfo.caseExecute[i].caseID, "%s", caseID.c_str());////add....

				//printf("  caseExecuteTimes:%d\n", testExecute.caseExecute[i].caseExecuteTimes);
				//printf("  interval:%d\n", testExecute.caseExecute[i].interval);
				//printf("  caseID:%s\n", caseID.c_str());
			}
			CCaseManager * pCaseManager = CCaseManager::getInstance();
			ret = pCaseManager->start(testExecuteInfo);
		}
	}
	//case<<

	REPORT(REPORT_INFO,"start finish\n");
	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::pause (void)
{
	int ret = 0;
	REPORT(REPORT_INFO,"pause finish\n");

	// cardmanager->
	// caseManager->

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::stop (void)
{
	int ret = 0;
	REPORT(REPORT_INFO,"stop finish\n");
#if(PRINTLOG)
	fclose(fp);
	fp = NULL;
#endif
	// cardmanager->
	// caseManager->
	CCaseManager * pCaseManager = CCaseManager::getInstance();
	CStorageManager *pStorageManager = CStorageManager::getInstance();
	pStorageManager->stopStorage();
	pCaseManager->m_CaseRunningFlag = CCaseManager::TEST_STOP;
	ret = pCaseManager->stop();
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();

	pHardwareManager->stop();

#if(PRINTLOG)
	fclose(fp);
	fp = NULL;
#endif
	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::stopStep (const ::autoTest::StepInfoSeq & stepInfo)
{
	int ret = 0;
	int num = stepInfo.length();

	CCaseManager * pCaseManager = CCaseManager::getInstance();
	for(int i = 0; i < num; i++)
	{
		string caseID = stepInfo[i].caseID.in();
		string stepID = stepInfo[i].stepID.in();
		printf("-stepID %s\n",stepID.c_str());
		ret = pCaseManager->stopStep(caseID, stepID);
	}
	REPORT(REPORT_INFO,"stopStep finish\n");
	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::getCaseStatus (const ::autoTest::CaseStatusSeq & cardStatusDess,
		::autoTest::CaseStatusSeq_out cardStatusVals)
{
	int ret = 0;
	int num = cardStatusDess.length();
	//printf("num:%d\n", num);

	int caseStatus = -1;

	cardStatusVals = new autoTest::CaseStatusSeq();
	cardStatusVals->length(num);
	CCaseManager * pCaseManager = CCaseManager::getInstance();
	for(int i = 0; i < num; i++)
	{
		string caseID = cardStatusDess[i].caseID.in();
		//printf("  caseID:%s\n", caseID.c_str());

		caseStatus = pCaseManager->getCaseStatus(caseID);

		cardStatusVals[i].caseStepStatus = (autoTest::CaseStepStatusE)caseStatus;
		//printf("  caseStepStatus:%d\n", cardStatusVals[i].caseStepStatus);
	}

	//REPORT(REPORT_INFO,"getCaseStatus finish\n");

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}

autoTest::ReturnInfo CorbaImpl::getStepStatus (const ::autoTest::StepStatusSeq & stepStatusDess,
		::autoTest::StepStatusSeq_out stepStatusVals)
{
	int ret = 0;
	int num = stepStatusDess.length();
	//printf("num:%d\n", num);

	int stepStatus = -1;

	stepStatusVals = new autoTest::StepStatusSeq();
	stepStatusVals->length(num);
	CCaseManager * pCaseManager = CCaseManager::getInstance();
	for(int i = 0; i < num; i++)
	{
		string caseID = stepStatusDess[i].caseID.in();
		//printf("  caseID:%s, i :%d\n", caseID.c_str(), i);
		string stepID = stepStatusDess[i].stepID.in();
		//printf("  stepID:%s\n", stepID.c_str());
		TY_STEP_STATUS stepInfo;
		
		stepStatus = pCaseManager->getStepStatus(caseID, stepID, stepInfo);
		//REPORT(REPORT_INFO,"getStepStatus, num: %d, CaseID%s, stepID:%s, stepstatus:%d\n", num, caseID.c_str(),stepID.c_str(),stepInfo.caseStepStatus);
		//				stepStatusVals[i].msg = CORBA::string_dup(msgTmp);
		stepStatusVals[i].msg = CORBA::string_dup(stepInfo.msg.c_str());
		stepStatusVals[i].attachInfo = CORBA::string_dup(stepInfo.attachInfo.c_str());
		stepStatusVals[i].caseStepStatus = (autoTest::CaseStepStatusE)stepInfo.caseStepStatus;
		//printf("  caseStepStatus:%d\n", stepStatusVals[i].caseStepStatus);
	}

	//REPORT(REPORT_INFO,"getStepStatus finish\n");

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}

autoTest::ReturnInfo CorbaImpl::getSubStepStatus(const ::autoTest::SubStepStatusSeq & subStepStatusDess,
		::autoTest::SubStepStatusSeq_out subStepStatusVals)
{
	int ret = 0;
	int num = subStepStatusDess.length();
	//printf("num:%d\n", num);

	int subStepStatus = -1;

	subStepStatusVals = new autoTest::SubStepStatusSeq();
	subStepStatusVals->length(num);
	CCaseManager * pCaseManager = CCaseManager::getInstance();
	for(int i = 0; i < num; i++)
	{
		string caseID = subStepStatusDess[i].caseID.in();
		//printf("  caseID:%s\n", caseID.c_str());
		string stepID = subStepStatusDess[i].stepID.in();
		//printf("  stepID:%s\n", stepID.c_str());
		string subStepID = subStepStatusDess[i].subStepID.in();
		TY_STEP_STATUS stepInfo;

		subStepStatus = pCaseManager->getSubStepStatus(caseID, stepID, subStepID, stepInfo);
		//				stepStatusVals[i].msg = CORBA::string_dup(msgTmp);
		subStepStatusVals[i].msg = CORBA::string_dup(stepInfo.msg.c_str());
		subStepStatusVals[i].attachInfo = CORBA::string_dup(stepInfo.attachInfo.c_str());
		subStepStatusVals[i].caseStepStatus = (autoTest::CaseStepStatusE)stepInfo.caseStepStatus;
		//printf("  caseStepStatus:%d\n", stepStatusVals[i].caseStepStatus);
	}

	//REPORT(REPORT_INFO,"getStepStatus finish\n");

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}
autoTest::ReturnInfo CorbaImpl::getFieldData (const ::autoTest::FieldData & fieldDataDess,
		::autoTest::FieldData_out fieldDataVals)
{
	int ret = 0;
	//REPORT(REPORT_INFO,"getFieldDatag success\n");

	autoTest::ReturnInfo info;
	struct timeval ccurTime;
	gettimeofday(&ccurTime, NULL);
	info.sec = ccurTime.tv_sec;
	info.usec = ccurTime.tv_usec;
	info.status = ret;
	return info;
}

/*******************************************************************************
 * Function Name  : CorbaImpl::getCardsInfo()
 * Description    : getCardsInfo
 * Input          : None
 * Output         : CardsInfo_out info
 * Return         : ::core::ReturnInfo
 *******************************************************************************/
autoTest::ReturnInfo CorbaImpl::getCardsInfo(::autoTest::CardManagerSeq_out info)
{

	autoTest::ReturnInfo rInfo;
#if 1
	CHardwareManager * pManager = CHardwareManager::getInstance();
	//CServerManager *pManager;
	//pManager = CServerManager::getInstance();
	REPORT(REPORT_INFO, "GetCardInfo\n");
	int cardManagerNum = 0;


	TY_CARD_MANAGER_MAP::iterator iter = pManager->m_typeCardManagerMap.begin();
	for(; iter != pManager->m_typeCardManagerMap.end(); iter++)
	{
		cardManagerNum++;
	}
	autoTest::CardManagerPara *cardManagerParas = new autoTest::CardManagerPara[cardManagerNum];

	//autoTest::CardPara *cardManagerParas = new autoTest::CardPara[cardManagerNum];
	cardManagerNum = 0;
	for(iter = pManager->m_typeCardManagerMap.begin(); iter != pManager->m_typeCardManagerMap.end(); iter++, cardManagerNum++)
	{
		autoTest::CardPara *cardPara = new autoTest::CardPara[iter->second->m_cardNum];
		for(int cardNum = 0; cardNum < iter->second->m_cardNum; cardNum++)
		{
			printf("num:%d cardNum:%d \n", cardNum, iter->second->m_cardNum);
			//cardParas[i].devID = iter->second->m_devId;
			cardPara[cardNum].type = CORBA::string_dup(iter->second->m_pCard[cardNum]->m_cardType.c_str());
			cardPara[cardNum].cardNo = iter->second->m_pCard[cardNum]->m_cardSlot;
			cardPara[cardNum].cardName = CORBA::string_dup(iter->second->m_pCard[cardNum]->m_cardName.c_str());
			printf("type:%s cardNo:%d cardName:%s\n", iter->second->m_pCard[cardNum]->m_cardType.c_str(), iter->second->m_pCard[cardNum]->m_cardSlot,
					iter->second->m_pCard[cardNum]->m_cardName.c_str());
			autoTest::ChannelPara * chnPara = new autoTest::ChannelPara[iter->second->m_pCard[cardNum]->m_chnNum];
			for (unsigned int chn = 0; chn < iter->second->m_pCard[cardNum]->m_chnNum; chn++)
			{
				printf("chn:%d chnNum:%d \n", chn, iter->second->m_pCard[cardNum]->m_chnNum);
				chnPara[chn].channelID = iter->second->m_pCard[cardNum]->m_chnInfo[chn].id;
				chnPara[chn].direction = iter->second->m_pCard[cardNum]->m_chnInfo[chn].direction;
				chnPara[chn].mode = iter->second->m_pCard[cardNum]->m_chnInfo[chn].mode;
				autoTest::Parameter * chpara = new autoTest::Parameter[iter->second->m_pCard[cardNum]->m_chnInfo[chn].para_num];

				for (int paraNum = 0; paraNum < iter->second->m_pCard[cardNum]->m_chnInfo[chn].para_num; paraNum++)
				{
					printf("para:%d paraNum:%d \n", paraNum, iter->second->m_pCard[cardNum]->m_chnInfo[chn].para_num);
					chpara[paraNum].parameterName = CORBA::string_dup(iter->second->m_pCard[cardNum]->m_chnInfo[chn].chn_para[paraNum].name.c_str());
					chpara[paraNum].parameterID = iter->second->m_pCard[cardNum]->m_chnInfo[chn].chn_para[paraNum].id;
					chpara[paraNum].value = CORBA::string_dup(iter->second->m_pCard[cardNum]->m_chnInfo[chn].chn_para[paraNum].value.c_str());
					chpara[paraNum].defaultValue = CORBA::string_dup(iter->second->m_pCard[cardNum]->m_chnInfo[chn].chn_para[paraNum].defaultValue.c_str());
				}

				autoTest::ParameterSeq * chparas = new autoTest::ParameterSeq(
						iter->second->m_pCard[cardNum]->m_chnInfo[chn].para_num,
						iter->second->m_pCard[cardNum]->m_chnInfo[chn].para_num,
						chpara);
				chnPara[chn].paras = *chparas;
			}
			autoTest::ChannelParaSeq * chnParas = new autoTest::ChannelParaSeq(
					iter->second->m_pCard[cardNum]->m_chnNum,
					iter->second->m_pCard[cardNum]->m_chnNum, chnPara);
			cardPara[cardNum].chnParas = *chnParas;

		}
		autoTest::CardParaSeq * cardParas = new autoTest::CardParaSeq(
				iter->second->m_cardNum,
				iter->second->m_cardNum, cardPara);
		cardManagerParas[cardManagerNum].cardParas = *cardParas;
	}
	info = new autoTest::CardManagerSeq[cardManagerNum];
	info->replace(cardManagerNum, cardManagerNum,
			cardManagerParas);
	//info = new autoTest::CardParaSeq(pManager->m_config.cardNum);
	//info->replace(pManager->m_config.cardNum, pManager->m_config.cardNum,
	//    cardParas);


#endif
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	rInfo.sec = current_time.tv_sec;
	rInfo.usec = current_time.tv_usec;
	rInfo.status = 0;
	return rInfo;
}
/*
   autoTest::ReturnInfo CorbaImpl::getServerStatus (::autoTest::ServerStatus_out ServerStatusVal)
   {
   ServerStatusVal = new autoTest::ServerStatus();
   int ret = 0;
   ServerStatusVal->id = 1;
   ServerStatusVal->serverRunStatus = (autoTest::ServerRunStatusE)1;//SERVER_INITIAL;
//ServerStatusVal.cardNum = 1;
ServerStatusVal->cardStatus.length(1);
ServerStatusVal->cardStatus[0].status = 1;
ServerStatusVal->cardStatus[0].id = 1;
printf("id:%d serverRunStatus:%d\n", ServerStatusVal->id, ServerStatusVal->serverRunStatus);
printf("0-id:%d cardStatus:%d\n", ServerStatusVal->cardStatus[0].id, ServerStatusVal->cardStatus[0].status);
REPORT(REPORT_INFO,"getServerStatus success\n");

autoTest::ReturnInfo info;
struct timeval ccurTime;
gettimeofday(&ccurTime, NULL);
info.sec = ccurTime.tv_sec;
info.usec = ccurTime.tv_usec;
info.status = ret;
return info;
}
*/

autoTest::ReturnInfo CorbaImpl::getServerStatus(::autoTest::StatusInfo_out info)
{

	autoTest::ReturnInfo rInfo;
	info = new autoTest::StatusInfo();
	CHardwareManager * pManager = CHardwareManager::getInstance();
	int cardManagerNum = 0;
	TY_CARD_MANAGER_MAP::iterator iter = pManager->m_typeCardManagerMap.begin();
	for(; iter != pManager->m_typeCardManagerMap.end(); iter++)
	{
		cardManagerNum++;

		printf("cardManagerNum:%d\n", cardManagerNum);
	}

	/*	info->targetStatus.id = pManager->m_config.serviceID;
	//parse server status
	if ((pManager->m_status == SERVER_STATUS_INIT) || (pManager->m_status
	== SERVER_STATUS_SETROUTER) || (pManager->m_status
	== SERVER_STATUS_ONLINE) || (pManager->m_status == SERVER_STATUS_STOP))
	{
	info->targetStatus.status = STATUS_ONLINE;
	}
	else if ((pManager->m_status == SERVER_STATUS_RUNNING))
	{
	info->targetStatus.status = STATUS_RUNNING;
	}
	else if ((pManager->m_status == SERVER_STATUS_FATAL))
	{
	info->targetStatus.status = STATUS_EXCEPTION;
	}
	info->targetStatus.statusMsg = CORBA::string_dup(
	pManager->m_statusMsg.c_str());
	*/
	//printf("info->targetStatus.status:%d \n", info->targetStatus.status);

	info->cardStatusInfos.length(cardManagerNum);
	cardManagerNum = 0;

	for(iter = pManager->m_typeCardManagerMap.begin(); iter != pManager->m_typeCardManagerMap.end(); iter++, cardManagerNum++)
		//for (int i = 0; i < pManager->m_config.cardNum; i++)
	{ //i:device
		info->cardStatusInfos[cardManagerNum].cardID
			= iter->second->m_pCard[cardManagerNum]->m_cardSlot;

		info->cardStatusInfos[cardManagerNum].cardStatus.status
			= iter->second->statusTransfer(iter->second->m_pCard[cardManagerNum]->m_status);

		info->cardStatusInfos[cardManagerNum].cardStatus.statusMsg = CORBA::string_dup(
				iter->second->m_pCard[cardManagerNum]->m_statusMsg.c_str());

		info->cardStatusInfos[cardManagerNum].channelStatusInfos.length(
				iter->second->m_pCard[cardManagerNum]->m_chnNum);

		for (unsigned int chn = 0; chn
				< iter->second->m_pCard[cardManagerNum]->m_chnNum; chn++)
		{ //chn:chn_id-1
			info->cardStatusInfos[cardManagerNum].channelStatusInfos[chn].id
				= iter->second->m_pCard[cardManagerNum]->m_chnStatus[chn].id;
			/*     info->cardStatusInfos[i].channelStatusInfos[chn].status
			       = pManager->m_pCardManager->m_pCard[i]->m_chnStatus[chn].status;*/
			info->cardStatusInfos[cardManagerNum].channelStatusInfos[chn].status = iter->second->statusTransfer(
					iter->second->m_pCard[cardManagerNum]->m_chnStatus[chn].status);
			info->cardStatusInfos[cardManagerNum].channelStatusInfos[chn].statusMsg
				= CORBA::string_dup(iter->second->m_pCard[cardManagerNum]->m_chnStatus[chn].statusMsg.c_str());
		}
	}

	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = 0;
	return rInfo;
}

autoTest::ReturnInfo CorbaImpl::getExecutiveInfo (::autoTest::ExecutiveInfo_out info)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	info = new autoTest::ExecutiveInfo();
	TY_ExecutiveInfo executiveInfo;
	CCaseManager * pCaseManager = CCaseManager::getInstance();

	ret = pCaseManager->getExecutiveInfo(executiveInfo);
	if(ret != 0)
	{
		string test= "";

		info->preCaseID =  CORBA::string_dup("");//CORBA::string_dup(executiveInfo.preCaseID);
		info->preStepID =  CORBA::string_dup("");//CORBA::string_dup(executiveInfo.preStepID);
		info->caseRunTimes = 0;
		info->successNum = 0;
		info->failNum = 0;
		info->runStatus = (autoTest::CaseStepStatusE)executiveInfo.runStatus;

	}
	else
	{
		info->preCaseID =  CORBA::string_dup(executiveInfo.preCaseID);
		info->preStepID = CORBA::string_dup(executiveInfo.preStepID);
		info->caseRunTimes = executiveInfo.caseRunTimes;
		info->successNum = executiveInfo.successNum;
		info->failNum = executiveInfo.failNum;
		info->runStatus = (autoTest::CaseStepStatusE)executiveInfo.runStatus;
	}


	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = 0;
	return rInfo;
}

autoTest::ReturnInfo CorbaImpl::getRecordListNum(::CORBA::Long_out listNum)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	CStorageManager *pStorageManager = CStorageManager::getInstance();
	long num = 0;
	ret = pStorageManager->getRecordListNum((unsigned int&)num);
	printf("[%s:%s:%d]--> getRecordListNum:%ld\n", __FILE__, __FUNCTION__, __LINE__,num);
	listNum = num;
	
	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = ret;
	return rInfo;
}

autoTest::ReturnInfo CorbaImpl::getRecordList(::autoTest::RecordListSeq_out pList, ::CORBA::Long_out listLen)
{
	//int ret = 0;
	autoTest::ReturnInfo rInfo;
	unsigned int num = 0;
	CStorageManager *pStorageManager = CStorageManager::getInstance();
	pStorageManager->getRecordListNum(num);
	TY_RecordList *pRecordList = new TY_RecordList[num];

	pList = new autoTest::RecordListSeq();
	pList->length(num);
	pStorageManager->getRecordList(pRecordList, num);
	autoTest::RecordList recordList;
	for(int listNum=0; listNum<num; listNum++)
	{
		//printf("listNum%d\n", listNum);
		recordList.startTime.usec = pRecordList[listNum].startTime.tv_usec;
		recordList.startTime.sec = pRecordList[listNum].startTime.tv_sec;
		recordList.stopTime.usec = pRecordList[listNum].stopTime.tv_usec;
		recordList.stopTime.sec = pRecordList[listNum].stopTime.tv_sec;
		recordList.dataCount = pRecordList[listNum].dataCount;
		recordList.dataLength = pRecordList[listNum].dataLength;
		recordList.icdCount = pRecordList[listNum].icdCount;
		//printf("recordList.icdCount:%d\n", recordList.icdCount);
		recordList.name = CORBA::string_dup(pRecordList[listNum].name);
		recordList.path = CORBA::string_dup(pRecordList[listNum].path);

		//recordList.icdInfo = pRecordList[i].stopTime;
		autoTest::RecordInfo info;
		recordList.icdInfo.length(recordList.icdCount);
		for(int icdNum = 0; icdNum < recordList.icdCount; icdNum++)
		{
			/*printf("icd:%s start sec:%d usec:%d  stop sec:%d usec:%d", pRecordList[listNum].icdInfo[icdNum].icdID,
			  pRecordList[listNum].icdInfo[icdNum].startTime.tv_sec,
			  pRecordList[listNum].icdInfo[icdNum].startTime.tv_usec,
			  pRecordList[listNum].icdInfo[icdNum].stopTime.tv_sec,
			  pRecordList[listNum].icdInfo[icdNum].stopTime.tv_usec);
			  */
			info.dataCount = pRecordList[listNum].icdInfo[icdNum].dataCount;
			info.dataLength = pRecordList[listNum].icdInfo[icdNum].dataLength;
			info.startTime.sec = pRecordList[listNum].icdInfo[icdNum].startTime.tv_sec;
			info.startTime.usec = pRecordList[listNum].icdInfo[icdNum].startTime.tv_usec;
			info.stopTime.sec = pRecordList[listNum].icdInfo[icdNum].stopTime.tv_sec;
			info.stopTime.usec = pRecordList[listNum].icdInfo[icdNum].stopTime.tv_usec;
			info.icdID = CORBA::string_dup(pRecordList[listNum].icdInfo[icdNum].icdID);
			info.dataType = pRecordList[listNum].icdInfo[icdNum].storageType;
			//printf("icdID:%s. storageType:%d\n", pRecordList[listNum].icdInfo[icdNum].icdID, pRecordList[listNum].icdInfo[icdNum].storageType);
			recordList.icdInfo[icdNum] = info;
		}

		pList[listNum] = recordList;
	}
	listLen = num;
	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = 0;
	return rInfo;
}

autoTest::ReturnInfo CorbaImpl::getDataByTime(const char *storageName, const::autoTest::FieldIcdInfo &varInfo, const::autoTest::TY_HI_TIME &startTime,
		const::autoTest::TY_HI_TIME& stopTime, ::autoTest::StorageDataSeq_out pData, ::autoTest::HiTimeSeq_out pTime, CORBA::Long &maxCount)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	CStorageManager *pStorageManager = CStorageManager::getInstance();
	string strStorageName = storageName;
	string tmp, icdID;
	TY_FIELD_ICD_INFO icdInfo;
	icdID = varInfo.ICDID.in();
	sprintf(icdInfo.ICDID, "%s", icdID.c_str());
	tmp = varInfo.caseID.in();
	sprintf(icdInfo.caseID, "%s", tmp.c_str());
	icdInfo.cardID = varInfo.cardID;
	icdInfo.channelID = varInfo.channelID;
	icdInfo.fieldNum = varInfo.fieldNum;
	pData = new autoTest::StorageDataSeq();

	pTime = new autoTest::HiTimeSeq();

	for(int fieldNum = 0; fieldNum < icdInfo.fieldNum; fieldNum++)
	{
		tmp = varInfo.fieldID[fieldNum].in();
		sprintf(icdInfo.fieldID[fieldNum], "%s", tmp.c_str());
	}
	TY_STORAGE_TIME startTimeTmp;
	TY_STORAGE_TIME stopTimeTmp;
	int maxCountTmp = 0;
	double value = 0;
	startTimeTmp.tv_sec = startTime.sec;
	startTimeTmp.tv_usec = startTime.usec;
	stopTimeTmp.tv_sec = stopTime.sec;
	stopTimeTmp.tv_usec = stopTime.usec;
	TY_STORAGE_DATA * pStorageData = new TY_STORAGE_DATA[maxCount];
	maxCountTmp = maxCount;
	char filePath[256] = {0};
	sprintf(filePath, "%s_%d", icdInfo.ICDID, icdInfo.channelID-1);
	ret = pStorageManager->getDataByTime(strStorageName, filePath, &startTimeTmp, &stopTimeTmp, pStorageData, maxCountTmp);
	if(ret != -1)
	{
		maxCount = ret;
		pData->length(icdInfo.fieldNum);
		pTime->length(maxCount);
		for(int fieldCount = 0; fieldCount < icdInfo.fieldNum; fieldCount++)
		{
			pData[fieldCount].data.length(maxCount);
		}
		CICDManager *pICDManager = CICDManager::getInstance();
		CBlock *pBlock = pICDManager->getBlock(icdID);
		if(pBlock == NULL)
		{
			REPORT(REPORT_ERROR, "Get m_ICDID failed\n");
			ret = -1;
		}
		else
		{
			for(int count = 0; count < ret; count++)
			{
				pBlock->setRxData((unsigned char*)pStorageData[count].payload, (int&)(pStorageData[count].length));
				pTime[count].sec = pStorageData[count].sec;
				pTime[count].usec = pStorageData[count].usec;
				for(int fieldCount = 0; fieldCount < icdInfo.fieldNum; fieldCount++)
				{
					tmp = icdInfo.fieldID[fieldCount];
					BaseField* pField = pBlock->getField(tmp);
					if(pField == NULL)
					{
						REPORT(REPORT_ERROR, "Get pField failed\n");
						ret = -1;
					}
					pBlock->unpackRxICD();
					pField->getData(&value);
					pData[fieldCount].data[count]= value;
				}
			}
		}
	}


	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = 0;
	return rInfo;
}

/*
 *
 virtual ::autoTest::ReturnInfo getDataByIndex (const char * storageName,
 const ::autoTest::FieldIcdInfo & varInfo,
 ::CORBA::Long startIndex,
 ::CORBA::Long stopIndex,
 ::autoTest::StorageDataSeq_out pData,
 ::autoTest::HiTimeSeq_out pTime,
 ::CORBA::Long maxCount) = 0;
 */
void showPac(char* pData,int length)
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
autoTest::ReturnInfo CorbaImpl::getDataByIndex(const char *storageName, const ::autoTest::FieldIcdInfo &varInfo, const ::CORBA::Long startIndex,
		const ::CORBA::Long stopIndex, ::autoTest::StorageDataSeq_out pData, ::autoTest::HiTimeSeq_out pTime, CORBA::Long &maxCount)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	CStorageManager *pStorageManager = CStorageManager::getInstance();

	string strStorageName = storageName;
	string tmp, icdID;
	TY_FIELD_ICD_INFO icdInfo;
	icdID = varInfo.ICDID.in();
	sprintf(icdInfo.ICDID, "%s", icdID.c_str());
	tmp = varInfo.caseID.in();
	sprintf(icdInfo.caseID, "%s", tmp.c_str());
	icdInfo.cardID = varInfo.cardID;
	icdInfo.channelID = varInfo.channelID;
	icdInfo.fieldNum = varInfo.fieldNum;
	pData = new autoTest::StorageDataSeq();
	pTime = new autoTest::HiTimeSeq();

	for(int fieldNum = 0; fieldNum < icdInfo.fieldNum; fieldNum++)
	{
		//printf("\nID%s fieldNum:%d\n",tmp.c_str(), fieldNum);
		tmp = varInfo.fieldID[fieldNum].in();
		sprintf(icdInfo.fieldID[fieldNum], "%s", tmp.c_str());
	}
	long startIndexTmp = startIndex;
	long stopIndexTmp = stopIndex;

	int maxCountTmp = 0;
	double value = 0;


	TY_STORAGE_DATA * pStorageData = new TY_STORAGE_DATA[maxCount];
	maxCountTmp = maxCount;
	char filePath[256] = {0};
	sprintf(filePath, "%s_%d", icdInfo.ICDID, icdInfo.channelID-1);
	ret = pStorageManager->getDataByIndex(storageName, filePath, startIndexTmp, stopIndexTmp, pStorageData, maxCountTmp);

	if(ret != -1)
	{

		maxCount = ret;
		pData->length(icdInfo.fieldNum);
		pTime->length(maxCount);
		for(int fieldCount = 0; fieldCount < icdInfo.fieldNum; fieldCount++)
		{
			pData[fieldCount].data.length(maxCount);
		}

		CICDManager *pICDManager = CICDManager::getInstance();
		CBlock *pBlock = pICDManager->getBlock(icdID);
		if(pBlock == NULL)
		{
			REPORT(REPORT_ERROR, "Get m_ICDID failed\n");
			ret = -1;
		}
		else
		{

			for(int count = 0; count < ret; count++)
			{
				pBlock->setRxData((unsigned char*)pStorageData[count].payload, (int&)(pStorageData[count].length));
				//printf("\n count:%d\n", count);
				//showPac(pStorageData->payload, pStorageData->length);
				pTime[count].sec = pStorageData[count].sec;
				pTime[count].usec = pStorageData[count].usec;
				for(int fieldCount = 0; fieldCount < icdInfo.fieldNum; fieldCount++)
				{
					//printf("\n count:%d fieldCount:%d\n", count, fieldCount);
					tmp = icdInfo.fieldID[fieldCount];
					BaseField* pField = pBlock->getField(tmp);
					if(pField == NULL)
					{
						REPORT(REPORT_ERROR, "Get pField failed\n");
						ret = -1;
					}
					pBlock->unpackRxICD();
					pField->getData(&value);
					//printf("value:%lf\n",value);
					pData[fieldCount].data[count]= value;
				}
			}
		}
	}


	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = 0;
	return rInfo;
}

/*
 *     virtual ::autoTest::ReturnInfo getIndexByTime (const char * storageName,
 const ::autoTest::FieldIcdInfo & varInfo,
 const ::autoTest::TY_HI_TIME & startTime,
 const ::autoTest::TY_HI_TIME & stopTime,
 ::CORBA::Long_out startIndex,
 ::CORBA::Long_out stopIndex) = 0;
 */
autoTest::ReturnInfo CorbaImpl::getIndexByTime(const char *storageName, const ::autoTest::FieldIcdInfo &varInfo, const ::autoTest::TY_HI_TIME& startTime,
		const ::autoTest::TY_HI_TIME& stopTime,  ::CORBA::Long_out startIndex, ::CORBA::Long_out stopIndex)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	string strStorageName = storageName;
	string tmp, icdID;
	TY_FIELD_ICD_INFO icdInfo;
	icdID = varInfo.ICDID.in();
	sprintf(icdInfo.ICDID, "%s", icdID.c_str());
	tmp = varInfo.caseID.in();
	sprintf(icdInfo.caseID, "%s", tmp.c_str());
	icdInfo.cardID = varInfo.cardID;
	icdInfo.channelID = varInfo.channelID;
	icdInfo.fieldNum = varInfo.fieldNum;

	TY_STORAGE_TIME startTimeTmp;
	TY_STORAGE_TIME stopTimeTmp;

	startTimeTmp.tv_sec = startTime.sec;
	startTimeTmp.tv_usec = startTime.usec;
	stopTimeTmp.tv_sec = stopTime.sec;
	stopTimeTmp.tv_usec = stopTime.usec;


	long startIndexTmp = 0;
	long stopIndexTmp = 0;
	CStorageManager *pStorageManager = CStorageManager::getInstance();
	char filePath[256] = {0};
	sprintf(filePath, "%s_%d", icdInfo.ICDID, icdInfo.channelID-1);
	ret = pStorageManager->getIndexByTime(strStorageName, filePath, &startTimeTmp, &stopTimeTmp, (int*)&startIndexTmp, (int*)&stopIndexTmp);
	startIndex = startIndexTmp;
	stopIndex = stopIndexTmp;
	//getIndexByTime(string storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME *startTime, TY_STORAGE_TIME *stopTime,int *startIndex,int *stopIndex);
	printf("startIndexTmp:%d stopIndexTmp:%d\n", startIndexTmp, stopIndexTmp);
	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = ret;
	return rInfo;
}


autoTest::ReturnInfo CorbaImpl::getModelDataByTime(const char *storageName, const ::autoTest::ModelInfo &varInfo, const ::autoTest::TY_HI_TIME &startTime,
		const ::autoTest::TY_HI_TIME& stopTime, ::autoTest::StorageDataSeq_out pData, ::autoTest::HiTimeSeq_out pTime, CORBA::Long &maxCount)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	CStorageManager *pStorageManager = CStorageManager::getInstance();

	string strStorageName = storageName;
	string tmp, modelName;
	TY_MODEL_VAR_INFO info;
	modelName = varInfo.modelName.in();
	sprintf(info.modelName, "%s", modelName.c_str());
	info.varNum = varInfo.varNum;

	TY_STORAGE_TIME startTimeTmp;
	TY_STORAGE_TIME stopTimeTmp;

	startTimeTmp.tv_sec = startTime.sec;
	startTimeTmp.tv_usec = startTime.usec;
	stopTimeTmp.tv_sec = stopTime.sec;
	stopTimeTmp.tv_usec = stopTime.usec;
	pData = new autoTest::StorageDataSeq();
	pTime = new autoTest::HiTimeSeq();

	for(int varNum = 0; varNum < info.varNum; varNum++)
	{
		tmp = varInfo.varName[varNum].in();
		sprintf(info.varName[varNum], "%s", tmp.c_str());
	}


	int maxCountTmp = 0;
	double value = 0;


	TY_STORAGE_DATA * pStorageData = new TY_STORAGE_DATA[maxCount];
	maxCountTmp = maxCount;
	char filePath[256] = {0};
	sprintf(filePath, "%s", info.modelName);

	ret = pStorageManager->getDataByTime(strStorageName, filePath, &startTimeTmp, &stopTimeTmp, pStorageData, maxCountTmp);

	if(ret != -1)
	{

		maxCount = ret;
		pData->length(info.varNum);
		pTime->length(maxCount);

		for(int varNum = 0; varNum < info.varNum; varNum++)
		{
			pData[varNum].data.length(maxCount);
		}
		int varNum = pStorageData[0].length / sizeof(TY_VAR_DATA);
		TY_VAR_DATA *varData = (TY_VAR_DATA*)new TY_VAR_DATA[varNum];

		for(int dataCount = 0; dataCount < ret; dataCount++)
		{
			memcpy(varData, pStorageData[dataCount].payload,  pStorageData[dataCount].length);

			pTime[dataCount].sec = pStorageData[dataCount].sec;
			pTime[dataCount].usec = pStorageData[dataCount].usec;

			for(int varCount = 0; varCount < varNum; varCount++)
			{
				for(int count = 0; count < varNum; count++)
				{
					if(strcmp(info.varName[varCount], varData[count].fullPath ))
					{
						printf("fullPath:%s, value:lf\n", varData[count].fullPath, varData[count].value);
						memcpy(&value, varData[count].value, sizeof(double));
						pData[varCount].data[dataCount]= value;
						break;
					}
				}
			}

		}

	}

	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = 0;
	return rInfo;
}

autoTest::ReturnInfo CorbaImpl::getModelDataByIndex(const char *storageName, const ::autoTest::ModelInfo &varInfo, ::CORBA::Long startIndex, ::CORBA::Long stopIndex,
		::autoTest::StorageDataSeq_out pData, ::autoTest::HiTimeSeq_out pTime, CORBA::Long &maxCount)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	CStorageManager *pStorageManager = CStorageManager::getInstance();

	string strStorageName = storageName;
	string tmp, modelName;
	TY_MODEL_VAR_INFO info;
	modelName = varInfo.modelName.in();
	sprintf(info.modelName, "%s", modelName.c_str());
	info.varNum = varInfo.varNum;

	pData = new autoTest::StorageDataSeq();
	pTime = new autoTest::HiTimeSeq();

	for(int varNum = 0; varNum < info.varNum; varNum++)
	{
		tmp = varInfo.varName[varNum].in();
		sprintf(info.varName[varNum], "%s", tmp.c_str());
	}
	long startIndexTmp = startIndex;
	long stopIndexTmp = stopIndex;

	int maxCountTmp = 0;
	double value = 0;

	TY_STORAGE_DATA * pStorageData = new TY_STORAGE_DATA[maxCount];
	maxCountTmp = maxCount;
	char filePath[256] = {0};
	sprintf(filePath, "%s", info.modelName);
	string varNameStr(filePath);
	ret = pStorageManager->getDataByIndex(storageName, filePath, startIndexTmp, stopIndexTmp, pStorageData, maxCountTmp);

	if(ret != -1)
	{
		string strModelName = varNameStr.substr(0,varNameStr.find_last_of("_"));
		maxCount = ret;
		pData->length(info.varNum);
		pTime->length(maxCount);

		for(int varNum = 0; varNum < info.varNum; varNum++)
		{
			pData[varNum].data.length(maxCount);
		}

		// >> modify by wc 20231013
		
		for(int dataCount = 0; dataCount < ret; dataCount++)
		{
			int varNum = pStorageData[0].length / sizeof(double);
			//memcpy(varData, pStorageData[dataCount].payload,  pStorageData[dataCount].length);
			char* pPaylod = pStorageData[dataCount].payload;// add .

			pTime[dataCount].sec = pStorageData[dataCount].sec;
			pTime[dataCount].usec = pStorageData[dataCount].usec;
			//REPORT(REPORT_WARNING, "------------modelName:%s, xiugaihou modelName:%s, maxCountTmp:%d\n",modelName.c_str(), strModelName.c_str(),maxCountTmp);
			TY_MODEL_STORAGE_MAP::iterator iterModel = pStorageManager->m_modelStorageMap.find(strModelName);
			if(iterModel != pStorageManager->m_modelStorageMap.end())
			{
				string strInput = "input";
				string strOutput = "output";
				//printf("---11111111----------------------------------maxCount:%d, filePath:%s\n", maxCount, filePath);
				if(varNameStr.find(strOutput, 0) < varNameStr.length())
				{
					for(TY_STORAGE_MODEL_VAR_MAP::iterator iterVar = iterModel->second.outputInfo.begin();
							iterVar != iterModel->second.outputInfo.end(); iterVar++)
					{
						char tmpValue[8] = {0};
						memcpy(tmpValue, pPaylod, sizeof(double));
						memcpy(&iterVar->second, tmpValue, sizeof(double));
						pPaylod += sizeof(double);
						//REPORT(REPORT_WARNING, "------------varName:%s, value:%lf\n",iterVar->first.c_str(), iterVar->second);
					}
				}
				else if(varNameStr.find(strInput, 0) < varNameStr.length())
				{
					for(TY_STORAGE_MODEL_VAR_MAP::iterator iterVar = iterModel->second.inputInfo.begin();
							iterVar != iterModel->second.inputInfo.end(); iterVar++)
					{
						char tmpValue[8] = {0};
						memcpy(tmpValue, pPaylod, sizeof(double));
						memcpy(&iterVar->second, tmpValue, sizeof(double));
						pPaylod += sizeof(double);
				//		REPORT(REPORT_WARNING, "------------varName:%s, value:%lf\n",iterVar->first.c_str(), iterVar->second);
					}

				}


			}

			for(int varCount = 0; varCount < varNum; varCount++)
			{
				string strVarName(info.varName[varCount]);

				TY_STORAGE_MODEL_VAR_MAP::iterator iterVarOutIn = iterModel->second.outputInfo.find(strVarName);

				if(iterVarOutIn != iterModel->second.outputInfo.end())
				{
					value = iterVarOutIn->second;
					pData[varCount].data[dataCount]= value;
					//REPORT(REPORT_WARNING, "------------varNum:%d, info.varName[%d]:%s, value:%lf\n", varNum,varCount, info.varName[varCount],value);
				}

				iterVarOutIn = iterModel->second.inputInfo.find(strVarName);

				if(iterVarOutIn != iterModel->second.inputInfo.end())
				{
					value = iterVarOutIn->second;
					pData[varCount].data[dataCount]= value;
					//REPORT(REPORT_WARNING, "------------varNum:%d, info.varName[%d]:%s, value:%lf\n", varNum,varCount, info.varName[varCount],value);
				}
				/*

				   if(strcmp(info.varName[varCount], varData[count].fullPath ) == 0)
				   {
				//REPORT(REPORT_ERROR, "info.varName[varCount]:%s, varData[count].fullPath:%s,dataCount:%d, value:%lf\n",
				//info.varName[varCount],varData[count].fullPath,dataCount,varData[count].value);


				memcpy(&value, varData[count].value, sizeof(double));
				pData[varCount].data[dataCount]= value;
#if PRINTFSTORAGE
fprintf(fp, "info.varName[varCount]:%s,dataCount:%d, value:%lf\n",
info.varName[varCount],dataCount,value);
#endif
break;
}
*/	
}

}
//<< wc
#if 0
int varNum = pStorageData[0].length / sizeof(TY_VAR_DATA);
TY_VAR_DATA *varData = (TY_VAR_DATA*)new TY_VAR_DATA[varNum];

for(int dataCount = 0; dataCount < ret; dataCount++)
{

	memcpy(varData, pStorageData[dataCount].payload,  pStorageData[dataCount].length);
#if PRINTFSTORAGE
	for(int i = 0; i< varNum; i++)
	{
		fprintf(fp, "varData[%d].fullPath:%s, varNum:%d\n",i,
				varData[i].fullPath,varNum);
	}
#endif
	pTime[dataCount].sec = pStorageData[dataCount].sec;
	pTime[dataCount].usec = pStorageData[dataCount].usec;
	for(int varCount = 0; varCount < varNum; varCount++)
	{
		for(int count = 0; count < varNum; count++)
		{

			if(strcmp(info.varName[varCount], varData[count].fullPath ) == 0)
			{
				//REPORT(REPORT_ERROR, "info.varName[varCount]:%s, varData[count].fullPath:%s,dataCount:%d, value:%lf\n",
				//info.varName[varCount],varData[count].fullPath,dataCount,varData[count].value);


				memcpy(&value, varData[count].value, sizeof(double));
				pData[varCount].data[dataCount]= value;
#if 0
				fprintf(fp, "info.varName[varCount]:%s,dataCount:%d, value:%lf\n",
						info.varName[varCount],dataCount,value);
#endif
				break;
			}
		}
	}

}
#endif

}


struct timeval now_time;
gettimeofday(&now_time, NULL);
rInfo.sec = now_time.tv_sec;
rInfo.usec = now_time.tv_usec;
rInfo.status = 0;
return rInfo;
}


autoTest::ReturnInfo CorbaImpl::getModelIndexByTime(const char *storageName, const ::autoTest::ModelInfo &varInfo, const ::autoTest::TY_HI_TIME& startTime, const ::autoTest::TY_HI_TIME& stopTime,
		::CORBA::Long_out startIndex, ::CORBA::Long_out stopIndex)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	string strStorageName = storageName;
	string tmp, modelName;
	TY_MODEL_VAR_INFO info;
	modelName = varInfo.modelName.in();
	sprintf(info.modelName, "%s", modelName.c_str());
	info.varNum = varInfo.varNum;


	TY_STORAGE_TIME startTimeTmp;
	TY_STORAGE_TIME stopTimeTmp;

	startTimeTmp.tv_sec = startTime.sec;
	startTimeTmp.tv_usec = startTime.usec;
	stopTimeTmp.tv_sec = stopTime.sec;
	stopTimeTmp.tv_usec = stopTime.usec;


	long startIndexTmp = 0;
	long stopIndexTmp = 0;
	CStorageManager *pStorageManager = CStorageManager::getInstance();
	char filePath[256] = {0};
	sprintf(filePath, "%s", info.modelName);
	//REPORT(REPORT_ERROR, "-----------1----filePath:%s, varnum:%d\n", filePath,info.varNum);
	ret = pStorageManager->getIndexByTime(strStorageName, filePath, &startTimeTmp, &stopTimeTmp, (int*)&startIndexTmp, (int*)&stopIndexTmp);
	startIndex = startIndexTmp;
	stopIndex = stopIndexTmp;
	//getIndexByTime(string storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME *startTime, TY_STORAGE_TIME *stopTime,int *startIndex,int *stopIndex);
	printf("startIndexTmp:%d stopIndexTmp:%d\n", startIndexTmp, stopIndexTmp);
	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::deleteStorageRecord(const char *storageName)
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	CStorageManager *pStorageManager = CStorageManager::getInstance();

	ret = pStorageManager->deleteStorageRecord(storageName);

	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::deleteAllStorageRecord()
{
	int ret = 0;
	autoTest::ReturnInfo rInfo;
	CStorageManager *pStorageManager = CStorageManager::getInstance();

	ret = pStorageManager->deleteAllStorageRecord();

	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = ret;
	return rInfo;
}
autoTest::ReturnInfo CorbaImpl::getIpInfo(CORBA::String_out ipInfo)
{	
	printf("enter autoTest::ReturnInfo CorbaImpl::getIpInfo(CORBA::String_out ipInfo)\n");
	autoTest::ReturnInfo rInfo;

	TiXmlDocument *pXmlManager = NULL;
	TiXmlNode *rootNode = NULL;
	TiXmlElement *pBusElement = NULL;
	int ret = 0;
	string ip = "";

	char filePath[256];
	getServerPath(filePath);

 	string hardXmlPath(filePath);
    hardXmlPath.append("Hardware.xml");

	pXmlManager = load_xml(hardXmlPath.c_str(), FROM_FILE);
	if (pXmlManager == NULL)
	{
		REPORT(REPORT_ERROR, "Xml is can't be parse\n");
		ret = -1;
	}
	else
	{
		rootNode = pXmlManager->FirstChild("hardware");
	}
	if (rootNode == NULL)
	{
		REPORT(REPORT_ERROR, "Hardware.xml can't find the node of hardware\n");
		ret = -1;
	}
	else
	{
		pBusElement = rootNode->FirstChildElement();
		if (pBusElement == NULL)
		{
			REPORT(REPORT_ERROR, "Hardware.xml can't find the node of bus\n");
			ret = -1;
		}
		for (pBusElement = rootNode->FirstChildElement(); pBusElement; pBusElement = pBusElement->NextSiblingElement())
		{
			bool bRet = GetValue(pBusElement, "IP", ip);
			if (!bRet)
			{
				REPORT(REPORT_ERROR, "Get IP failed.\n");
				ret = -1;
			}
			printf("ip:%s\n", ip.c_str());
		}
	}

	int len = strlen(ip.c_str());
	printf("len:%d\n", len);
	ipInfo = CORBA::string_dup(ip.c_str());
	//REPORT(REPORT_INFO, "getIpInfo ip:%s ip:%s\n", (char *)ipInfo, ip);
	
	struct timeval now_time;
	gettimeofday(&now_time, NULL);
	rInfo.sec = now_time.tv_sec;
	rInfo.usec = now_time.tv_usec;
	rInfo.status = ret;
	return rInfo;
}
