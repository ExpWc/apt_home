#pragma once
#include "CCard.h"
#include "HiCommon.h"

#define CARDMAMAGER_LIB_VERSION "4.1.0"
#define SELF_CHECK_INFO_LEN 10240
#define CommonCard CBaseRFMCard

class CLASS_EXPORT CRFMCardManager :
	public CCardManager
{
public:
	CRFMCardManager(char *name);
	~CRFMCardManager(void);
	/* card manager type define */
	enum RUNING_FLAG
	{
		SIM_STOP = 0, SIM_RUN
	};
private:
	static CRFMCardManager *m_instance;
#ifdef WIN32
	HANDLE m_sendDdsThreadID;
	HANDLE m_recvHilThreadID;
	HANDLE m_recvIilThreadID;
	HANDLE m_recvSilThreadID;
#else
	pthread_t m_sendDdsThreadID;
	pthread_t m_recvHilThreadID;
	pthread_t m_recvIilThreadID;
	pthread_t m_recvSilThreadID;
#endif

public:

	/**************************************************************
	*Function name: createCardInstance
	*Description:	Parse the conf file and get card information, then 
	*				create the card objects
	*Parameters:	devId: The device id
	*				cardNum:The number of card controlled by cardManager
	*Return:		0 - success other-failed				
	***************************************************************/
	virtual int createCardInstance(char* pData);
	/**************************************************************
	*Function name: initCardInstance
	*Description:	Open card and load the conf file
	*Parameters:	msg: The output error message
	*Return:		0 - success other-failed				
	***************************************************************/
	virtual int initCardInstance(char *msg);
	/**************************************************************
	*Function name: getLibVersion
	*Description:	Get the lib version 
	*Parameters:	None
	*Return:		lib version				
	***************************************************************/
	virtual string getLibVersion();
	/**************************************************************
	*Function name: setCardRouter
	*Description:	Parse router xml file
	*Parameters:	msg: The output error message
	*Return:		0 - success other-failed				
	***************************************************************/
	virtual int setCardRouter(char *pData, char *msg);
	/**************************************************************
	*Function name: initCard
	*Description:	Parse init xml file
	*Parameters:	msg: The output error message
	*Return:		0 - success other-failed				
	***************************************************************/
	virtual int initCard(char *pData, char *msg);
	/**************************************************************
	*Function name: startSim
	*Description:	Start receive and transmit
	*Parameters:	msg: The output error message
	*Return:		0 - success other-failed				
	***************************************************************/
	virtual int startSim(char *msg);
	/**************************************************************
	*Function name: stopSim
	*Description:	Stop receive and transmit
	*Parameters:	msg: The output error message
	*Return:		0 - success other-failed				
	***************************************************************/
	virtual int stopSim(char *msg);
	/**************************************************************
	*Function name: deleteCardInstance
	*Description:	Delete the card instance and release resources
	*Parameters:	None
	*Return:		0 - success other-failed				
	***************************************************************/
	virtual int deleteCardInstance();
	/**************************************************************
	*Function name: selfcheck
	*Description:	Get the lib version 
	*Parameters:	info: The output selfcheck information string
	*Return:		0 - success other-failed				
	***************************************************************/
	virtual int selfcheck(char *info);

	static CRFMCardManager* getInstance(char *name);
	//virtual int selfcheck(char *info);
	//int log(string msg);
	int log(TY_LOG_TYPE logType, const char *fileName, const char *funName, int lineNo, string msg);
	//void clearCommonStatisticInfo(TY_COMMON_STATISTIC_INFO *pInfo);
#if 0
public:
	CCard *m_pCard[MAX_CARD_NUM];
	//int m_devId;/*Device id configured from conf file*/
	int m_cardNum;/*Card number configured from conf file*/
	string m_configFileName;/*conf file name*/
	RUNING_FLAG m_simRunFlag;
	string m_busType;
#endif
};

extern "C"
{
	CLASS_EXPORT CCardManager*  CreateInstance(char *name);
}
