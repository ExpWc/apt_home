#include "RFMCardManager.h"
#include "BaseRFMCard.h"
#include "Ini.h"
#include "HiReport.h"
//#include "DDSManager.h"
#include "ParseXml.h"
#include "ThreadImpl.h"

CRFMCardManager *CRFMCardManager::m_instance = NULL;

CCardManager* CreateInstance(char *name)
{
	return CRFMCardManager::getInstance(name);
}

CRFMCardManager* CRFMCardManager::getInstance(char *name)
{
	if (m_instance == NULL)
	{
		m_instance = new CRFMCardManager(name);
	}
	return m_instance;
}

CRFMCardManager::CRFMCardManager(char *name)
{
	//m_serverName = string(name);
	//m_simRunFlag = SIM_STOP;
}

CRFMCardManager::~CRFMCardManager(void)
{
}

int CRFMCardManager::createCardInstance(char* pData)
{
	/*load config and init card info*/
#if 1
	TiXmlElement *pBusElement = (TiXmlElement *)pData;
	TiXmlElement *pCardElement;
	int cardIndex = 0;
	const char* res = NULL;
	for(pCardElement=pBusElement->FirstChildElement("card");pCardElement!=NULL;pCardElement=pCardElement->NextSiblingElement())
	{
		res = pCardElement->Attribute("CardLibName");
		if(!isValidAttribute(res,"RFM:bus","CardLibName"))
		{
			return -1;
		}
		char filePath[256];
		getServerPath(filePath);
		string fullPath(filePath);
		fullPath.append(res);
		m_pCard[cardIndex] = getCardInstance(fullPath.c_str());
		m_pCard[cardIndex]->loadCardConfig((char*)pCardElement);
		cardIndex++;
	}
	m_cardNum = cardIndex;

	/*res = pBusElement->Attribute("RuleFile");
	if(!isValidAttribute(res,"bus","RuleFile"))
	{
		return -1;
	}*/
	//m_ruleFileName = res;

	REPORT(REPORT_INFO,"CSerialCardManager::card number %d\n",m_cardNum);

	res = pBusElement->Attribute("BusType");
	if(!isValidAttribute(res,"bus","BusType"))
	{
		//return -1;
	}
	m_busType = string(res);
	//log>>
	string msg = "create instance success";
	CARDMNGLOG(LOG_INFO, msg);
#else 0
	m_devId = devId;

	//get the config file name
	char filePath[STR_SHORT];
	getServerPath(filePath);
	m_configFileName.append(filePath);
	m_configFileName.append(m_serverName.c_str());
	m_configFileName.append(".conf");

	CIni iniFile;
	bool ret = false;
	iniFile.Read(m_configFileName.c_str());

	if (cardNum > MAX_CARD_NUM)
	{
		m_cardNum = MAX_CARD_NUM;
	}
	else
	{
		m_cardNum = cardNum;
	}

	char tmpStr[STR_SHORT];
	string cardLibName = "";
	sprintf(tmpStr, "cardLibName");
	ret = iniFile.GetValue(m_serverName.c_str(), tmpStr, cardLibName);
	if (!ret)
	{
		REPORT(REPORT_ERROR, "Can't get the config of cardLibName\n");
	}
	else
	{
		REPORT(REPORT_INFO, "Get cardLibName config:%s\n", cardLibName.c_str());
	}
	string tmpName = "";
	tmpName.append(filePath);
	tmpName.append(cardLibName.c_str());
	cardLibName = tmpName;
	
	for (int i = 0; i < m_cardNum; i++)
	{
		//create card instance from the lib
		m_pCard[i] = getCardInstance(cardLibName.c_str());

		//Convert CCard pointer to BaseRFMCard pointer.
		CommonCard *pCard = (CommonCard*) m_pCard[i];
		if (pCard==NULL)
		{
			REPORT(REPORT_ERROR, "getCardInstance failed\n");
			return -1;
		}
		pCard->loadCardConfig(m_configFileName.c_str(),m_serverName.c_str(),i);
	}
#endif
	return 0;
}

int CRFMCardManager::initCardInstance(char *msg)
{
	int ret = 0;
	
	string strInfo;
	char info[STR_SHORT];

	for (int i = 0; i < m_cardNum; i++)
	{
		if (m_pCard[i] != NULL)
		{
			CommonCard * pCard = (CommonCard *) this->m_pCard[i];
			//m_pCard[i]->m_statisticInfo.chnStatisticInfos = TY_CHN_STATISTIC_INFO_VEC(pCard->m_chnNum);
			ret = m_pCard[i]->initInstance(info);
			if (ret != 0)
			{
				strInfo.append(info);
				return ret;
			}
		}
	}
	
	/*init dds*/
	//CDDSManager* pDDSManager = CDDSManager::getInstance();
	//ret =pDDSManager->initUnknownPartition();

	/*init statistic info*/
	/*for (int i = 0; i < m_cardNum; i++)
	{
		m_pCard[i]->m_statisticInfo.numHilTotal = 0;
		m_pCard[i]->m_statisticInfo.numIllTotal = 0;

		clearCommonStatisticInfo(&m_pCard[i]->m_statisticInfo.statisticInfo);
		for (unsigned int j = 0; j < m_pCard[i]->m_chnNum; j++)
		{
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].numRecvFrmHil = 0;
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].numRecvFrmIll = 0;
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].numSendToCard = 0;
			clearCommonStatisticInfo(
				&m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].statisticInfo);
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].hwStatisticInfos.clear();
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].icdStatisticInfos.clear();

			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].chnID = j + 1;
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].statisticInfo.id = j + 1;
		}
	}*/

	return ret ;
}


std::string CRFMCardManager::getLibVersion()
{
	string libVersion = "CardManager Library Version: " ;
	libVersion.append(CARDMAMAGER_LIB_VERSION);
	if (m_cardNum>0)
	{
		libVersion.append(" Card Library Version:");
		libVersion.append(m_pCard[0]->getLibVersion());
	}
	
	return libVersion;
}

int CRFMCardManager::setCardRouter(char *pData, char *msg)
{
	int ret = 0;
	ret = pasreRouterXml((const char *) pData, msg, this);

	/*CDDSManager* pDDSManager = CDDSManager::getInstance();
	pDDSManager->initSimRxPartition();
	pDDSManager->initMonPartition();
	pDDSManager->initStiPartition();
	pDDSManager->initSimTxPartition();*/
	return ret;
}

int CRFMCardManager::initCard(char *pData, char *msg)
{
	int ret = 0;
	ret = pasreInitXml((const char*) pData, msg, this);
	if (ret != 0)
	{
		REPORT(REPORT_ERROR, "%s \n", msg);
		return -1;
	}

	for (int cardId = 0; cardId < m_cardNum; cardId++)
	{
		if (m_pCard[cardId]->m_initFlag == 1)
		{
			CommonCard * pCard = (CommonCard *)m_pCard[cardId];
			if (0 != pCard->init(msg))
			{
				REPORT(REPORT_ERROR, "Init card failed. slot:%d\n", pCard->m_cardSlot);
				return -1;
			}
			pCard->m_status = CARD_STATUS_OPEN;
			pCard->m_statusMsg = CARD_STATUS_OPEN_MSG;
		}
	}
	return ret;
}

int CRFMCardManager::startSim(char *msg)
{
	/*if (m_simRunFlag != SIM_STOP)
	{
		return 0;
	}*/
	/*
	//init statistic info
	for (int i = 0; i < m_cardNum; i++)
	{
		m_pCard[i]->m_statisticInfo.numHilTotal = 0;
		m_pCard[i]->m_statisticInfo.numIllTotal = 0;

		clearCommonStatisticInfo(&m_pCard[i]->m_statisticInfo.statisticInfo);
		for (unsigned int j = 0; j < m_pCard[i]->m_chnNum; j++)
		{
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].numRecvFrmHil = 0;
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].numRecvFrmIll = 0;
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].numSendToCard = 0;
			clearCommonStatisticInfo(
				&m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].statisticInfo);
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].hwStatisticInfos.clear();
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].icdStatisticInfos.clear();

			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].chnID = j + 1;
			m_pCard[i]->m_statisticInfo.chnStatisticInfos[j].statisticInfo.id = j + 1;
		}
	}
	

	m_simRunFlag = SIM_RUN;

	
#ifdef WIN32
	Sleep(1);
	//sendDataToDDS is defined in ThreadImpl.cpp
	m_sendDdsThreadID = CreateThread(NULL,NULL,sendDataToDDS,this,NULL,NULL);
	if (m_sendDdsThreadID==NULL)
	{
		REPORT(REPORT_ERROR,"pthread_create m_sendDdsThreadID failed \n");
		return -1;
	}
	//threadRecvFromIil is defined in ThreadImpl.cpp
	m_recvIilThreadID = CreateThread(NULL,NULL,threadRecvFromIil,this,NULL,NULL);
	if (m_recvIilThreadID==NULL)
	{
		REPORT(REPORT_ERROR,"pthread_create m_recvIilThreadID failed \n");
		return -1;
	}
	//threadRecvFromHil is defined in ThreadImpl.cpp
	m_recvHilThreadID = CreateThread(NULL,NULL,threadRecvFromHil,this,NULL,NULL);
	if (m_recvHilThreadID==NULL)
	{
		REPORT(REPORT_ERROR,"pthread_create m_recvHilThreadID failed \n");
		return -1;
	}

	   //////////////////////////////////////////////////////////////

	m_recvSilThreadID = CreateThread(NULL,NULL,threadRecvFromSil,this,NULL,NULL);
	if (m_recvSilThreadID==NULL)
	{
		REPORT(REPORT_ERROR,"pthread_create m_recvSilThreadID failed \n");
		return -1;
	}


#else
	usleep(1000);
	int thread_status = 0;
	thread_status = pthread_create(&m_sendDdsThreadID, NULL, sendDataToDDS,
		this);
	if (0 != thread_status)
	{
		REPORT(REPORT_ERROR, "pthread_create  sendDataToDDS failed \n");
		return -1;
	}
	thread_status = pthread_create(&m_recvHilThreadID, NULL, threadRecvFromHil,
		this);
	if (0 != thread_status)
	{
		REPORT(REPORT_ERROR, "pthread_create failed \n");
		return -1;
	}

	thread_status = pthread_create(&m_recvIilThreadID, NULL, threadRecvFromIil,
		this);
	if (0 != thread_status)
	{
		REPORT(REPORT_ERROR, "pthread_create failed \n");
		return -1;
	}

	/////////////////////////////////////////
	thread_status = pthread_create(&m_recvSilThreadID, NULL, threadRecvFromSil,
		this);
	if (0 != thread_status)
	{
		REPORT(REPORT_ERROR, "pthread_create failed \n");
		return -1;
	}


#endif
*/
	char errMsg[STR_LONG]={0};
	for (int cardId = 0; cardId < this->m_cardNum; cardId++)
	{
		if (m_pCard[cardId]->m_initFlag == 1)
		{
			CommonCard * pCard = (CommonCard *) this->m_pCard[cardId];

			pCard->m_status = CARD_STATUS_RUNNING;
			pCard->m_statusMsg = CARD_STATUS_RUNNING_MSG;
			for (unsigned int chn = 0; chn < pCard->m_chnNum; chn++)
			{
				if(pCard->m_chnValidFlag[chn] == 1)
				{
					pCard->m_chnStatus[chn].status = CARD_STATUS_RUNNING;
					pCard->m_chnStatus[chn].id = chn + 1;
					pCard->m_chnStatus[chn].statusMsg = CARD_STATUS_RUNNING_MSG;
				}
			}//end for chnNum

			//clear updataFlag for RFM
			TY_ROUTER_MAP::iterator pos;
			pos = pCard->m_sendMap.begin();
			while(pos != pCard->m_sendMap.end())
			{
				pos->second.updateCount = 0;
				pos++;
			}

			pos = pCard->m_recvMap.begin();
			while(pos != pCard->m_recvMap.end())
			{
				pos->second.updateCount = 0;
				pos++;
			}

			if (0 != pCard->start(errMsg))
			{
				REPORT(REPORT_ERROR, "Card start failed: %s\n", errMsg);
				return -1;
			}
		}
	}//end for cardNum
	
	return 0;

}

int CRFMCardManager::stopSim(char *msg)
{
	/*if (m_simRunFlag != SIM_RUN)
	{
		return 0;
	}
	m_simRunFlag = SIM_STOP;*/

	char errMsg[STR_LONG];
	for (int cardId = 0; cardId < this->m_cardNum; cardId++)
	{
		CommonCard * pCard = (CommonCard *) this->m_pCard[cardId];

		pCard->m_status = CARD_STATUS_OPEN;
        pCard->m_statusMsg = CARD_STATUS_OPEN_MSG;
        for (unsigned int chn = 0; chn < pCard->m_chnNum; chn++)
        {
			if(pCard->m_chnValidFlag[chn] == 1)
			{
				pCard->m_chnStatus[chn].status = CARD_STATUS_OPEN;
				pCard->m_chnStatus[chn].id = chn + 1;
				pCard->m_chnStatus[chn].statusMsg = CARD_STATUS_OPEN_MSG;
			}
        }//end for chnNum

        pCard->stop(errMsg);
	}//end for cardNum
/*
#ifdef WIN32
	WaitForSingleObject(m_recvHilThreadID,2000);
	WaitForSingleObject(m_recvIilThreadID,2000);
	WaitForSingleObject(m_sendDdsThreadID,2000);
	//if ()
	{
		WaitForSingleObject(m_recvSilThreadID,2000);
	}

#else

	pthread_join(m_recvIilThreadID, NULL);
	pthread_join(m_recvHilThreadID, NULL);
	pthread_join(m_sendDdsThreadID, NULL);
	//if ()
	{
		pthread_join(m_recvSilThreadID, NULL);
	}
#endif
*/
	return 0;
}

int CRFMCardManager::deleteCardInstance()
{
	char msg[STR_SHORT];
	/*if (m_simRunFlag == CCardManager::SIM_RUN)
	{
		stopSim(msg);
	}*/

	for (int card = 0; card < m_cardNum; card++)
	{
		m_pCard[card]->deleteInstance(msg);

	}
	/*CDDSManager *pDDSManager = CDDSManager::getInstance();
	pDDSManager->dispose();*/
	return 0;
}

int selfcheckFlag = 0;
void selfcheckcat(char *info, char *tmpInfo)
{
	if(selfcheckFlag == 0)
	{
		if((strlen(info) + strlen(tmpInfo))  > SELF_CHECK_INFO_LEN)
		{
			selfcheckFlag = 1;
			strcat(info + strlen(info) - strlen("selfcheck info is full\n"), "selfcheck info is full\n");
			return;
		}
		strcat(info, tmpInfo);
	}

}

int CRFMCardManager::selfcheck(char *info)
{
	/*
	char tmpInfo[SELF_CHECK_INFO_LEN];
	memset(tmpInfo, 0, sizeof(tmpInfo));

	selfcheckFlag = 0;
	FILE * fd = fopen("RFMSelfCheck.log","w");
	selfcheckcat(info, "selfcheck info will write into the file :RFMSelfCheck.log\n");

	sprintf(tmpInfo, "Service status ok.");
	selfcheckcat(info, tmpInfo);
	fprintf(fd, "%s", tmpInfo);

	sprintf(tmpInfo, "Card number : %d\n", m_cardNum);
	selfcheckcat(info, tmpInfo);
	fprintf(fd, "%s", tmpInfo);

	for (int i = 0; i < m_cardNum; i++)
	{
		//selfcheck info
		CommonCard * pCard = (CommonCard *) this->m_pCard[i];
		sprintf(tmpInfo, "=====Card slot:%d %s %s=====\n", pCard->m_cardSlot,
			pCard->m_cardName.c_str(), pCard->m_statusMsg.c_str());
		selfcheckcat(info, tmpInfo);
		fprintf(fd, "%s", tmpInfo);
		for (unsigned int j = 0; j < m_pCard[i]->m_chnNum; j++)
		{
			sprintf(
				tmpInfo,
				"Chn %02d\tinit %d\trecvFromHil %d\trecvFromIil %d\tsendToCard %d\tsendToDDS %d\t ",
				j,
				pCard->m_chnValidFlag[j],
				pCard->m_statisticInfo.chnStatisticInfos[j].numRecvFrmHil,
				pCard->m_statisticInfo.chnStatisticInfos[j].numRecvFrmIll,
				pCard->m_statisticInfo.chnStatisticInfos[j].numSendToCard,
				pCard->m_statisticInfo.chnStatisticInfos[j].statisticInfo.msgNum
				);
			selfcheckcat(info, tmpInfo);
			fprintf(fd, "%s", tmpInfo);

			sprintf(tmpInfo,"unknown %d\n",
				pCard->m_statisticInfo.chnStatisticInfos[j].statisticInfo.uMsgNum);
			selfcheckcat(info, tmpInfo);
			fprintf(fd, "%s", tmpInfo);
		}

		fprintf(fd, "======TxRouterInfo:%d======\n",pCard->m_sendMap.size());

		TY_ROUTER_MAP::iterator tpos;
		for (tpos = pCard->m_sendMap.begin(); tpos != pCard->m_sendMap.end(); tpos++)
		{
			fprintf(fd, " \tDDSKey: %d\n", tpos->second.ddsId);
		}
		fprintf(fd, "======RxRouterInfo:%d======\n",pCard->m_recvMap.size());

		TY_ROUTER_MAP::iterator rpos;
		for (rpos = pCard->m_recvMap.begin(); rpos != pCard->m_recvMap.end(); rpos++)
		{
			fprintf(fd, " \tDDSKey: %d\n", rpos->second.ddsId);
		}
	}
	fclose(fd);
	*/
	return 0;
}
