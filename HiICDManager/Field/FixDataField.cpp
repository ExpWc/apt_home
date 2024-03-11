#include "FixDataField.h"

CFixDataField::CFixDataField(unsigned char _bitLen, unsigned int _idx, unsigned char _offset,const char* type, const char* name)
    :IntergerField(_bitLen,_idx,_offset, type,name)
{

}


int CFixDataField::parseXML(const char * xmlData)
{
	int ret = 0;
	bool bRet = false;
	TiXmlElement *pFieldElement = (TiXmlElement*)xmlData;
	//lsd
	string tmpLsb = "";
	bRet = GetValue(pFieldElement, "lsb", tmpLsb);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Init.xml didn't find the node of xsi:type.\n");
		ret = -1;
	}
	m_lsb = atof(tmpLsb.c_str());
	string tmp;
	bRet = GetValue(pFieldElement, "uuid", tmp);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Init.xml didn't find the node of xsi:type.\n");
		ret = -1;
	}
	//offset
	string tmpOffsetValue = "";
	bRet = GetValue(pFieldElement, "offsetValue", tmpOffsetValue);
	if (!bRet)
	{
		REPORT(REPORT_ERROR, "Init.xml didn't find the node of xsi:type.\n");
		ret = -1;
	}
	m_offsetValue = atof(tmpOffsetValue.c_str());

	return ret;
}
#if 0
void CFixDataField::setDoubleData(unsigned long long data)
{

    double tmp = 0;
    memcpy(&tmp, &data, 8);

   // REPORT(REPORT_INFO, "tmp : %lf.\n", tmp);
    m_srcValue.setDouble(tmp);
}
int CFixDataField::encode(unsigned char* pData, int length)
{

    double val = m_srcValue.getDouble();

     memcpy(pData + m_byteOffset, &val, sizeof(double));

	REPORT(REPORT_DEBUG, "val : %lf.\n", val);
    return 0;
}
#endif