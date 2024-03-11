/******************** (C) COPYRIGHT 2018 ********************
* File Name          : ThreadImpl.cpp
* Author             : hao of  
* Date               : 2018/07/06
* Description        :
*******************************************************************/

#include "ThreadImpl.h"
#include "Mil1394CardManager.h"
#include "BaseMil1394Card.h"
#include "DDSManager.h"
#include "CStorageManager.h"
#include "DataType.h"

#define DDS_TMP_DATA_BUF_LEN 1024
#define MSG_READ_COUNT 1024

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
	Mil1394CardManager *pCardManager = (Mil1394CardManager*) pParam;
	CDDSManager *pDDSManager = CDDSManager::getInstance();
	TY_DDS_DATA *pRecvData =
			new TY_DDS_DATA[DDS_TMP_DATA_BUF_LEN];
	int dataLen = DDS_TMP_DATA_BUF_LEN;
	TY_HR_1394_MSG txMsg;
	int ret =0;

	/*take the history data ,and drop it */
	pDDSManager->takeHilMsgData(pRecvData, dataLen);
	TY_ROUTER_MAP::iterator pos;
	while (pCardManager->m_simRunFlag == CCardManager::SIM_RUN)
	{
		dataLen = 0;
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
					if (pCard->m_chnValidFlag[pos->second.chnId])
					{
						txMsg.channelId = pos->second.chnId;
						memcpy((void*) &txMsg.payload, pRecvData[i].dataBuf,pRecvData[i].length);
						ret =pCard->sendDataToCard(txMsg);
						if(ret == 1)
						{
							pCard->m_statisticInfo.chnStatisticInfos[txMsg.channelId].numSendToCard++;
						}
						pCard->m_statisticInfo.numHilTotal ++ ;
						pCard->m_statisticInfo.chnStatisticInfos[txMsg.channelId].numRecvFrmHil++;
					}

					break;
				}

			}
		}
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
	Mil1394CardManager *pCardManager = (Mil1394CardManager*) pParam;
	CDDSManager *pDDSManager = CDDSManager::getInstance();
	TY_DDS_DATA *pRecvData =
		new TY_DDS_DATA[DDS_TMP_DATA_BUF_LEN];
	int dataLen = DDS_TMP_DATA_BUF_LEN;
	TY_HR_1394_MSG txMsg;
	int ret =0;

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
					if (pCard->m_chnValidFlag[pos->second.chnId])
					{
						
						txMsg.channelId  = pos->second.chnId;
						txMsg.length    = pRecvData[i].length;
						txMsg.ddsId     = pRecvData[i].id;
						memcpy((void*) &txMsg.payload, pRecvData[i].dataBuf,pRecvData[i].length);
						ret = pCard->sendDataToCard(txMsg);
						if(ret == 1)
						{
							pCard->m_statisticInfo.chnStatisticInfos[txMsg.channelId].numSendToCard++;
						}
						pCard->m_statisticInfo.numIllTotal ++ ;
						pCard->m_statisticInfo.chnStatisticInfos[txMsg.channelId].numRecvFrmIll ++;

					}

					break;
				}
			}
		}
	}
	delete[] pRecvData;
	return NULL;
}

extern 	timeval startTime;

void storageData(CStorageManager *pStorageManager, TY_STORAGE_DATA *pData, TY_STORAGE_1394_DATA *pStorageData,
		TY_HR_1394_MSG pRecvPac, unsigned int src)
{
	//set value
	memset(pData, 0, sizeof(TY_STORAGE_DATA));
	memset(pStorageData, 0, sizeof(TY_STORAGE_1394_DATA)+ MAX_1394_PAYLOAD_LEN);
	pData->id = pRecvPac.ddsId;
	pData->src = src;
	pData->sec = pRecvPac.sec;
	pData->usec = pRecvPac.usec;

	pStorageData->id = pRecvPac.ddsId;
	pStorageData->msgId = pRecvPac.msgId;
	pStorageData->nodeId = pRecvPac.nodeId;
	pStorageData->channelId = pRecvPac.channelId;
	pStorageData->heartBeart = pRecvPac.heartBeart;
	pStorageData->healthStatus = pRecvPac.healthStatus;
	pStorageData->VPCError = pRecvPac.VPCError;
	pStorageData->src = src;
	pStorageData->sec = pRecvPac.sec;
	pStorageData->usec = pRecvPac.usec;

	if(pRecvPac.channelId == 31)
	{
		pStorageData->length = 40;
	}
	else
	{
		pStorageData->length = pRecvPac.length;
	}

	//pStorageData->length = sizeof(pRecvPac.payload);
	memcpy(pStorageData->payload, &pRecvPac.payload, pStorageData->length);
	memcpy(pData->payload, pStorageData, sizeof(TY_STORAGE_1394_DATA) + pStorageData->length
			- EX_PAYLOAD);

	pData->length = sizeof(TY_STORAGE_1394_DATA) + pStorageData->length
			- EX_PAYLOAD;
	//printf("recv length:%d \n", pData->length);
	pStorageManager->insertData(pData);

}
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
	Mil1394CardManager *pCardManager = (Mil1394CardManager*) pParam;
	CDDSManager *pDDSManager = CDDSManager::getInstance();
	/*add by ty,for storage data*/
	CStorageManager *pStorageManager = CStorageManager::getInstance();
	TY_STORAGE_DATA *pData =
			(TY_STORAGE_DATA *) malloc(sizeof(TY_STORAGE_DATA));
	memset(pData, 0, sizeof(TY_STORAGE_DATA));
	TY_STORAGE_1394_DATA *pStorageData = (TY_STORAGE_1394_DATA *) malloc(
			sizeof(TY_STORAGE_1394_DATA) + MAX_1394_PAYLOAD_LEN);
	/*add by ty,for storage data*/

	int ret = 0;
	int keyMark = 0;
	unsigned int len =0;   
	CHANNEL_UNION chnIDUnion;
	chnIDUnion.sn = 0;
	chnIDUnion.info.dev_type = IO_TYPE_COMMON_IO;
	chnIDUnion.info.dev_id = pCardManager->m_devId;
	unsigned short data_len = 0;
	const int PAC_CNT = 100;
	TY_HR_1394_MSG* pRecvPac = new TY_HR_1394_MSG[PAC_CNT];
	TY_HR_TIME recvTime, tmpTime;

	while (pCardManager->m_simRunFlag == Mil1394CardManager::SIM_RUN)
	{
		for (int cardId = 0; cardId < pCardManager->m_cardNum; cardId++)
		{
			CommonCard * pCard = (CommonCard *) pCardManager->m_pCard[cardId];
			chnIDUnion.info.card_id = pCard->m_cardSlot;
			memset(pRecvPac, 0, sizeof(TY_HR_1394_MSG)*PAC_CNT);
			for (int chnId = 0; chnId < pCard->m_chnNum; chnId++)
			{
				if (pCard->m_chnValidFlag[chnId] == 0
						|| pCard->m_chnInfo[chnId].direction
								== DIRECTION_SEND)
				{
					continue;
				}
				chnIDUnion.info.channel_id = chnId + 1;

			    len = PAC_CNT;             // rev 1 packet                         
				ret = pCard->recvDataFromCard(chnId,pRecvPac,len);
				if (ret== 0)
				{
					//usleep(10);
					continue;
				}


				for (int i=0;i<len ;i++)
				{
					if(pRecvPac[i].ddsId != -1)
					{
					    //printf("%d %d %d\n",pRecvPac->heartBeart,pRecvPac->healthStatus,pRecvPac->VPCError);
						unsigned long long keyMark = pRecvPac[i].msgId;
					    keyMark = keyMark<<32 | chnId;
						TY_ROUTER_MAP::iterator pos =  pCard->m_recvMap.find(keyMark);
						if(pos!=pCard->m_recvMap.end())
						{
							pRecvPac[i].ddsId = pos->second.ddsId;
						}
						else
						{

							pCard->m_statisticInfo.chnStatisticInfos[chnId].statisticInfo.uMsgNum++;
						}
					}
					

					recvTime.sec = pRecvPac[i].sec;
					recvTime.usec =pRecvPac[i].usec;

					tmpTime = recvTime;
				//m_statisticInfo 
					pCard->m_statisticInfo.cardID           = pCard->m_cardSlot;
					pCard->m_statisticInfo.statisticInfo.id = pCard->m_cardSlot;
					pCard->m_statisticInfo.statisticInfo.msgNum ++;
					pCard->m_statisticInfo.statisticInfo.totalLen += pRecvPac[i].length;
					pCard->m_statisticInfo.statisticInfo.interTime = recvTime - pCard->m_statisticInfo.statisticInfo.lastTime;
					pCard->m_statisticInfo.statisticInfo.lastTime  = recvTime;
					if(pCard->m_statisticInfo.statisticInfo.msgNum == 1)
					{
						pCard->m_statisticInfo.statisticInfo.maxTime = pCard->m_statisticInfo.statisticInfo.interTime ;
						pCard->m_statisticInfo.statisticInfo.minTime = pCard->m_statisticInfo.statisticInfo.interTime ; 
					}
					else
					{
						if(pCard->m_statisticInfo.statisticInfo.maxTime < pCard->m_statisticInfo.statisticInfo.interTime )
							pCard->m_statisticInfo.statisticInfo.maxTime = pCard->m_statisticInfo.statisticInfo.interTime;
						if(pCard->m_statisticInfo.statisticInfo.minTime > pCard->m_statisticInfo.statisticInfo.interTime)
							pCard->m_statisticInfo.statisticInfo.minTime = pCard->m_statisticInfo.statisticInfo.interTime;
					}
					pCard->m_statisticInfo.chnStatisticInfos[chnId].chnID = chnId +1;
					pCard->m_statisticInfo.chnStatisticInfos[chnId].statisticInfo.id = chnId +1;
					pCard->m_statisticInfo.chnStatisticInfos[chnId].statisticInfo.msgNum++;
					pCard->m_statisticInfo.chnStatisticInfos[chnId].statisticInfo.totalLen += pRecvPac[i].length;
					pCard->m_statisticInfo.chnStatisticInfos[chnId].statisticInfo.interTime = recvTime -
							pCard->m_statisticInfo.chnStatisticInfos[chnId].statisticInfo.lastTime;
					pCard->m_statisticInfo.chnStatisticInfos[chnId].statisticInfo.lastTime = recvTime;
					
					/*update chn icd statistic info*/
					TY_COMMON_STATISTIC_INFO_MAP::iterator statisticPos;
					statisticPos
						= pCard->m_statisticInfo.chnStatisticInfos[chnId].icdStatisticInfos.find(pRecvPac[i].ddsId);
					if (statisticPos
						== pCard->m_statisticInfo.chnStatisticInfos[chnId].icdStatisticInfos.end())
					{
						TY_COMMON_STATISTIC_INFO statisticInfo;
						statisticInfo.id = pRecvPac[i].ddsId;
						statisticInfo.lastTime = recvTime;
						statisticInfo.interTime = 0;
						statisticInfo.msgNum = 1;
						if(pRecvPac[i].ddsId == 0)
						{
							statisticInfo.uMsgNum = 1;
						}
						statisticInfo.totalLen = 0;
						statisticInfo.desc = "";
						pCard->m_statisticInfo.chnStatisticInfos[chnId].icdStatisticInfos.insert(
							make_pair(pRecvPac[i].ddsId, statisticInfo));
					}
					else
					{
						//update the this icd info
						TY_COMMON_STATISTIC_INFO *pStatisticInfo =
							&(statisticPos->second);
						pStatisticInfo->msgNum++;
						if(pRecvPac[i].ddsId == 0)
						{
							pStatisticInfo->uMsgNum++;
						}
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
						/*storage data>>>>>*/
						storageData(pStorageManager, pData, pStorageData, pRecvPac[i], chnIDUnion.sn);
					}

					pDDSManager->sendDDSMsgData(chnIDUnion.sn,&pRecvPac[i],pRecvPac[i].length);
				}
				
			}//end for chn
		}//end for card
#ifdef WIN32
		//Sleep(1);
#else
		usleep(10);
#endif

	}
	REPORT(REPORT_INFO, "sendDataToDDS thread exit!\n");
	return NULL;

}


