#pragma once
//#ifndef _H_BaseStep_API
//#define _H_BaseStep_API
#include "PublicInclude.h"
#include "CommonCase.h"
#include "HiLog.h"


#define STEPLOG(type, description, args...) \
	CBaseStep::log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)

class CBaseStep
{
public:
    CBaseStep();
    virtual ~CBaseStep();

public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep *pStep);
    virtual int init();
    virtual int run();
    virtual int stop();
    virtual int setStepValue(int frequency);
    virtual int getStatus();
    virtual string getMsg();
    virtual CBaseStep* getSubStep(string subStepID);
    virtual int log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
			int lineNo, string msg);
    virtual string getAttachInfo();
    virtual int reset();

public:
    string m_stepID;
    string m_command;
    string m_stepName;
    string m_msg;
    int m_periodMs;
    int m_stepStatus;

};
//#endif
