#include "GeneralXmlParser.h"
#include "ICDManager.h"
#include "HiReport.h"

CGeneralXmlParser::CGeneralXmlParser()
{

}

CGeneralXmlParser::~CGeneralXmlParser()
{
	 
}
int CGeneralXmlParser::parse(TiXmlElement* pBlockElement, CBlock* pBlock)
{
	try
	{
		int ret = 0;
		ret = CAbstractXmlParser::parse(pBlockElement, pBlock);
		if(ret != 0)
		{
			return ret;
		}

		//Parse fields
		TiXmlElement *fieldElement = pBlockElement ->FirstChildElement("fields");
		for (fieldElement; fieldElement != NULL; fieldElement = fieldElement->NextSiblingElement("fields"))
		{
			BaseField* pField = parseField(fieldElement);
			if(pField != NULL)
			{
				pBlock->insertField(pField); 
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
	//REPORT(REPORT_INFO, "parse finish.\n");
	return 0;
}



