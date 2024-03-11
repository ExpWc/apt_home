#include "AbstractXmlParser.h"
#include "Block.h"
#include "CommonType.h"
#include "../Field/IntergerField.h"
#include "../Field/DoubleField.h"
#include "../Field/FloatField.h"
#include "../Field/LabelField.h"
#include "../Field/FixDataField.h"

CAbstractXmlParser::CAbstractXmlParser()
{

}
CAbstractXmlParser::~CAbstractXmlParser()
{

}
static inline VARAINT_TYPE getFieldType(const char *type, int fieldSigned, int fieldLength, string & dataType)
{
	VARAINT_TYPE varType;
    dataType = "";
	if(strcmp(type, "icd:FloatField") == 0)
	{
        if(fieldLength == 32)
		{
		    varType = V_FLOAT;
            dataType = "float";
		}
		else
		{
			return V_UNKNOWN;
		}
	}
    else if(strcmp(type, "icd:DoubleField") == 0)
	{
        if(fieldLength == 64)
		{
		    varType = V_DOUBLE;
            dataType = "double";
		}
		else
		{
			return V_UNKNOWN;
		}
	}
	else if((strcmp(type,"icd:IntegerField") == 0)) //|| (strcmp(type,"unicode") == 0))
	{
		if (fieldLength == 64)
		{
			if (fieldSigned == 1)
			{
				varType = V_LONG;
                dataType = "long";
			}
			else if (fieldSigned == 0)
			{
				varType = V_ULONG;
                dataType = "unsigned long";
			}
			else
			{
				return V_UNKNOWN;
			}
		}
		else if(fieldLength == 32)
		{
			if(fieldSigned == 1)
			{
				varType = V_INT;
                dataType = "int";
			}
			else if(fieldSigned == 0)
			{
				varType = V_UINT;
                dataType = "unsigned int";
			}
			else
			{
				return V_UNKNOWN;
			}
		}
		else if(fieldLength == 16)
		{
			if(fieldSigned == 1)
			{
				varType = V_SHORT;
                dataType = "short";
			}
			else if(fieldSigned == 0)
			{
				varType = V_USHORT;
                dataType = "unsigned short";
			}
			else
			{
			    return V_UNKNOWN;
			}
		}
		else if(fieldLength == 8)
		{
			if(fieldSigned == 1)
			{
				varType = V_CHAR;
                dataType = "char";
			}
			else if(fieldSigned == 0)
			{
				varType = V_UCHAR;
                dataType = "unsigned char";
	    	}
			else
			{
			    return V_UNKNOWN;
			}
		}
		else
		{
		    return V_UNKNOWN;
		}
	}
	else if(strcmp(type,"icd:BoolField") == 0)
	{
		varType = V_BOOL;
        dataType = "bool";
	}
    else
    {
    	if(fieldSigned == 1)
    	{
    		varType = V_LONGLONG;
    		dataType = "long long";
    	}
    	else
    	{
			varType = V_ULONGLONG;
			dataType = "unsigned long long";
    	}

    }
    
    return varType;
}

/*parse:type uuid name endianType size*/
int CAbstractXmlParser::parse(TiXmlElement* pBlockElement, CBlock* pBlock)
{
    int ret = 0;
    bool bRet = false;
    bRet = GetValue(pBlockElement, "xsi:type", pBlock->m_type);
    if (!bRet)
    {
        REPORT(REPORT_DEBUG, "Init.xml didn't find the node of xsi:type.\n");
        ret = -1;
    }
    bRet = GetValue(pBlockElement, "uuid", pBlock->m_ICDID);
    if (!bRet)
    {
        REPORT(REPORT_DEBUG, "Init.xml didn't find the node of uuid.\n");
        ret = -1;
    }
    bRet = GetValue(pBlockElement, "name", pBlock->m_name);
    if (!bRet)
    {
        REPORT(REPORT_DEBUG, "Init.xml didn't find the node of name.\n");
        ret = -1;
    }
    string endianTypeStr = "";
    bRet = GetValue(pBlockElement, "endianType", endianTypeStr);
    if (!bRet)
    {
        REPORT(REPORT_DEBUG, "Init.xml didn't find the node of endianType.\n");
        //ret = -1;
    }
    TY_ENDIAN_TYPE_E endianType = T_BIG_ENDIAN;
    if (endianTypeStr == "LITTLE_ENDIAN")
    {
        endianType = T_LITTLE_ENDIAN;
    }
    pBlock->m_endianType = endianType;
    string sizeStr  = "";
    bRet = GetValue(pBlockElement, "size", sizeStr);
    if (!bRet)
    {
        //REPORT(REPORT_WARNING, "Init.xml didn't find the node of size.\n");
        //ret = -1;
    }
    pBlock->m_length = atoi(sizeStr.c_str());

    return ret;
}

BaseField* CAbstractXmlParser::parseField(TiXmlElement *pFieldElement)
{
    int ret = 0;
    bool bRet = false;
    string tmpType = "";
    bRet = GetValue(pFieldElement, "xsi:type", tmpType);
    if (!bRet)
    {
        REPORT(REPORT_DEBUG, "Init.xml didn't find the node of xsi:type.\n");
        ret = -1;
    }
    string tmpSigned = "";
    bRet = GetValue(pFieldElement, "signed", tmpSigned);
    if (!bRet)
    {
      //  REPORT(REPORT_WARNING, "Init.xml didn't find the node of signed.\n");
        //ret = -1;
    }
    string tmpSize = "";
    bRet = GetValue(pFieldElement, "size", tmpSize);
    if (!bRet)
    {
        REPORT(REPORT_DEBUG, "Init.xml didn't find the node of size.\n");
        ret = -1;
    }
    int bitLen = atoi(tmpSize.c_str());
    int fieldSigned = 0;
    if(tmpSigned == "true")
    {
        fieldSigned = 1;
    }
    string typeStr;
    VARAINT_TYPE type = getFieldType(tmpType.c_str(), fieldSigned, bitLen, typeStr);///haimeixie
    if (type == V_UNKNOWN)
    {
        REPORT(REPORT_DEBUG, "Type Error.\n");
        ret = -1;
    }

    string tmpID = "";
    bRet = GetValue(pFieldElement, "uuid", tmpID);
    if (!bRet)
    {
        REPORT(REPORT_DEBUG, "Init.xml didn't find the node of uuid.\n");
        ret = -1;
    }
    string tmpOffset = "";
    int bitOffset = 0;
    bRet = GetValue(pFieldElement, "offset", tmpOffset);
    if (!bRet)
    {
       // REPORT(REPORT_WARNING, "Init.xml didn't find the node of offset.\n");
    }
    else
    {
        bitOffset = atoi(tmpOffset.c_str());
    }
    
    int byteOffset = bitOffset / 8;
    int bitOfByte = bitOffset % 8;
    //>>>>>>
    //BaseField(unsigned char _bitLen, unsigned char _idx/*m_byteOffset*/, unsigned char _offset/*m_bitOffset*/, const char* type, const char* name)
    BaseField* pField = NULL;
    if(type == V_FLOAT)
    {
        pField = new FloatField(byteOffset, tmpID.c_str());
    }
    else if(type == V_DOUBLE)
    {
        pField = new DoubleField(byteOffset, tmpID.c_str());
    }
    else
    {
        if(tmpType == "icd:FixdataField")
        {
        	pField = new CFixDataField(bitLen, byteOffset,
        	            	bitOfByte, typeStr.c_str(), tmpID.c_str());

//        	if(strcmp("4f27572e-4784-41a2-b146-1236aecdec2d",tmpID.c_str())==0)
//			{
//        		printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>%s byteOffset %d bitOffset %d\n", tmpID.c_str(),byteOffset,bitOffset);
//        		exit(0);
//			}
        }
        else if(tmpType == "icd:LabelField")
        {
        	pField = new CLabelField(bitLen, byteOffset,
            	bitOfByte, typeStr.c_str(), tmpID.c_str());

        }
        else
        {
        	pField = new IntergerField(bitLen, byteOffset,
        	     bitOfByte, typeStr.c_str(), tmpID.c_str());
        }
    }
    //<<<<<<
    
    string tmpDefaultValue = "";
    bRet = GetValue(pFieldElement, "defaultValue", tmpDefaultValue);
    if (!bRet)
    {
        //REPORT(REPORT_WARNING, "Init.xml didn't find the node of defaultValue.\n");
        //ret = -1;
    }
    if(tmpDefaultValue == "NULL")
    {
        pField->m_defaultValue = 0.0;
    }
    else
    {
        pField->m_defaultValue = atof(tmpDefaultValue.c_str());
    }
    pField->m_type = tmpType;
    pField->m_uuid = tmpID;
    pField->m_signed = fieldSigned;
    //pField->m_bitOffset = bitOffset;
    //printf("---field tmpID:%s type:%s bitLen:%d byteOffset:%d bitOffset:%d bitOfByte:%d m_signed:%d tmp:%s m_defaultValue:%lf\n",
//        tmpID.c_str(), pField->m_type.c_str(), bitLen, byteOffset, bitOffset, bitOfByte, pField->m_signed,
//        tmpDefaultValue.c_str(), pField->m_defaultValue);

    //pField->parseXml(pElement);
    pField->parseXML((const char *)pFieldElement);


    return pField;	
}
