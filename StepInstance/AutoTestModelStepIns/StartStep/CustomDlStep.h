#ifndef _CUSTOMDLSTEP_H
#define _CUSTOMDLSTEP_H
#pragma once
#include <semaphore.h>
#include "BaseICDStep.h"
#include "AutoTestInterface.h"
#include "BaseStep.h"


typedef int (*dlDoInitializationTYPE)(char *);
typedef int (*dlDoOneStepTYPE)(unsigned long long);
typedef int (*dlDoDisposeTYPE)();
//typedef int (*dlSetXMLTYPE)(TiXmlElement *);
typedef int (*dlSetSubStepIDTYPE)(char *);
typedef int (*dlGetSubStepAttachInfoTYPE)(char *);

enum CustomDlStepStatusE
{
    STEP_NULL,
    STEP_INIT,
    STEP_RUN,
    STEP_PAUSE,
    STEP_STOP,
    STEP_FAILED
};
typedef struct _sub_param_info_
{

	string strParamName;
	string strParamType;
	string strReferenced;
	TY_VALUEFIELD value;;
	unsigned int uParamLength;
	unsigned int offset;
    int length;
   /*
	bool operator == (const string &pos)
	{
		return (this->strParamName == pos);
	}
	*/
}TY_PARAM_INFO;

//typedef struct _sub_param_info_
//{
//    string name;
//    string dataType;
//    int length;
//    char value[8];
//	bool operator == (const string &pos)
//	{
//		return (this->name == pos);
//	}
//}TY_PARAM_INFO;

typedef struct _sub_var_info_
{
	string strVarName;
    string strReferenced;
	string strIcdID;
	string strBindType;
	string strCurveID;
	string strCardType;
	unsigned int cardID;
	unsigned int chnID;
	string strFieldID;
	string strVarType;
	TY_VALUEFIELD value;
	//char value[8];
    string name;
    CBaseICDStep ICDInfo;
    TY_ROUTER m_router;
}TY_VAR_INFO;

//typedef struct _sub_var_info_
//{
//    string name;
//    char value[8];
//    CBaseICDStep ICDInfo;
//    TY_ROUTER m_router;
//}TY_VAR_INFO;


typedef union _valuefield_1
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
}TY_VALUEFIELD_1;

/*
typedef struct _sub_step_info_
{
    vector<TY_PARAM_INFO> paramInfo;
    vector<TY_VAR_INFO> inputVarInfo;
    vector<TY_VAR_INFO> outputVarInfo;
}TY_SUB_STEP_INFO;
*/
/***************************   manual test**************************************/

typedef struct _sub_step_info_
{
    vector<TY_PARAM_INFO> paramInfo;
    vector<TY_VAR_INFO> inputVarInfo;
    vector<TY_VAR_INFO> outputVarInfo;
}TY_SUB_STEP_INFO;


/***************************   manual test**************************************/


void* threadCustomStepExecute(void *pParam);

class CCustomDlStep : public CBaseStep
{
private:
    /* data */
public:
    CCustomDlStep(/* args */);
    //CCustomDlStep(int periodMs);
    ~CCustomDlStep();

    virtual int parse(TiXmlElement *pSubStepElement, CBaseStep *pStep);
    virtual int init();//set param icd init value;
    virtual int run();//start thread
    virtual int stop();
    virtual string getAttachInfo();
    int pause();
    int getStatus();
    int setStatus(int status);
    int simpoleRun();

public:
    string m_dlName;
    string m_modelName;
    string m_subStepID;
    TY_SUB_STEP_INFO m_subStepInfo;
    sem_t m_sem;
    int m_stepValue;
    void* dlHandle;
    int m_statusFlag;
    unsigned long long m_times;
    string m_attachInfo;
    TiXmlElement *m_pSubStepElement;

    int m_simpoleModelFlag; //modify by chao.wu 20231008
    int m_modelSourceFlag;

private:
    pthread_t m_customExecuteThreadId; 
    pthread_t m_simpoledataThreadId; // modify by chao.wu 20231008

    int initLibrary();
    int initICD();

public:    
    dlDoInitializationTYPE dlDoInitialization;
    dlDoOneStepTYPE dlDoOneStep;
    dlDoDisposeTYPE dlDoDispose;
//   dlSetXMLTYPE dlSetXML;
    dlSetSubStepIDTYPE dlSetSubStepID;
    dlGetSubStepAttachInfoTYPE dlGetSubStepAttachInfo;
};

typedef map<string, CCustomDlStep*> TY_MODEL_CARD_MAP;
extern TY_MODEL_CARD_MAP modelCard;
extern int gSimpoleModelStatus;
#endif
