#include "XmlParserFactory.h"
#include "CommonType.h"
#include "A429XmlParser.h"
#include "GeneralXmlParser.h"
#include "NoBusXmlParser.h"

CXmlParserFactory::CXmlParserFactory(void)
{
}

CXmlParserFactory::~CXmlParserFactory(void)
{
}

CAbstractXmlParser * CXmlParserFactory::createXmlParser(const char* typeStr)
{
	try
	{
		
		//REPORT(REPORT_DEBUG, "typeStr:%s\n",typeStr);
		CAbstractXmlParser *pXmlParser = NULL;
		if (strcmp(typeStr, Type429) == 0)
		{
			//printf("%s\n", (char *)typeStr);
			pXmlParser = new CA429XmlParser();
		}
		else if (	(strcmp(typeStr, Type1553) == 0) ||
				 	(strcmp(typeStr, Type422) == 0)  ||
					(strcmp(typeStr, Type1394) == 0) ||
					(strcmp(typeStr, TypeCAN) == 0))
		{
			//printf("%s\n", (char *)typeStr);
			pXmlParser = new CGeneralXmlParser();
		}
		else if ((strcmp(typeStr, TypeAnalog) == 0) || (strcmp(typeStr, TypeDIO) == 0) )
		{
			//printf("%s\n", (char *)typeStr);
			pXmlParser = new CNoBusXmlParser();
			//REPORT(REPORT_ERROR, "CNoBusXmlParser---------------------->>>>\n");
		}
		else
		{
			//printf("%s\n", (char *)typeStr);
			char exceptionMsg[2048];
			sprintf(exceptionMsg,"ERROR: Unknown block type %s!(%s:%s:%d)",typeStr,__FILE__,__FUNCTION__
				,__LINE__);
			throw exceptionMsg;
		}
		return pXmlParser;
	}
	catch(const char* e)
	{
		printf(e);
		return NULL;
	}	
}
