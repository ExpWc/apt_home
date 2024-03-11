#include "ThreadImpl.h"
#include "RFMCardManager.h"
#include "BaseRFMCard.h"
#include "DDSManager.h"
#define DDS_TMP_DATA_BUF_LEN 1024
#define MSG_READ_COUNT 10



#ifdef WIN32
unsigned long WINAPI threadRecvFromSil(void *pParam)
#else
void* threadRecvFromSil(void *pParam)
#endif
{
	CRFMCardManager *pCardManager = (CRFMCardManager*) pParam;
	CDDSManager *pDDSManager = CDDSManager::getInstance();
	TY_DDS_DATA *pRecvData =
		new TY_DDS_DATA[DDS_TMP_DATA_BUF_LEN];
	int dataLen = DDS_TMP_DATA_BUF_LEN;
	TY_RFM_DATA txMsg;
	int ret = 0;
	
	TY_RFM_DATA dataFlag;
	//int dataFlagAddr;
	

	/*take the history data ,and drop it */
	pDDSManager->takeSilMsgData(pRecvData, dataLen);
	TY_ROUTER_MAP::iterator pos;
	while (pCardManager->m_simRunFlag == CCardManager::SIM_RUN)
	{
		dataLen = DDS_TMP_DATA_BUF_LEN;
		pDDSManager->takeSilMsgData(pRecvData, dataLen);

		for (int i = 0; i < dataLen; i++)
		{
			for (int cardId = 0; cardId < pCardManager->m_cardNum; cardId++)
			{
				CommonCard * pCard =
					(CommonCard *) pCardManager->m_pCard[cardId];
				//m_statisticInfo 
				pos = pCard->m_sendMap.find(pRecvData[i].id);
				if (pos != pCard->m_sendMap.end())
				{
					txMsg.chnId = 0;
					txMsg.ddsKey = pRecvData[i].id;
					txMsg.addr = pos->second.addr;
					txMsg.length = pos->second.length;
					txMsg.sampleRate = pos->second.sampleRate;

					memcpy((void*) &txMsg.data, pRecvData[i].payload, pRecvData[i].length);

					if (pCard->m_cardInfo.updateFlag != 0)
					{
						dataFlag.addr = txMsg.addr - pCard->m_cardInfo.updateFlagByte;
						if (dataFlag.addr < 0)
						{
							REPORT(REPORT_ERROR,"updateFlagByte is greater than addr");
							return NULL;
						}
						if (pCard->m_cardInfo.updateFlag == 1)
						{
							pos->second.updateCount = !pos->second.updateCount;
						}
						else if (pCard->m_cardInfo.updateFlag == 2)
						{
							pos->second.updateCount++;
						}
						if (pCard->m_cardInfo.updateFlagByte > sizeof(pos->second.updateCount))
						{
							memcpy(dataFlag.data, &pos->second.updateCount,
								sizeof(pos->second.updateCount));
						}
						else
						{
							memcpy(dataFlag.data, &pos->second.updateCount,
								pCard->m_cardInfo.updateFlagByte);
						}
						dataFlag.length = pCard->m_cardInfo.updateFlagByte;
						dataFlag.sampleRate = 1;
						ret = pCard->sendDataToCard(dataFlag);

					}
					ret = pCard->sendDataToCard(txMsg);
					if(ret == 0)
					{
						pCard->m_statisticInfo.chnStatisticInfos[txMsg.chnId].numSendToCard++;
					}

					//pCard->m_statisticInfo.numHilTotal ++ ;
					//pCard->m_statisticInfo.chnStatisticInfos[txMsg.chnId].numRecvFrmHil++;
				

					break;
				}

			}
		}
#ifdef WIN32
		Sleep(1);
#else
		usleep(10);
#endif
	}
	delete[] pRecvData;
	return NULL;
}



/*********************************************
 * FunctionName :threadRecvFromHil
 * Parameter    :pParam - thread parameter
 * Description  :Thread receive data from simulation modules
 * Return       :
 *********************************************/
#ifdef WIN32
unsigned long WINAPI threadRecvFromHil(void *pParam)
#else
void* threadRecvFromHil(void *pParam)
#endif
{
	CRFMCardManager *pCardManager = (CRFMCardManager*) pParam;
	CDDSManager *pDDSManager = CDDSManager::getInstance();
	TY_DDS_DATA *pRecvData =
			new TY_DDS_DATA[DDS_TMP_DATA_BUF_LEN];
	int dataLen = DDS_TMP_DATA_BUF_LEN;
	TY_RFM_DATA txMsg;
	int ret = 0;
	TY_RFM_DATA dataFlag;

	/*take the history data ,and drop it */
	pDDSManager->takeHilMsgData(pRecvData, dataLen);
	TY_ROUTER_MAP::iterator pos;
	while (pCardManager->m_simRunFlag == CCardManager::SIM_RUN)
	{
		dataLen = DDS_TMP_DATA_BUF_LEN;
		pDDSManager->takeHilMsgData(pRecvData, dataLen);
		
		for (int i = 0; i < dataLen; i++)
		{
			for (int cardId = 0; cardId < pCardManager->m_cardNum; cardId++)
			{
				CommonCard * pCard =
					(CommonCard *) pCardManager->m_pCard[cardId];
				//m_statisticInfo 
				pos = pCard->m_sendMap.find(pRecvData[i].id);
				if (pos != pCard->m_sendMap.end())
				{

						txMsg.chnId = 0;
						txMsg.ddsKey = pRecvData[i].id;
						txMsg.addr = pos->second.addr;
						txMsg.length = pos->second.length;
						txMsg.sampleRate = pos->second.sampleRate;

						memcpy((void*) &txMsg.data, pRecvData[i].payload, pRecvData[i].length);

						if (pCard->m_cardInfo.updateFlag != 0)
						{
							dataFlag.addr = txMsg.addr - pCard->m_cardInfo.updateFlagByte;
							if (dataFlag.addr < 0)
							{
								REPORT(REPORT_ERROR,"updateFlagByte is greater than addr");
								return NULL;
							}
							if (pCard->m_cardInfo.updateFlag == 1)
							{
								pos->second.updateCount = !pos->second.updateCount;
							}
							else if (pCard->m_cardInfo.updateFlag == 2)
							{
								pos->second.updateCount++;
							}
							if (pCard->m_cardInfo.updateFlagByte > sizeof(pos->second.updateCount))
							{
								memcpy(dataFlag.data, &pos->second.updateCount,
									sizeof(pos->second.updateCount));
							}
							else
							{
								memcpy(dataFlag.data, &pos->second.updateCount,
									pCard->m_cardInfo.updateFlagByte);
							}
							dataFlag.length = pCard->m_cardInfo.updateFlagByte;
							dataFlag.sampleRate = 1;
							ret = pCard->sendDataToCard(dataFlag);

						}

						ret = pCard->sendDataToCard(txMsg);
						if(ret == 0)
						{
							pCard->m_statisticInfo.chnStatisticInfos[txMsg.chnId].numSendToCard++;
						}					

						pCard->m_statisticInfo.numHilTotal ++ ;
						pCard->m_statisticInfo.chnStatisticInfos[txMsg.chnId].numRecvFrmHil++;

					break;
				}

			}
		}
#ifdef WIN32
		Sleep(1);
#else
		usleep(10);
#endif
	}
	delete[] pRecvData;
	return NULL;
}

/*********************************************
 * FunctionName :threadRecvFromIil
 * Parameter    :pParam - thread parameter
 * Description  :Thread receive data from stimulation
 * Return       :
 *********************************************/
#ifdef WIN32
unsigned long WINAPI threadRecvFromIil(void *pParam)
#else
void* threadRecvFromIil(void *pParam)
#endif
{
	CRFMCardManager *pCardManager = (CRFMCardManager*) pParam;
	CDDSManager *pDDSManager = CDDSManager::getInstance();
	TY_DDS_DATA *pRecvData =
		new TY_DDS_DATA[DDS_TMP_DATA_BUF_LEN];
	int dataLen = DDS_TMP_DATA_BUF_LEN;
	TY_RFM_DATA txMsg;
	int ret = 0;
	TY_RFM_DATA dataFlag;

	/*take the history data ,and drop it */
	pDDSManager->takeIilMsgData(pRecvData, dataLen);
	TY_ROUTER_MAP::iterator pos;
	while (pCardManager->m_simRunFlag == CCardManager::SIM_RUN)
	{
		dataLen = DDS_TMP_DATA_BUF_LEN;
		pDDSManager->takeIilMsgData(pRecvData, dataLen);

		for (int i = 0; i < dataLen; i++)
		{
			for (int cardId = 0; cardId < pCardManager->m_cardNum; cardId++)
			{
				CommonCard * pCard =
					(CommonCard *) pCardManager->m_pCard[cardId];
				//m_statisticInfo 
				pos = pCard->m_sendMap.find(pRecvData[i].id);
				if (pos != pCard->m_sendMap.end())
				{
					txMsg.chnId = 0;
					txMsg.ddsKey = pRecvData[i].id;
					txMsg.addr = pos->second.addr;
					txMsg.length = pos->second.length;
					txMsg.sampleRate = pos->second.sampleRate;

					memcpy((void*) &txMsg.data, pRecvData[i].payload, pRecvData[i].length);

					if (pCard->m_cardInfo.updateFlag != 0)
					{
						dataFlag.addr = txMsg.addr - pCard->m_cardInfo.updateFlagByte;
						if (dataFlag.addr < 0)
						{
							REPORT(REPORT_ERROR,"updateFlagByte is greater than addr");
							return NULL;
						}
						if (pCard->m_cardInfo.updateFlag == 1)
						{
							pos->second.updateCount = !pos->second.updateCount;
						}
						else if (pCard->m_cardInfo.updateFlag == 2)
						{
							pos->second.updateCount++;
						}
						if (pCard->m_cardInfo.updateFlagByte > sizeof(pos->second.updateCount))
						{
							memcpy(dataFlag.data, &pos->second.updateCount,
								sizeof(pos->second.updateCount));
						}
						else
						{
							memcpy(dataFlag.data, &pos->second.updateCount,
								pCard->m_cardInfo.updateFlagByte);
						}

						dataFlag.length = pCard->m_cardInfo.updateFlagByte;
						dataFlag.sampleRate = 1;
						
						ret = pCard->sendDataToCard(dataFlag);

					}

					ret = pCard->sendDataToCard(txMsg);
					if(ret == 0)
					{
						pCard->m_statisticInfo.chnStatisticInfos[txMsg.chnId].numSendToCard++;
					}
					pCard->m_statisticInfo.numIllTotal ++ ;
					pCard->m_statisticInfo.chnStatisticInfos[txMsg.chnId].numRecvFrmIll ++;

					break;
				}
			}
		}
#ifdef WIN32
		Sleep(1);
#else
		usleep(10);
#endif
	}
	delete[] pRecvData;
	return NULL;
}

extern 	timeval startTime;
/*********************************************
 * FunctionName :sendDataToDDS
 * Parameter    :pParam - thread parameter
 * Description  :Thread receive data from card
 * 					and send the data to DDS
 * Return       :
 *********************************************/
#ifdef WIN32
unsigned long WINAPI sendDataToDDS(void *pParam)
#else
void* sendDataToDDS(void *pParam)
#endif
{
	REPORT(REPORT_INFO, "sendDataToDDS thread start success!\n");
	CRFMCardManager *pCardManager = (CRFMCardManager*) pParam;
	CDDSManager *pDDSManager = CDDSManager::getInstance();
	int ret = 0;
	int keyMark = 0;
	TY_HR_TIME recvTime;

	CHANNEL_UNION chnIDUnion;
	chnIDUnion.sn = 0;
	chnIDUnion.info.dev_type = IO_TYPE_COMMON_IO;
	chnIDUnion.info.dev_id = pCardManager->m_devId;
	unsigned short data_len = 0;
	TY_RFM_DATA* pRecvPac = new TY_RFM_DATA[MSG_READ_COUNT];

	while (pCardManager->m_simRunFlag == CRFMCardManager::SIM_RUN)
	{
		for (int cardId = 0; cardId < pCardManager->m_cardNum; cardId++)
		{
			CommonCard * pCard = (CommonCard *) pCardManager->m_pCard[cardId];
			chnIDUnion.info.card_id = pCard->m_cardSlot;
			if(pCard->m_status != CARD_STATUS_RUNNING)
			{
#ifdef WIN32
				Sleep(1);
#else
				usleep(10);
#endif
				continue;
			}
			for (int chnId = 0; chnId < pCard->m_chnNum; chnId++)
			{

				chnIDUnion.info.channel_id = chnId + 1;
				ret = pCard->recvDataFromCard(chnId,pRecvPac,MSG_READ_COUNT);
				
				if (ret>0)
				{
					int keyMark = 0;
					for (int i=0;i<ret;i++)
					{
						recvTime.sec = pRecvPac[i].sec;
						recvTime.usec = pRecvPac[i].usec;


						keyMark =  pRecvPac[i].addr | (pRecvPac[i].length << 16);


						TY_ROUTER_MAP::iterator pos =  pCard->m_recvMap.find(keyMark);
						if(pos!=pCard->m_recvMap.end())
						{
							pRecvPac[i].ddsKey = pos->second.ddsId;
						}
						else
						{
							pRecvPac[i].ddsKey = 0;
							pCard->m_statisticInfo.chnStatisticInfos[pRecvPac[i].chnId].statisticInfo.uMsgNum++;
							pCard->m_statisticInfo.statisticInfo.uMsgNum++;
						}
						pCard->m_statisticInfo.cardID = pCard->m_cardSlot;
						pCard->m_statisticInfo.statisticInfo.id = pCard->m_cardSlot;
						pCard->m_statisticInfo.statisticInfo.msgNum++;
						pCard->m_statisticInfo.statisticInfo.totalLen += pRecvPac[i].length;
						pCard->m_statisticInfo.statisticInfo.interTime = recvTime - pCard->m_statisticInfo.statisticInfo.lastTime;
						pCard->m_statisticInfo.statisticInfo.lastTime = recvTime;

						if(pCard->m_statisticInfo.statisticInfo.msgNum ==1)
						{
							pCard->m_statisticInfo.statisticInfo.maxTime
								= pCard->m_statisticInfo.statisticInfo.interTime;
							pCard->m_statisticInfo.statisticInfo.minTime
								=pCard->m_statisticInfo.statisticInfo.interTime;
						}
						else
						{
							if(pCard->m_statisticInfo.statisticInfo.maxTime
								< pCard->m_statisticInfo.statisticInfo.interTime)
								pCard->m_statisticInfo.statisticInfo.maxTime
								= pCard->m_statisticInfo.statisticInfo.interTime;
							if(pCard->m_statisticInfo.statisticInfo.minTime
								> pCard->m_statisticInfo.statisticInfo.interTime)
								pCard->m_statisticInfo.statisticInfo.minTime
								= pCard->m_statisticInfo.statisticInfo.interTime;
						}

						pCard->m_statisticInfo.chnStatisticInfos[pRecvPac[i].chnId].chnID = pRecvPac[i].chnId + 1;
						pCard->m_statisticInfo.chnStatisticInfos[pRecvPac[i].chnId].statisticInfo.id = pRecvPac[i].chnId  + 1;
						pCard->m_statisticInfo.chnStatisticInfos[pRecvPac[i].chnId].statisticInfo.msgNum++;
						pCard->m_statisticInfo.chnStatisticInfos[pRecvPac[i].chnId].statisticInfo.totalLen += pRecvPac[i].length;
						pCard->m_statisticInfo.chnStatisticInfos[pRecvPac[i].chnId].statisticInfo.interTime 
							= recvTime - pCard->m_statisticInfo.chnStatisticInfos[pRecvPac[i].chnId].statisticInfo.lastTime;
						pCard->m_statisticInfo.chnStatisticInfos[pRecvPac[i].chnId].statisticInfo.lastTime = recvTime;

						/*update chn icd statistic info*/
						TY_COMMON_STATISTIC_INFO_MAP::iterator statisticPos;
						statisticPos
							= pCard->m_statisticInfo.chnStatisticInfos[chnId].icdStatisticInfos.find(pRecvPac[i].ddsKey);
						if (statisticPos
							== pCard->m_statisticInfo.chnStatisticInfos[chnId].icdStatisticInfos.end())
						{
							TY_COMMON_STATISTIC_INFO statisticInfo;
							statisticInfo.id = pRecvPac[i].ddsKey;
							statisticInfo.lastTime = recvTime;
							statisticInfo.interTime = 0;
							statisticInfo.msgNum = 1;
							statisticInfo.totalLen = 0;
							statisticInfo.desc = "";
							pCard->m_statisticInfo.chnStatisticInfos[chnId].icdStatisticInfos.insert(
								make_pair(pRecvPac[i].ddsKey, statisticInfo));
						}
						else
						{
							//update the this icd info
							TY_COMMON_STATISTIC_INFO *pStatisticInfo =
								&(statisticPos->second);
							pStatisticInfo->msgNum++;
							pStatisticInfo->interTime = recvTime
								- pStatisticInfo->lastTime;
							pStatisticInfo->lastTime = recvTime;
							//pStatisticInfo->idr1 = ddsData[i].label;//tell itb this unknown info's hwInfo;

							if (pStatisticInfo->msgNum == 2)
							{
								pStatisticInfo->maxTime = pStatisticInfo->interTime;
								pStatisticInfo->minTime = pStatisticInfo->interTime;
							}
							else
							{
								//update the max min time
								if (pStatisticInfo->maxTime
									< pStatisticInfo->interTime)
								{
									pStatisticInfo->maxTime
										= pStatisticInfo->interTime;
								}
								if (pStatisticInfo->minTime
									> pStatisticInfo->interTime)
								{
									pStatisticInfo->minTime
										= pStatisticInfo->interTime;
								}
							}
						}						
					}
					//m_statisticInfo 

					pDDSManager->sendDDSMsgData(chnIDUnion.sn, pRecvPac, ret);
				}
			}//end for chn
		}//end for card
#ifdef WIN32
		Sleep(1);
#else
		usleep(10);
#endif

	}
	REPORT(REPORT_INFO, "sendDataToDDS thread exit!\n");
	return NULL;

}
