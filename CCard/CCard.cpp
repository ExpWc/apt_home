/*
 * CCard.cpp
 *
 *  Created on: 2013-10-24
 *      Author: Thinkpad
 */
#include "CCard.h"
#ifndef _WIN32
#include <dlfcn.h>
#include "HiReport.h"
#endif

CCard::CCard() 
{
	// TODO Auto-generated constructor stub

}

CCard::~CCard() 
{
	// TODO Auto-generated destructor stub
}
int CCard::readData(string uuid, char* pData, int length,struct timeval *t)
{

	 int ret =0;
	 ICDQueue* pQueue = m_dataManager.getQueue(uuid);
	 if(pQueue!=NULL)
	 {
		 ret = pQueue->getLastPacket(pData,length,&t->tv_sec,&t->tv_usec);
	//	 printf("=====file:%s,sfunction:%s line:%d,======, uuid:%s, data:%x, len:%d\n",__FILE__,__FUNCTION__,__LINE__,uuid.c_str(),pData[0],length);
	 }
	 else
	 {
		 //printf("---------go to CCard -readData APIand read queue = NULL\n");
		 ret = -3;
	 }
	 return ret;
}
int CCard::writeData(int channel,char* pData,int length)
{
	return 0;
}
int CCard::readData(int channel,char* pData,int &length,struct timeval *t)
{
	return 0;
}
int CCard::initInstance(char *msg)
{
	return 0;
}

int CCard::deleteInstance(char *msg)
{
	sprintf(msg, "%s", "ok");
	return 0;
}
int CCard::start(char *msg)
{
	m_dataManager.reset();
	return 0;
}
int CCard::stop(char *msg) 
{
	sprintf(msg, "%s", "ok");
	return 0;
}

std::string CCard::getLibVersion()
{
	return "1.0.0";
}

std::string CCard::getMsg()
{
	return m_msg;
}

//int CCard::log(string msg)
//{
//
//	CLog *pLog = CLog::getInstance();
//
//	char tmp[10240];
//	sprintf(tmp, "m_cardType:%s m_cardSlot:%d m_chnNum:%d\n",
//			m_cardType.c_str(), m_cardSlot, m_chnNum);
//	pLog->log(tmp);
//	return 0;
//
//}
int CCard::log(TY_LOG_TYPE logType, const char *fileName, const char *funName, int lineNo, string msg)
{

	CLog *pLog = CLog::getInstance();

	char tmp[10240];
	sprintf(tmp, "m_cardType:%s m_cardSlot:%d m_chnNum:%d\n",
			m_cardType.c_str(), m_cardSlot, m_chnNum);
	pLog->log(logType, fileName, funName, lineNo, tmp);

	return 0;

}
int CCard::pacLog(CPacLog *pPacLog, char* pData, int length, string msg)
{
//	char tmp[1024];
//	sprintf(tmp, "m_cardType:%s m_cardSlot:%d m_chnNum:%d\n",
//			m_cardType.c_str(), m_cardSlot, m_chnNum);
//	msg = string(tmp);
//	CBaseStep::log(msg);

	char tmp[10240];
	sprintf(tmp, "m_cardType:%s m_cardSlot:%d m_chnNum:%d\n",
			m_cardType.c_str(), m_cardSlot, m_chnNum);
	pPacLog->pacLog(pData, length, tmp);
	return 0;
}



// void CCardManager::clearCommonStatisticInfo(TY_COMMON_STATISTIC_INFO *pInfo)
// {
// 	pInfo->interTime = 0;
// 	pInfo->lastTime = 0;
// 	pInfo->maxTime = 0;
// 	pInfo->minTime = 0;
// 	pInfo->totalLen = 0;
// 	pInfo->msgNum = 0;
// 	pInfo->uMsgNum = 0;
// 	pInfo->reserver1=0;
// 	pInfo->reserver2=0;
// 	pInfo->reserver3=0;
// 	pInfo->desc="";
// }
CCardManager::CCardManager(char *name) 
{
	// TODO Auto-generated constructor stub

}

CCardManager::~CCardManager() 
{
	// TODO Auto-generated destructor stub
}

/*sim interface */
int CCardManager::startSim(char *msg) 
{
	sprintf(msg, "%s", "ok");
	return 0;
}
int CCardManager::stopSim(char *msg) 
{
	sprintf(msg, "%s", "ok");
	return 0;
}
int CCardManager::initCard(char *pData, char *msg)
{
	pData = pData;
	sprintf(msg, "%s", "ok");
	return 0;
}

int CCardManager::createCardInstance(char* pData)
{
	return 0;
}
int CCardManager::deleteCardInstance() 
{
	return 0;
}
int CCardManager::initCardInstance(char *msg) 
{
	sprintf(msg, "%s", "ok");
	return 0;
}


//param : card status
//return : itb health status
int CCardManager::statusTransfer(int status) 
{
	switch (status) 
	{
	case CARD_STATUS_OPEN:
	case CARD_STATUS_INIT:
	case CARD_STATUS_ROUTER:
		return CARD_OPEN;
	case CARD_STATUS_RUNNING:
		return CARD_RUNING;
	case CARD_STATUS_CLOSE:
		return CARD_CLOSE;
	case CARD_STATUS_FATAL:
		return CARD_ERR;
	default:
		break;
	}
	return CARD_ERR;
}

/*static info */
string CCardManager::getLibVersion() 
{
	string tmp;
	return tmp;
}


int CCardManager::selfcheck(char *info) 
{
	char tmpInfo[1024];
	tmpInfo[0] = 0;
	sprintf(tmpInfo, "Service status ok.");

	for(int i = 0; i < m_cardNum; i++)
	{
		printf("---busType %s slot %d---\n", m_busType.c_str(), m_pCard[i]->m_cardSlot);
	}
	strcat(info, tmpInfo);
	return 0;
}

CCard* getCardInstance(const char* libName)
{
#ifdef _WIN32
	HMODULE libHandle = LoadLibrary(libName);
#else
	//void* libHandle = dlopen(m_config.cardLibName.c_str(), RTLD_LAZY);
	void* libHandle = dlopen(libName, RTLD_LAZY);
#endif

	if (libHandle == NULL)
	{
		char errMsg[STR_SHORT];
		sprintf(errMsg, "Can't open file:%s", libName);
		REPORT(REPORT_ERROR,errMsg);
#ifndef _WIN32
		sprintf(errMsg, "Reason:%s\n", dlerror());
		REPORT(REPORT_ERROR,errMsg);
#endif
	
		return NULL;
	}
	else
	{
		REPORT(REPORT_INFO,"open file:%s success\n",
			libName);
	}
	CCard * (*CreateCardFunc)();
#ifdef _WIN32
	CreateCardFunc = (CCard *(*)())GetProcAddress(libHandle, "CreateCard");
#else
	CreateCardFunc = (CCard *(*)()) dlsym(libHandle, "CreateCard");
#endif

	if (CreateCardFunc == NULL)
	{
		char errMsg[STR_SHORT];
#ifdef _WIN32
		sprintf(errMsg, "can't CreateCard:%s\n",
			libName);
#else
		sprintf(errMsg, "can't CreateCard:%s reason:%s\n",
			libName, dlerror());
#endif
		REPORT(REPORT_INFO, errMsg);
		return NULL;
	}
	return CreateCardFunc();
}


//int CCardManager::log(string msg)
//{
//
//	CLog *pLog = CLog::getInstance();
//
//	char tmp[1024];
//	sprintf(tmp, "busType:%s cardNum:%d %s", m_busType.c_str(), m_cardNum, msg.c_str());
//	pLog->log(tmp);
//	return 0;
//}


int CCardManager::log(TY_LOG_TYPE logType, const char *fileName, const char *funName, int lineNo, string msg)
{

	CLog *pLog = CLog::getInstance();

	char tmp[1024];
	sprintf(tmp, "busType:%s cardNum:%d %s", m_busType.c_str(), m_cardNum, msg.c_str());
	pLog->log(logType, fileName, funName, lineNo, tmp);

	return 0;
}
