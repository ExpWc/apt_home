/*
 * CStorageManager.h
 *
 *  Created on: 2019-09-10
 *      Author: root
 */

#ifndef CSTORAGEMANAGER_H
#define CSTORAGEMANAGER_H

#include <queue>
#include <string>
#include <map>
#include <vector>
#include "DataType.h"
#include <pthread.h>
#include "CStorageRule.h"
#include "TimeManager.h"
#ifndef WIN32

#endif

#ifdef _WIN32
#define STORAGE_EXPORT __declspec(dllexport) 
#else
#define STORAGE_EXPORT 
#endif

#define MAX_STACK (64*1024*8)

#define TMP_COUNT 2500 //the tmp buffer for afdx sizea

#define MAX_STORE (64*1024) //  write 64K of int type to a file--//*1024
#define MAX_ONEFILE_SIZE (2*1024*1024*1024) // store 24M for one file--//2*1024*1024*1024
#define MAX_ONEFILE_STORE (MAX_ONEFILE_SIZE/MAX_STORE)

#define MAX_FIFO_SIZE 40960

#define HERADER_LEN 64
#define RECORD_NAME_LEN 255


using namespace std;

#define   STORAGE_SERVER_STATUS_INIT_MSG      "init"
#define   STORAGE_SERVER_STATUS_RUNNING_MSG   "start"
#define   STORAGE_SERVER_STATUS_STOP_MSG      "stop"
#define   STORAGE_SERVER_STATUS_OFFLINE_MSG   "offline"
#define   STORAGE_SERVER_STATUS_EXCEPTION_MSG "exception"

#define MAX_PAYLOAD_LEN 1024
#pragma pack(1)
typedef queue<TY_STORAGE_DATA> TY_STORAGE_FIFO;

//add log
#define STORAGELOG(type, description, args...) \
	CStorageManager::log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)


typedef struct _storage_time
{
    unsigned int tv_sec;
    unsigned int tv_usec;
}TY_STORAGE_TIME;

typedef struct
{
	TY_STORAGE_TIME startTime;//8 byte
	TY_STORAGE_TIME endTime;	//8 byte
	int ICDCountPerFile;//4 byte
	char reserve[44];
}TY_DATA_HEADER_INFO;//64byte

typedef struct
{
	TY_STORAGE_TIME firstPacketTime;//8 byte
	TY_STORAGE_TIME endTime;//8 byte
	unsigned int ICDCount;//4 byte
	unsigned int fileCount;//4 byte
	unsigned int ICDDataLen;//4 byte
	char reserve[36];
}TY_INDEX_HEADER_INFO;//64byte


typedef struct storage_data_info
{
	char icdID[128];
	char cardType[32];
	int cardID;
	int chnID;

	int ddsID;
	int sn;
	char storageName[256];
	char caseID[256];

	TY_DATA_HEADER_INFO dataHeader;
	TY_INDEX_HEADER_INFO indexHeader;
	int dataType;
	int dataFileFd;
	int infoIndexFileFd;
	int timeIndexFileFd;
	char maxDataBuff[MAX_STORE + 10];//once write max buffer
	int allLength; //all packets data length in buffer
	int writeFileCount;//per paragraph write 64K till 2G file

	/*record dataIndex for writing infoindx and timeIndex file when change dataFile>>>>>>*/
	int lastDataLen;
	unsigned int dataIndex;
	unsigned int lastDataIndex;
	/*<<<<<<<<<*/
	long long allDataFileSize;//all storage data file size
	TY_STORAGE_TIME lastIndexTime;//save the last time you write time index file
}TY_STORAGE_DATA_INFO;


//>> modefy by chao.wu3
typedef map<string, double> TY_STORAGE_MODEL_VAR_MAP; 

typedef struct _storage_model_info_
{
    TY_STORAGE_MODEL_VAR_MAP inputInfo;
    TY_STORAGE_MODEL_VAR_MAP outputInfo;
}TY_STORAGE_MODEL_INFO;


//typedef map<string, TY_STORAGE_MODEL_INFO> TY_STORAGE_MODEL_INFO_MAP;
typedef map<string, TY_STORAGE_MODEL_INFO> TY_MODEL_STORAGE_MAP; 
//<<

typedef map<string, TY_STORAGE_DATA_INFO*> TY_ICD_STORAGE_MAP;//<sn, dataInfo>
typedef map<int, TY_STORAGE_DATA_INFO*> TY_STORAGE_ICD_MAP;//<sn, dataInfo>
typedef map<int, TY_STORAGE_ICD_MAP> TY_STORAGE_ICD_MAP_MAP;  //<DDSID, snMap >

typedef map<int, TY_STORAGE_DATA_INFO*> TY_STORAGE_SN_MAP;



typedef struct _packet_info
{
	int id;//ddsid
	int sn;
	unsigned int count;//data count
	long long size;//data file size
	TY_STORAGE_TIME startTime;//8 byte
	TY_STORAGE_TIME stopTime;//8 byte
} TY_PACKET_INFO;


typedef struct _storage_info
{
	int status;
	int freeDisk;//MB
	string statusMsg;
	string IP;
	string path;
	string busType;
	long long storageStartTimeSec;
	long long storageStopTimeSec;
	long long startTimeSec;
	long long startTimeUsec;
	long long stopTimeSec;
	long long stopTimeUsec;
	queue <TY_PACKET_INFO> icdInfo;
	queue <TY_PACKET_INFO> channelInfo;
} TY_STORAGE_STATUS;


typedef struct _storage_config
{
//	string targetPath;
//	string localPath;
//	string dataPath;
//	int debugFlag;
//	int autoStartFlag;

	int spaceWarn;
	int freeDisk;//MB

	string busType;
	string IP;
	string storageServerName;
	string storagePath;
	string storageName;
	string caseID;
	//this time is sys time,ITB use it to display storage record.
	//the packet time is too small when the time type is IRIGB,because the time is not contain year.
	long long storageStartTimeSec;
	long long storageStopTimeSec;
	long long startTimeSec;
	long long startTimeUsec;
	long long stopTimeSec;
	long long stopTimeUsec;
	//<<<<<<<<<<<<<<<<
	unsigned int startSn;
	unsigned int stopSn;
}TY_STORAGE_CONFIG;
typedef struct _var_info
{
	char fullPath[256];
	char value[8];
}TY_VAR_DATA;
typedef struct _model_var_info
{
	char varName[1024][256];
	unsigned int varNum;
	char modelName[128];
}TY_MODEL_VAR_INFO;

typedef struct _record_info
{
	char icdID[64];
	TY_STORAGE_TIME startTime;
	TY_STORAGE_TIME stopTime;
	int storageType; // 0:icd  1:model
	int dataCount;
	int dataLength;
}TY_RECORD_INFO;

typedef struct _field_icd_info
{
	char fieldID[1024][128];
	int fieldNum;
	char ICDID[128];
	char caseID[128];
	int cardID;
	unsigned int channelID;
}TY_FIELD_ICD_INFO;


typedef struct _record_list
{
	char name[256];
	char path[256];
	TY_STORAGE_TIME startTime;
	TY_STORAGE_TIME stopTime;
	int dataCount;
	int dataLength;
	TY_RECORD_INFO icdInfo[256];
	int icdCount;

}TY_RecordList;

class STORAGE_EXPORT CStorageManager
{
private:
	CStorageManager();
	virtual ~CStorageManager();
	static CStorageManager *instance;//static
public:
	static CStorageManager * getInstance();
	int initInstance(char *ServerName);
	int loadConfig(char *ServerName);
	int getStoreInfo();
	int clearStoreInfo();

	/*add by ty*/
	int m_storageStatus;//run,stop

	TY_STORAGE_FIFO serviceDataFIFO;// for insert data
	pthread_mutex_t fifoLock;

    pthread_attr_t threadAttr;
    pthread_t threadId;
    pthread_t spaceWarnThreadId;

    pthread_t storeInfoThreadId;
	int storeInfoThreadFlag;	//used for start the thread of storeStauts.
    TY_STORAGE_ICD_MAP_MAP m_storageICDMap;

    string storeXml;
	TY_ICD_STORAGE_MAP m_icdStorageMap;

	TY_STORAGE_CONFIG m_config;

    CStorageRule m_storageRule;
	//>> modefy by chao.wu3
	TY_MODEL_STORAGE_MAP m_modelStorageMap;
	//>>

	string fileName;
	string m_storageName;
	string m_caseId;
	/*add by ty*/

	string m_exePath;

	TimeManager *pTimeManager;
	TimeManager fromTimeManage;
	TimeManager toTimeManage;

	string getLibVersion()
	{
		string tmp = "V4.0.0";
		return tmp;
	}
	int initStorage(const char* storeContent);
	int getRecordList(TY_RecordList* pList, unsigned int &listLen);
	int getRecordListNum(unsigned int &listLen);
	int getDataByIndex(string storageName, char* pName, unsigned int startIndex, unsigned int stopIndex, TY_STORAGE_DATA* pData, int maxLen);
	int getDataByTime(string storageName, char* pName, TY_STORAGE_TIME *startTime, TY_STORAGE_TIME *stopTime, TY_STORAGE_DATA* pData, int maxCount);
	int getIndexByTime(string storageName, char* pName, TY_STORAGE_TIME *startTime, TY_STORAGE_TIME *stopTime,int *startIndex,int *stopIndex);
	int deleteStorageRecord(string storageName);

	int insertStorageXml();
	int insertModelData(TY_STORAGE_DATA *data);
	int insertData(TY_STORAGE_DATA *data);
	void clearStorageDataInfo();
	void deleteStorageDataInfo();
	void addSnStorageRange(int devId);
	int deleteAllStorageRecord();

	int startStorage(string caseID);
	//int startStorage();	
	int stopStorage();

	int getStorageStatus(TY_STORAGE_STATUS *info);

	//int getParseInfo();
	int getParseInfo(string caseID);
    int log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
			int lineNo, string msg);
};
#pragma pack()
#endif /* CSTORAGEMANAGER_H */
