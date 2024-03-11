#pragma once
#include "BaseField.h"

class IntergerField :
    public BaseField
{
public:
    IntergerField(unsigned char _bitLen, unsigned int _idx, unsigned char _offset, const char* type, const char* name);
    virtual int encode(unsigned char* pData, int length) ;
    virtual int decode(unsigned char* pData, int length) ;
//ZR>>
    virtual void setData(unsigned long long data);
    virtual void getData(double* data);
    virtual void setDoubleData(unsigned long long data);
};


