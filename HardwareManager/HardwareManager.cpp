#include "HardwareManager.h"
#include "tinyxml.h"
#include "HiReport.h"
#include "ParseXmlApi.h"
#include "HiCommon.h"
#include <dlfcn.h>

CHardwareManager::CHardwareManager()
{

}
CHardwareManager::~CHardwareManager()
{
	m_config.clear();
}

CHardwareManager* CHardwareManager::m_instance = NULL;
CHardwareManager* CHardwareManager::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new CHardwareManager();
	}
	return m_instance;
}
int CHardwareManager::loadConfig(const char* xmlstr)
{
	int ret = 0;
	TiXmlDocument * pXmlManager = NULL;
	TiXmlNode* rootNode = NULL;
	//printf("%s", xmlstr);
	pXmlManager = load_xml(xmlstr, FROM_DATA);//TODO:CHANGE FROM_FILE TO FROM_DATA
	if (pXmlManager == NULL)
	{
		REPORT(REPORT_ERROR, "Xml is can't be parse\n");
		ret = -1;
	}
	else
	{
		rootNode = pXmlManager->FirstChild("autoTest");
	}
	if(rootNode ==NULL)
	{
		REPORT(REPORT_ERROR,"Can not find node autoTest.");
		return -1;
	}
	TiXmlNode* cardRootNode = rootNode->FirstChild("cardRoot");
	if(cardRootNode ==NULL)
	{
		REPORT(REPORT_ERROR,"Can not find node cardRoot.");
		return -1;
	}

	TiXmlElement *pCardElement = NULL;

	for(pCardElement = cardRootNode->FirstChildElement("card");pCardElement!=NULL;pCardElement=pCardElement->NextSiblingElement("card"))
	{
		const char* typeStr = pCardElement->Attribute("Type");
		if(typeStr!=NULL)
		{
			CCardManager* pCardManager = getCardManager(typeStr);
			if(pCardManager==NULL)
			{
				continue;
			}
			//REPORT(REPORT_INFO,"Init card %s.",typeStr);
			char tmp[1024];
			ret = pCardManager->initCard((char*)pCardElement, tmp);
			if(ret != 0)
			{
				REPORT(REPORT_ERROR,"initCard failed.");
			}
			REPORT(REPORT_INFO,"Init card %s finished.",typeStr);
		}
		else
		{
			REPORT(REPORT_ERROR,"Card element can not find attribute Type.");
		}
	}
	//REPORT(REPORT_ERROR, "demo--------123------hardware.xml");
	delete pXmlManager;
    pXmlManager = NULL;
	return ret;
}
int CHardwareManager::loadResource(char* configFileName)
{
	m_configFileName = string(configFileName);
	TiXmlDocument * pXmlManager = NULL;
	TiXmlNode* rootNode = NULL;
	TiXmlElement *pBusElement = NULL;
	TiXmlElement *pCardElement = NULL;
	int ret = 0;
	pXmlManager = load_xml(configFileName, FROM_FILE);
	if (pXmlManager == NULL)
	{
		REPORT(REPORT_ERROR, "Xml is can't be parse\n");
		ret = -1;
	}
	else
	{
		rootNode = pXmlManager->FirstChild("hardware");
	}
	if (rootNode == NULL)
	{
		REPORT(REPORT_ERROR, "Init.xml can't find the node of hardware\n");
		return -1;
	}
	else
	{
		pBusElement = rootNode->FirstChildElement();
		if (pBusElement == NULL)
		{
			REPORT(REPORT_ERROR, "Init.xml can't find the node of card\n");
			ret = -1;
		}
		for (pBusElement = rootNode->FirstChildElement(); pBusElement; pBusElement
				= pBusElement->NextSiblingElement())
		{
			string tmpBusType = "";
			string tmpCardNum = "";
			string tmpCardManagerLibName = "";
			string tmpCardLibName = "";

			bool bRet = GetValue(pBusElement, "BusType", tmpBusType);
			if (!bRet)
			{
				REPORT(REPORT_ERROR, "Get BusType failed.\n");
				ret = -1;
				continue;
			}
			bRet = GetValue(pBusElement, "CardNum", tmpCardNum);
			if (!bRet)
			{
				REPORT(REPORT_ERROR, "Get CardNum failed.\n");
				ret = -1;
				continue;
			}
			bRet = GetValue(pBusElement, "CardManagerLibName",
					tmpCardManagerLibName);
			if (!bRet)
			{
				REPORT(REPORT_ERROR, "Get CardManagerLibName failed.\n");
				ret = -1;
				continue;
			}
			bRet = GetValue(pBusElement, "CardLibName", tmpCardLibName);
			if (!bRet)
			{
				REPORT(REPORT_WARNING, "Get CardLibName failed.\n");
			}
			pCardElement = pBusElement->FirstChildElement();
			if (pCardElement == NULL)
			{
				REPORT(REPORT_ERROR, "Init.xml can't find the node of card\n");
				ret = -1;
			}
			TY_HARDWARE_CONFIG hwConfig;
			hwConfig.busType = tmpBusType;
			hwConfig.cardNum = atoi(tmpCardNum.c_str());
			hwConfig.cardManagerLibName = tmpCardManagerLibName;
			hwConfig.cardLibName = tmpCardLibName;
			//m_config.push_back(hwConfig);
			if (0!=createHWInstance(hwConfig,pBusElement))
			{
				REPORT(REPORT_ERROR, "create Card Instance error\n");
				ret = -1;
			}
		}
	}
	initCardInstance();
	return ret;
}

int CHardwareManager::initCardInstance()
{
	int ret = 0;
	TY_CARD_MANAGER_MAP::iterator it = m_typeCardManagerMap.begin();
	for (it; it != m_typeCardManagerMap.end(); it++)
	{
		ret = it->second->initCardInstance("");
		if (ret != 0)
		{
			REPORT(REPORT_ERROR, "initCardInstance error\n");
		}
	}
	return ret;
}

int CHardwareManager::createHWInstance(TY_HARDWARE_CONFIG hwConfig,TiXmlElement *pBusElement )
{
	void * m_soHandle;
	 char filePath[256];
	getServerPath(filePath);
	string fullPath(filePath);
	fullPath.append(hwConfig.cardManagerLibName);
	m_soHandle = dlopen(fullPath.c_str(), RTLD_LAZY);
	if (m_soHandle == NULL)
	{
		char errMsg[256];

		sprintf(errMsg, "Reason:%s\n", dlerror());
		REPORT(REPORT_INFO,errMsg);
		return -1;
	}
	else
	{
		REPORT(REPORT_INFO,"open file:%s success\n", hwConfig.cardManagerLibName.c_str());
	}
	CCardManager * (*CreateInstance)(char *);
	CreateInstance = (CCardManager *(*)(char *)) dlsym(m_soHandle,
			"CreateInstance");
	if (CreateInstance == NULL)
	{
		char errMsg[256];

		sprintf(errMsg, "can't CreateInstance:%s reason:%s\n",
				hwConfig.cardManagerLibName.c_str(), dlerror());

		REPORT(REPORT_ERROR, errMsg);

		return -1;
	}

	//TODO;change m_configFileName.c_str() to server name
	CCardManager *pCardManager = CreateInstance(
			(char*) m_configFileName.c_str());
	if (pCardManager == NULL)
	{
		REPORT(REPORT_ERROR, "Create instance of CardManager failed.\n");
		return -1;
	}

	if (0 != pCardManager->createCardInstance((char*)pBusElement))
	{
		REPORT(REPORT_ERROR, "createCardInstance failed.\n");
		return -1;
	}

	insertCardManagerMap(hwConfig.busType, pCardManager);

	REPORT(REPORT_INFO, "bustype: %s cardManager %p\n",hwConfig.busType.c_str(), pCardManager);
	return 0;
}

int CHardwareManager::insertCardManagerMap(string busType,
		CCardManager* pCardManager)
{
	int ret = 0;
	TY_CARD_MANAGER_MAP::iterator pos = m_typeCardManagerMap.find(busType);
	if (pos == m_typeCardManagerMap.end())
	{
		m_typeCardManagerMap.insert(std::make_pair(busType, pCardManager));
		//REPORT(REPORT_INFO, "busType:%s.\n", busType.c_str());
	}
	else
	{
		ret = -1;
	}
	return ret;
}

CCardManager* CHardwareManager::getCardManager(string busType)
{
	CCardManager* pCardManager = NULL;
	TY_CARD_MANAGER_MAP::iterator pos = m_typeCardManagerMap.find(busType);
	if (pos == m_typeCardManagerMap.end())
	{
		REPORT(REPORT_WARNING, "busType:%s is not exsit.\n", busType.c_str());
	}
	else
	{
		pCardManager = pos->second;
	}
	return pCardManager;
}


int CHardwareManager::start(string busType)
{
	int ret = 0;
	CCardManager* pCardManager = getCardManager(busType);
	if (pCardManager == NULL)
	{
		ret = -1;
		REPORT(REPORT_ERROR, "busType:%s is not exsit.\n", busType.c_str());
	}
	else
	{
		char errmsg[1024];
		ret = pCardManager->startSim(errmsg);
		if (ret != 0)
		{
			REPORT(REPORT_ERROR,errmsg);
		}
	}
	return ret;
}
int CHardwareManager::stop(string busType)
{
	int ret = 0;
	CCardManager* pCardManager = getCardManager(busType);
	if (pCardManager == NULL)
	{
		ret = -1;
		REPORT(REPORT_ERROR, "busType:%s is not exsit.\n", busType.c_str());
	}
	else
	{
		char errmsg[1024];
		ret = pCardManager->stopSim(errmsg);
		if (ret != 0)
		{
			REPORT(REPORT_ERROR,errmsg);
		}
	}
	return ret;
}
int CHardwareManager::start()
{
	int ret = 0;

	CCardManager* pCardManager = NULL;
	for(TY_CARD_MANAGER_MAP::iterator pos = m_typeCardManagerMap.begin();pos!=m_typeCardManagerMap.end();pos++)
	{
		char errmsg[1024];
		ret = pos->second->startSim(errmsg);
		if (ret != 0)
		{
			REPORT(REPORT_ERROR,errmsg);
		}
	}
	return ret;
}
int CHardwareManager::stop()
{
	int ret = 0;
	CCardManager* pCardManager = NULL;
	for(TY_CARD_MANAGER_MAP::iterator pos = m_typeCardManagerMap.begin();pos!=m_typeCardManagerMap.end();pos++)
	{
		char errmsg[1024];
		
		ret = pos->second->stopSim(errmsg);
		if (ret != 0)
		{
			REPORT(REPORT_ERROR,errmsg);
		}

	}

	return ret;
}
int CHardwareManager::deleteCardInstance()
{
	int ret = 0;
	TY_CARD_MANAGER_MAP::iterator it = m_typeCardManagerMap.begin();
	for (it; it != m_typeCardManagerMap.end(); it++)
	{
		ret = it->second->deleteCardInstance();
		if (ret == 0)
		{
			REPORT(REPORT_ERROR, "parse Channel XML error\n");
		}
	}
	return ret;
}
int CHardwareManager::selfcheck(char* info)
{
	/*
	sprintf(info, "selfcheck\n");
	string strInfo = "selfcheck\n";
    CCardManager* pCardManager = getCardManager(cardType);
    if(pCardManager == NULL)
    {
    	strInfo.append("Get pCardManager failed\n");
    }

	TiXmlDocument * pXmlManager = NULL;
	TiXmlNode* rootNode = NULL;
	TiXmlElement *pBusElement = NULL;
	TiXmlElement *pCardElement = NULL;
	int ret = 0;
	pXmlManager = load_xml(configFileName, FROM_FILE);
	if (pXmlManager == NULL)
	{
		strInfo.append("Xml is can't be parse\n");
	}
	else
	{
		rootNode = pXmlManager->FirstChild("hardware");
	}
	if (rootNode == NULL)
	{
		strInfo.append("Init.xml can't find the node of hardware\n");
	}
	else
	{
		pBusElement = rootNode->FirstChildElement();
		if (pBusElement == NULL)
		{
			strInfo.append("Init.xml can't find the node of card\n");
		}
		for (pBusElement = rootNode->FirstChildElement(); pBusElement; pBusElement
				= pBusElement->NextSiblingElement())
		{
			string tmpBusType = "";
			string tmpCardNum = "";
			string tmpCardManagerLibName = "";
			string tmpCardLibName = "";

			bool bRet = GetValue(pBusElement, "BusType", tmpBusType);
			if (!bRet)
			{
				strInfo.append("Get BusType failed.\n");
			}
			bRet = GetValue(pBusElement, "CardNum", tmpCardNum);
			if (!bRet)
			{
				strInfo.append("Get CardNum failed.\n");
			}
			bRet = GetValue(pBusElement, "CardManagerLibName",
					tmpCardManagerLibName);
			if (!bRet)
			{
				strInfo.append("Get CardManagerLibName failed.\n");
			}
			bRet = GetValue(pBusElement, "CardLibName", tmpCardLibName);
			if (!bRet)
			{
				strInfo.append("Get CardLibName failed.\n");
			}
			pCardElement = pBusElement->FirstChildElement();
			if (pCardElement == NULL)
			{
				strInfo.append("Init.xml can't find the node of card\n");
			}
			TY_HARDWARE_CONFIG hwConfig;
			hwConfig.busType = tmpBusType;
			hwConfig.cardNum = atoi(tmpCardNum.c_str());
			hwConfig.cardManagerLibName = tmpCardManagerLibName;
			hwConfig.cardLibName = tmpCardLibName;
			//m_config.push_back(hwConfig);
			CCard* pCard = pCardManager->getCard(cardID);
			if(pCard == NULL)
			{
				REPORT(REPORT_ERROR, "Get pCard failed(id:%d)\n",cardID);
				return FAIL;
		    }

		}
	}




*/
	return 0;
}

int CHardwareManager::log(string msg)
{
	CLog *pLog = CLog::getInstance();

	char tmp[1024];
	sprintf(tmp, "%s", msg.c_str());
	pLog->log(tmp);
	return 0;
}
