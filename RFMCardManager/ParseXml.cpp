#include <stdio.h>
#include <string>
#include "ParseXml.h"

using namespace std;
const char xmlRootName[] = "xmlconfig";
#define FROM_FILE 0
#define FROM_DATA 1

static bool GetParaValue(TiXmlElement *element, const char * name,
		std::string &strVal)
{
	bool ret = false;

	TiXmlElement *paramElement;
	for (paramElement = element->FirstChildElement("parameter"); paramElement; paramElement
			= paramElement->NextSiblingElement("parameter"))
	{
		std::string tmp = paramElement->Attribute("Name");
		if (tmp == name)
		{
			ret = true;
			strVal = paramElement->Attribute("Value");
		}
	}
	return ret;
}

static bool GetValue(TiXmlElement *element, const char * name,
		std::string &strVal)
{
	bool ret = false;
	TiXmlAttribute *Attribute = NULL;
	for (Attribute = element->FirstAttribute(); Attribute; Attribute
			= Attribute->Next())
	{
		std::string tmp = Attribute->Name();
		if (tmp == name)
		{
			ret = true;
			strVal = Attribute->Value();
		}
	}
	return ret;
}


int strto_hex(string str)
{

	char tmp_name[256];
	strcpy(tmp_name, str.c_str());
	char tmp = 'x';
	char *pos;
	int temp = 0;
	int temp2 = 0;
	int len = str.size();
	char * str_name = strrchr(tmp_name, tmp);
	if (NULL != str_name)
	{
		pos = str_name + 1;
		len = len - 2;
	}
	else
	{
		pos = tmp_name;
	}

	for (int i = 0; i < len; i++)
	{

		temp2 = temp2 * 16;
		char c = *pos;
		if (c >= 'a' && c <= 'f')
			temp = (c - 'a') + 10;
		else if (c >= 'A' && c <= 'F')
			temp = (c - 'A') + 10;
		else
			temp = (c - '0');
		pos++;
		temp2 += temp;
	}
	return temp2;
}
#if 0
int pasreInitXml(const char *pData, char *msg, CRFMCardManager *pManager)
{
	//int i = 0;
	int id = 0;
	unsigned int chnId = 0;
	//unsigned int chnNum = 0;

	bool bRet = false;
	string tmpId = "";
	string tmpType = "";
	string tmpValue = "";
	string tmpName = "";
	TiXmlNode* initNode = NULL;
	TiXmlNode* rootNode = NULL;
	TiXmlElement *pTargetElement = NULL;
	TiXmlElement *pCardElement = NULL;
	TiXmlElement *pChnElemnet = NULL;
	TiXmlDocument *pXmlManager = (TiXmlDocument*) pData;

	//unsigned int cardIndex = 0;

	/*set card init flag to 0*/
	for (int i = 0; i < pManager->m_cardNum; i++)
	{
		pManager->m_pCard[i]->m_initFlag = 0;
		for (int j=0;j<pManager->m_pCard[i]->m_chnNum;j++)
		{
			pManager->m_pCard[i]->m_chnValidFlag[j] = 0;
		}
	}

	if (pXmlManager == NULL)
	{
		sprintf(msg, "%s", "Xml is not valid\n");
		return -1;
	}
	else
	{
		rootNode = pXmlManager->FirstChild(xmlRootName);
	}
	if (rootNode == NULL)
	{
		sprintf(msg, "Init.xml can't find the node of%s\n", xmlRootName);
		return -1;
	}
	else
	{
		initNode = rootNode->FirstChild("init");
	}
#ifdef WIN32 
	Sleep(1);
#else
	usleep(1000);
#endif 
	if (initNode == NULL)
	{
		REPORT(REPORT_ERROR, "Init.xml can't find the node of init\n");
		return -1;
	}
	else
	{
		pTargetElement = initNode->FirstChildElement();
		if (pTargetElement == NULL)
		{
			sprintf(msg, "%s",
					"Init.xml can't find the node of target\n");
			return -1;
		}

		for (pTargetElement = initNode->FirstChildElement(); pTargetElement; pTargetElement
				= pTargetElement->NextSiblingElement())
		{
			pCardElement = pTargetElement->FirstChildElement();
			if (pCardElement == NULL)
			{
				sprintf(msg, "%s",
						"Init.xml can't find the node of card\n");
			}
			for (pCardElement = pTargetElement->FirstChildElement(); pCardElement; pCardElement
					= pCardElement->NextSiblingElement())
			{
				tmpId = "";
				id = 0;
				/*get card id*/
				bRet = GetValue(pCardElement, "Id", tmpId);
				if (!bRet)
				{
					continue;
				}
				id = atoi(tmpId.c_str());

				CommonCard * pCard = NULL;
				for (int i = 0; i < pManager->m_cardNum; i++)
				{
					if (pManager->m_pCard[i]->m_cardSlot == id)
					{
						pCard = (CommonCard *) pManager->m_pCard[i];
					}
				}
				if (pCard == NULL)
				{
					REPORT(
						REPORT_ERROR,
						"Get card failed.pManager->m_pCard[i]->m_cardSlot:%d id:%d",
						pManager->m_pCard[0]->m_cardSlot, id);
					continue;
				}
				 pCard->m_initFlag = 1;
				/*get card Name*/
				tmpName = "";
				bRet = GetValue(pCardElement, "Name", tmpName); // name;
				if (!bRet)
				{
					continue;
				}

				/* parse the chn set*/
				pChnElemnet = NULL;
				for (pChnElemnet
						= pCardElement->FirstChildElement("channel"); pChnElemnet; pChnElemnet
						= pChnElemnet->NextSiblingElement())
				{
					/*get channel id*/
					bRet = GetValue(pChnElemnet, "Id", tmpValue);
					if (bRet)
					{
						chnId = atoi(tmpValue.c_str());
						chnId = chnId - 1;
						pCard->m_chnValidFlag[chnId] = 1;
						pCard->m_chnStatus[chnId].status = CARD_STATUS_OPEN;
						pCard->m_chnStatus[chnId].id = chnId + 1;
						pCard->m_chnStatus[chnId].statusMsg = CARD_STATUS_OPEN_MSG;
					}
					else
					{
						REPORT(REPORT_ERROR, "Get  chn id failed\n");
						continue;
					}
					
				}
			}
		}
	}

	return 0;
}
#endif

int pasreInitXml(const char *pData, char *msg, CRFMCardManager *pManager)
{
	/*get card slot*/
	int cardId = -1;
	TiXmlElement *pCardElement = NULL;
	pCardElement = (TiXmlElement *)pData;
	const char* res = pCardElement->Attribute("ID",&cardId);
	if(!isValidAttribute(res,"card","ID"))
	{
		return -1;
	}
	//printf("cardId:%d\n", cardId);
	/*get card ptr by slot*/
	CommonCard * pCard = NULL;
	for (int i = 0; i < pManager->m_cardNum; i++)
	{

		if (pManager->m_pCard[i]->m_cardSlot == cardId)
		{
			pCard = (CommonCard *) pManager->m_pCard[i];
		}
		if (pCard == NULL)
		{
			REPORT(
				REPORT_ERROR,
				"Get card failed.pManager->m_pCard[i]->m_cardSlot:%d cardId:%d",
				pManager->m_pCard[0]->m_cardSlot, cardId);
			//continue;
		}
		pCard->m_initFlag = 1;
		//printf("pCard->m_initFlag:%d\n", pCard->m_initFlag);
		pCard->m_sendMap.clear();
		pCard->m_recvMap.clear();
	}	
	/*CommonCard * pCard = (CommonCard *)pManager->getCard(cardId);
	if (pCard == NULL)
	{
		REPORT(REPORT_ERROR,"RFM can not find card(id = %d)",cardId);
		return -2;
	}
	printf("pCard:%x\n", pCard);*/
	/*get card type */
	res = pCardElement->Attribute("Type");
	if(!isValidAttribute(res, "card", "Type"))
	{
		return -1;
	}
	if(strcmp(res,"RFM")!=0)
	{
		REPORT(REPORT_ERROR,"Card %d incompatible bus type %s\n", cardId, res);
		return -1;
	}
	/*Bus Table*/
	TiXmlElement *pBusTableElement = pCardElement->FirstChildElement("busTable");
	if(pBusTableElement!=NULL)
	{
		TY_RFM_ROUTER router;
		TiXmlElement *pItemElement = pBusTableElement->FirstChildElement("item");
		for(pItemElement;pItemElement!=NULL;pItemElement=pItemElement->NextSiblingElement("item"))
		{
			/*UUID*/
			res = pItemElement->Attribute("UUID");
			if(!isValidAttribute(res,"item","UUID"))
			{
				return -1;
			}
			strcpy(router.uuid,res);
			//printf("router.uuid:%s\n", router.uuid);
			/*LENGTH*/
			res = pItemElement->Attribute("Length", &router.Length);
			if(!isValidAttribute(res,"item","Length"))
			{
				return -1;
			}
			//printf("router.Length:%d\n", router.Length);
			/*PERIOD*/
			int PeriodMs = 0;
			res = pItemElement->Attribute("PeriodMs",&PeriodMs);
			if(!isValidAttribute(res,"item","PeriodMs"))
			{
				return -1;
			}
			router.PeriodMs = PeriodMs;
			//printf("router.PeriodMs:%d\n", router.PeriodMs);
			/*ADDR*/
			res = pItemElement->Attribute("Addr");
			if(!isValidAttribute(res,"item","Addr"))
			{
				return -1;
			}
			router.Addr = strto_hex(res);
			//printf("router.addr:%x\n", router.Addr);
			/*DIRECTION*/
			res = pItemElement->Attribute("Direction");
			if (!isValidAttribute(res, "item", "Direction"))
			{
				return -1;
			}
			if (strcmp("Rx", res) == 0)
			{
				//m_chnInfo[0].direction = DIRECTION_RECV;//0->chnId
				router.Direction = DIRECTION_RECV;
			}
			else
			{
				router.Direction = DIRECTION_SEND;
			}
			if(router.Direction == DIRECTION_SEND)
			{
				TY_ROUTER_MAP::iterator pos;
				pos = pCard->m_sendMap.find(router.uuid);
				if (pos == pCard->m_sendMap.end())
				{
					pCard->m_sendMap.insert(make_pair(router.uuid, router));
					//printf("---uuid :%s chn %d period %d label %d sdi %d\n",router.uuid, router.chn, router.period,router.label,router.sdi);
					//printf("--====RFM-uuid :%s Addr 0x%x PeriodMs %d Direction %d Length %d\n",router.uuid, router.Addr, router.PeriodMs,router.Direction,router.Length);
				}
			}
			if(router.Direction == DIRECTION_RECV)
			{
				TY_ROUTER_MAP::iterator pos;
				pos = pCard->m_recvMap.find(router.uuid);
				if (pos == pCard->m_recvMap.end())
				{
					pCard->m_recvMap.insert(make_pair(router.uuid, router));
					//printf("--====RFM-uuid :%s Addr 0x%x PeriodMs %d Direction %d Length %d\n",router.uuid, router.Addr, router.PeriodMs,router.Direction,router.Length);
				}
			}
			printf("-------------------------------------------------------------\n");
		}/*for(pItemElemnet...*/
	}
	return 0;
}

#if 0
int pasreRouterXml(const char *pData, char *msg, CRFMCardManager *pManager)
{
	int dds_id = 0;
	int boardId = 0;
	unsigned int chnId = 0;
	TY_RFM_ROUTER router;
	bool bRet = false;

	string tmpId = "";
	string tmpValue = "";
	string tmpchNum = "";
	string tmpcardtype = "";

	TiXmlNode* firstChild = NULL;
	TiXmlNode* rootNode = NULL;
	TiXmlElement *pTargetElement = NULL;
	TiXmlElement *pCardElement = NULL;
	TiXmlDocument *pXmlManager = (TiXmlDocument*) pData;
	TiXmlElement *pItemElement = NULL;

	for (int i = 0; i < pManager->m_cardNum; i++)
	{
		CommonCard * pCard = (CommonCard *) pManager->m_pCard[i];
		pCard->m_sendMap.clear();
		pCard->m_recvMap.clear();
	}
	if (pXmlManager == NULL)
	{
		sprintf(msg, "%s", "Xml is not valid\n");
		return -1;
	}
	else
	{
		rootNode = pXmlManager->FirstChild(xmlRootName);
	}

	if (rootNode == NULL)
	{
		sprintf(msg, "Init.xml can't find the node of%s\n", xmlRootName);
		return -1;
	}
	else
	{
		firstChild = rootNode->FirstChild("routertable");
	}
#if 1
	if (firstChild == NULL)
	{
		sprintf(msg, "%s", "Init.xml can't find the node of routertable\n");
		return -1;
	}
	else
	{
		pTargetElement = firstChild->FirstChildElement();
		if (pTargetElement == NULL)
		{
			sprintf(msg, "%s",
					"Init.xml can't find the node of target\n");
			return -1;
		}

		for (pTargetElement = firstChild->FirstChildElement(); pTargetElement; pTargetElement
				= pTargetElement->NextSiblingElement())
		{
			pCardElement = pTargetElement->FirstChildElement();
			if (pCardElement == NULL)
			{
				sprintf(msg, "%s",
						"Init.xml can't find the node of card\n");
			}

			for (pCardElement = pTargetElement->FirstChildElement(); pCardElement; pCardElement
					= pCardElement->NextSiblingElement())
			{
				/*get card id*/
				boardId = 0;
				bRet = GetValue(pCardElement, "Id", tmpId);
				if (!bRet)
				{
					continue;
				}
				boardId = atoi(tmpId.c_str());

				CommonCard * pCard = NULL;
				for (int i = 0; i < pManager->m_cardNum; i++)
				{
					if (pManager->m_pCard[i]->m_cardSlot == boardId)
					{
						pCard = (CommonCard *) pManager->m_pCard[i];
					}
				}
				if (pCard == NULL)
				{
					continue;
				}

				/*get CardType*/
				string tmpName = "";
				bRet = GetValue(pCardElement, "Type", tmpName);
				if (!bRet)
				{
					continue;
				}

				/*get Name*/
				bRet = GetValue(pCardElement, "Name", tmpName);
				if (!bRet)
				{
					continue;
				}

				/* parse the chn set*/
				pItemElement = NULL;

				for (pItemElement = pCardElement->FirstChildElement(
						"item"); pItemElement; pItemElement
						= pItemElement->NextSiblingElement())
				{
					memset(&router, 0, sizeof(router)); //clear router


					/*get dds_id*/
					dds_id = 0;
					bRet = GetValue(pItemElement, "DDSId", tmpValue);
					if (bRet)
					{
						dds_id = atoi(tmpValue.c_str());
						router.ddsId = dds_id;
					}
					else
					{
						REPORT(REPORT_ERROR, "Get  dds_id id failed\n");
						continue;
					}

					/*get Address*/
					bRet = GetValue(pItemElement, "Address", tmpValue);
					if (!bRet)
					{
						REPORT(REPORT_ERROR, "Get router Address config failed\n");
						continue;
					}
					router.addr = strto_hex(tmpValue);

					/*get Length*/
					bRet = GetValue(pItemElement, "Length", tmpValue);
					if (!bRet)
					{
						REPORT(REPORT_ERROR, "Get router Length config failed\n");
						continue;
					}
					router.length = atoi(tmpValue.c_str());

					/*get SampleRate*/
					bRet = GetValue(pItemElement, "SampleRate", tmpValue);
					if (!bRet)
					{
						REPORT(REPORT_ERROR, "Get router SampleRate config failed\n");
						continue;
					}
					router.sampleRate = atoi(tmpValue.c_str());

					/*get Direction*/
					bRet = GetValue(pItemElement, "Direction", tmpValue);
					if (!bRet)
					{
						REPORT(REPORT_ERROR, "Get router Direction config failed\n");
						continue;
					}
					else
					{
						if (strcmp(tmpValue.c_str(), "RFM-SIL") == 0)
						{
							router.dataSrc = 0;
							if (pCard->m_chnInfo[0].sendFlag == DDS_TO_MON)
							{
								router.dataDst = DDS_TO_MON;
							}
							else
							{
								router.dataDst = DDS_TO_SIMRX;
							}
						}
						else if (strcmp(tmpValue.c_str(), "SIL-RFM") == 0)
						{
							router.dataSrc = DDS_TO_SIMRX;
							router.dataDst = 0;
						}
						else if (strcmp(tmpValue.c_str(), "HIL-RFM") == 0)
						{
							router.dataSrc = DDS_TO_SIMTX;
							router.dataDst = 0;
						}
						else if (strcmp(tmpValue.c_str(), "RFM-HIL") == 0)
						{
							router.dataSrc = 0;
							router.dataDst = DDS_TO_SIMTX;
						}
					}
					

					/*config Tx router map*/
					if (router.dataDst == 0)
					{
						TY_ROUTER_MAP::iterator pos;
						pos = pCard->m_sendMap.find(router.ddsId);
						if (pos == pCard->m_sendMap.end())
						{
							pCard->m_sendMap.insert(make_pair(router.ddsId,
								router));
						}
						else
						{
							REPORT(REPORT_WARNING, " key_dds %d is in the map.\n",
								router.ddsId);
						}

					}

					router.keyMark = router.addr|(router.length << 16);

					/*config Rx router map*/
					if (router.dataSrc == 0)
					{
						TY_ROUTER_MAP::iterator pos;
						pos = pCard->m_recvMap.find(router.keyMark);
						if (pos == pCard->m_recvMap.end())
						{
							pCard->m_recvMap.insert(make_pair(router.keyMark,
								router));
						}
						else
						{
							REPORT(REPORT_WARNING, " key_dds %d is in the map.\n",
								router.ddsId);
						}
					}
				}//end for (pItemElement 
			}//end for (pCardElement
		}//end for (pTargetElement
	}
#endif

	return 0;
}
#endif
