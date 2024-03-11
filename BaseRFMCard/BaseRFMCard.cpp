#include "BaseRFMCard.h"
#include "Ini.h"
#include "tinyxml.h"

CBaseRFMCard::CBaseRFMCard()
{

}

CBaseRFMCard::~CBaseRFMCard(void)
{
}

inline bool isValidAttribute(const char *res, const char *element, const char *attr)
{
	if(res == NULL)
	{
		REPORT(REPORT_ERROR, "Element %s cannot find attibut %s", element, attr);
		return false;
	}
	return true;
}

void CBaseRFMCard::loadCardConfig(const char *fileName)
{	
	TiXmlElement *pCardElement = (TiXmlElement *)fileName;
	const char *res = pCardElement->Attribute("CardSlot", &m_cardSlot);
	if(!isValidAttribute(res,"card", "CardSlot"))
	{
		return;
	}
}

void CBaseRFMCard::loadChannelConfig(const char* fileName)
{
}

int CBaseRFMCard::initInstance(char *msg)
{
	/*Open card*/
	unsigned int chn;
	int ret = 0;
	ret = open();
	if (ret != 0)
	{
		REPORT(REPORT_ERROR, "Open card failed.\n");
		return -1;
	}

	/*init the card name*/
	char tmpName[256];
	sprintf(tmpName, "RFM_%d", m_cardSlot);//CardName_id
	m_cardName = string(tmpName);
	m_cardType = "rfm";

	/*Set card para num */
	//m_paraNum = 0;

	/*Set card para*/
	/*m_para[m_paraNum].id = m_paraNum + 1;
	m_para[m_paraNum].name = "LibVersion";
	m_para[m_paraNum].value = string(getLibVersion());
	m_paraNum++;

	m_para[m_paraNum].id = m_paraNum + 1;
	m_para[m_paraNum].name = "CardType";
	m_para[m_paraNum].value = m_cardType;
	m_para[m_paraNum].defaultValue = m_cardType;
	m_paraNum++;*/

	/*set chn info and chn para*/
	for (chn = 0; chn < m_chnNum; chn++)
	{
		int chnParam = 0;

		m_chnInfo[chn].mode = MODE_STATIC;
		m_chnInfo[chn].id = chn + 1;

		m_chnInfo[chn].para_num = chnParam;
	}

	m_status = CARD_STATUS_CLOSE;
	m_statusMsg = CARD_STATUS_CLOSE_MSG;
	for (unsigned int i = 0; i < m_chnNum; i++)
	{
		m_chnStatus[i].status = CARD_STATUS_CLOSE;
		m_chnStatus[i].id = i + 1;
		m_chnStatus[i].statusMsg = CARD_STATUS_CLOSE_MSG;
	}

	return 0;
}

int CBaseRFMCard::deleteInstance(char *msg)
{
	int ret = 0;
	ret = close();
	if (ret != 0)
	{
		REPORT(REPORT_ERROR, "close card failed.\n");
		return -1;
	}

	return 0;
}

int CBaseRFMCard::writeData(string uuid, char* pData, int length)
{
	/*printf("uuid:%s\n", uuid.c_str());
	for(int i = 0;i < length;i++)
	{
		printf("pData[%d]:0x%x\n", i, pData[i]&0xff);
	}*/
	TY_ROUTER_MAP::iterator pos = m_sendMap.find(uuid);
	if(pos!=m_sendMap.end())
	{
		TY_RFM_DATA rfmData;
		memset(&rfmData, 0, sizeof(TY_RFM_DATA));
		strcpy(rfmData.uuid, uuid.c_str());
		rfmData.Addr = pos->second.Addr;
		rfmData.Length = length;
		memcpy(rfmData.payload, pData, length);
		sendDataToCard(rfmData);
	}
	else
	{
		REPORT(REPORT_ERROR,"sendDataToCard didn't found uuid %s.\n",uuid.c_str());
	}
	return 0;
}
