#pragma once
#include "AbstractXmlParser.h"

class CGeneralXmlParser:public CAbstractXmlParser
{
public:
	CGeneralXmlParser();
	~CGeneralXmlParser();

	virtual int parse(TiXmlElement* pBlockElement, CBlock* pBlock);

};


