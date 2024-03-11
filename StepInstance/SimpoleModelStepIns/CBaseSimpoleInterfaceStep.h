/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-07 18:13:55
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-09 00:15:41
 * @FilePath: /ATP_V1.0.0/StepInstance/SimpoleModelStepIns/CBaseSimpoleInterfaceStep.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "BaseStep.h"
#include "AutoTestInterface.h"
#include "SimpoleInterfaceLinux.h" 

#define MAX_DATA_PAYLOAD_LEN      (2048+16)
#define MAX_VALUE_LEN              4
#define MAX_MODEL_MUM   16
#define MAX_MODEL_NAME_NUM 128
#define MAX_IP_LEN 16 
#define MAX_URL_LEN 256

#define COMSTEPLOG(type, description, args...) \
	CBaseSimpoleInterfaceStep::log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)

extern int gStartModelFlag;

extern "C"  CBaseStep* CreateStep();

class CBaseSimpoleInterfaceStep : public CBaseStep
{
public:
    CBaseSimpoleInterfaceStep();
    ~CBaseSimpoleInterfaceStep();

public:
    TY_ROUTER m_router;
    CBaseStep* m_pSubStep;

    char m_simpoleModelName[MAX_MODEL_MUM][MAX_MODEL_NAME_NUM];
    char m_addr[MAX_URL_LEN];
    int m_modelNum;
    pthread_t m_startModelRunThreadId;
    string m_simpoleIp;
    string m_controlModelStepName;
public:
	virtual int parse(TiXmlElement *pStepElement, CBaseStep *pStep);
	virtual int init();
	virtual int run();
	virtual string getAttachInfo();
public:
    int parseRouter(TiXmlElement *pStepElement);
    int log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
			int lineNo, string msg);
};
