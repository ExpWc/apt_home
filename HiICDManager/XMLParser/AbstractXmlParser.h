#pragma once
#include "tinyxml.h"
#include "Block.h"
#include "BaseField.h"

class CAbstractXmlParser
{
public:
	CAbstractXmlParser();
	~CAbstractXmlParser();
//ZR>>	
	virtual int parse(TiXmlElement* pBlockElement, CBlock* pBlock);
	BaseField* parseField(TiXmlElement *pFieldElement);
};
