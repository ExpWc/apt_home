/******************** (C) COPYRIGHT 2012  ********************
 * File Name          : CCard.h
 * Author             : zs of 
 * Version            : V4.1.0
 * Date               : 06/01/2018
 * Description        : Header file for CCard and CCardManager class
 * Built with Linux g++
 *******************************************************************/
/**********************************************************************************************
 *   Revision History:
 *
 *   Version     Date         Initials       Descriptions
 *   --------    ---------    ------------   ---------------
 *   4.1.0       18/05/30     shen.zhang     1.Re-construct the code
 ************************************************************************************************/
#ifndef CCARD_H_
#define CCARD_H_
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include "HRTime.h"
#include "DataManager.h"
#include "HiLog.h"

using namespace std;

extern "C"
{
#ifdef _WIN32
#define CLASS_EXPORT __declspec(dllexport) 
#else
#define CLASS_EXPORT 
#endif
using namespace std;
#define PARA_NUM				20
#define MAX_CHN_NUM				256
#define MAX_CARD_NUM			20

#define STR_SHORT				256
#define STR_LONG				1025
#define STR_LONG_LONG			4096

#define MODE_CONFIG				1
#define MODE_STATIC				2

#define DIRECTION_SEND			2
#define DIRECTION_RECV			1
#define DIRECTION_BIDIRECTION	3

//card status
#define   CARD_STATUS_OPEN		0
#define   CARD_STATUS_INIT		1
#define   CARD_STATUS_ROUTER	2
#define   CARD_STATUS_RUNNING	3
#define   CARD_STATUS_FATAL		4
#define   CARD_STATUS_CLOSE		5

//macro define of card status
#define CARD_CLOSE				1
#define CARD_OPEN				2
#define CARD_RUNING				3
#define CARD_ERR				-1

//macro define of card status string
#define   CARD_STATUS_OPEN_MSG		"open"
#define   CARD_STATUS_INIT_MSG		"open"
#define   CARD_STATUS_ROUTER_MSG	"open"
#define   CARD_STATUS_RUNNING_MSG	"running"
#define   CARD_STATUS_FATAL_MSG		"error"
#define   CARD_STATUS_CLOSE_MSG		"close"

typedef struct _parameters
{
	int id;
	string name;
	string value;
	string defaultValue;
} TY_PARA;

typedef struct _io_chn
{
	short id;
	short direction;
	short mode;
	short sendFlag;/*1-Sim 2-Mon*/
	short para_num;
	TY_PARA chn_para[PARA_NUM];
} TY_CHN_INFO;

typedef struct _io_chn_status
{
	short id;
	int status;
	string statusMsg;
} TY_CHN_STATUS;

typedef struct _chn_static_info_
{
	short chnID;
	int numSendToCard;
	int numrecvfromCard;
}TY_CHN_STATIC_INFO;
typedef struct _statistic
{
	int chnTryRead[MAX_CHN_NUM];
	int chnRxCnt[MAX_CHN_NUM];
	int chnTxCnt[MAX_CHN_NUM];
	int chnAnalysisCnt[MAX_CHN_NUM];
	int chnKnownPacCnt[MAX_CHN_NUM];
	int chnUnknownPacCnt[MAX_CHN_NUM];
}TY_STATISTIC_INFO;

#define CARDLOG(type, description, args...) \
	CCard::log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)

#define CARDMNGLOG(type, description, args...) \
	CCardManager::log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)

class CLASS_EXPORT CCard
{
public:
	CCard();
	virtual ~CCard();
public:
	virtual int reset()
	{
		m_initFlag = 0;
		for (unsigned int i = 0; i < m_chnNum; i++)
		{
			m_chnValidFlag[i] = 0;
		}
		return 0;
	}

	virtual void loadCardConfig(const char* xmlStr){}
	virtual void loadChannelConfig(const char* xmlStr){}
	/**************************************************************
	 *Function name: initCardInstance
	 *Description:	Open card and load the conf file
	 *Parameters:	msg: The output error message
	 *Return:		0 - success other-failed

	 ***************************************************************/
	virtual int initInstance(char *msg);
	/**************************************************************
	 *Function name: deleteCardInstance
	 *Description:	Delete the card instance and release resources
	 *Parameters:	None
	 *Return:		0 - success other-failed
	 ***************************************************************/
	virtual int deleteInstance(char *msg);
	/**************************************************************
	 *Function name: start
	 *Description:	Start data collection and transmit
	 *Parameters:	msg: The output error message string
	 *Return:		0 - success other-failed
	 ***************************************************************/
	virtual int start(char *msg);
	/**************************************************************
	 *Function name: stop
	 *Description:	Stop data collection and transmit
	 *Parameters:	msg: The output error message string
	 *Return:		0 - success other-failed
	 ***************************************************************/
	virtual int stop(char *msg);
	/**************************************************************
	 *Function name: getLibVersion
	 *Description:	Get the lib version
	 *Parameters:	None
	 *Return:		lib version
	 ***************************************************************/
	virtual string getLibVersion();

	virtual int writeData(string uuid, char* pData, int length) = 0;
	virtual int readData(string uuid, char* pData, int length,struct timeval *t);
	string getMsg();
	//>> 20230618--wc to solve the problem of calling the board API int PowerStep
	virtual int writeData(int channel, char* pData, int length);
	virtual int readData(int channel, char* pData, int &length, struct timeval *t);
	//<<
	//int log(string msg);
	int log(TY_LOG_TYPE logType, const char *fileName, const char *funName, int lineNo, string msg);
	int pacLog(CPacLog *pPacLog, char* pData, int length, string msg);

protected:
	//>>
	//virtual int writeData(int channel, char* pData, int length);
	//virtual int readData(int channel, char* pData, int &length, struct timeval *t);
	//<<
public:
	/*The card type to be reported to the ITB*/
	string m_cardType;

	/*The card installed slot to be reported to the ITB*/
	int m_cardSlot;

	/*The card name to be reported to the ITB*/
	string m_cardName;
	unsigned int m_boardNo;//zr
public:
	/*channel number of the card*/
	unsigned int m_chnNum;

	/*Information of each channel*/
	TY_CHN_INFO m_chnInfo[MAX_CHN_NUM];

	/*Initialize flag of each channel*/
	unsigned char m_chnValidFlag[MAX_CHN_NUM];

	/*The card initialize flag*/
	int m_initFlag;

	/*The card and it's chns status*/
	int m_status;
	string m_statusMsg;
	TY_CHN_STATUS m_chnStatus[MAX_CHN_NUM];
	DataManager m_dataManager;
	string m_msg;
	CPacLog* m_pSendPacLog;
	CPacLog* m_pRecvPacLog;

};

/*Load the dynamic library and get the card instance*/
extern "C" CLASS_EXPORT CCard* getCardInstance(const char* fileName);

class CLASS_EXPORT CCardManager
{
public:
	/* card manager type define */
	enum RUNING_FLAG
	{
		SIM_STOP = 0, SIM_RUN
	};
public:
	CCardManager()
	{
	}
	CCardManager(char *name);
	virtual ~CCardManager();
public:
	virtual int reset()
	{
		for (int i = 0; i < m_cardNum; i++)
		{
			m_pCard[i]->reset();
		}
		return 0;
	}
	virtual CCard* getCard(int cardSlot)
	{
		for (int i = 0; i < m_cardNum; i++)
		{

			if (m_pCard[i]->m_cardSlot == cardSlot)
			{
				return m_pCard[i];
			}
		}
		printf("CardSlot:");
		for (int i = 0; i < m_cardNum; i++)
		{
			printf(" %d ",m_pCard[i]->m_cardSlot);
		}
		printf("\n");
		return NULL;
	}
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
	 *Function name: initCard
	 *Description:	Parse init xml file
	 *Parameters:	msg: The output error message
	 *Return:		0 - success other-failed
	 ***************************************************************/
	virtual int initCard(char *pData, char *msg);
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
	 *Function name: deleteCardInstance
	 *Description:	Delete the card instance and release resources
	 *Parameters:	None
	 *Return:		0 - success other-failed
	 ***************************************************************/
	virtual int deleteCardInstance();
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

	virtual int statusTransfer(int status);

	/**************************************************************
	 *Function name: selfcheck
	 *Description:	Get the lib version
	 *Parameters:	info: The output selfcheck information string
	 *Return:		0 - success other-failed
	 ***************************************************************/
	virtual int selfcheck(char *info);
	//int log(string msg);
	int log(TY_LOG_TYPE logType, const char *fileName, const char *funName, int lineNo, string msg);
	//void clearCommonStatisticInfo(TY_COMMON_STATISTIC_INFO *pInfo);
public:
	CCard *m_pCard[MAX_CARD_NUM];
	//int m_devId;/*Device id configured from conf file*/
	int m_cardNum;/*Card number configured from conf file*/
	string m_configFileName;/*conf file name*/
	RUNING_FLAG m_simRunFlag;
	string m_busType;
};

};

#endif /* CCARD_H_ */

