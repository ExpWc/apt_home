/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-07 23:51:46
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-07 23:51:47
 * @FilePath: /ATP_V1.0.0/StepBase/BaseICDStep.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once
#include "BaseStep.h"
#include "ICDManager.h"

#define ICDSTEPLOG(type, description, args...) \
	CBaseICDStep::log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)

class CBaseICDStep : public CBaseStep
{
public:
    CBaseICDStep();
    ~CBaseICDStep();

public:
    string m_fieldID;
    string m_ICDID;
    int init();
    int parse(TiXmlElement *pStepElement);
    int getData(double * data);
    int setData(unsigned long long data);
   
    //int setMultiData(unsigned long long data);
    //int getMultiData(double * data);
    int log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
			int lineNo, string msg);

private:
    int m_blockLen;
    int m_field;
    int m_byteOffset;
    int m_bitLen;
    int m_bitOffset;
};
