#include "A429XmlParser.h"
#include "HiReport.h"
CA429XmlParser::CA429XmlParser()
{

}

CA429XmlParser::~CA429XmlParser()
{
	 
}

int CA429XmlParser::parse(TiXmlElement* pBlockElement, CBlock* pBlock)
{
	try
	{
		int ret = 0;
		ret = CAbstractXmlParser::parse(pBlockElement, pBlock);
		if(ret != 0)
		{
			return ret;
		}

		/*const char* labelStr = blockElement->Attribute("label");
		if(nameStr == NULL)
		{
			char exceptionMsg[2048];
			sprintf(exceptionMsg,"ERROR: Attribute <name> does not exist!(%s:%s:%d)\n",__FILE__,__FUNCTION__
						,__LINE__);
			throw exceptionMsg;
		}
		((CA429Block*)pBlock)->m_label = atoi(labelStr);*/

		pBlock->m_length = 4;

		//Parse fields
		TiXmlElement *fieldElement = pBlockElement ->FirstChildElement("fields");
		for (fieldElement; fieldElement != NULL; fieldElement = fieldElement->NextSiblingElement("fields"))
		{
			BaseField* pField = parseField(fieldElement);
			if(pField != NULL)
			{
				pBlock->insertField(pField); 

				pField->setData((unsigned long long)pField->m_defaultValue);

				if(pField->m_byteOffset == 0)
				{
					pField->setData((unsigned long long)m_lable);
					REPORT(REPORT_DEBUG, "This is 429 Lable field. lable %d\n", m_lable);
				}
			}
		}

	}
	catch (const char* errMsg)
	{
		REPORT(REPORT_ERROR,errMsg);
		delete pBlock;
		pBlock = NULL;
		return -1;
	}
	return 0;
}
BaseField* CA429XmlParser::parseField(TiXmlElement *pFieldElement)
{
	int ret = 0;
	BaseField* pField = NULL;
	pField = CAbstractXmlParser::parseField(pFieldElement);
	if(pField == NULL)
	{
		REPORT(REPORT_ERROR, "parse field faild\n");
		return NULL;
	}
	bool bRet = false;
	string valueStr;
	bRet = GetValue(pFieldElement, "value", valueStr);
	if (!bRet)
	{
		//REPORT(REPORT_INFO, "This is not 429 Lable field.\n");
	}
	else
	{
		m_lable = atoi(valueStr.c_str());

	}
	return pField;
}

