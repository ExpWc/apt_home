#pragma once

#ifndef _SIMPOLE_INTERFACE_LINUX_H
#define _SIMPOLE_INTERFACE_LINUX_H

//#define DLL_EXPORT __declspec(dllexport)
#include <iostream>

#define STR_LEN 64
#define MAX_MODEL_NUM 256
#define MAX_MODEL_NAME_S 128
#define MAX_TIME_NAME_S 64
#define MAX_STORAGE_NAME_S 128
#define MAX_VAR_UINIT_LENGTH 8
#define MAX_PATH_NAME_S 1024
#define MAX_VAR_NUM 1024
#define MAX_VAR_NAME_S 1024
#define MAX_VAR_PATH_LEN 256
#define MAX_SAMPLE_NUM 2048

extern "C"
{
#pragma pack(1)
	//struct>>>

	typedef struct _model_list
	{
		int modelNumber;
		char modelName[MAX_MODEL_NUM][MAX_MODEL_NAME_S];
		char uuid[MAX_MODEL_NUM][MAX_MODEL_NAME_S];

	} TY_MODEL_LIST;

	enum ModelRunStatusE
	{
		MODEL_RUNNING,
		MODEL_INITIAL,
		MODEL_NULL,
		MODEL_WAIT,
		MODEL_PAUSE
	};
	typedef struct _sim_resource_info
	{
		short timeResNum;
		short modelNum;
		char timeResNames[MAX_MODEL_NUM][MAX_TIME_NAME_S];
		short cpuNum;
		char modelNames[MAX_MODEL_NUM][MAX_MODEL_NAME_S];
	} TY_SIM_RESCOURCE_INFO;
	typedef struct _model_status
	{
		bool autoWrite;
		bool autoRead;
		char modelName[MAX_MODEL_NAME_S];
		short cpuIndex;
		long pid;
		ModelRunStatusE modelRunStatus;
		long period;
		long frameCount;
		long overrunTimes;
		long runTime;
		long avgRunTime;
		long maxRunTime;
		long delayTime;
		long maxDelayTime;
	} TY_MODEL_STATUS;

	enum ModelDataVarTypeE
	{
		M_BOOL,
		M_CHAR,
		M_UCHAR,
		M_SHORT,
		M_USHORT,
		M_INT,
		M_UINT,
		M_FLOAT,
		M_DOUBLE
	};
	typedef struct _parameter_des
	{
		//long modelParaOffset;
		//unsigned int modelParaLen;
		ModelDataVarTypeE dataType;
		char paraName[MAX_MODEL_NAME_S];
		unsigned char value[MAX_VAR_UINIT_LENGTH];
		char fullPath[MAX_PATH_NAME_S];
		char uuid[MAX_PATH_NAME_S]; //uuid
	} TY_PARA_DES;
	typedef struct _variable_des
	{
		ModelDataVarTypeE dataType;
		char nodeType[MAX_MODEL_NAME_S];
		char paraName[MAX_MODEL_NAME_S];
		unsigned char value[MAX_VAR_UINIT_LENGTH];
		short varType;
		char fullPath[MAX_PATH_NAME_S];
		char uuid[MAX_PATH_NAME_S];
		unsigned long long lastUpdateTime;
	} TY_VAR_DES;

	typedef struct _model_config
	{
		char modelName[MAX_MODEL_NAME_S];
		int period;
		short cpuIndex;
		int storageSignalNum;
		int storageVarNum;
		char storageVar[MAX_VAR_NUM][256];
		char storageSignal[MAX_VAR_NUM][256];
		char uuid[MAX_MODEL_NAME_S];
	} TY_MODEL_CONFIG;
	typedef struct _sim_conig
	{
		char timeResName[MAX_TIME_NAME_S];
		short modelNum;
		char storageName[MAX_STORAGE_NAME_S];
		bool stored;
		TY_MODEL_CONFIG *pModelConfigs;
	} TY_SIM_CONFIG;

	typedef struct _modelName_value
	{
		int modelNumber;
		char modelName[MAX_MODEL_NUM][MAX_MODEL_NAME_S];
	} TY_MODELNAME_VALUE;

	/*add by lyc*/
	typedef struct _project_value
	{
		char name[MAX_MODEL_NAME_S];
		char uuid[MAX_MODEL_NAME_S];
		long long createdAt;
		char description[MAX_MODEL_NAME_S];
		long long visitedDate;
		bool status;
		char paramConfigurations[MAX_MODEL_NUM][MAX_MODEL_NAME_S];
		long paramConfigurationsNum;
		char testConfigurations[MAX_MODEL_NUM][MAX_MODEL_NAME_S];
		long testConfigurationsNum;
		char simModels[MAX_MODEL_NUM][MAX_MODEL_NAME_S];
		size_t simModelsNum;
		char panels[MAX_MODEL_NUM][MAX_MODEL_NAME_S];
		size_t panelsNum;
		char storageRecords[MAX_MODEL_NUM][MAX_MODEL_NAME_S];
		size_t storageRecordsNum;
	} TY_PROJECT_VALUE;
	/*  end   */

	typedef struct _queueValues
	{
		unsigned long long time;
		int length;
		ModelDataVarTypeE type;
		char value[STR_LEN];
	} TY_QUEUE_VALUE;
	typedef struct _storageRecord_Values
	{
		char storageUuid[MAX_MODEL_NAME_S];
		char storageName[MAX_MODEL_NAME_S];
	} TY_STORAGERECORD_VALUES;
	typedef struct _storageModeList
	{
		char cpuIndex[STR_LEN];
		char modelName[MAX_MODEL_NAME_S];
		char periodMul[STR_LEN];
		int length[MAX_VAR_NUM];
		char name[MAX_VAR_NUM][MAX_MODEL_NAME_S];
		char type[MAX_VAR_NUM][32];
		char uuid[MAX_VAR_NUM][MAX_MODEL_NAME_S];
		char varPath[MAX_VAR_NUM][MAX_MODEL_NAME_S];
		int varType[MAX_VAR_NUM];
		int varNum;
	} TY_STORAGEMODELIST;

	typedef struct _record_time
	{
		unsigned long long startTime;
		unsigned long long endTime;
	} _RECORD_TIME;

	typedef struct _ramSample_values
	{
		_RECORD_TIME recordTime;
		char storageUuid[256];
		char varUuid[MAX_VAR_NUM][MAX_MODEL_NAME_S];
		char fullPath[MAX_VAR_NUM][MAX_MODEL_NAME_S];
		char varType[MAX_VAR_NUM];
		char type[MAX_VAR_NUM][32];
		int varLength;
	} TY_RAMSAMPLE_VALUES;

	typedef struct VarValue
	{
		char uuid[MAX_STORAGE_NAME_S];
		char varPath[MAX_STORAGE_NAME_S];
		long sampleNum;
		long startTime;
		long long stepUnitInNs;
		TY_QUEUE_VALUE queueValues[MAX_SAMPLE_NUM];
		short varType; //0 var;1 signal
		int queueLen;
	} TY_VAR_VALUE;

	int login(const char *url);
	int getModelListNum(int &modelNum, const char *url);
	int getModelList(TY_MODEL_LIST *modelList, const char *url);
	int getCurrentProject(TY_PROJECT_VALUE *pProjectValue, const char *url);

	int getParameterValue(TY_PARA_DES *pParaDes, int num, const char *url);
	int getParameterList(TY_MODELNAME_VALUE modelNameValue, TY_PARA_DES *pList, int &paraNum, const char *url);
	int getParameterListNum(TY_MODELNAME_VALUE modelNameValue, int &paraNum, const char *url);
	int setParameterValue(TY_PARA_DES *pParaDes, int num, const char *url);
	int getResourceInfo(TY_SIM_RESCOURCE_INFO *pSimRes, const char *url);

	int getModelStatus(char *modelName, TY_MODEL_STATUS *pModelStatus, const char *url);

	int loadModel(TY_SIM_CONFIG simConfig, const char *url);
	int unloadModel(const char *url);
	int startModel(TY_MODELNAME_VALUE modelNameValue, const char *url);
	int stopModel(TY_MODELNAME_VALUE modelNameValue, const char *url);
	int pauseModel(TY_MODELNAME_VALUE modelNameValue, const char *url);

	int getVariableListNum(TY_MODELNAME_VALUE modelNameValue, int &varNum, const char *url);
	int getVariableList(TY_MODELNAME_VALUE modelNameValue, TY_VAR_DES *pList, int &varNum, const char *url);
	int addVarMonitor(TY_VAR_DES *pVars, int num, const char *url);
	int delMonitorVar(TY_VAR_DES *pVars, int num, const char*url);
	int getRamSampleInTimeWindow(TY_VAR_VALUE *pList, int num, const char *url);

	int getStorageRecordListNum(int &varNum, const char *url);
	int getStorageRecordList(TY_STORAGERECORD_VALUES *pList, int& varNum, const char*url);
	int getStorageModeListNum(int& varNum, const char *modelUuid, const char*url);
	int getStorageModeList(TY_STORAGEMODELIST *pList, int& varNum, const char *storageUuid, const char*url);
	int getRecordTimeRange(char* uuid, _RECORD_TIME& recordTime, const char*url);
	int getRamSampleRecorde(TY_RAMSAMPLE_VALUES ramSample, TY_VAR_VALUE* pList,  const char*url);
	int deleteStorageRecord(const char* storageUuid, const char*url);

}

#endif
