#pragma once
#include "AbstractXmlParser.h"

class CA429XmlParser : public CAbstractXmlParser
{
public:
	CA429XmlParser();
	~CA429XmlParser();

	virtual int parse(TiXmlElement* blockElement, CBlock* pBlock);

private:
	int m_lable;
	BaseField* parseField(TiXmlElement *pFieldElement);

};


