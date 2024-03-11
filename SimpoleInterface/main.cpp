#include "SimpoleInterfaceLinux.h"
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include <cpprest/json.h>
#include <map>
//#include "SimpoleInterfaceLinux.h"

using namespace std;

const char addr[256] = "http://192.168.1.120:10031";
char firstModelStorageVar[256] = "demo1/Shared Memory Set1/Shared Memory Sett";
char firstModelStorageSignal[256] = "demo1/Shared Memory Get1/SharedMemoryGet1";
char firstModelVarPath[256] = "";
char secondModelVarPath[256] = "";
void ShowCmd();

int GetCommandIndex()
{
	ShowCmd();
	static char cmdIndexStr[80]; /* User input                         */
	if (fgets(cmdIndexStr, sizeof(cmdIndexStr), stdin) != (char *)NULL)
	{
		return atoi(cmdIndexStr);
	}
	return -1;
}

int Login()
{
	//login>>
	int ret = login(addr);
	if (ret == 0)
	{
		printf("Login sucess.\n");
	}
	else
	{
		printf("Login failed  ret=%d\n", ret);
	}
	return ret;
}

int GetModelList()
{
	int ret = 0;

	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr); //2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}
	printf("ModelList:\n");
	for (int i = 0; i < modelList.modelNumber; i++)
	{
		printf("%d modelName:%s UUID:%s\n", i + 1, modelList.modelName[i], modelList.uuid[i]);
	}
	printf("\n");

	return 0;
}

int GetModelStatus()
{

	int modelNum = 1;


	int ret = getModelListNum(modelNum, addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr);//2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}

	char(*modelName)[128] = new char[modelNum][128];
	for (int i = 0; i < modelNum; i++)
	{
		//strcpy(modelName[i], "demo1");
		strcpy(modelName[i], modelList.modelName[i]);
	}

	TY_MODEL_STATUS modelStatus;
	for (int i = 0; i < modelNum; i++)
	{
		ret = getModelStatus(modelName[i], &modelStatus, addr);
		if (ret < 0)
		{
			printf("Get model status failed.\n");
			return -1;
		}

		printf("\tmodelName:%s CpuIndex %d frameCount %ld \n\tavgRunTime %ld maxRunTime %ld runTime %ld delayTime %ld \n\tmaxDelayTime %ld \
	overrunTimes %ld period %ld modelRunStatus %ld PID %ld\n\n",
			   modelStatus.modelName, modelStatus.cpuIndex,
			   modelStatus.frameCount, modelStatus.avgRunTime, modelStatus.maxRunTime, modelStatus.runTime,
			   modelStatus.delayTime, modelStatus.maxDelayTime, modelStatus.overrunTimes, modelStatus.period,
			   modelStatus.modelRunStatus, modelStatus.pid);
	}

	return 0;
}

int GetResourceInfo()
{
	TY_SIM_RESCOURCE_INFO simRes;
	memset(&simRes, 0, sizeof(TY_SIM_RESCOURCE_INFO));
	int ret = getResourceInfo(&simRes, addr); //2.5
	if (ret < 0)
	{
		printf("Get resource infp failed.\n");
		return -1;
	}
	printf("modelNum:%d timeResNum:%d timeResNames:%s  cpuNum:%d\n\n", simRes.modelNum, simRes.timeResNum, simRes.timeResNames[0], simRes.cpuNum);
	for (int i = 0; i < simRes.modelNum; i++)
	{
		printf("modelNames:%s \n", simRes.modelNames[i]);
	}

	return 0;
}

int LoadModel()
{
	//loadModel>>
	TY_SIM_CONFIG simConfig = {0};

	int modelNum;
#if 1
	int ret = getModelListNum(modelNum, addr); //2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}

	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr); //2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}
#endif
	modelNum = modelList.modelNumber;
	//modelNum = 1;
	simConfig.modelNum = modelNum;
	simConfig.pModelConfigs = new TY_MODEL_CONFIG[simConfig.modelNum];
	sprintf(simConfig.storageName, "%s", "test");
	sprintf(simConfig.timeResName, "%s", "SoftTime");

	
	for (int i = 0; i < modelNum; i++)
	{
		simConfig.pModelConfigs[i].storageVarNum = 1;
		simConfig.pModelConfigs[i].storageSignalNum = 1;
		simConfig.pModelConfigs[i].cpuIndex = 1;
		//sprintf(simConfig.pModelConfigs[i].modelName, "demo1");
		sprintf(simConfig.pModelConfigs[i].modelName, "%s", modelList.modelName[i]);
		//sprintf_s(simConfig.pModelConfigs[i].uuid, "%s", modelList.uuid[i]);
		simConfig.pModelConfigs[i].period = 1000;
		sprintf(simConfig.pModelConfigs[i].storageVar[0], "%s", firstModelStorageVar);
		sprintf(simConfig.pModelConfigs[i].storageSignal[0], "%s", firstModelStorageSignal);
	}
	if ((simConfig.pModelConfigs[0].storageVarNum | simConfig.pModelConfigs[0].storageSignalNum) != 0)
		simConfig.stored = true;

	ret = loadModel(simConfig, addr);
	if (ret < 0)
	{
		printf("Load model failed.\n");
	}
	else
	{
		printf("Load model sucess.\n");
	}
	delete[] simConfig.pModelConfigs;

	return 0;
}

int StartModel()
{
	TY_MODELNAME_VALUE modelNameValue = {0};

	int modelNum = 1;
#if 1
	int ret = getModelListNum(modelNum, addr); //2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr); //2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}
#endif
	modelNameValue.modelNumber = modelNum;
	for (int i = 0; i < modelNum; i++)
	{
		//sprintf(modelNameValue.modelName[i], "S1");
		printf("modelNameValue.modelName[i]:%s", modelList.modelName[i]);
		sprintf(modelNameValue.modelName[i], "%s", modelList.modelName[i]);
		//strcpy(&modelNameValue.modelName[i]， &modelList.modelName[i]);
	}

	ret = startModel(modelNameValue, addr);
	if (ret != 0)
	{
		printf("Start model failed. %d\n", ret);
	}
	else
	{
		printf("Start model success.\n");
	}

	return ret;
}

int StopModel()
{
	TY_MODELNAME_VALUE modelNameValue = {0};

	int modelNum = 1;
#if 1
	int ret = getModelListNum(modelNum, addr); //2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr); //2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}
#endif
	modelNameValue.modelNumber = modelNum;
	for (int i = 0; i < modelNum; i++)
	{
		//sprintf(modelNameValue.modelName[i], "demo1");
		sprintf(modelNameValue.modelName[i], "%s", modelList.modelName[i]);
	}

	ret = stopModel(modelNameValue, addr);
	if (ret != 0)
	{
		printf("Stop model failed.   %d\n", ret);
	}
	else
	{
		printf("Stop model success.\n");
	}
	return ret;
}

int PauseModel()
{
	TY_MODELNAME_VALUE modelNameValue;
	int modelNum = 1;
#if 1
	int ret = getModelListNum(modelNum, addr); //2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr); //2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}
#endif
	modelNameValue.modelNumber = modelNum;
	for (int i = 0; i < modelNum; i++)
	{
		//sprintf(modelNameValue.modelName[i], "demo1");
		sprintf(modelNameValue.modelName[i], "%s", modelList.modelName[i]);
	}
	ret = pauseModel(modelNameValue, addr);
	if (ret != 0)
	{
		printf("Pause model failed. %d\n", ret);
	}
	else
	{
		printf("Pause model success.\n");
	}

	return ret;
}


int GetVariableList()
{
	TY_MODELNAME_VALUE modelNameValue;

	memset(&modelNameValue, 0, sizeof(TY_MODELNAME_VALUE));
	int modelNum;
#if 1
	int ret = getModelListNum(modelNum, addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr);//2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}
#endif
	modelNameValue.modelNumber = modelNum;

	for (int i = 0; i < modelNum; i++)
	{
		//sprintf(modelNameValue.modelName[i], "demo1");
		sprintf(modelNameValue.modelName[i], "%s", modelList.modelName[i]);
	}

	int num = 0;
	ret = getVariableListNum(modelNameValue, num, addr);
	if (ret != 0)
	{
		printf("Get variables num failed.\n");
		return ret;
	}

	TY_VAR_DES *paraDes = new TY_VAR_DES[num];
	ret = getVariableList(modelNameValue, paraDes, num, addr);
	if (ret != 0)
	{
		printf("Get variables failed.\n");
		delete[]paraDes;
		return ret;
	}
	for (int i = 0; i < num; i++)
	{
		printf("%d Path:%s \n\tName:%s dataType:%d\n  Uuid:%s\n varType:%d\n", i + 1, paraDes[i].fullPath, paraDes[i].paraName,paraDes[i].dataType, paraDes[i].uuid, paraDes[i].varType);
	}
	delete[]paraDes;
	return ret;
}

int GetRamSampleInTimeWindow()
{

	//>> time
	struct timeval time_1;
	struct timeval time_2;
	//<<
	int ret = 0;
	int num = 1;
	TY_VAR_VALUE *pList;
#if 1
	TY_MODELNAME_VALUE modelNameValue;
	memset(&modelNameValue, 0, sizeof(TY_MODELNAME_VALUE));
	int modelNum;
#if 1
	ret = getModelListNum(modelNum, addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr);//2.2
	if (ret < 0)
	{
		printf("Get model list failed.\n");
		return -1;
	}
#endif
	modelNameValue.modelNumber = modelNum;

	for (int i = 0; i < modelNum; i++)
	{
		//sprintf(modelNameValue.modelName[i], "demo1");
		sprintf(modelNameValue.modelName[i], "%s", modelList.modelName[i]);
	}

	//int num = 0;
	ret = getVariableListNum(modelNameValue, num, addr);
	if (ret != 0)
	{
		printf("Get variables num failed.\n");
		return ret;
	}

	TY_VAR_DES *paraDes = new TY_VAR_DES[num];
	ret = getVariableList(modelNameValue, paraDes, num, addr);
	if (ret != 0)
	{
		printf("Get variables failed.\n");
		delete[]paraDes;
		return ret;
	}
#endif
	pList = new TY_VAR_VALUE[num];
	for(int i = 0; i < num; i++)
	{
		pList[i].varType = paraDes[i].varType;
		strcpy(pList[i].varPath, paraDes[i].fullPath);
	}
	//(pList.,"ModelVariable");//ModelVariable 
	//strcpy(pList.varPath,"demo1/Constant1/Constant1");
	//pList.varType = 1;
	ret = gettimeofday(&time_1, NULL);
	ret = getRamSampleInTimeWindow(pList, num, addr);
	if (ret != 0)
	{
		printf("AGetRamSampleInTimeWindow failed.\n");
	}
	else
	{
		ret = gettimeofday(&time_2, NULL);
		printf("===GetRamSampleInTimeWindow %ds, %dus\n", time_2.tv_sec - time_1.tv_sec, time_2.tv_usec - time_1.tv_usec);
		for(int i = 0; i < num; i++)
		{
			printf("varPath:%s value:%s\n", pList[i].varPath, pList[i].queueValues[0].value);
		}
		
	}
	delete[]paraDes;
	delete[]pList;

	return ret;
}

int GetParameterList()
{
	TY_MODELNAME_VALUE modelNameValue;
	memset(&modelNameValue, 0, sizeof(TY_MODELNAME_VALUE));
	int modelNum;
	int ret = getModelListNum(modelNum, addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr);//2.2
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

	int num = 2;
	ret = getParameterListNum(modelNameValue, num, addr);
	if (ret != 0)
	{
		printf("Get parameter num failed.\n");
		return ret;
	}

	TY_PARA_DES *paraDes = new TY_PARA_DES[num];
	ret = getParameterList(modelNameValue, paraDes, num, addr);
	if(ret !=0)
	{
		printf("Get parameter failed.\n");
		return ret;
	}
	for (int i = 0; i < num; i++)
	{
		printf("%d Path:%s len:%d\n\tName:%s  Type:%d\n  uuid:%s len:%d\n", i + 1, paraDes[i].fullPath, strlen(paraDes[i].fullPath), paraDes[i].paraName, paraDes[i].dataType, paraDes[i].uuid, strlen(paraDes[i].uuid));
	}
	delete[]paraDes;
	return ret;
}

int AddMonitorVar()
{
	TY_MODELNAME_VALUE modelNameValue;

	int modelNum;
	int ret = getModelListNum(modelNum, addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr);//2.2
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
	ret = getVariableListNum(modelNameValue, num, addr);
	if (ret != 0)
	{
		printf("Get variables num failed.\n");
		return ret;
	}

	TY_VAR_DES *paraDes = new TY_VAR_DES[num];
	ret = getVariableList(modelNameValue, paraDes, num, addr);
	if (ret != 0)
	{
		printf("Get variables failed.\n");
		delete[]paraDes;
		return ret;
	}
	ret = addVarMonitor(paraDes, num, addr);
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

int GetParamValue()
{	
	TY_MODELNAME_VALUE modelNameValue;
	memset(&modelNameValue, 0 , sizeof(TY_MODELNAME_VALUE));

	int modelNum;
	int ret = getModelListNum(modelNum, addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	memset(&modelList, 0 , sizeof(TY_MODEL_LIST));
	getModelList(&modelList, addr);//2.2
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
	ret = getParameterListNum(modelNameValue, num, addr);
	if (ret != 0)
	{
		printf("Get parameter num failed.\n");
		return ret;
	}

	TY_PARA_DES *paraDes = new TY_PARA_DES[num];
	modelNameValue.modelNumber = modelNum;
	ret = getParameterList(modelNameValue, paraDes, num, addr);
	if (ret != 0)
	{
		printf("Get parameter failed.\n");
		return ret;
	}
	ret = getParameterValue(paraDes, num, addr);
	if (ret != 0)
	{
		printf("Get parameter value failed.num:%d\n",num);
		return ret;
	}

	for (int i = 0; i < num; i++)
	{
		if (paraDes[i].dataType == M_DOUBLE)
		{
			double t = 0;
			memcpy(&t, paraDes[i].value, 8);
			printf("%d %d fullPath:%s Type:%d Value:%f\n", num, i + 1, paraDes[i].fullPath, paraDes[i].dataType, t);
		}
		
	}
	delete[]paraDes;
	paraDes = NULL;
	return ret;
}

int SetParamValue()
{
	//>> time
	struct timeval time_1;
	struct timeval time_2;
	//<<

	TY_MODELNAME_VALUE modelNameValue;
	memset(&modelNameValue, 0, sizeof(TY_MODELNAME_VALUE));
	int modelNum;
	int ret = getModelListNum(modelNum, addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	memset(&modelList, 0, sizeof(TY_MODEL_LIST));
	getModelList(&modelList, addr);//2.2
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
	ret = getParameterListNum(modelNameValue, num, addr);
	if (ret != 0)
	{
		printf("Get parameter num failed.\n");
		return ret;
	}

	TY_PARA_DES *paraDes = new TY_PARA_DES[num];
	modelNameValue.modelNumber = 1;
	ret = getParameterList(modelNameValue, paraDes, num, addr);
	if (ret != 0)
	{
		printf("Get parameter failed.\n");
		return ret;
	}

	for (int i = 0; i < num; i++)
	{
		if (paraDes[i].dataType == M_DOUBLE)
		{
			double dValue = i + 5;
			memcpy(paraDes[i].value ,&dValue,8);
		}
		else
		{
			char dValue = i + 1;
			memcpy(paraDes[i].value, &dValue, 1);
		}

	}
	ret = gettimeofday(&time_1, NULL);
	ret= setParameterValue(paraDes, num, addr);
	if (ret != 0)
	{
		printf("Set parameter failed.\n");
		return ret;
	}
	else
	{
		ret = gettimeofday(&time_2, NULL);
		printf("===SetParamValue %ds, %dus\n", time_2.tv_sec - time_1.tv_sec, time_2.tv_usec - time_1.tv_usec);
		printf("Set parameter success.\n");
	}

	delete[]paraDes;
	return ret;
}

int DelMonitorVar()
{
	TY_MODELNAME_VALUE modelNameValue;

	int modelNum;
	int ret = getModelListNum(modelNum, addr);//2.2
	if (ret != 0)
	{
		printf("Get model list num	failed.\n");
		return -1;
	}
	TY_MODEL_LIST modelList;
	getModelList(&modelList, addr);//2.2
	modelNum = modelList.modelNumber;
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
	ret = getVariableListNum(modelNameValue, num, addr);
	if (ret != 0)
	{
		printf("Get variables num failed.\n");
		return ret;
	}

	TY_VAR_DES *paraDes = new TY_VAR_DES[num];
	ret = getVariableList(modelNameValue, paraDes, num, addr);
	if (ret != 0)
	{
		printf("Get variables failed.\n");
		delete[]paraDes;
		return ret;
	}
	ret = delMonitorVar(paraDes, num, addr);
	//ret = delMonitorVar(paraDes[0], num, addr);
	printf("delMonitorVar ret = %d",ret);
	if (ret != 0)
	{
		printf("delete monitor variables failed.\n");
	}

	delete[]paraDes;
	return ret;
}
int GetStorageRecordList()
{
	int num, ret = 0;
	if (getStorageRecordListNum(num, addr) != 0)
	{
		printf("getStorageRecordListNum error\n");
	}
	printf("getStorageRecordListNum : %d\n", num);
	TY_STORAGERECORD_VALUES *storageRecord = new TY_STORAGERECORD_VALUES[num];
	getStorageRecordList(storageRecord, num, addr);
	for (int i = 0; i < num; i++)
	{
		printf("%d storageName:%s   storageUuid:%s \n", i, storageRecord[i].storageName, storageRecord[i].storageUuid);
	}
	delete[]storageRecord;
	return ret;
}
int GetStorageModeList()
{
	int storageNum = 0, ret = 0, storageModelNum = 0;
	if (getStorageRecordListNum(storageNum, addr) != 0)
	{
		printf("getStorageRecordListNum faild\n");
		return -1;
	}
	TY_STORAGERECORD_VALUES *storageRecord = new TY_STORAGERECORD_VALUES[storageNum];
	if (getStorageRecordList(storageRecord, storageNum, addr) != 0)
	{
		printf("getStorageRecordList faild\n");
		return -1;
	}


	for (int i = 0; i < storageNum; i++)
	{
		printf("%d storageName:%s   storageUuid:%s \n\n", i, storageRecord[i].storageName, storageRecord[i].storageUuid);
		if (getStorageModeListNum(storageModelNum, storageRecord[i].storageUuid, addr) != 0)
		{
			printf("getStorageModeListNum faild\n");
			return -1;
		}
		TY_STORAGEMODELIST *paraDes = new TY_STORAGEMODELIST[storageModelNum];
		if (getStorageModeList(paraDes, storageModelNum, storageRecord[i].storageUuid, addr) != 0)
		{
			printf("getStorageModeList faild\n");
			return -1;
		}
		for (int modelNum = 0; modelNum < storageModelNum; modelNum++)
		{
			printf("******************modelName : %s******************\n", paraDes[modelNum].modelName);
			for (int varNum = 0; varNum < paraDes[modelNum].varNum; varNum++)
			{
				printf("varName:%s,varUuid:%s, varPath:%s\n", paraDes[modelNum].name[varNum], paraDes[modelNum].uuid[varNum], paraDes[modelNum].varPath[varNum]);
			}
		}
		printf("\n\n");
		delete[]paraDes;
	}

	delete[]storageRecord;
	return 0;
}
int GetRecordTimeRange()
{
	int storageNum = 0, ret = 0, storageModelNum = 0;

	if (getStorageRecordListNum(storageNum, addr) != 0)
	{
		printf("��ȡ�洢�б�ʧ��");
		return -1;
	}
	TY_STORAGERECORD_VALUES *storageRecord = new TY_STORAGERECORD_VALUES[storageNum];
	if (getStorageRecordList(storageRecord, storageNum, addr) != 0)
	{
		printf("��ȡ�洢�б�ʧ��");
		return -1;
	}


	_RECORD_TIME *recordTime = new _RECORD_TIME[storageNum];
	for (int i = 0; i < storageNum; i++)
	{
		if (getRecordTimeRange(storageRecord[i].storageUuid, recordTime[i], addr) != 0)
		{
			printf("%d storageName:%s storageUuid:%s get record time fail\n", i, storageRecord[i].storageName, storageRecord[i].storageUuid);
		}
		else
		{
			printf("%d storageName:%s storageUuid:%s startTime:%lld endTime:%lld\n", i, storageRecord[i].storageName, storageRecord[i].storageUuid, recordTime[i].startTime, recordTime[i].endTime);
		}
	}
	delete[]storageRecord;
	delete[]recordTime;
	return ret;
}
int GetRamSampleRecorde()
{

	TY_RAMSAMPLE_VALUES ramSample;
	TY_VAR_VALUE varDes;
	ramSample.recordTime.startTime = 0;
	ramSample.recordTime.endTime = 10000000000;
	strcpy(ramSample.storageUuid, "c06b28a7-04a6-4338-bfaf-ca23829df9a3");
	strcpy(ramSample.fullPath[0], "demo1/Shared Memory Get1/SharedMemoryGet1");
	ramSample.varLength = 1;
	if(getRamSampleRecorde(ramSample, &varDes, addr) != 0)
	{
		printf("getRamSampleRecorde faild\n");
		return -1;
	}
	
	for (int queneNum = 0; queneNum < varDes.queueLen; queneNum++)
	{
		printf("%d time:%lld, value:%s\n", queneNum, varDes.queueValues[queneNum].time, varDes.queueValues[queneNum].value);
	}
	return 0;

}
int DeleteStorageRecord()
{
	int storageNum = 0, ret = 0, storageModelNum = 0;
	if (getStorageRecordListNum(storageNum, addr) != 0)
	{
		printf("getStorageRecordListNum faild\n");
		return -1;
	}
	TY_STORAGERECORD_VALUES *storageRecord = new TY_STORAGERECORD_VALUES[storageNum];
	if (getStorageRecordList(storageRecord, storageNum, addr) != 0)
	{
		printf("getStorageRecordList faild\n");
		return -1;
	}

	for (int i = 0; i < storageNum; i++)
	{
		printf("%d storageName:%s   storageUuid:%s \n", i, storageRecord[i].storageName, storageRecord[i].storageUuid);
	}
	for (int i = 0; i < storageNum; i++)
	{
		if (deleteStorageRecord(storageRecord[i].storageUuid, addr) != 0)
		{
			printf("delete %s success \n", storageRecord[i].storageName);
		}
		else
		{
			printf("delete Storage Record fail \n");
		}
	}
	delete[]storageRecord;
	return ret;
}

int GetCurrentProject()
{
	TY_PROJECT_VALUE projectValue;
	int ret = getCurrentProject(&projectValue, addr);
	if (ret == 0)
	{
		printf("projectName:%s\nprojectUuid:%s\n", projectValue.name, projectValue.uuid);
	}
	else
	{
		printf("get project fail");
	}
	
	return ret;
}

void ShowCmd()
{
	printf("============Menu=============\n");
	printf("\t1.Login\n\
	2.getCurrentProject\n\
	3.GetModelList\n\
	4.GetModelStatus\n\
	5.GetVarialbelsList\n\
	6.GetResourceInfo\n\
	7.LoadModel\n\
	8.UnloadModel\n\
	9.StartModel\n\
	10.PauseModel\n\
	11.StopModel\n\
	12.GetParameters\n\
	13.GetParameterValue\n\
	14.SetParamValue\n\
	15.AddVarMonitor\n\
	16.DeleteVarMonitor\n\
	17.GetRamSampleInTimeWindow\n\
	19.GetStoreRecordList\n\
	20.GetStorageModeList\n\
	21.GetRecordTimeRange\n\
	22.GetRamSampleRecorde\n\
	23.DeleteStorageRecord\n\
	99.Exit\n");
	printf("Please select the command:");
}

int main(int argc, char *argv[])
{
	struct timeval time_1;
	struct timeval time_2;
	int ret = 0;
	bool runFlag = true;
	while (runFlag)
	{
		switch (GetCommandIndex())
		{

		case 1:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			Login();
			break;
		case 2:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			GetCurrentProject();
			break;
		case 3:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			GetModelList();
			break;
		case 4:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			GetModelStatus();
			break;
		case 5:
			GetVariableList();//getVariableList
			break;
		case 6:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			GetResourceInfo();
			break;
		case 7:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			LoadModel();
			break;
		case 8:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			if (unloadModel(addr) == 0)
			{
				printf("ok\n");
			}
			else
			{
				printf("no\n");
			}
			break;
		case 9:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			StartModel();
			break;
		case 10:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			PauseModel();
			break;
		case 11:
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);
			StopModel();
			break;
		case 12:
			GetParameterList();//getParameterList
			break;
		case 13:
			GetParamValue();//getParameterList
			break;
		case 14:
			SetParamValue();
			break;
		case 15:
			AddMonitorVar();
			break;
		case 16:
			DelMonitorVar();
			break;
		case 17:	
			ret = gettimeofday(&time_1, NULL);
			printf("--->time1-->%d:%d\n", time_1.tv_sec, time_1.tv_usec);		
			GetRamSampleInTimeWindow();
			break;
		case 19:
			GetStorageRecordList();
			break;
		case 20:
			GetStorageModeList();
			break;
		case 21:
			GetRecordTimeRange();
			break;
		case 22:
			GetRamSampleRecorde();
			break;
		case 23:
			DeleteStorageRecord();
			break;
		case 99:
			runFlag = false;
			break;
		default:
			printf("Invalid command.\n");
			break;
		}
		ret = gettimeofday(&time_2, NULL);
		printf("--->time2-->%d:%d\n", time_2.tv_sec, time_2.tv_usec);
		printf("===consumed %ds, %dus\n", time_2.tv_sec - time_1.tv_sec, time_2.tv_usec - time_1.tv_usec);
		printf("\n");
	}

#if 0
	int ret = 0;
	//const char* url = "http://192.168.1.120:10031";
 
  cout << "<-----------------------login-API-------------------------->" << endl;
	ret = login(url);	
	if(ret != 0)
	{
		cout << "return error" << endl;
	}
	
   cout << "<-----------------------getModelList-API-------------------------->" << endl;
	TY_MODEL_LIST modelList;
	ret = getModelList(&modelList, url);
	if(ret != 0)
	{
		cout << "return error" << endl;
	}
 
  cout << "<-----------------------getResourceInfo-API-------------------------->" << endl;
  TY_SIM_RESCOURCE_INFO pSimRes;
   ret = getResourceInfo(&pSimRes, url);
	if(ret != 0)
	{
		cout << "return error" << endl;
	}
 
   cout << "<-----------------------getModelStatus-API-------------------------->" << endl;
  TY_MODEL_STATUS pModelStatus;
   ret = getModelStatus(modelList.modelName[0], &pModelStatus, url);
	if(ret != 0)
	{
		cout << "return error" << endl;
	}

#endif

	return 0;
}
