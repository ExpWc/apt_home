#pragma once
#include "IntergerField.h"

class CFixDataField : public IntergerField
{

public:
	CFixDataField(unsigned char _bitLen, unsigned int _idx, unsigned char _offset,const char* type, const char* name);
	virtual int parseXML(const char * xmlData);
#if 0
	virtual void setDoubleData(unsigned long long data);
	  virtual int encode(unsigned char* pData, int length);
#endif 
};
