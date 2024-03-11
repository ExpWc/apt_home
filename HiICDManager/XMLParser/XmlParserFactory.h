#pragma once
#include "AbstractXmlParser.h"
class CXmlParserFactory
{
public:
	CXmlParserFactory(void);
	~CXmlParserFactory(void);

	static CAbstractXmlParser *createXmlParser(const char* type);
};

