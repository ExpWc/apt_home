#include "CStorageManager.h"
#include "CStorageRule.h"
#include "DataType.h"
#include "Ini.h"

#include "CStorageRule.h"
#include "parse_xml_api.h"
#include "hr_time.h"
#include "BaseSerialCard.h"
#include <sys/time.h>
#include <unistd.h>

void storageData(CStorageManager *pStorageManager, TY_STORAGE_DATA *pData, TY_STORAGE_422_DATA* pStorageData, TY_HR_422_MSG pRecvPac, unsigned int src);
int main()
{
	struct timeval tv;
	unsigned int recordNum = 0;
	string storeContent = "/home/V1.0.0/HiStorageManager/StorageConfig.xml";
	CStorageManager *pStorageManager = CStorageManager::getInstance();
	pStorageManager->getRecordListNum(recordNum);
	TY_RecordList list;
	pStorageManager->getRecordList(&list, recordNum);
	int startIndex = 0;
	int stopIndex = 0;
	string name = list.name;
	int ret = 0;
	pStorageManager->getIndexByTime(name, list.icdInfo[0].icdID, &(list.icdInfo[0].startTime), &(list.icdInfo[0].stopTime), &startIndex, &stopIndex);

	TY_STORAGE_DATA *pData = (TY_STORAGE_DATA*) malloc(sizeof(TY_STORAGE_DATA) * (stopIndex - startIndex));

	ret = pStorageManager->getDataByIndex(name, list.icdInfo[0].icdID, startIndex, stopIndex, pData, stopIndex - startIndex);
	if(ret >= 0)
	{
		int varNum = pData[0].length / sizeof(TY_VAR_DATA);
		TY_VAR_DATA *varData = (TY_VAR_DATA*)new TY_VAR_DATA[varNum];

		for(int count = 0; count < ret; count++)
		{
			memcpy(varData, pData[count].payload,  pData[0].length);
			for(int varCount = 0; varCount < varNum; varCount++)
			{

				printf("%s:%lf\n", varData[varCount].fullPath, varData[varCount].value);
			}


		}
		delete[] varData;
	}

	free(pData);

	/*printf("recordNum:%d\n", recordNum);
	TY_STORAGE_DATA *pData = (TY_STORAGE_DATA*) malloc(sizeof(TY_STORAGE_DATA));

	memset(pData, 0, sizeof(TY_STORAGE_DATA));
	TY_STORAGE_422_DATA* pStorageData = (TY_STORAGE_422_DATA*) malloc(sizeof(TY_STORAGE_422_DATA));

	TY_HR_422_MSG pRecvPac = {0};

	//pStorageData->payload = (char*) malloc(sizeof(pRecvPac.data));
	strcpy(pRecvPac.uuid, "456");
	//pRecvPac.cardId = 3;
	pRecvPac.chnId = 1;
	long tmp = 0xAA55AA55;
	memcpy(pRecvPac.headValue, &tmp, 4);
	pRecvPac.headBytes = 4;
	tmp = 0x55AA55AA;
	memcpy(pRecvPac.tailValue, &tmp, 4);
	pRecvPac.tailBytes = 4;

	for(int i = 0; i<1000; i++)
	{
		pRecvPac.data[i] = i;
	}
	pRecvPac.len = 1000;
	pStorageManager->initStorage((char*)storeContent.c_str());
	pStorageManager->startStorage();
	sleep(1);
	for(int i = 0; i<1000; i++)
	{
		gettimeofday(&tv, NULL);
		strcpy(pRecvPac.uuid, "456");
		pRecvPac.sec = tv.tv_sec;
		pRecvPac.usec = tv.tv_usec;
		storageData(pStorageManager, pData, pStorageData, pRecvPac, 1);
		strcpy(pRecvPac.uuid, "789");
		pRecvPac.sec = tv.tv_sec;
		pRecvPac.usec = tv.tv_usec;
		storageData(pStorageManager, pData, pStorageData, pRecvPac, 1);
		usleep(100);
		if(i % 1000 == 0)
		{
			printf("times:%d\n", i);
		}
	}

	pStorageManager->stopStorage();
	//int CStorageManager::getDataByIndex(string storageName, TY_FIELD_ICD_INFO varInfo,
		//	unsigned int startIndex, unsigned int stopIndex, TY_STORAGE_DATA* pData, int maxCount, int flag)
	//getDataByTime(string storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME *startTime, TY_STORAGE_TIME *stopTime,
	//									TY_STORAGE_DATA* pData, int maxCount, int flag)

/*
	TY_FIELD_ICD_INFO varInfo = {0};
	strcpy(varInfo.ICDID, "9df6c392-d69c-4e0d-8d96-304e75b0a528");

	varInfo.channelID = 1;

	string storageName = "test_b31a69f0-d9d5-4cb3-ae9a-21d019b4d7da_2022.08.11_11:25:59";
	TY_STORAGE_TIME startTime;
	TY_STORAGE_TIME stopTime;
	startTime.tv_sec = 1662895569;
	startTime.tv_usec = 497221;
	stopTime.tv_sec = 1662895576;
	stopTime.tv_usec = 650033;
	int startIndex = 0;
	int stopIndex = 0;
	TY_RecordList pList;
	unsigned int listLen;
	//pStorageManager->getRecordList(&pList, listLen);
	pStorageManager->getIndexByTime(storageName, varInfo, &startTime, &stopTime, &startIndex, &stopIndex);
	/*TY_STORAGE_DATA* dataBuffer = (TY_STORAGE_DATA*)malloc(sizeof(TY_STORAGE_DATA) * 100);
	pStorageManager->getDataByIndex(storageName, varInfo, 0, 20000, dataBuffer, 100, 1);

	//pStorageManager->deleteStorageRecord("test-2022.4.11_15:46:33");
	pStorageManager->getRecordListNum(recordNum);
	free(dataBuffer);
*/


	//free(pStorageData);


	return 0;
}
void storageData(CStorageManager *pStorageManager, TY_STORAGE_DATA *pData, TY_STORAGE_422_DATA* pStorageData, TY_HR_422_MSG pRecvPac, unsigned int src)
{


	strcpy(pData->uuid, pRecvPac.uuid);
	//pData->cardId = pRecvPac.cardId;
	pData->chnId = pRecvPac.chnId;
	pData->src = src;
	pData->chnId = pRecvPac.chnId;
	pData->sec = pRecvPac.sec;
	pData->usec = pRecvPac.usec;

	strcpy(pStorageData->uuid, pRecvPac.uuid);
	pStorageData->sec = pRecvPac.sec;
	pStorageData->usec = pRecvPac.usec;
	//pStorageData->cardId = pRecvPac.cardId;
	pStorageData->chnId = pRecvPac.chnId;
	memcpy(pStorageData->frame_begin, pRecvPac.headValue, 4);
	pStorageData->frame_begin_length = pRecvPac.headBytes;
	memcpy(pStorageData->frame_end, pRecvPac.tailValue, 4);
	pStorageData->frame_end_length = pRecvPac.tailBytes;
	pStorageData->length = pRecvPac.len;
	memcpy(pData->payload, pRecvPac.data, pStorageData->length);
	pData->length = sizeof(TY_STORAGE_422_DATA) + pStorageData->length;
	pStorageManager->insertData(pData);
}
