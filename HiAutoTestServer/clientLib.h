#include <iostream>

#include <map>

#include "HiAutoTestServerC.h"


#define SERVER_DOWN 10030

#define MAX_CASE_NUM 128
#define MAX_UUID_LENGTH 64
#define MAX_NAME_LENGTH 64
#define MAX_STEP_NUM 128
#define MAX_CARD_NUM 32

// Auto Test
#define MAX_AUTO_UUID_LEN  64
#define	MAX_AUTO_PAYLOAD_LEN 256
#define	MAX_AUTO_COL_LEN  1024
#define	MAX_AUTO_RECODE_LIST_LEN  256
#define	MAX_AUTO_VANAME_LEN  256
#define	MAX_AUTO_SOTRAGE_UUID_LEN 128
#define	MAX_AUTO_MODEL_LEN  128

// Manual Test
#define	MAX_MANUAL_UUID_LEN  128
#define	MAX_MANUAL_MODEL_LEN  128
#define	MAX_MANUAL_COL_LEN  1024
#define	MAX_MANUAL_CARDTYPE_LEN  64
#define	MAX_MANUAL_VANAME_LEN  256

#define MAX_IP_LEN 32
#define MAX_SERVER_NAME_LEN 128

#ifdef WIN32
#define TAO_EXPORT __declspec(dllexport)
#else
#define TAO_EXPORT __attribute((visibility("default")))
#endif
enum CORBA_RUN_FLAG
{
    CORBA_RUN,
    CORBA_EXIT
};
enum CaseStepStatusE
{
	STATUS_READY,//0锛屽緟杩愯
	STATUS_RUNNING,//1锛岃繍琛屼腑
	STATUS_SUCCESS, //2锛岃繍琛屽畬鎴愶紝鎴愬姛
	STATUS_FAILED,//3锛岃繍琛屽畬鎴愶紝澶辫触
	STATUS_ERROR    //4锛屽紓甯?
};
enum ServerRunStatusE
{
	SERVER_NULL, //0,
	SERVER_INITIAL, 
	SERVER_RUNNING,
	SERVER_PAUSE
};

typedef struct _case_execute_
{
	char caseID[MAX_AUTO_UUID_LEN];//鐢ㄤ緥鐨刄UID
	int caseExecuteTimes;//鐢ㄤ緥鎵ц娆℃暟锛?>0
	int interval;//涓庝笅涓?涓渶鎵ц鐨勭敤渚嬩箣闂寸殑鏃堕棿闂撮殧锛屽崟浣島s
}TY_CASE_EXECUTE;
typedef struct _test_execute_
{
	int testExecuteNum; //娴嬭瘯鎵ц娆℃暟锛?>0
	int interval; //涓庝笅涓?娆￠渶鎵ц鐨勬祴璇曚箣闂寸殑鏃堕棿闂撮殧锛屽崟浣島s
	TY_CASE_EXECUTE caseExecute[MAX_CASE_NUM];//鏌愪竴鐢ㄤ緥鎵ц
}TY_TEST_EXECUTE;

typedef struct _case_status_
{
	char caseID[MAX_AUTO_UUID_LEN];//鐢ㄤ緥鐨刄UID
	CaseStepStatusE caseStepStatus;// 鐢ㄤ緥鐘舵??
}TY_CASE_STATUS;
typedef struct _step_status_
{
	char caseID[MAX_AUTO_UUID_LEN];//鐢ㄤ緥鐨刄UID
	char stepID[MAX_AUTO_UUID_LEN];//姝ラ鐨刄UID
	CaseStepStatusE caseStepStatus;//姝ラ鐘舵??
	char msg[MAX_AUTO_PAYLOAD_LEN];
	char attachInfo[MAX_AUTO_PAYLOAD_LEN];
}TY_STEP_STATUS;

typedef struct _subStep_status_
{
	char caseID[MAX_AUTO_UUID_LEN];//鐢ㄤ緥鐨刄UID
	char stepID[MAX_AUTO_UUID_LEN];//姝ラ鐨刄UID
	char subStepID[MAX_AUTO_UUID_LEN];//zi姝ラ鐨刄UID
	CaseStepStatusE caseStepStatus;//姝ラ鐘舵??
	char msg[MAX_AUTO_PAYLOAD_LEN];
	char attachInfo[MAX_AUTO_PAYLOAD_LEN];
}TY_SUBSTEP_STATUS;
typedef struct _field_data_
{
	char icdID[64];//ICD鐨刄UID
	char fieldID[64];//鍩熺殑UUID
	char data[8];//鍊?
} TY_FIELD_DATA;

typedef struct _card_status_
{
	long id;//鏉垮崱id
	long status;//鏉垮崱鐘舵??
}TY_CARD_STATUS;
typedef struct _server_status_
{
	long id;//鏈嶅姟id
	ServerRunStatusE serverRunStatus;//鏈嶅姟杩愯鐘舵??
	short cardNum;
	TY_CARD_STATUS cardStatus[MAX_CARD_NUM];
}TY_SERVER_STATUS;
typedef struct _executive_info
{
  char preCaseID[MAX_AUTO_UUID_LEN];
  char preStepID[MAX_AUTO_UUID_LEN];
  long caseRunTimes;
  long successNum;
  long failNum;
  CaseStepStatusE runStatus;
}TY_ExecutiveInfo;


typedef struct _storage_time
{
    unsigned int tv_sec;
    unsigned int tv_usec;
}TY_STORAGE_TIME;


typedef struct _record_info
{
	char icdID[MAX_AUTO_UUID_LEN];
	TY_STORAGE_TIME startTime;
	TY_STORAGE_TIME stopTime;
	int storageType; // 0:icd  1:model
	int dataCount;
	int dataLength;
}TY_RECORD_INFO;


typedef struct _field_icd_info
{
	char fieldID[MAX_AUTO_COL_LEN][MAX_AUTO_SOTRAGE_UUID_LEN];
	int fieldNum;
	char ICDID[MAX_AUTO_SOTRAGE_UUID_LEN];
	char caseID[MAX_AUTO_SOTRAGE_UUID_LEN];
	int cardID;
	unsigned int channelID;
}TY_FIELD_ICD_INFO;



typedef struct _record_list
{
	char name[MAX_AUTO_RECODE_LIST_LEN];
	char path[MAX_AUTO_RECODE_LIST_LEN];
	TY_STORAGE_TIME startTime;
	TY_STORAGE_TIME stopTime;
	int dataCount;
	int dataLength;
	TY_RECORD_INFO icdInfo[MAX_AUTO_RECODE_LIST_LEN];
	int icdCount;

}TY_RecordList;



typedef struct _mdoel_record_info
{
	char varName[64];
	TY_STORAGE_TIME startTime;
	TY_STORAGE_TIME stopTime;
	int dataCount;
	int dataLength;
}TY_MODEL_RECORD_INFO;


typedef struct _model_var_info
{
	char varName[MAX_AUTO_COL_LEN][MAX_AUTO_VANAME_LEN];
	unsigned int varNum;
	char modelName[MAX_AUTO_MODEL_LEN];
}TY_MODEL_VAR_INFO;


typedef struct _model_record_list
{
	char name[MAX_AUTO_RECODE_LIST_LEN];
	char path[MAX_AUTO_RECODE_LIST_LEN];
	TY_STORAGE_TIME startTime;
	TY_STORAGE_TIME stopTime;
	int dataCount;
	int dataLength;
	TY_RECORD_INFO icdInfo[MAX_AUTO_PAYLOAD_LEN];
	int icdCount;
}TY_ModelRecordList;
//>>manual test
typedef struct _get_icd_info
{
    char icdID[MAX_MANUAL_UUID_LEN];
    char fieldID[MAX_MANUAL_COL_LEN][MAX_MANUAL_UUID_LEN];
    char cardType[MAX_MANUAL_CARDTYPE_LEN];
    int cardID;
    int fieldNum;
    double data[MAX_MANUAL_COL_LEN];
}TY_GET_ICD_INFO;

typedef struct _send_icd_info
{
    char icdID[MAX_MANUAL_UUID_LEN];
    char cardType[MAX_MANUAL_CARDTYPE_LEN];
    int cardID;
}TY_SEND_ICD_INFO;

typedef struct _model_info
{
    char modelName[MAX_MANUAL_MODEL_LEN];
    char fullPath[MAX_MANUAL_COL_LEN][MAX_MANUAL_VANAME_LEN];
    double data[MAX_MANUAL_COL_LEN];
    int num;
}TY_MODEL_INFO;

typedef struct _set_icd_info
{
    char icdID[MAX_MANUAL_UUID_LEN];
    char fieldID[MAX_MANUAL_COL_LEN][MAX_MANUAL_UUID_LEN];
    double data[MAX_MANUAL_COL_LEN];
    int fieldNum;
}TY_SET_ICD_INFO;

typedef struct _data
{
	char payload[8];
} TY_DATA;


typedef struct _step_info_
{
	char caseID[MAX_AUTO_UUID_LEN];//用例的UUID
	char stepID[MAX_AUTO_UUID_LEN];//步骤的UUID
}TY_STEP_INFO;

typedef struct _node_info_
{
 	char serverName[MAX_SERVER_NAME_LEN];
	char ip[MAX_IP_LEN];
}TY_NODE_INFO;

#ifdef __cplusplus
extern "C"
{
#endif
	//mannualTest
	TAO_EXPORT int startManualTest (void);
	TAO_EXPORT int stopManualTest (void);
	TAO_EXPORT int startModel (const char* pData);
	TAO_EXPORT int stopModel (char* modelName);
	TAO_EXPORT int getModelRunStatus (char* modelName, int *modelStatus);
	TAO_EXPORT int setICDData (TY_SET_ICD_INFO *icdInfo, int icdNum);
	TAO_EXPORT int sendICDData(TY_SEND_ICD_INFO *icdInfo, int icdNum);
	TAO_EXPORT int getICDData(TY_GET_ICD_INFO *icdInfo, int icdNum);
	TAO_EXPORT int getModelParam(TY_MODEL_INFO *modelInfo, int modelNum);
	TAO_EXPORT int setModelParam(TY_MODEL_INFO *modelInfo, int modelNum);
	TAO_EXPORT int getModelVar(TY_MODEL_INFO *modelInfo, int modelNum);
	TAO_EXPORT int setModelVar(TY_MODEL_INFO *modelInfo, int modelNum);
	TAO_EXPORT int optManualTestOneStep(const char* oneStep, TY_STEP_STATUS *pStepStatus);																		
	//autotest
	//go to vs2010
	TAO_EXPORT int init(const char *pNameServerIp, const char *pNameServerName);
	TAO_EXPORT int setICD(const char * ICDInfo);
	TAO_EXPORT int setCase(const char * caseData);
	TAO_EXPORT int setChannelConfig(const char * channelConfig);
	TAO_EXPORT int setCurveConfig(const char * channelConfig);
	TAO_EXPORT int start(TY_TEST_EXECUTE testExecute);
	TAO_EXPORT int stop();
	TAO_EXPORT int stopStep(TY_STEP_INFO* stepInfo, int num);
	TAO_EXPORT int getCaseStatus(TY_CASE_STATUS *pCaseStatus, int num);
	TAO_EXPORT int getStepStatus(TY_STEP_STATUS *pStepStatus, int num);
	TAO_EXPORT int getSubStepStatus(TY_SUBSTEP_STATUS *pSubStepStatus, int num);
	TAO_EXPORT int getFieldData(TY_FIELD_DATA *pFieldData, int num);
	TAO_EXPORT int getServerStatus(TY_SERVER_STATUS *pServerStatus);
	TAO_EXPORT int dispose();
	TAO_EXPORT int getCardsInfo();
	TAO_EXPORT int getExecutiveInfo(TY_ExecutiveInfo *pInfo);
	//storage
	TAO_EXPORT int setStorageConfig(const char * pStorageConfig);
	TAO_EXPORT int getRecordListNum(int *listNum);
	TAO_EXPORT int getRecordList(TY_RecordList* pList, int *listLen);
	TAO_EXPORT int getDataByTime(char* storageName, TY_FIELD_ICD_INFO varInfo,TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, TY_DATA** pData, TY_STORAGE_TIME *pTime, int *maxCount);
	TAO_EXPORT int getDataByIndex(char* storageName, TY_FIELD_ICD_INFO varInfo, int startIndex, int stopIndex, TY_DATA** pData, TY_STORAGE_TIME *pTime, int *maxCount);
	TAO_EXPORT int getIndexByTime(char* storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime,int * startIndex,int * stopIndex);

	TAO_EXPORT int getModelDataByTime(char* storageName, TY_MODEL_VAR_INFO varInfo,TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, TY_DATA** pData, TY_STORAGE_TIME *pTime,int *maxCount);
	TAO_EXPORT int getModelDataByIndex(char* storageName, TY_MODEL_VAR_INFO varInfo, int startIndex, int stopIndex, TY_DATA** pData, TY_STORAGE_TIME *pTime,int *maxCount);
	TAO_EXPORT int getModelIndexByTime(char* storageName, TY_MODEL_VAR_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime,int * startIndex,int * stopIndex);
	TAO_EXPORT int deleteStorageRecord(char* storageName);
	TAO_EXPORT int deleteAllStorageRecord();
	
	TAO_EXPORT int getIpInfo(char *pIpInfo);
	TAO_EXPORT int getNodeInfo(TY_NODE_INFO *pNodeInfo, int *pCnt);
	TAO_EXPORT int setNodeICD(const char *pIp, const char * pICDInfo);
	TAO_EXPORT int setNodeChannelConfig(const char *pIp, const char * pChannelConfig);
	TAO_EXPORT int setNodeCase(const char *pIp, const char * pCaseData);
	TAO_EXPORT int startNode(const char *pIp, TY_TEST_EXECUTE testExecute);
	TAO_EXPORT int getNodeStepStatus(const char *pIp, TY_STEP_STATUS *pStepStatus, int num);
	TAO_EXPORT int getNodeCaseStatus(const char *pIp, TY_CASE_STATUS *pCaseStatus, int num);
	TAO_EXPORT int setNodeCurveConfig(const char *pIp, const char *channelConfig);
	TAO_EXPORT int stopNode(const char *pIp);
	TAO_EXPORT int stopNodeStep(const char *pIp, TY_STEP_INFO* stepInfo, int num);
	TAO_EXPORT int getNodeSubStepStatus(const char *pIp, TY_SUBSTEP_STATUS *pSubStepStatus, int num);
	TAO_EXPORT int getNodeFieldData(const char *pIp, TY_FIELD_DATA *pFieldData, int num);
	TAO_EXPORT int getNodeServerStatus(const char *pIp, TY_SERVER_STATUS *pServerStatus);
	TAO_EXPORT int getNodeCardsInfo(const char *pIp);
	TAO_EXPORT int getNodeExecutiveInfo(const char *pIp, TY_ExecutiveInfo *pInfo);
	
	TAO_EXPORT int setNodeStorageConfig(const char *pIp, const char *pStorageConfig);
	TAO_EXPORT int getNodeRecordListNum(const char *pIp, int *listNum);
	TAO_EXPORT int getNodeRecordList(const char *pIp, TY_RecordList *pList, int *listLen);
	TAO_EXPORT int getNodeDataByTime(const char *pIp, char* storageName, TY_FIELD_ICD_INFO varInfo,TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, TY_DATA** pData, TY_STORAGE_TIME *pTime, int *maxCount);
	TAO_EXPORT int getNodeDataByIndex(const char *pIp, char* storageName, TY_FIELD_ICD_INFO varInfo, int startIndex, int stopIndex, TY_DATA** pData, TY_STORAGE_TIME *pTime, int *maxCount);
	TAO_EXPORT int getNodeIndexByTime(const char *pIp, char* storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime,int * startIndex,int * stopIndex);

	TAO_EXPORT int getNodeModelDataByTime(const char *pIp, char* storageName, TY_MODEL_VAR_INFO varInfo,TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, TY_DATA** pData, TY_STORAGE_TIME *pTime,int *maxCount);
	TAO_EXPORT int getNodeModelDataByIndex(const char *pIp, char* storageName, TY_MODEL_VAR_INFO varInfo, int startIndex, int stopIndex, TY_DATA** pData, TY_STORAGE_TIME *pTime,int *maxCount);
	TAO_EXPORT int getNodeModelIndexByTime(const char *pIp, char* storageName, TY_MODEL_VAR_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime,int * startIndex,int * stopIndex);
	TAO_EXPORT int deleteNodeStorageRecord(const char *pIp, char* storageName);
	TAO_EXPORT int deleteNodeAllStorageRecord(const char *pIp);

	TAO_EXPORT int startNodeManualTest (const char *pIp);
	TAO_EXPORT int stopNodeManualTest (const char *pIp);
	TAO_EXPORT int startNodeModel (const char *pIp, const char* pData);
	TAO_EXPORT int stopNodeModel (const char *pIp, char* modelName);
	TAO_EXPORT int getNodeModelRunStatus (const char *pIp, char* modelName, int *modelStatus);
	TAO_EXPORT int setNodeICDData (const char *pIp, TY_SET_ICD_INFO *icdInfo, int icdNum);
	TAO_EXPORT int sendNodeICDData(const char *pIp, TY_SEND_ICD_INFO *icdInfo, int icdNum);
	TAO_EXPORT int getNodeICDData(const char *pIp, TY_GET_ICD_INFO *icdInfo, int icdNum);
	TAO_EXPORT int getNodeModelParam(const char *pIp, TY_MODEL_INFO *modelInfo, int modelNum);
	TAO_EXPORT int setNodeModelParam(const char *pIp, TY_MODEL_INFO *modelInfo, int modelNum);
	TAO_EXPORT int getNodeModelVar(const char *pIp, TY_MODEL_INFO *modelInfo, int modelNum);
	TAO_EXPORT int setNodeModelVar(const char *pIp, TY_MODEL_INFO *modelInfo, int modelNum);
	TAO_EXPORT int optNodeManualTestOneStep(const char *pIp, const char* oneStep, TY_STEP_STATUS *pStepStatus);	
#ifdef __cplusplus
}
#endif
