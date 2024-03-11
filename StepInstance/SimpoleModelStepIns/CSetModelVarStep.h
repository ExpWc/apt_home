#pragma once
#include "CBaseSimpoleInterfaceStep.h"
#include "HardwareManager.h"

#define MAX_TIME_NAME_S 64


class CSetModelVarStep : public CBaseSimpoleInterfaceStep
{
public:
    CSetModelVarStep();
    ~CSetModelVarStep();

    virtual int parse(TiXmlElement *pStepElement, CBaseStep* pStep);
    virtual int init();
    virtual int run();
    virtual string getAttachInfo();

public:
    int setParamValue();
    int setVarValue();

public:
    double m_modelVal;
    string m_varType;
    string m_dataType;

private:

   TY_PARA_DES m_pList;

};
