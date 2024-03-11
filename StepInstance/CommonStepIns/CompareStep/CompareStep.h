#pragma once
#include "BaseStep.h"
#include "BaseICDStep.h"

#define EQUAL_TYPE 1
#define GREATER_THAN_TYPE 2
#define LESS_THAN_TYPE 3
#define GREATER_AND_LESS_THAN_TYPE 4
#define TYPE_ICD 0
#define TYPE_MODEL 1

extern "C"  CBaseStep* CreateStep();

class CCompareStep : public CBaseICDStep
{
public:
    CCompareStep();
    ~CCompareStep();

public:
    virtual int parse(TiXmlElement *pStepElement, CBaseStep *pStep);
    virtual int init();
    virtual int run();
    virtual string getAttachInfo();

public:
    double m_expectation;
    double m_upperLimit;
    double m_lowLimit;
    string m_fieldID;
    string m_ICDID;
    string m_varName;
    string m_referenced;
    int m_type;
private:
    int m_compareType;
    double m_dataDouble;
    int compare(double data);
};
