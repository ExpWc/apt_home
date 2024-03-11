#pragma once
#include "AbstractXmlParser.h"

class CNoBusXmlParser : public CAbstractXmlParser
{
public:
	CNoBusXmlParser();
	~CNoBusXmlParser();

	virtual int parse(TiXmlElement* pBlockElement, CBlock* pBlock);

};