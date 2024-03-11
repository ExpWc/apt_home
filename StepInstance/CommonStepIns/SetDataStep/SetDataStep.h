/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-08 22:51:58
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-08 22:56:55
 * @FilePath: /ATP_V1.0.0/StepInstance/CommonStepIns/SetDataStep/SetDataStep.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "BaseStep.h"
#include "BaseICDStep.h"

#define ICDSTEPLOGFLAG  1
#define STEPLOGFLAG        2

extern "C"  CBaseStep* CreateStep();
class CSetDataStep : public CBaseICDStep
{
public:
    CSetDataStep();
    ~CSetDataStep();

public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep *pStep);
    virtual int init();
    virtual int run();

public:
    unsigned long long m_setValue;
    string m_type;
    string m_varName;
    string m_varType;
    string m_referenced;
    int m_logFlag;    //to handle log : 1 - card,to log ICDSTEPLOG,  2 - model stepLOG


private:
    int stringConvertType(string value, string type, 
        unsigned long long & setValue);
};