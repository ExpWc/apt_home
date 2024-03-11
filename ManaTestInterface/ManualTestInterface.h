/*
 * @Author: chao.wu3
 * @Date: 2024-03-08 02:48:00
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-08 18:37:29
 * @FilePath: /ATP_V1.0.0/ManaTestInterface/ManualTestInterface.h
 * @Description: 用于手動測試接口的實現
 */

#ifndef __MANUAL_TEST_INTERFACE_
#define __MANUAL_TEST_INTERFACE_
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include "CommonCase.h"
#define MAX_STRING_LEN 256

#ifdef __cplusplus
extern "C"
{
#endif

//變量申明----
// manual test

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

int startManualTest ();
int stopManualTest (); 
int optManualTestOneStep(const char * pOneStep, TY_STEP_STATUS *pInfo);

int startModel (const char* pData);
int stopModel (const char* pModelName);
int getModelRunStatus (const char* pModelName, int modelStatus);
int getModelParam(TY_MODEL_INFO *  pModelInfo, int modelNum);
int setModelParam( TY_MODEL_INFO *  pModelInfo, int modelNum);
int getModelVar(TY_MODEL_INFO *  pModelInfo, int modelNum);
int setModelVar(TY_MODEL_INFO *  pModelInfo, int modelNum);

int setICDData (TY_SET_ICD_INFO *pIcdInfo, int icdNum);
int sendICDData(TY_SEND_ICD_INFO *pIcdInfo, int icdNum);
int getICDData(TY_GET_ICD_INFO *pIcdInfoIn, int icdNum);

#ifdef __cplusplus
}

#endif



#endif

