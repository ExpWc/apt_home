#include "LabelField.h"

CLabelField::CLabelField(unsigned char _bitLen, unsigned int _idx, unsigned char _offset,const char* type, const char* name)
    :IntergerField(_bitLen,_idx,_offset, type,name)
{

}

int CLabelField::parseXML(const char * xmlData)
{
	int ret = 0;
	bool bRet = false;
	TiXmlElement *pFieldElement = (TiXmlElement*)xmlData;
	string tmpStr = "";
	bRet = GetValue(pFieldElement, "value", tmpStr);
	//printf("--->%s", tmpStr.c_str());
	if (!bRet)
	{
		//REPORT(REPORT_INFO, "Init.xml didn't find the node of value.\n");
		ret = -1;
	}

	m_value = atoi(tmpStr.c_str());
	IntergerField::setData(m_value);
	return ret;
}
