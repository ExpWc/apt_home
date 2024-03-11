#pragma once

void* threadCaseExecute(void *pParam);


/*enum CommandTypeE
{
	COMMAND_SET_DATA, //0,
	COMMAND_SEND, 
	COMMAND_RECEIVE,
	COMMAND_WAIT,
	COMMAND_COMPARE
};


typedef struct _comparecommand
{
	char command[32];
	char stepId[128];
	char stepName[64];
	char expectation[8];
	char upperLimit[128];
	char lowLimit[128];
}TY_COMPARECOMMAND;
typedef struct _receivecommand
{
	char command[32];
	char stepId[128];
	char stepName[64];
	TY_ICD icd;
}TY_RECEIVECOMMAND;
typedef struct _waitcommand
{
	char command[32];
	char stepId[128];
	char stepName[64];
	char timeS[8];
	char timeUS[8];
}TY_WAITCOMMAND;
typedef struct _setsignalcommand
{
	char command[32];
	char stepId[128];
	char stepName[64];
	TY_STEPPARA stepPara;
	TY_ICDFIELD icdField;
}TY_SETSIGNALCOMMAND;

typedef struct _setdatacommand
{
	char command[32];
	char stepId[128];
	char stepName[64];
	char value[8];
	char type[8];
	TY_ICDFIELD icdField;
}TY_SETDATACOMMAND;

typedef struct _sendcommand
{
	char command[32];
	char stepId[128];
	char stepName[64];
	TY_ICD icd;
}TY_SENDCOMMAND;



typedef struct _step_info
{
	CommandTypeE commandState;
	char stepID[MAX_UUID_LENGTH];
	char name[MAX_NAME_LENGTH];
	void *commandData;
}TY_STEP_INFO;

type struct _case_info_
{
	char caseID[MAX_UUID_LENGTH];//用例的UUID
	char name[MAX_NAME_LENGTH];
	int stepNum;
	TY_STEP_INFO stepInfo[MAX_STEP_NUM];
}TY_CASE_INFO;

type struct _case_infos_
{
	int caseNum;
	TY_CASE_INFO caseInfo[MAX_CASE_NUM];
}TY_CASE_INFOS;*/

