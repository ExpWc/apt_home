#include "NoBusXmlParser.h"
#include "CommonType.h"
#include "BaseField.h"
#include "DoubleField.h"
#include "IntergerField.h"

CNoBusXmlParser::CNoBusXmlParser()
{

}
CNoBusXmlParser::~CNoBusXmlParser()
{

}

int CNoBusXmlParser::parse(TiXmlElement* pBlockElement, CBlock* pBlock)
{
    int ret = 0;
    ret = CAbstractXmlParser::parse(pBlockElement, pBlock);
    if(ret != 0)
    {
        REPORT(REPORT_ERROR, "No Bus Xml Parse Failed.\n");
        ret = -1;
    }
    BaseField* pField;
    if(pBlock->m_type == TypeAnalog)
    {
    	pBlock->m_length = 8;
		pField = new DoubleField(0, pBlock->m_ICDID.c_str());
		REPORT(REPORT_DEBUG, "double field new success%d\n", pField);
		
    }
    else if(pBlock->m_type == TypeDIO)
    {
    	//pField->m_bitLen = 1;
    	pBlock->m_length = 1;
    	pField = new IntergerField((unsigned char)1, 0,
    	        	     (unsigned char)0, "unsigned char", pBlock->m_ICDID.c_str());
        REPORT(REPORT_DEBUG, "Interger field new success%d\n", pField);
    }
    
    if(pField != NULL)
	{
    	pField->m_uuid = pBlock->m_ICDID;

       // REPORT(REPORT_DEBUG, "yes.===============%s\n", pField->m_uuid.c_str());
        pBlock->insertField(pField);
    }
    else
    {
        REPORT(REPORT_ERROR, "No Bus insert Field Failed.\n");
        ret = -1;
    }
    return ret;
}
