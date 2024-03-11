#pragma once
#include <string.h>
#include <iostream>
#include "Varaint.h"
#include "tinyxml.h"
#include "parseXmlApi.h"
#include "HiReport.h"
using namespace std;


class BaseField
{
//private:
public:
    string m_name;
    Varaint m_srcValue;
    unsigned long long m_mask;
    unsigned char m_bitLen;
    unsigned int m_byteOffset; //�ֽ�ƫ��
    unsigned int m_bitOffset;//bitƫ��
    unsigned char m_shiftL;//����
    unsigned char m_shiftR;//����
    bool m_isUnsigned;//����λ
    int m_k;
    int m_b;
//ZR>>   
public:
    string m_type;
    string m_uuid;
    string m_fieldName;
    double m_defaultValue;
    double m_lsb;
    double m_offsetValue;
    int m_signed;
//ZR<<

public :
    BaseField(unsigned char _bitLen, unsigned int _idx, unsigned char _offset,const char* type,const char* name);
    virtual int encode(unsigned char* pData,int length)=0;
    virtual int decode(unsigned char* pData, int length)=0;
//ZR>>
    virtual void setDoubleData(unsigned long long data)=0;
    virtual void setData(unsigned long long data) = 0;
    virtual void getData(double* data) = 0;
    virtual int parseXML(const char * xmlData);
};

