#include "Block.h"

CBlock::CBlock()
{

}
CBlock::~CBlock()
{
	memset(m_txBuffer, 0, MAX_PAYLOAD_LEN);
	memset(m_rxBuffer, 0, MAX_PAYLOAD_LEN);
}

int CBlock::insertField(BaseField* pField)
{
	TY_FIELD_MAP::iterator pos = m_fields.find(pField->m_uuid);
	if (pos != m_fields.end())
	{
		REPORT(REPORT_ERROR," Already have the field m_uuid : %s\n", pField->m_uuid.c_str());
		//printf("Already have the field m_uuid : %s\n", pField->m_uuid.c_str());
		return -1;
	}
	m_fields.insert(std::make_pair(pField->m_uuid.c_str(), pField));
	return 0;
}
int CBlock::reset()
{
	for (TY_FIELD_MAP::iterator pos = m_fields.begin(); pos != m_fields.end(); pos++)
	{

		delete pos->second;
		pos->second = NULL;
		
	}
	m_fields.clear();

	return 0;
}

int CBlock::unpack(unsigned char* pData, int length)
{
	//printFieldValue();
	for (TY_FIELD_MAP::iterator pos = m_fields.begin(); pos != m_fields.end(); pos++)
	{
		if (pos->second != NULL)
		{
			pos->second->decode(pData,length);
		}
	}

	return 0;
}
int CBlock::pack(unsigned char* pData, int& length)
{
	double var = 0;
	memset(pData, 0, length);
	for (TY_FIELD_MAP::iterator pos = m_fields.begin(); pos != m_fields.end(); pos++)
	{
		if (pos->second != NULL)
		{
			pos->second->encode(pData, length);
		}
		memcpy(&var, pData, 1);
		//REPORT(REPORT_ERROR," pData : %s, var:%lf, pData[0]= %02x,pData[1]= %02x,\n", pData, var, pData[0],pData[1]);
	}
	return 0;
}

BaseField* CBlock::getField(string fieldName)
{
	TY_FIELD_MAP::iterator pos = m_fields.find(fieldName);
	if (pos != m_fields.end())
	{
		return pos->second;
	}
	return NULL;
}

void CBlock::printFieldValue()
{
	for (TY_FIELD_MAP::iterator pos = m_fields.begin(); pos != m_fields.end(); pos++)
	{
		if (pos->second != NULL)
		{
			switch (pos->second->m_srcValue.type)
			{
			case V_BOOL:
				printf("%s \t%s \n", pos->second->m_name.c_str(), pos->second->m_srcValue.getBool() ? "true":"false");
				break;
			case V_CHAR:
			case V_SHORT:
			case V_INT:
			case V_LONG:
				printf("%s \t%d \n", pos->second->m_name.c_str(), pos->second->m_srcValue.getLong());
				break;
			case V_LONGLONG:
				printf("%s \t%lld \n", pos->second->m_name.c_str(), pos->second->m_srcValue.getLongLong());
				break;
			case V_UCHAR:
			case V_USHORT:
			case V_UINT:
			case V_ULONG:
				printf("%s \t%d \n", pos->second->m_name.c_str(), pos->second->m_srcValue.getULong());
				break;
			case V_ULONGLONG:
				printf("%s \t%d \n", pos->second->m_name.c_str(), pos->second->m_srcValue.getULongLong());
				break;
			case V_FLOAT:
				printf("%s \t%f \n", pos->second->m_name.c_str(), pos->second->m_srcValue.getFloat());
				break;
			case V_DOUBLE:
				printf("%s \t%f \n", pos->second->m_name.c_str(), pos->second->m_srcValue.getDouble());
				break;
			default:
				printf("%s \tUNKNOWN \n", pos->second->m_name.c_str());
				break;
			}
			
		}
	}
}


int CBlock::packTxICD()
{
	//double var1;
	/*m_length: init in create Block of Parse ICDInfo.xml, 429:4, analog:8, dio:1*/
	pack(m_txBuffer, m_length);
	//memcpy(&var1, m_txBuffer, m_length);
	//REPORT(REPORT_ERROR," m_length:%d, \n", m_length);
	
	return 0;
}
int CBlock::getTxData(unsigned char* txData, int& length)
{

	if(length > m_length)
	{
		length = m_length;
	}
	//double val = m_srcValue.getDouble();
	//If getting length of data is less, data will lose.
	memcpy(txData, m_txBuffer, length);
	//memcpy(&var, txData, length);
	//REPORT(REPORT_ERROR," var:%lf, m_length:%d\n", var, m_length);
	return length;
}
int CBlock::setRxData(unsigned char* rxData, int& length)
{
	if(length > m_length)
	{
		length = m_length;
	}
	memset(m_rxBuffer, 0, 2048);
	memcpy(m_rxBuffer, rxData, length);

	//printFieldValue();
	return length;
}
int CBlock::unpackRxICD()
{
	unpack(m_rxBuffer, m_length);
	return 0;
}
