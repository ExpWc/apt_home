#pragma once
#include "CCard.h"
#include "HiCommon.h"
#include "HiReport.h"


#define DDS_TO_SIM	 1
#define DDS_TO_MON	 2

#define DDS_TO_SIMTX 3
#define DDS_TO_SIMRX 4

#define MAX_DATA_LEN 4096

#define PAGE_SIZE 4096
#define MAX_DMA_PAGE 100

typedef struct _rfm_data
{
	unsigned int ddsKey;
	unsigned int chnId;
	unsigned short length;
	unsigned char data[MAX_DATA_LEN];
	unsigned char sendFlag;
	long long  addr;
	unsigned int sampleRate;
	unsigned int sec;
	unsigned int usec;
	char uuid[128];
	int Addr;
	char payload[2048];
	int Length;
	int periodMs;
} TY_RFM_DATA;



typedef struct _rfm_card_info
{
	unsigned int updateFlag;
	unsigned int updateFlagByte;

} TY_RFM_CARD_INFO;


typedef struct _rfm_router
{
	unsigned int ddsId;
	unsigned int keyMark;
	unsigned long long  addr;
	unsigned int length;
	unsigned int sampleRate;
	unsigned long long sampleRateTmp;
	char dataSrc;
	char dataDst;
	unsigned int updateCount;
	timeval updateTime;
	char uuid[128];
	int Length;
	int PeriodMs;
	int Addr;
	int Direction;
} TY_RFM_ROUTER;

//typedef map<unsigned int, TY_RFM_ROUTER> TY_ROUTER_MAP;
typedef map<string, TY_RFM_ROUTER> TY_ROUTER_MAP;

class CLASS_EXPORT CBaseRFMCard :
	public CCard
{
public:
	CBaseRFMCard();
	~CBaseRFMCard(void);

public:

	int m_cardTimer;
	TY_RFM_CARD_INFO m_cardInfo;


	TY_ROUTER_MAP m_sendMap;/*Tx router map. key: ddsId value: router info*/
	TY_ROUTER_MAP m_recvMap;/*Rx router map. key: keyMark(addr length) value: router info*/

public:
	/**************************************************************
	*Function name: init
	*Description:	Init card
	*Parameters:	msg: The output error message string
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int init(char* msg)=0;
	/**************************************************************
	*Function name: open
	*Description:	Open card
	*Parameters:	None
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int open()=0;
	/**************************************************************
	*Function name: close
	*Description:	Close card
	*Parameters:	None
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int close()=0;
	/**************************************************************
	*Function name: loadCardConfig
	*Description:	Load the conf file and get the card configurations
	*Parameters:	fileName: The conf file name
	*				section:  Section name
	*				cardIndex:Index of the card (start from 0) 
	*Return:		None	
	***************************************************************/
	virtual void loadCardConfig(const char* fileName);

	/**************************************************************
	*Function name: loadChannelConfig
	*Description:	Load the conf file and get the channel configurations
	*Parameters:	fileName: The conf file name
	*Return:		None
	***************************************************************/
	virtual void loadChannelConfig(const char* fileName);

	/**************************************************************
	*Function name: initInstance
	*Description:	Set the parameters of card and channels, initialize
	*				the card status
	*Parameters:	msg: The output error message string
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int initInstance(char *msg);


	virtual int deleteInstance(char *msg);

	/**************************************************************
	*Function name: sendDataToCard
	*Description:	Send the 429 message to card
	*Parameters:	msg: The message send to card
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int sendDataToCard(TY_RFM_DATA pData) = 0;

	/**************************************************************
	*Function name: recvDataFromCard
	*Description:	Read data from card
	*Parameters:	chnId: The channel you want to read
	*				pData: The output data read from card
	*				len; The message number you want to read
	*Return:		The valid length of pData
	***************************************************************/
	virtual int  recvDataFromCard(int chnId,TY_RFM_DATA* pData,int len) = 0;

	virtual string getLibVersion() = 0;

	int writeData(string uuid, char* pData, int length);
};

