#include "BaseICDStep.h"

CBaseICDStep::CBaseICDStep()
{

}
CBaseICDStep::~CBaseICDStep()
{

}

int CBaseICDStep::parse(TiXmlElement *pFieldElement)
{
    bool bRet = false;
    int ret = 0;

    //for (pFieldElement = pStepElement->FirstChildElement(); pFieldElement; 
    //    pFieldElement = pFieldElement->NextSiblingElement())
    {
        bRet = GetValue(pFieldElement, "FieldID", m_fieldID);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get FieldID failed\n");
            ret = -1;
        }
        REPORT(REPORT_INFO, "Get FieldID Success m_fieldID:%s\n", m_fieldID.c_str());
        bRet = GetValue(pFieldElement, "ICDID", m_ICDID);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get ICDID failed\n");
            ret = -1;
        }
        //REPORT(REPORT_INFO, "Get FieldID Success m_ICDID:%s\n", m_ICDID.c_str());
    }
    return ret;
}

int CBaseICDStep::init()
{
    //>>
    int ret = 0;
/*
    CICDManager *pICDManager = CICDManager::getInstance();
    if(pICDManager->getBlock(m_ICDID) == NULL)
    {
        REPORT(REPORT_ERROR, "Get m_ICDID failed\n");
        ret = -1;
    }    
    if(pICDManager->getBlock(m_ICDID)->getField(m_fieldID) == NULL)
    {
        REPORT(REPORT_ERROR, "Get m_fieldID failed\n");
        ret = -1;
    }*/
    //>>>
    REPORT(REPORT_DEBUG, "init\n");
    return ret;
}
inline  void showPac(char* pData,int length)
{
	//printf("=====file:%s,sfunction:%s line:%d,======length:%d\n",__FILE__,__FUNCTION__,__LINE__,length);
	for(int i=0;i<length;i++)
	{
		if(i%16==0)
		{
			printf("%02X\t",i/16);
		}
		printf("%02X ",pData[i]&0xFF);
		if((i+1)%8==0)
		{
			printf("\t");
			if((i+1)%16==0)
			{
				printf("\n");
			}
		}
	}
}

int CBaseICDStep::getData(double * data)
{
    int ret = 0;
    CICDManager *pICDManager = CICDManager::getInstance();
    CBlock *pBlock = pICDManager->getBlock(m_ICDID);
	if(pBlock == NULL)
	{
		REPORT(REPORT_ERROR, "Get block failed\n");
		m_msg = string("Get block failed\n");
		m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
		//log>>
		m_msg = string("Not found ICDID:") + m_ICDID;
		ICDSTEPLOG(LOG_ERROR, m_msg);
		//log<<
#endif
		return -1;
	}
	//printf("\nRXBUFFER icdID:%s fieldID:%s\n",m_ICDID.c_str(), m_fieldID.c_str());
	//showPac((char*)pBlock->m_rxBuffer,500);
	BaseField* pField = pBlock->getField(m_fieldID);
	if(pField == NULL)
	{
		REPORT(REPORT_ERROR, "Get field:%s failed\n", m_fieldID.c_str());
		m_msg = string("Get field failed\n");
		m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
		//log>>
		m_msg = string("Not found fieldID:") + m_fieldID;
		ICDSTEPLOG(LOG_ERROR, m_msg);
		//log<<
#endif
		return -1;
	}
    //REPORT(REPORT_INFO, " CCompareStep::run >>>>>\n");
    
    //pBlock->printFieldValue();
    pBlock->unpackRxICD();
    m_blockLen = pBlock->m_length;

   //pBlock->printFieldValue();
    pField->getData(data);
    m_byteOffset = pField->m_byteOffset;
    m_bitLen = pField->m_bitLen;
    m_bitOffset = pField->m_bitOffset;
    return ret;
}

int CBaseICDStep::setData(unsigned long long data)
{
    int ret = 0;
    CICDManager *pICDManager = CICDManager::getInstance();
    CBlock *pBlock = pICDManager->getBlock(m_ICDID);
	if(pBlock == NULL)
	{
		REPORT(REPORT_ERROR, "Get block failed\n");
		m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
		//log>>
		double tmpDouble = 0;
		memcpy(&tmpDouble, &data, 8);
		m_msg = string("Not found ICDID:") + m_ICDID;
		ICDSTEPLOG(LOG_ERROR, m_msg);
		//log<<
#endif
		return -1;
	}

	BaseField* pField = pBlock->getField(m_fieldID);
	//REPORT(REPORT_ERROR, "Get field:m_ICDID:%s, m_fieldID:%s,m_length:%d,data:%lld\n",m_ICDID.c_str(),m_fieldID.c_str(),pBlock->m_length,data);
	if(pField == NULL)
	{
		REPORT(REPORT_ERROR, "Get field failed, icdId:%s, fieldId:%s\n",m_ICDID.c_str(),m_fieldID.c_str());
		m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
		//log>>
		m_msg = string("Not found fieldID:") + m_fieldID;
		ICDSTEPLOG(LOG_ERROR, m_msg);
		//log<<
#endif
		return -1;
	}

    pField->setDoubleData(data);
    m_blockLen = pBlock->m_length;

    pBlock->packTxICD();

  m_byteOffset = pField->m_byteOffset;
	m_bitLen = pField->m_bitLen;
	m_bitOffset = pField->m_bitOffset;

    return ret;
}

// int setMultiData(unsigned long long data)
// {
// 	juest pField->m_type;
// setBool(bool val)
// {
//     m_val.llVal = val;
// }
// void setChar(char val)
// }

int CBaseICDStep::log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
		int lineNo, string msg)
{
	char tmp[1024];
	sprintf(tmp, "icdID:%s-len:%d fieldID:%s %d-%d-%d\n",
			m_ICDID.c_str(), m_blockLen,
			m_fieldID.c_str(), m_byteOffset, m_bitLen, m_bitOffset);
	string msgTmp = string(tmp) + msg;
	CBaseStep::log(logType, fileName, funName, lineNo, msgTmp);
}
