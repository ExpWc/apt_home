#pragma once

#define MAX_CASE_NUM 128
#define MAX_UUID_LENGTH 64
#define MAX_NAME_LENGTH 64
#define MAX_STEP_NUM 128
#define MAX_CARD_NUM 32

enum CaseStepStatusE
{
	STATUS_READY,	//0锛屽緟杩愯
	STATUS_RUNNING, //1锛岃繍琛屼腑
	STATUS_SUCCESS, //2锛岃繍琛屽畬鎴愶紝鎴愬姛
	STATUS_FAILED,	//3锛岃繍琛屽畬鎴愶紝澶辫触
	STATUS_ERROR,    //4锛屽紓甯�
	STATUS_FINISH

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
	char caseID[64];	  //鐢ㄤ緥鐨刄UID
	int caseExecuteTimes; //鐢ㄤ緥鎵ц娆℃暟锛�>0
	int interval;		  //涓庝笅涓�涓渶鎵ц鐨勭敤渚嬩箣闂寸殑鏃堕棿闂撮殧锛屽崟浣島s
} TY_CASE_EXECUTE;

typedef struct _test_execute_
{
	int testExecuteNum;						   //娴嬭瘯鎵ц娆℃暟锛�>0--->涓暟
	int interval;							   //涓庝笅涓�娆￠渶鎵ц鐨勬祴璇曚箣闂寸殑鏃堕棿闂撮殧锛屽崟浣島s
	TY_CASE_EXECUTE caseExecute[MAX_CASE_NUM]; //鏌愪竴鐢ㄤ緥鎵ц
} TY_TEST_EXECUTE;

typedef struct _case_status_
{
	string caseID;//char caseID[64];//鐢ㄤ緥鐨刄UID
	CaseStepStatusE caseStepStatus; // 鐢ㄤ緥鐘舵��
} TY_CASE_STATUS;

typedef struct _step_status_
{
	string stepID;//char stepID[64];//姝ラ鐨刄UID
	CaseStepStatusE caseStepStatus; //姝ラ鐘舵��
	string msg;
	string attachInfo;
} TY_STEP_STATUS;

typedef struct _subStep_status_
{
	string stepID;//char stepID[64];//姝ラ鐨刄UID
	string subStepID;//char stepID[64];//姝ラ鐨刄UID
	CaseStepStatusE caseStepStatus; //姝ラ鐘舵��
	string msg;
	string attachInfo;
} TY_SUBSTEP_STATUS;
typedef struct _executive_info
{
  char preCaseID[64];
  char preStepID[64];
  long caseRunTimes;
  long successNum;
  long failNum;
  CaseStepStatusE runStatus;
}TY_ExecutiveInfo;

typedef struct _field_data_
{
	char icdID[64];	  //ICD鐨刄UID
	char fieldID[64]; //鍩熺殑UUID
	char data[8];	  //鍊�
} TY_FIELD_DATA;

typedef struct _card_status_
{
	long id;	 //鏉垮崱id
	long status; //鏉垮崱鐘舵��
} TY_CARD_STATUS;
typedef struct _server_status_
{
	long id;						  //鏈嶅姟id
	ServerRunStatusE serverRunStatus; //鏈嶅姟杩愯鐘舵��
	TY_CARD_STATUS cardStatus[MAX_CARD_NUM];
} TY_SERVER_STATUS;

typedef union hw_union
{
	unsigned long long HWKey;
	struct
	{
		unsigned int boardNo;
		unsigned int chnNo;
	}hwInfo;
}HW_UNION;

typedef struct _router_
{
    string command;
    string caseID;//
	string stepID;
	string icdID;
    string CardType;
	unsigned int CardID;
    unsigned int ChnID;
} TY_ROUTER;
typedef map<string, TY_ROUTER> TY_ROUTER_MAP;


typedef struct _case_statistic_
{
    int initStepNum;

    
}TY_CASE_STATISTIC ;
