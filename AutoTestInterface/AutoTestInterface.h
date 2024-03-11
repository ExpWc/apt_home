#ifndef __AUTO_TEST_INTERFACE_
#define __AUTO_TEST_INTERFACE_
#include <stdio.h>
#include <stdlib.h>
#define MAX_STRING_LEN 256
typedef char CHAR_ARRAY[MAX_STRING_LEN];

#ifdef __cplusplus
extern "C"
{
#endif

typedef union _valuefield
{
	double dValue;
	long long llValue;
	float fValue;
	long lValue;
	unsigned long uLongValue;
	int iValue;
	unsigned int uIntValue;
	short sValue;
	unsigned short uShortValue;
	char cValue;
	unsigned char uCharValue;
	char value[8];
}TY_VALUEFIELD;
typedef struct _param_val_
{
	char paramName[256];
	char paraReferenced[256];
	TY_VALUEFIELD value;
	unsigned int uParamLength;
	unsigned int offset;
}TY_PARAM_VAL;
/*
typedef struct _time_compare_{
	struct timeval tStart,tStop;
	int flag;
	struct timeval tOld;
	long long  ttold;
}TY_TIME_COMPARE;
*/
//typedef map<string, TY_TIME_COMPARE> REFRESH_MAP;
/*Set the transition flags for manual and automated tests: MANUAL_TEST_FLAG*/
extern __attribute__ ((visibility("default"))) int MANUAL_TEST_FLAG;

int parseXMLData(char* pSubStep, char* value);

int createDataDictionaryXml(char *testPath);
int creatDataDictionaryManualXml(char * testPath, const char *modelName, int modeSelect); //modelSelect: 0->manual, 1->simpole modify by wc 20231008

int getRunCycle(int *periodMs, int flag);

int setICDData(char *pIcdID, char *pFieldID, TY_VALUEFIELD value);
int getICDData(char *pIcdID, char *pFieldID, TY_VALUEFIELD* value);
int sendICDChnData(char *pIcdID, char *pCardType, unsigned int cardID);
int receiveICDChnData(char *pIcdID, char *pCardType, unsigned int cardID);

int setPresentStep(char* stepId);
int getPresentStep(char* stepId);
int setPresentCase(char* caseId);
int getPresentCase(char* caseId);
//int sendICDData(char* output);
//int receiveICDData(char* input, TY_VALUEFIELD& value);
int updateParamData(void* pParamComInfo, char *modelName);

int updateInputData(char *modelName);
int updateOutputData(char *modelName);
int parseCaseBinding(char* xmlPath);
int getCurveData(char *pCurveID, double* value);
int getCurveMapByStepID(char* stepID);
int parseCurveConfig(char* xmlPath);
int checkCurveStatus();
int storeModelVar(char *modelName);
//int getModelVar(char *varName, double *value);
int getModelVarInterface(char *varName, double *value);

int setModelVarInterface(char *varName, double value);
int getParamVal(char *modelName, TY_PARAM_VAL *pParamVal);
int setModelParamInterface(char *varName, double value);

int clearModelMap();
int clearModelVarMap();
int refreshRecvCache(const char* pIcdID, struct timeval t);
int resetICDData();

#ifdef __cplusplus
}

#endif



#endif

