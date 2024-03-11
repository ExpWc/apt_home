#include "DoubleField.h"



DoubleField::DoubleField(unsigned int _idx, const char* name)
    :BaseField(64, _idx, 0,"double",name)
{
    m_isUnsigned = false;
}

int DoubleField::encode(unsigned char* pData, int length)
{
    double val = m_srcValue.getDouble();
    memcpy(pData + m_byteOffset, &val, sizeof(double));
   // REPORT(REPORT_ERROR, "====val : %lf. m_byteOffset:%d\n", val, m_byteOffset);
    return 0;
}

int DoubleField::decode(unsigned char* pData, int length)
{
    double val = 0;
    memcpy(&val, pData + m_byteOffset, sizeof(double));
    m_srcValue.setDouble(val);
    return 0;
}


//ZR>>
void DoubleField::setData(unsigned long long data)
{
    double tmp = 0;
    memcpy(&tmp, &data, 8);

    m_srcValue.setDouble(tmp);
}
void DoubleField::setDoubleData(unsigned long long data)
{
	double rtu = 0;
	double tmp = 0;
	double c = (double)data;
	memcpy(&tmp, &data, 8);
	//REPORT(REPORT_ERROR, "====tmp : %lf. c: %lf, data:%lld\n", tmp, c, data);
	m_srcValue.setDouble(tmp);
	rtu = m_srcValue.getDouble();
	//REPORT(REPORT_ERROR, "====rtu : %lf.\n", rtu);
}
void DoubleField::getData(double* data)
{
    double tmp = m_srcValue.getDouble();
    *data = tmp;
}
