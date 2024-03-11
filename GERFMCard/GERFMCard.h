#pragma once
#include "BaseRFMCard.h"
#include "DataBuffer.h"

#include "rfm2g_api.h"

extern "C" CLASS_EXPORT CCard* CreateCard();

#define BUFFER_BLOCK_SIZE    4096+64
#define CARD_LIB_VERSION "4.1.0"

#ifdef _WIN32
#define DEVICE_PREFIX "\\\\.\\rfm2g"
#else
#define DEVICE_PREFIX "/dev/rfm2g"
#endif

#define TIMEOUT         500	//used for configure the time of event wait
#define RFM2GEVENTINTR  RFM2GEVENT_INTR2	//configure the interrupt type

class CGERFMCard :
	public CBaseRFMCard
{
public:
	CGERFMCard();
	~CGERFMCard(void);
	/**************************************************************
	*Function name: getLibVersion
	*Description:	Get the lib version 
	*Parameters:	None
	*Return:		lib version				
	***************************************************************/
    virtual string getLibVersion();
	/**************************************************************
	*Function name: loadCardConfig
	*Description:	Load the conf file and get the card configurations
	*Parameters:	fileName: The conf file name
	*				section:  Section name
	*				cardIndex:Index of the card (start from 0) 
	*Return:		None	
	***************************************************************/
	void loadCardConfig(const char* fileName);
	/**************************************************************
	*Function name: open
	*Description:	Open card
	*Parameters:	None
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int open();
	/**************************************************************
	*Function name: close
	*Description:	Close card
	*Parameters:	None
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int close();
	/**************************************************************
	*Function name: init
	*Description:	Init card
	*Parameters:	msg: The output error message string
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int init(char *msg);
	/**************************************************************
	*Function name: start
	*Description:	start card
	*Parameters:	msg: The output error message string
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int start(char *msg);
	/**************************************************************
	*Function name: stop
	*Description:	stop card
	*Parameters:	msg: The output error message string
	*Return:		0 - success other-failed	
	***************************************************************/
	virtual int stop(char *msg);
	/*********************************************
	* FunctionName :sendDataToCard
	* Description  :send data from channel
	* Parameter    :msg - the data of TY_A429_MSG
	* Return       :0 - success other-failed
	*********************************************/
	virtual int sendDataToCard(TY_RFM_DATA pData);
	/*********************************************
	* FunctionName :recvDataFromCard
	* Description  :Get data from channel
	* Parameter    :chnId - the id of the channel to be read
	* Return       :The data get from the channel,it is an
	* 					invalid data while bld429Msg->data=0
	*********************************************/
	virtual int recvDataFromCard(int chnId,TY_RFM_DATA* pData,int len);

	int GetMemorySize(unsigned int * memSize);

	int readData(unsigned int addr, unsigned int length, unsigned char *readBuf);
public:
	RFM2GHANDLE m_hCard;

	TY_BUFFER_FIFO m_chnFIFO;

	unsigned char *writeDMABuf;
	unsigned char *readDMABuf;

private:

	long m_DMAAddr;
	unsigned int m_DMAThreshold;
	unsigned int m_DMAPageMax;

	int m_cardId;

	timeval recvTime;
	
#ifdef WIN32
	HANDLE m_threadReadDataID;
	HANDLE m_threadReadApDataID;
#else
	pthread_t m_threadReadDataID;
	pthread_t m_threadReadApDataID;
#endif

	
};

