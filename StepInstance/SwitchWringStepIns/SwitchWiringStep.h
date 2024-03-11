/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-08 23:41:45
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-08 23:43:15
 * @FilePath: /ATP_V1.0.0/StepInstance/SwitchWringStepIns/SwitchWiringStep.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "BaseStep.h"
#include "AutoTestInterface.h"
#include <iostream>  
//#include <cpprest/http_client.h>
//#include <cpprest/filestream.h>
//#include <cpprest/json.h>
#include <errno.h>
#include "HiLog.h"
/*using namespace utility;                    
using namespace web;                         
using namespace web::http;                   
using namespace web::http::client;          
using namespace concurrency::streams;        
using namespace web::json;*/ 

#define URL "http://192.168.1.121:11011"  //  IP：Port

extern "C"  CBaseStep* CreateStep();

#define SWITCHWIRINGLOG(type, description, args...) \
	CSwitchWiringStep::log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)

class CSwitchWiringStep : public CBaseStep
{
public:
    CSwitchWiringStep();
    ~CSwitchWiringStep();

public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep *pStep);
    virtual int init();
    virtual int run();
    virtual int stop();
    //add log method
    virtual int log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
			int lineNo, string msg);

public:
    string m_value;
    string m_command;
    string m_type;
	string m_projectuuid;
};

