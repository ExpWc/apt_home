#pragma once
#include "BaseField.h"
class DoubleField :
    public  BaseField
{
public:
    DoubleField(unsigned int _idx, const char* name);
    virtual int encode(unsigned char* pData, int length);
    virtual int decode(unsigned char* pData, int length);
//ZR>>
    virtual void setData(unsigned long long data);
    virtual void getData(double* data);
    virtual void setDoubleData(unsigned long long data);
};

