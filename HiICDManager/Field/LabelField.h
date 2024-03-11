#pragma once
#include "IntergerField.h"

class CLabelField : public IntergerField
{

public:
	unsigned long long m_value;
public:
	CLabelField(unsigned char _bitLen, unsigned int _idx, unsigned char _offset,const char* type, const char* name);
	int parseXML(const char * xmlData);
};
