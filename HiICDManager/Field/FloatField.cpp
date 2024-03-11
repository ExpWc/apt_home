#include "FloatField.h"

FloatField::FloatField(unsigned int _idx, const char* name)
    :BaseField(32, _idx, 0,"float",name)
{
        m_isUnsigned = false;
}

int FloatField::encode(unsigned char* pData, int length)
{
    float val = m_srcValue.getFloat();
    memcpy(pData + m_byteOffset, &val, sizeof(float));
    return 0;
}

int FloatField::decode(unsigned char* pData, int length)
{
    float val = 0;
    memcpy(&val, pData + m_byteOffset, sizeof(float));
    m_srcValue.setFloat(val);
    return 0;
}


//ZR>>
void FloatField::setData(unsigned long long data)
{
    float tmp = 0;
    memcpy(&tmp, &data, 4);
    m_srcValue.setFloat(tmp);
}
void FloatField::setDoubleData(unsigned long long data)
{
    double tmp = 0;
    memcpy(&tmp, &data, 8);
    float tmpFloat = (float)tmp;
    //REPORT(REPORT_ERROR, "tmp : %lf.\n", tmp);
    m_srcValue.setFloat(tmpFloat);
}
void FloatField::getData(double* data)
{
    float tmp = m_srcValue.getFloat();
    double tmpDouble = (double)tmp;
    *data = tmpDouble;
}

