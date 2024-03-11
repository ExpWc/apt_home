#include "GERFMCard.h"
#include "Ini.h"
CCard* CreateCard()
{
	return new CGERFMCard();
}

CGERFMCard::CGERFMCard()
{
	//writeDMABuf = new unsigned char[MAX_DMA_PAGE * PAGE_SIZE]; //TST
	//readDMABuf = new unsigned char[MAX_DMA_PAGE * PAGE_SIZE];
}


CGERFMCard::~CGERFMCard(void)
{
	delete [] writeDMABuf;
	delete [] readDMABuf;
}


#if 0
#ifdef WIN32
unsigned long WINAPI threadReadData(void *pParam)
#else
void* threadReadData(void *pParam)
#endif
{
	//printf("start threadReadData\n");
	CGERFMCard *pCard = (CGERFMCard *) pParam;

	TY_ROUTER_MAP::iterator pos;

	TY_RFM_DATA rfmData;
	memset(&rfmData, 0, sizeof(rfmData));
	long long readAddr = 0;
	int ret;
	unsigned int updateCountTmp;
	int readFlag;
	static int firstUpdataFlag = 0;

	timeval startTime;
	timeval endTime;
	gettimeofday(&startTime, NULL);
	firstUpdataFlag = 0;
	while(pCard->m_status == CARD_STATUS_RUNNING)
	{
		for (pos = pCard->m_recvMap.begin(); pos
			!= pCard->m_recvMap.end(); pos++)
		{		
			if(pos->second.sampleRate == 0)
			{
				continue;
			}
			pos->second.sampleRateTmp ++;
			if (pos->second.sampleRate <= 5)
			{
				if (pos->second.sampleRateTmp == pos->second.sampleRate)
				{
					readFlag = 1;
					pos->second.sampleRateTmp = 0;
				}
				else
				{
					readFlag = 0;
					continue;
				}
			}
			else
			{
				gettimeofday(&endTime, NULL);
				pos->second.sampleRateTmp = ((endTime.tv_sec - startTime.tv_sec) * 1000000
						+ (endTime.tv_usec - startTime.tv_usec)) / pCard->m_cardTimer;
				//printf("pos->second.sampleRateTmp  %d\n",pos->second.sampleRateTmp);

				if ((pos->second.sampleRateTmp % (pos->second.sampleRate * 1000)) > (pos->second.sampleRate * 1000 - 500)
						|| (pos->second.sampleRateTmp % (pos->second.sampleRate * 1000)) < 600)
				{
					if(((endTime.tv_sec - pos->second.updateTime.tv_sec) * 1000000
							+ (endTime.tv_usec - pos->second.updateTime.tv_usec)) < (pos->second.sampleRate * 900))
					{
						continue;
					}

					pos->second.updateTime = endTime;
					readFlag = 1;
					pos->second.sampleRateTmp = 0;
				}
				else
				{
					readFlag = 0;
					continue;
				}
			}

			if ((pCard->m_cardInfo.updateFlag != 0)
				&& (pCard->m_cardInfo.updateFlagByte > 0))
			{
				readAddr = pos->second.addr - pCard->m_cardInfo.updateFlagByte;
				if (readAddr < 0)
				{
					REPORT(REPORT_ERROR,"updateFlagByte is greater than addr");
					continue;
				}
				ret = pCard->readData(readAddr, pCard->m_cardInfo.updateFlagByte, pCard->readDMABuf);
				memcpy(&updateCountTmp,pCard->readDMABuf,pCard->m_cardInfo.updateFlagByte);

				//drop first unknown data
				if((firstUpdataFlag == 0) && (updateCountTmp != 1))
				{
//						printf("%d , %d\n", updateCountTmp, pos->second.updateCount);
//						printf("drop it\n");
					continue;
				}

				firstUpdataFlag = 1;
				if (pCard->m_cardInfo.updateFlag == 1)
				{
					if (updateCountTmp == pos->second.updateCount)
					{
						pos->second.updateCount = updateCountTmp;

						continue;
					}
				}
				else if (pCard->m_cardInfo.updateFlag == 2)
				{
					if (updateCountTmp <= pos->second.updateCount)
					{
						pos->second.updateCount = updateCountTmp;
						continue;
					}
				}
				pos->second.updateCount = updateCountTmp;
				//printf(">>>pos->second.updateCount  %x  \n",pos->second.updateCount);
			}

			if(pos->second.ddsId < 400000 || pos->second.ddsId > 600000)
			{
				ret = pCard->readData(pos->second.addr, pos->second.length, pCard->readDMABuf);
				memcpy(rfmData.data, pCard->readDMABuf, pos->second.length);
				rfmData.length = pos->second.length;
				rfmData.addr = pos->second.addr;
			}
			else
			{
				ret = pCard->readData(pos->second.addr, sizeof(double), pCard->readDMABuf);
				memcpy(rfmData.data, pCard->readDMABuf, sizeof(double));
				rfmData.length = pos->second.length;
				rfmData.addr = pos->second.addr;
			}
			rfmData.sec = endTime.tv_sec;
			rfmData.usec = endTime.tv_usec;
			rfmData.ddsKey = pos->second.ddsId;
//				printf("read data time:%d s, %dus\n", endTime.tv_sec, endTime.tv_usec);
			dataBufferWrite(&pCard->m_chnFIFO, (char*)&rfmData, sizeof(rfmData));

		}//end of recvMap

#ifdef WIN32
		timeBeginPeriod(1);
		Sleep(pCard->m_cardTimer);
#else
		usleep(pCard->m_cardTimer * 1000);
#endif
	}

	return NULL;
}
#endif

#ifdef WIN32
unsigned long WINAPI threadReadData(void *pParam)
#else
void* threadReadData(void *pParam)
#endif
{
	CGERFMCard *pCard = (CGERFMCard *) pParam;
	TY_ROUTER_MAP::iterator pos;
	TY_RFM_DATA rfmData;
	memset(&rfmData, 0, sizeof(rfmData));
	int ret;
	timeval endTime;
	//timeval t;
	//char rxData[2048];
	//int rxDataLen = 0;
	while(pCard->m_status == CARD_STATUS_RUNNING)
	{
		for(pos = pCard->m_recvMap.begin(); pos!= pCard->m_recvMap.end(); pos++)
		{
			gettimeofday(&endTime, NULL);
			ret = pCard->readData(pos->second.Addr, pos->second.Length, (unsigned char *)rfmData.payload);
			if(ret < 0)
			{
				printf("read failed\n");
			}
			strcpy(rfmData.uuid, pos->second.uuid);
			rfmData.Length = pos->second.Length;
			rfmData.Addr = pos->second.Addr;
			rfmData.sec = endTime.tv_sec;
			rfmData.usec = endTime.tv_usec;
			rfmData.periodMs = pos->second.PeriodMs;
			ICDQueue* pQueue = pCard->m_dataManager.getQueue(rfmData.uuid);
			if(pQueue == NULL)
			{
				pCard->m_dataManager.initQueue(rfmData.uuid, 1000, rfmData.Length);
			}
			pCard->m_dataManager.insert(rfmData.uuid, (char*)rfmData.payload, rfmData.Length, rfmData.sec, rfmData.usec);
			//pQueue = pCard->m_dataManager.getQueue(rfmData.uuid);
			//ret = pQueue->getLastPacket((char*)rxData, rxDataLen, &t.tv_sec, &t.tv_usec);
		}

#ifdef WIN32
		timeBeginPeriod(1);
		Sleep(pCard->m_cardTimer);
#else
		usleep(rfmData.periodMs);
		//usleep(1);
#endif
	}

	return NULL;
}

#if 0
#ifdef WIN32
unsigned long WINAPI threadReadApData(void *pParam)
#else
void* threadReadApData(void *pParam)
#endif
{
	int ret;
	RFM2G_STATUS result; /* Return codes from RFM2g API calls */
	RFM2GEVENTINFO EventInfo;

	EventInfo.Event = RFM2GEVENTINTR; /* We'll wait on this interrupt */
	EventInfo.Timeout = TIMEOUT; /* We'll wait this many milliseconds */

	CGERFMCard *pCard = (CGERFMCard *) pParam;

	TY_ROUTER_MAP::iterator pos;


	TY_RFM_DATA rfmData;
	memset(&rfmData, 0, sizeof(rfmData));


	while(pCard->m_status == CARD_STATUS_RUNNING)
	{
		result = RFM2gWaitForEvent(pCard->m_hCard, &EventInfo);
		if (result == RFM2G_SUCCESS)
		{
			//printf("recv int data\n");
			for (pos = pCard->m_recvMap.begin(); pos
				!= pCard->m_recvMap.end(); pos++)
			{
				if(pos->second.sampleRate != 0)
				{
					continue;
				}
				if(pos->second.addr == EventInfo.ExtendedInfo)
				{
					ret = pCard->readData(pos->second.addr, pos->second.length, pCard->readDMABuf);

					memcpy(rfmData.data, pCard->readDMABuf, pos->second.length);
					rfmData.length = pos->second.length;
					rfmData.addr = pos->second.addr;

					dataBufferWrite(&pCard->m_chnFIFO,
						(char*)&rfmData, sizeof(rfmData));
				}
			}
		}
#ifdef WIN32
		Sleep(1);
#else
		usleep(100);
#endif
	}
	return NULL;
}
#endif

void CGERFMCard::loadCardConfig(const char* fileName)
{
	
	CBaseRFMCard::loadCardConfig(fileName);
	
}

int CGERFMCard::open()
{
	RFM2G_STATUS ret;
	RFM2G_CHAR device[40];

	sprintf((char*)device, "%s%d", DEVICE_PREFIX, m_cardId);

	/*Open card*/
	ret = RFM2gOpen((char*)device, &m_hCard);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR, "Open GERFM card : %d failed! ", m_cardId);
		REPORT(REPORT_ERROR, "ERROR : %s ", RFM2gErrorMsg(ret));
		return -1;
	}

	ret = RFM2gSetDMAThreshold(m_hCard, m_DMAThreshold);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR, "RFM2gSetDMAThreshold failed! ");
		REPORT(REPORT_ERROR, "ERROR : %s ", RFM2gErrorMsg(ret));
		return -1;
	}
#ifdef _WIN32
	ret = RFM2gUserMemory(m_hCard, (volatile void **)(&readDMABuf),
			m_DMAAddr , MAX_DMA_PAGE);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR," RFM2gUserMemoryBytes(W) failed.\n");
		REPORT(REPORT_ERROR, "ERROR : %s\n", RFM2gErrorMsg(ret));
	}
#else
	if(m_DMAAddr == 0)
	{
		ret = RFM2gUserMemory(m_hCard, (volatile void **)(&readDMABuf),
			m_DMAAddr, MAX_DMA_PAGE);
	}
	else
	{
		ret = RFM2gUserMemory(m_hCard, (volatile void **)(&readDMABuf),
			m_DMAAddr | RFM2G_DMA_MMAP_OFFSET, MAX_DMA_PAGE);
	}
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR," RFM2gUserMemoryBytes(W) failed.\n");
		REPORT(REPORT_ERROR, "ERROR : %s\n", RFM2gErrorMsg(ret));
	}
#endif
	/*ret = RFM2gUserMemory(m_hCard, (volatile void **)(&writeDMABuf),
		(m_DMAAddr + MAX_DMA_PAGE * PAGE_SIZE) | RFM2G_DMA_MMAP_OFFSET, MAX_DMA_PAGE);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR," RFM2gUserMemoryBytes(R) failed.\n");
		REPORT(REPORT_ERROR, "ERROR : %s\n", RFM2gErrorMsg(ret));
	}*/

	ret = RFM2gEnableEvent( m_hCard, RFM2GEVENTINTR);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR," RFM2gEnableEvent failed.\n");
		REPORT(REPORT_ERROR, "ERROR : %s\n", RFM2gErrorMsg(ret));
	}


	return 0;
}

int CGERFMCard::close()
{
	RFM2G_STATUS ret;

	ret = RFM2gUnMapUserMemory(m_hCard, (volatile void **)(&readDMABuf), MAX_DMA_PAGE);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR," RFM2gUnMapUserMemory(W) failed.\n");
		REPORT(REPORT_ERROR, "ERROR : %s\n", RFM2gErrorMsg(ret));
	}

	/*ret = RFM2gUnMapUserMemory(m_hCard, (volatile void **)(&writeDMABuf), MAX_DMA_PAGE);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR," RFM2gUnMapUserMemory(R) failed.\n");
		REPORT(REPORT_ERROR, "ERROR : %s\n", RFM2gErrorMsg(ret));
	}*/

	ret = RFM2gClose(&m_hCard);
	if( ret != RFM2G_SUCCESS )
	{
		REPORT(REPORT_ERROR, "card %d : Close GERFM failed!", m_cardId);
		REPORT(REPORT_ERROR,"Error: %s.",  RFM2gErrorMsg(ret));
		return -1;
	}

	return 0;
}


int  CGERFMCard::recvDataFromCard(int chnId,TY_RFM_DATA* pData,int len)
{
	int ret;

	timeval recvTime;
	TY_RFM_DATA pRecvPack;
	unsigned short dataLen = 0;
	int msgCount = 0;
	memset(pData,0,len*sizeof(TY_RFM_DATA));
	if (m_chnValidFlag[chnId] == 1) 
	{
		do 
		{
			ret = dataBufferRead(&m_chnFIFO,
				(char*)&pRecvPack, &dataLen);
			if (0 != ret)
			{
				break;
			}

			gettimeofday(&recvTime, NULL);

			pData[msgCount].addr = pRecvPack.addr;
			pData[msgCount].length = pRecvPack.length;
			if(pRecvPack.ddsKey < 400000 || pRecvPack.ddsKey > 600000)
			{
				memcpy(pData[msgCount].data, pRecvPack.data, pRecvPack.length);
			}
			else
			{
				memcpy(pData[msgCount].data, pRecvPack.data, sizeof(double));
			}


			pData[msgCount].sec = recvTime.tv_sec;
			pData[msgCount].usec = recvTime.tv_usec;
//			pData[msgCount].sec = pRecvPack.sec;
//			pData[msgCount].usec = pRecvPack.usec;
			msgCount++;
			if (msgCount == len)
			{
				break;
			}
		} while (ret == 0);
	}

	return msgCount;
}
#if 0 
int CGERFMCard::sendDataToCard(TY_RFM_DATA pData)
{
	RFM2G_STATUS ret;

	//memcpy(writeDMABuf, pData.data, pData.length);

	//printf("*****  Write addr %d length %d data[0]=%x data[1]=%x\n",
			//pData.addr,pData.length,pData.data[0],pData.data[1]);
	//ret = RFM2gWrite(m_hCard, pData.addr, (void *)writeDMABuf, pData.length);

	if(pData.ddsKey < 400000 || pData.ddsKey > 600000)
	{
		ret = RFM2gWrite(m_hCard, pData.addr, (void *)pData.data, pData.length);
	}
	else
	{
		ret = RFM2gWrite(m_hCard, pData.addr, (void *)pData.data, sizeof(double));
	}

	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR, "card %d : RFM2gWrite failed!", m_cardId);
		REPORT(REPORT_ERROR,"Error: %s.",  RFM2gErrorMsg(ret));
		return -1;
	}
	if(pData.sampleRate == 0)
	{
		//printf("send intterpt data\n");
		 /* Send an interrupt to the other Reflective Memory board */
		ret = RFM2gSendEvent(m_hCard, RFM2G_NODE_ALL, RFM2GEVENT_INTR1, pData.addr);
		if (ret != RFM2G_SUCCESS)
		{
			REPORT(REPORT_ERROR, "card %d addr %x: RFM2gSendEvent failed!", m_cardId, pData.addr);
			REPORT(REPORT_ERROR,"Error: %s.",  RFM2gErrorMsg(ret));
			return -1;
		}
		//printf("send EVent\n");
	}
	return ret;
}
#endif

int CGERFMCard::sendDataToCard(TY_RFM_DATA pData)
{
	RFM2G_STATUS ret;
	/*printf("pData.uuid:%s\n", pData.uuid);
	printf("pData.Addr:%x\n", pData.Addr);
	printf("pData.Length:%d\n", pData.Length);
	for(int i = 0;i < pData.Length;i++)
	{
		printf("pData[%d]:0x%x\n", i, pData.payload[i]&0xff);
	}*/
	ret = RFM2gWrite(m_hCard, pData.Addr, (void *)pData.payload, pData.Length);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR, "card %d : RFM2gWrite failed!", m_cardId);
		REPORT(REPORT_ERROR,"Error: %s.",  RFM2gErrorMsg(ret));
		return -1;
	}
	return ret;
}

int CGERFMCard::readData(unsigned int addr, unsigned int length, unsigned char *readBuf)
{
	RFM2G_STATUS ret; /* Return codes from RFM2g API calls */

	if (m_status != CARD_STATUS_RUNNING)
	{
		REPORT(REPORT_ERROR,"Card has not started.\n");
		return -1;
	}

	ret = RFM2gRead(m_hCard, (RFM2G_UINT32)addr, (void *)readBuf, (RFM2G_UINT32)length);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR, "ERROR: RFM2gRead failed.");
		REPORT(REPORT_ERROR,"Error: %s.",  RFM2gErrorMsg(ret));
	}

	return 0;
}


int CGERFMCard::init(char *msg)
{
	//printf("=========CGERFCard=====init====m_DMAPageMax:%d\n", m_DMAPageMax);  //TST
	writeDMABuf = new unsigned char[m_DMAPageMax * PAGE_SIZE];
	readDMABuf = new unsigned char[m_DMAPageMax * PAGE_SIZE];
	return 0;
}



int CGERFMCard::start(char *msg)
{
	int thread_status;
	dataBufferInit(&m_chnFIFO, BUFFER_BLOCK_SIZE);
	dataBufferReset(&m_chnFIFO);
	m_statusMsg = "running";
	m_status = CARD_STATUS_RUNNING;
#ifdef WIN32
	m_threadReadDataID = CreateThread(NULL,NULL,threadReadData,this,NULL,NULL);
	if (m_threadReadDataID==NULL)
	{
		REPORT(REPORT_ERROR,"pthread_create m_threadReadDataID failed \n");
		return -1;
	}
	m_threadReadApDataID = CreateThread(NULL,NULL,threadReadApData,this,NULL,NULL);
	if (m_threadReadApDataID==NULL)
	{
		REPORT(REPORT_ERROR,"pthread_create m_threadReadApDataID failed \n");
		return -1;
	}
#else
	thread_status = pthread_create(&m_threadReadDataID, NULL, threadReadData,
			this);
	if (0 != thread_status)
	{
		REPORT(REPORT_ERROR, "pthread_create  threadReadData failed \n");
		return -1;
	}

	/*thread_status = pthread_create(&m_threadReadApDataID, NULL, threadReadApData,
				this);
	if (0 != thread_status)
	{
		REPORT(REPORT_ERROR, "pthread_create  threadReadData failed \n");
		return -1;
	}*/
#endif
	return 0;
}

int CGERFMCard::stop(char *msg)
{
#ifdef WIN32
	WaitForSingleObject(m_threadReadDataID,2000);
	WaitForSingleObject(m_threadReadApDataID,2000);
#else
if(m_threadReadDataID)
{


	pthread_join(m_threadReadDataID, NULL);
	}
	//pthread_join(m_threadReadApDataID, NULL);
#endif
	dataBufferClear(&m_chnFIFO);
	return 0;
}

int CGERFMCard::GetMemorySize(unsigned int * memSize)
{
	RFM2G_STATUS ret;
	ret = RFM2gSize(m_hCard, (RFM2G_UINT32*)memSize);
	if (ret != RFM2G_SUCCESS)
	{
		REPORT(REPORT_ERROR, "ERROR: RFM2gSize failed.");
		REPORT(REPORT_ERROR,"Error: %s.",  RFM2gErrorMsg(ret));
	}
	return 0;
}

std::string CGERFMCard::getLibVersion()
{
	return CARD_LIB_VERSION;
}



