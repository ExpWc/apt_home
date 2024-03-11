/*
 * CStorageManager.cpp
 *
 *  Created on: 2019-09-10
 *      Author: root
 */
#include "CStorageManager.h"
#ifndef _WIN32
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/vfs.h>
#else
#include <io.h>
#endif
#include "global.h"
#include "Ini.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "tinystr.h"
#include "tinyxml.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "tinystr.h"
#include "tinyxml.h"
#include "parse_xml_api.h"
#include "HiReport.h"
//#include "AutoTestInterface.h"

#define SERVICE_RUN 1
#define SERVICE_STOP 0

char confFileName[64] = "StorageServer";
const char *setAutoPath = "AUTO_HOME";


#define OPEN_FILE_MODE 0666

#define INFO_INDEX_TYPE 1
#define TIME_INDEX_TYPE 2

#define DATA_TYPE_ICD	 1
#define DATA_TYPE_SN	 2
#define DATA_TYPE_MODEL	 3

#define NO_ICD_DDSID  0

/* index define by ty*/
#define INDEX_DATA_COUNT  2000
#define INDEX_DATA_TIME   2
/* index define by ty*/

#define MAX_STORAGE_ICD_NUM		4096

#define ANALYSE_COUNT_INDEX		0
#define ANALYSE_TIME_INDEX 		1

void writeDataHeader(TY_STORAGE_DATA_INFO *ICDInfo);
int getLocalIP(string &IP);

CStorageManager::CStorageManager()
{
	char* tmpstoragePath = NULL;
	char filePath[256];
	getServerPath(filePath);// TODO Auto-generated constructor stub
	if(tmpstoragePath == NULL)
	{
		tmpstoragePath = getenv(setAutoPath);//"AUTO_HOME"
	}

	REPORT(REPORT_INFO, "file path:%s\n", filePath);
	
	m_storageStatus = SERVICE_STOP;
	m_exePath.append(filePath);
	m_exePath.append(confFileName);

	pthread_mutex_init(&fifoLock,NULL);

	m_config.storagePath = tmpstoragePath;
	m_config.storagePath = m_config.storagePath + "/../autoStorage";
	//m_config.storagePath = "/home/autoStorage";
	m_config.spaceWarn = 5;
	m_config.storageStartTimeSec = 0;
	m_config.storageStopTimeSec = 0;

	loadConfig(confFileName);

	/*get local IP*/
//	getLocalIP(m_config.IP);

}

CStorageManager::~CStorageManager()
{
	pthread_mutex_destroy(&fifoLock);
}

int CStorageManager::loadConfig(char *ServerName)
{

	string fileName;
	fileName = m_exePath;
	fileName.append(".conf");

	CIni iniFile;
	bool ret = false;
	iniFile.Read(fileName.c_str());

	//storagePath
	ret = iniFile.GetValue(ServerName, "storagePath", m_config.storagePath);
	if (!ret)
	{
		iniFile.SetValue(ServerName, "storagePath", m_config.storagePath);
		REPORT(REPORT_ERROR, "Get storagePath error,use defaul path:%s\n",m_config.storagePath.c_str());
	}
	else
	{
		REPORT(REPORT_INFO, "sucess Get storagePath:%s\n",m_config.storagePath.c_str());
	}

	//spaceWarn
	ret = iniFile.GetValue(ServerName, "spaceWarn", m_config.spaceWarn);
	if (!ret)
	{
		iniFile.SetValue(ServerName, "spaceWarn", m_config.spaceWarn);
		REPORT(REPORT_ERROR, "Get spaceWarn error,use defaul value:%d\n",m_config.spaceWarn);
	}
	else
	{
		REPORT(REPORT_INFO, "sucess Get spaceWarn:%d\n",m_config.spaceWarn);
	}
	iniFile.Write(fileName.c_str());

	return 0;
}

CStorageManager * CStorageManager::instance = NULL;
CStorageManager * CStorageManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new CStorageManager();
	}
	return instance;
}

#ifndef _WIN32
int getLocalIP(string &IP)
{
	int sockfd;
	struct ifconf ifconf;
	struct ifreq *ifreq;
	char buf[512];

	ifconf.ifc_len =512;
	ifconf.ifc_buf = buf;
	if ((sockfd =socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket" );
		exit(1);
	}
	ioctl(sockfd, SIOCGIFCONF, &ifconf);


	ifreq = (struct ifreq*)ifconf.ifc_buf;

	for (int i=(ifconf.ifc_len/sizeof (struct ifreq)); i>0; i--)
	{
		if(ifreq->ifr_flags == AF_INET)
		{ //for ipv4
			if( strncmp("192.168.1.", inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr), 10) == 0)
			{
				IP = inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr);
//				printf("name =[%s]\n" , ifreq->ifr_name);
//				printf("local addr = [%s]\n" ,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
			}
			ifreq++;
		}
	}

	return 0;
}
#else
#define MAX_PATH 260
int getLocalIP(string &IP)
{
	char hostname[MAX_PATH] = {0};
	gethostname(hostname, MAX_PATH);

	struct hostent* hostEnt;
	hostEnt = gethostbyname(hostname);
	if(hostEnt == NULL)
	{
		return -1;
	}

	for(int i = 0; hostEnt->h_addr_list[i]; i++)
	{
		if( strncmp("192.168.1.", inet_ntoa(*(struct in_addr*)(hostEnt->h_addr_list[i])), 10) == 0)
		{
			IP = inet_ntoa(*(struct in_addr*)(hostEnt->h_addr_list[i]));
		}
	}

	return 0;
}

#endif

const char xml_root_name[] = "autoTest";
#if 0
int pasreConfigXml(string storeContent)
{

	TY_STORAGE_ICD_MAP_MAP::iterator pos;
	TiXmlDocument * pDoc = NULL;

	CStorageManager *pManager;
	pManager = CStorageManager::getInstance();

	/* load xml file */
	pDoc = load_xml(storeContent.c_str(), FROM_DATA);//FROM_FILE  FROM_DATA
	if(pDoc == NULL)
	{
		printf("open xml fail\n");
		return -1;
	}
	TiXmlElement* rootElement = pDoc->RootElement();
	TiXmlElement* pRootXML = rootElement;
	if(pRootXML == NULL)
	{
		REPORT(REPORT_ERROR,"storage XML is NULL");
		return -1;
	}
	TiXmlElement* pStorageRoot = pRootXML->FirstChildElement();

	int ret = GetValue(pStorageRoot, "StorageName", pManager->m_storageName);
	if(ret == false)
	{
		REPORT(REPORT_ERROR, "get StorageName error");
		return -1;
	}
	TiXmlElement* pCase = pStorageRoot->FirstChildElement();
	TiXmlElement* pIcdID = NULL;
	TiXmlElement* pModel = NULL;
	TiXmlElement* pOutput = NULL;
	TiXmlElement* pInput = NULL;
	TiXmlElement* pParamInfo = NULL;
	pManager->m_modelStorageMap.clear();
	for(; pCase != NULL; pCase = pCase->NextSiblingElement())
	{
		string strTmp;
		string modelStorageName;
		TY_STORAGE_MODEL_VAR_MAP modelVarMap;
		TY_STORAGE_MODEL_INFO modelVarInfo;

		ret = GetValue(pCase, "CaseID", strTmp);
		if(ret == false)
		{
			REPORT(REPORT_ERROR, "get CaseID error");
			return -1;
		}
		char pPreCaseID[64] = {"0"};
		getPresentCase(pPreCaseID);

		if(strcmp((const char*)(strTmp.c_str()), pPreCaseID) == 0)
		{
			pManager->m_caseId = strTmp;
			//memset(pIcdInfo, 0, sizeof(TY_STORAGE_DATA_INFO));
			for(pIcdID = pCase->FirstChildElement("icd"); pIcdID != NULL; pIcdID = pIcdID->NextSiblingElement())
			{
				string tmpName = "ICDID";
				string strttt;
				//ret = GetValue(pIcdID, "ICDID", strTmp);
		#if 1
				TiXmlAttribute *Attribute = NULL;
				for (Attribute = pIcdID->FirstAttribute(); Attribute; Attribute
						= Attribute->Next())
				{
					string tmp = Attribute->Name();
					//printf("[%s:%s:%d]--> ---->name:%s, tmpName:%s\n", __FILE__, __FUNCTION__, __LINE__, tmp.c_str(), tmpName.c_str());
					if (tmp == tmpName)
					{
						ret = true;
						//strVal = Attribute->Value();
						strttt =  Attribute->Value();
						printf("[%s:%s:%d]--> true ret:%d,uuid:%s\n", __FILE__, __FUNCTION__, __LINE__,ret, strttt.c_str());
						break;
					}
					else
					{
						ret = false;
					}
				}
		#endif
				if(ret == false)
				{
					printf("[%s:%s:%d]--> icd parse error\n", __FILE__, __FUNCTION__, __LINE__);
					ret = -1;
					break;
				}
	#if 0
				ret = GetValue(pIcdID, "CardType", tmpName);
				if(ret == false)
				{
					printf("[%s:%s:%d]--> icd parse error\n", __FILE__, __FUNCTION__, __LINE__);
					ret = -1;
				}
				//strcpy(pIcdInfo->cardType, (char*)tmpName.c_str());

				ret = GetValue(pIcdID, "CardID", tmpName);
				if(ret == false)
				{
					printf("[%s:%s:%d]--> icd parse error\n", __FILE__, __FUNCTION__, __LINE__);
					ret = -1;
				}
				//pIcdInfo->cardID = atoi(tmpName.c_str());
				ret = GetValue(pIcdID, "ChnID", tmpName);
				if(ret == false)
				{
					printf("[%s:%s:%d]--> icd parse error\n", __FILE__, __FUNCTION__, __LINE__);
					ret = -1;
					break;
				}
				//pIcdInfo->chnID = atoi(tmpName.c_str());
	#endif
				string strIcdID = strttt;
				//REPORT(REPORT_INFO, "storage------ICDID:%s\n", strIcdID.c_str());
				//printf("------------------storage------------ICDID:%s\n",strIcdID.c_str());
				TY_ICD_STORAGE_MAP::iterator iter = pManager->m_icdStorageMap.find(strIcdID);
				if(iter == pManager->m_icdStorageMap.end())
				{
					TY_STORAGE_DATA_INFO* pIcdInfo = (TY_STORAGE_DATA_INFO*)new(TY_STORAGE_DATA_INFO);
					pIcdInfo->lastIndexTime.tv_sec = 65535;
					pManager->m_icdStorageMap.insert(pair<string, TY_STORAGE_DATA_INFO*>(strIcdID, pIcdInfo));
				}

			}
			
			for(pModel = pCase->FirstChildElement("model"); pModel!= NULL; pModel = pModel->NextSiblingElement())
			{	
				
				string outputName = "output";
				string inputName = "input";	

				ret = GetValue(pModel, "Name", modelStorageName);
				if(ret == false)
				{
					REPORT(REPORT_ERROR, "model name is NULL");
					return -1;
				}

				pOutput = pModel->FirstChildElement("output");
				if(pOutput == NULL)
				{
					REPORT(REPORT_ERROR,"pOutput nood is NULL");
					return -1;
				}

				for(pParamInfo = pOutput->FirstChildElement(); pParamInfo != NULL; pParamInfo = pParamInfo->NextSiblingElement())
				{
					string tmp1;
					ret = GetValue(pParamInfo, "Name", strTmp);
					if(ret == false)
					{
						REPORT(REPORT_ERROR,"pOutput param not have element: Name");
						return -1;
					}
					
					//tmp1 = "untitled1/" + strTmp;
				

					TY_STORAGE_MODEL_VAR_MAP::iterator pos = modelVarInfo.outputInfo.find(tmp1);
					if (pos == modelVarInfo.outputInfo.end())
					{
						modelVarInfo.outputInfo.insert(pair<string, double>(tmp1, 0));
						printf("[%s:%s:%d]--> tmp1:%s\n", __FILE__, __FUNCTION__, __LINE__,tmp1.c_str());
					}
					
				}

				//pInput = pOutput->NextSiblingElement("input");
				pInput = pModel->FirstChildElement("input");
				if(pInput == NULL)
				{
					REPORT(REPORT_ERROR,"pInput nood is NULL");
					return -1;
				}
				for(pParamInfo = pInput->FirstChildElement(); pParamInfo != NULL; pParamInfo = pParamInfo->NextSiblingElement())
				{
					ret = GetValue(pParamInfo, "Referenced", strTmp);
					if(ret == false)
					{
						REPORT(REPORT_ERROR,"pInput param not have element: VarName");
						return -1;
					}

					TY_STORAGE_MODEL_VAR_MAP::iterator pos = modelVarInfo.inputInfo.find(strTmp);
					if (pos == modelVarInfo.inputInfo.end())
					{
						modelVarInfo.inputInfo.insert(pair<string, double>(strTmp, 0));
					}
				}

				string strModelName = modelStorageName;
				TY_MODEL_STORAGE_MAP::iterator iter = pManager->m_modelStorageMap.find(strModelName);
				if(iter == pManager->m_modelStorageMap.end())
				{
					pManager->m_modelStorageMap.insert(pair<string, TY_STORAGE_MODEL_INFO>(strModelName, modelVarInfo));
				}
			}

		}
	}
	/*
	   TY_MODEL_STORAGE_MAP::iterator iterModel = pManager->m_modelStorageMap.begin();
	   if(iterModel != pManager->m_modelStorageMap.end())
	   {
	   for(unsigned int i = 0; i < iterModel->second.inputPath.size(); i++)
	   {
	   printf("--->iterModel->second.inputPath[i].c_str():%s\n", iterModel->second.inputPath[i].c_str());
	   }

	   }
	   */

	return 0;
}
#endif
int pasreConfigXml(string storeContent, string caseID)
{

	TY_STORAGE_ICD_MAP_MAP::iterator pos;
	TiXmlDocument * pDoc = NULL;

	CStorageManager *pManager;
	pManager = CStorageManager::getInstance();

	/* load xml file */
	pDoc = load_xml(storeContent.c_str(), FROM_DATA);//FROM_FILE  FROM_DATA
	if(pDoc == NULL)
	{
		printf("open xml fail\n");
		return -1;
	}
	TiXmlElement* rootElement = pDoc->RootElement();
	TiXmlElement* pRootXML = rootElement;
	if(pRootXML == NULL)
	{
		REPORT(REPORT_ERROR,"storage XML is NULL");
		return -1;
	}
	TiXmlElement* pStorageRoot = pRootXML->FirstChildElement();

	int ret = GetValue(pStorageRoot, "StorageName", pManager->m_storageName);
	if(ret == false)
	{
		REPORT(REPORT_ERROR, "get StorageName error");
		return -1;
	}
	TiXmlElement* pCase = pStorageRoot->FirstChildElement();
	TiXmlElement* pIcdID = NULL;
	TiXmlElement* pModel = NULL;
	TiXmlElement* pOutput = NULL;
	TiXmlElement* pInput = NULL;
	TiXmlElement* pParamInfo = NULL;
	pManager->m_modelStorageMap.clear();
	for(; pCase != NULL; pCase = pCase->NextSiblingElement())
	{
		string strTmp;
		string modelStorageName;
		TY_STORAGE_MODEL_VAR_MAP modelVarMap;
		TY_STORAGE_MODEL_INFO modelVarInfo;

		ret = GetValue(pCase, "CaseID", strTmp);
		if(ret == false)
		{
			REPORT(REPORT_ERROR, "get CaseID error");
			return -1;
		}
		char pPreCaseID[64] = {"0"};
		//getPresentCase(pPreCaseID);
		strcpy(pPreCaseID, caseID.c_str());

		if(strcmp((const char*)(strTmp.c_str()), pPreCaseID) == 0)
		{
			pManager->m_caseId = strTmp;
			//memset(pIcdInfo, 0, sizeof(TY_STORAGE_DATA_INFO));
			for(pIcdID = pCase->FirstChildElement("icd"); pIcdID != NULL; pIcdID = pIcdID->NextSiblingElement())
			{
				string tmpName = "ICDID";
				string strttt;
				//ret = GetValue(pIcdID, "ICDID", strTmp);
		#if 1
				TiXmlAttribute *Attribute = NULL;
				for (Attribute = pIcdID->FirstAttribute(); Attribute; Attribute
						= Attribute->Next())
				{
					string tmp = Attribute->Name();
					//printf("[%s:%s:%d]--> ---->name:%s, tmpName:%s\n", __FILE__, __FUNCTION__, __LINE__, tmp.c_str(), tmpName.c_str());
					if (tmp == tmpName)
					{
						ret = true;
						//strVal = Attribute->Value();
						strttt =  Attribute->Value();
						printf("[%s:%s:%d]--> true ret:%d,uuid:%s\n", __FILE__, __FUNCTION__, __LINE__,ret, strttt.c_str());
						break;
					}
					else
					{
						ret = false;
					}
				}
		#endif
				if(ret == false)
				{
					printf("[%s:%s:%d]--> icd parse error\n", __FILE__, __FUNCTION__, __LINE__);
					ret = -1;
					break;
				}
	#if 0
				ret = GetValue(pIcdID, "CardType", tmpName);
				if(ret == false)
				{
					printf("[%s:%s:%d]--> icd parse error\n", __FILE__, __FUNCTION__, __LINE__);
					ret = -1;
				}
				//strcpy(pIcdInfo->cardType, (char*)tmpName.c_str());

				ret = GetValue(pIcdID, "CardID", tmpName);
				if(ret == false)
				{
					printf("[%s:%s:%d]--> icd parse error\n", __FILE__, __FUNCTION__, __LINE__);
					ret = -1;
				}
				//pIcdInfo->cardID = atoi(tmpName.c_str());
				ret = GetValue(pIcdID, "ChnID", tmpName);
				if(ret == false)
				{
					printf("[%s:%s:%d]--> icd parse error\n", __FILE__, __FUNCTION__, __LINE__);
					ret = -1;
					break;
				}
				//pIcdInfo->chnID = atoi(tmpName.c_str());
	#endif
				string strIcdID = strttt;
				//REPORT(REPORT_ERROR, "storage------ICDID:%s\n", strIcdID.c_str());
				//printf("------------------storage------------ICDID:%s\n",strIcdID.c_str());
				TY_ICD_STORAGE_MAP::iterator iter = pManager->m_icdStorageMap.find(strIcdID);
				if(iter == pManager->m_icdStorageMap.end())
				{
					TY_STORAGE_DATA_INFO* pIcdInfo = (TY_STORAGE_DATA_INFO*)new(TY_STORAGE_DATA_INFO);
					pIcdInfo->lastIndexTime.tv_sec = 65535;
					//strcpy(pIcdInfo->icdID, strIcdID.c_str());
                   //REPORT(REPORT_ERROR, "storage---insert Map. pIcdInfo.icd = %s \n", pIcdInfo->icdID);
					pManager->m_icdStorageMap.insert(pair<string, TY_STORAGE_DATA_INFO*>(strIcdID, pIcdInfo));
				}
				//REPORT(REPORT_ERROR, "storage------m_icdStorageMap.size:  %d \n", pManager->m_icdStorageMap.size());

			}
			
			for(pModel = pCase->FirstChildElement("model"); pModel!= NULL; pModel = pModel->NextSiblingElement())
			{	
				
				string outputName = "output";
				string inputName = "input";	

				ret = GetValue(pModel, "Name", modelStorageName);
				if(ret == false)
				{
					REPORT(REPORT_ERROR, "model name is NULL");
					return -1;
				}

				pOutput = pModel->FirstChildElement("output");
				if(pOutput == NULL)
				{
					REPORT(REPORT_ERROR,"pOutput nood is NULL");
					return -1;
				}

				for(pParamInfo = pOutput->FirstChildElement(); pParamInfo != NULL; pParamInfo = pParamInfo->NextSiblingElement())
				{
					string tmp1;
					ret = GetValue(pParamInfo, "Referenced", strTmp);
					if(ret == false)
					{
						REPORT(REPORT_ERROR,"pOutput param not have element: VarName");
						return -1;
					}
					
					//tmp1 = "untitled1/" + strTmp;
				

					TY_STORAGE_MODEL_VAR_MAP::iterator pos = modelVarInfo.outputInfo.find(strTmp);
					if (pos == modelVarInfo.outputInfo.end())
					{
						modelVarInfo.outputInfo.insert(pair<string, double>(strTmp, 0));
						printf("[%s:%s:%d]--> strTmp:%s\n", __FILE__, __FUNCTION__, __LINE__,strTmp.c_str());
					}
					
				}

				//pInput = pOutput->NextSiblingElement("input");
				pInput = pModel->FirstChildElement("input");
				if(pInput == NULL)
				{
					REPORT(REPORT_ERROR,"pInput nood is NULL");
					return -1;
				}
				for(pParamInfo = pInput->FirstChildElement(); pParamInfo != NULL; pParamInfo = pParamInfo->NextSiblingElement())
				{
					ret = GetValue(pParamInfo, "Name", strTmp);
					if(ret == false)
					{
						REPORT(REPORT_ERROR,"pInput param not have element: Name");
						return -1;
					}
                    string tmpReferenced;
                    ret = GetValue(pParamInfo, "Referenced", tmpReferenced);
					if(ret == false)
					{
						REPORT(REPORT_ERROR,"pInput param not have element: Referenced");
						return -1;
					}

					//TY_STORAGE_MODEL_VAR_MAP::iterator pos = modelVarInfo.inputInfo.find(strTmp);
                    TY_STORAGE_MODEL_VAR_MAP::iterator pos = modelVarInfo.inputInfo.find(tmpReferenced);
					if (pos == modelVarInfo.inputInfo.end())
					{
						modelVarInfo.inputInfo.insert(pair<string, double>(tmpReferenced, 0));
					}
				}

				string strModelName = modelStorageName;
				TY_MODEL_STORAGE_MAP::iterator iter = pManager->m_modelStorageMap.find(strModelName);
				if(iter == pManager->m_modelStorageMap.end())
				{
					pManager->m_modelStorageMap.insert(pair<string, TY_STORAGE_MODEL_INFO>(strModelName, modelVarInfo));
				}
			}

		}
	}
	/*
	   TY_MODEL_STORAGE_MAP::iterator iterModel = pManager->m_modelStorageMap.begin();
	   if(iterModel != pManager->m_modelStorageMap.end())
	   {
	   for(unsigned int i = 0; i < iterModel->second.inputPath.size(); i++)
	   {
	   printf("--->iterModel->second.inputPath[i].c_str():%s\n", iterModel->second.inputPath[i].c_str());
	   }

	   }
	   */

	return 0;
}

/*int CStorageManager::getParseInfo()
{
	int ret = pasreConfigXml(storeXml);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "pasreConfigXml error\n");
		return -1;
	}
	return 0;
}*/

int CStorageManager::getParseInfo(string caseID)
{
	int ret = pasreConfigXml(storeXml, caseID);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "pasreConfigXml error\n");
		return -1;
	}
	return 0;
}

/*******************************************************************************
 * Function Name  : pasreRuleConfigXml
 * Description    : parse_rule_xml
 * Input          : pData: xml doc name
 *                  pManager: CServerManager*
 * Output         : msg
 * Return         : success return 0;failed return -1;
 *******************************************************************************/
int pasreRuleConfigXml(string storeContent)
{

	TiXmlNode* firstChild = NULL;
	TiXmlNode* rootNode = NULL;
	TiXmlElement *tmpElement = NULL;
	TiXmlAttribute *Attribute = NULL;

	TiXmlDocument * pDoc = NULL;

	CStorageManager *pManager;
	pManager = CStorageManager::getInstance();

	/* load xml file */
	pDoc = load_xml(storeContent.c_str(), FROM_DATA);//FROM_FILE
	if (pDoc == NULL)
	{
		REPORT(REPORT_ERROR, "load xml faild\n");
		return -1;
	}
	else
	{
		rootNode = pDoc->FirstChild(xml_root_name);
	}


	if (rootNode == NULL)
	{
		REPORT(REPORT_ERROR, "Router can't find the node of %s\n", xml_root_name);
		return -1;
	}

	tmpElement = (TiXmlElement *)rootNode;

	//store rule  manul_control? timing ? triger?>>
	pManager->m_storageRule.m_type = MANUAL_CONTROL;//manual defalut
	firstChild = rootNode->FirstChild("storeControlStyle");
	if (firstChild == NULL)
	{
		//err_msg("Can't find the node of storeControlStyle\n");
		REPORT(REPORT_ERROR,"Can't find the node of storeControlStyle\n");
		return -1;
	}
	if (firstChild != NULL)
	{
		for (tmpElement = firstChild->FirstChildElement(); tmpElement; tmpElement
				= tmpElement->NextSiblingElement())
		{
			Attribute = tmpElement->FirstAttribute();
			pManager->m_storageRule.m_type = atoi(Attribute->Value());
			//			pManager->storeRecords.storeStyle = atoi(Attribute->Value());
			//DEBUG("Storage rule(1M,2Ti,3Tr):  %d \n",pManager->m_storageRule.m_type);
			REPORT(REPORT_INFO,"Storage rule(1M,2Ti,3Tr):  %d \n",pManager->m_storageRule.m_type);
		}
	}

	if (pManager->m_storageRule.m_type == TIMING_CONTROL)
	{//store rule  timing >>
		firstChild = rootNode->FirstChild("storeRule");

		if (firstChild == NULL)
		{
			//err_msg("Can't find the node of storeRule\n");
			REPORT(REPORT_ERROR,"Can't find the node of storeRule\n");
			return -1;
		}
		if (firstChild != NULL)
		{
			//for (tmpElement = firstChild->FirstChildElement(); tmpElement; tmpElement = tmpElement->NextSiblingElement())
			for (Attribute = ((TiXmlElement*) firstChild)->FirstAttribute(); Attribute; Attribute = Attribute->Next())
			{
				//DEBUG("%s %s \n", Attribute->Name(),Attribute->Value());
				REPORT(REPORT_INFO,"%s %s \n", Attribute->Name(),Attribute->Value());

				string tmpStr = Attribute->Name();
				if (tmpStr == "startTime")
				{
					pManager->m_storageRule.m_timeRule.fromSec = atoi(Attribute->Value());
					REPORT(REPORT_INFO,"pManager->m_storageRule.m_timeRule.fromSec : %ld \n",pManager->m_storageRule.m_timeRule.fromSec);
				}
				else if (tmpStr == "stopTime")
				{
					pManager->m_storageRule.m_timeRule.toSec = atoi(Attribute->Value());
					//DEBUG("pManager->m_storageRule.m_timeRule.toSec : %ld \n", pManager->m_storageRule.m_timeRule.toSec);
					REPORT(REPORT_INFO,"pManager->m_storageRule.m_timeRule.toSec : %ld \n", pManager->m_storageRule.m_timeRule.toSec);
				}
			}
		}
	}

	return 0;
}


static int storageData(TY_STORAGE_DATA *msg, TY_STORAGE_DATA_INFO *ICDInfo, int type)
{
	CStorageManager *pManager = CStorageManager::getInstance();
	string tmpFileName = pManager->fileName;
	char datafileNum[256];

	if( (msg == NULL) || (ICDInfo == NULL))
	{
		REPORT(REPORT_ERROR, "para point is empty\n");
		return -1;
	}
	if( (type != DATA_TYPE_ICD) && (type != DATA_TYPE_MODEL))
	{
		REPORT(REPORT_ERROR, "storage type is error\n");
		return -1;
	}

	memset(datafileNum, 0, sizeof(datafileNum));
    //REPORT(REPORT_ERROR, "tmpFileName :%s \n", tmpFileName.c_str());
	/*record icd info*/
	ICDInfo->indexHeader.ICDCount++;
	strcpy(ICDInfo->icdID, msg->uuid);
	ICDInfo->dataType = msg->dataType;
	ICDInfo->chnID = msg->chnId;
	ICDInfo->dataHeader.ICDCountPerFile++;

	ICDInfo->dataHeader.endTime.tv_sec = msg->sec;
	ICDInfo->dataHeader.endTime.tv_usec = msg->usec;

	ICDInfo->indexHeader.endTime.tv_sec = msg->sec;
	ICDInfo->indexHeader.endTime.tv_usec = msg->usec;
    //REPORT(REPORT_ERROR, "dataHeader :endTime %d  endTime.tv_usec = %d  \n",msg->sec, msg->usec);
	/*first data in this storage*/
	if(ICDInfo->indexHeader.ICDCount == 1)
	{
		ICDInfo->indexHeader.fileCount = 1;
		ICDInfo->indexHeader.firstPacketTime.tv_sec = msg->sec;
		ICDInfo->indexHeader.firstPacketTime.tv_usec = msg->usec;

		if(type == DATA_TYPE_ICD)
		{
			sprintf(datafileNum, "%s_%d.data%d", ICDInfo->icdID, ICDInfo->chnID, ICDInfo->indexHeader.fileCount-1);
		}
		else if(type == DATA_TYPE_MODEL)
		{
			sprintf(datafileNum, "%s.data%d", ICDInfo->icdID, ICDInfo->indexHeader.fileCount-1);
		}
		tmpFileName.append(datafileNum);
#ifndef _WIN32
		ICDInfo->dataFileFd = open(tmpFileName.c_str(), O_CREAT | O_RDWR, OPEN_FILE_MODE);
#else
		ICDInfo->dataFileFd = open(tmpFileName.c_str(), O_CREAT | O_RDWR | O_BINARY, OPEN_FILE_MODE);
#endif

		if(ICDInfo->dataFileFd <= 0)
		{
			REPORT(REPORT_ERROR, "fileName :%s fd:%d \n", tmpFileName.c_str(), ICDInfo->dataFileFd);
			REPORT(REPORT_ERROR, "open first ICD data file faild\n");
			return -1;
		}
		lseek(ICDInfo->dataFileFd, HERADER_LEN, SEEK_SET);
	}

	if(ICDInfo->dataFileFd == 0)
	{
		ICDInfo->indexHeader.ICDCount--;
		ICDInfo->dataHeader.ICDCountPerFile--;
		return -1;
	}

	/*first data in each data file*/
	if(ICDInfo->dataHeader.ICDCountPerFile == 1)
	{
		ICDInfo->dataHeader.startTime.tv_sec = msg->sec;
		ICDInfo->dataHeader.startTime.tv_usec = msg->usec;
	}

	int pack_length = sizeof(TY_STORAGE_DATA) - PAYLOAD_LEN + msg->length;
	if( (type == DATA_TYPE_ICD) ||  (type == DATA_TYPE_MODEL))
	{
		ICDInfo->indexHeader.ICDDataLen = pack_length;
	}

	ICDInfo->lastDataLen = pack_length;
	ICDInfo->dataIndex += pack_length;
	/****** copy the tmp buffer data to 64K buffer***/
	if ( (ICDInfo->allLength + pack_length ) < MAX_STORE )////-----------
	{
		memcpy((ICDInfo->maxDataBuff + ICDInfo->allLength), msg, pack_length);
	}
	ICDInfo->allLength += pack_length;

	//write data to file
	if ( (ICDInfo->allLength + sizeof(TY_STORAGE_DATA)) >= MAX_STORE )
	{
		if (write(ICDInfo->dataFileFd, ICDInfo->maxDataBuff, ICDInfo->allLength) <= 0)
		{
			REPORT(REPORT_ERROR, "write maxBuf into data file failed !!!\n");
		}
		memset(ICDInfo->maxDataBuff, 0, MAX_STORE);

		ICDInfo->allLength = 0;
		ICDInfo->writeFileCount++;
	}

	//change data files
	if (ICDInfo->writeFileCount == MAX_ONEFILE_STORE)
	{

		/*record data file size*/
		ICDInfo->allDataFileSize += ICDInfo->dataIndex + HERADER_LEN;
		//storage dataIndex when change data files
		ICDInfo->lastDataIndex = ICDInfo->dataIndex;
		ICDInfo->dataIndex = 0;

		//write header in data file
		writeDataHeader(ICDInfo);

		//close current data file
		close(ICDInfo->dataFileFd);
		ICDInfo->dataFileFd = 0;
		ICDInfo->dataHeader.ICDCountPerFile = 0;
		ICDInfo->indexHeader.fileCount++;
		ICDInfo->writeFileCount = 0;

		//open next data file
		if(type == DATA_TYPE_ICD)
		{
			sprintf(datafileNum, "%s_%d.data%d", ICDInfo->icdID, ICDInfo->chnID, ICDInfo->indexHeader.fileCount-1);
		}
		else if(type == DATA_TYPE_MODEL)
		{
			sprintf(datafileNum, "%s.data%d", ICDInfo->icdID, ICDInfo->indexHeader.fileCount-1);
		}

		tmpFileName.append(datafileNum);
#ifndef _WIN32
		ICDInfo->dataFileFd = open(tmpFileName.c_str(), O_CREAT | O_RDWR, OPEN_FILE_MODE);
#else
		ICDInfo->dataFileFd = open(tmpFileName.c_str(), O_CREAT | O_RDWR | O_BINARY, OPEN_FILE_MODE);
#endif
		if(ICDInfo->dataFileFd <= 0)
		{
			REPORT(REPORT_ERROR, "open next data file faild!!!\n");
			ICDInfo->indexHeader.fileCount--;
			return -1;
		}
		lseek(ICDInfo->dataFileFd, HERADER_LEN, SEEK_SET);
	}
	return 0;
}

static int storageIndex(TY_STORAGE_DATA_INFO *ICDInfo, int dataType, int indexType)
{
	CStorageManager *pManager = CStorageManager::getInstance();
	string tmpFileName = pManager->fileName;
	char fileName[256];
	unsigned int dataIndex = 0;
	int *tmpFd = NULL;

	memset(fileName, 0, sizeof(fileName));

	if(ICDInfo == NULL)
	{
		REPORT(REPORT_ERROR, "write index file faild\n");
		return -1;
	}

	if( dataType == DATA_TYPE_ICD )
	{
		sprintf(fileName, "%s_%d", ICDInfo->icdID, ICDInfo->chnID);
		tmpFileName.append(fileName);
	}
	else if( dataType == DATA_TYPE_MODEL )
	{
		sprintf(fileName, "%s", ICDInfo->icdID);
		tmpFileName.append(fileName);
		//REPORT(REPORT_ERROR, "fileName :%s \n", tmpFileName.c_str());
	}
	else
	{
		REPORT(REPORT_ERROR, "write index file faild\n");
		return -1;
	}
	if( indexType == INFO_INDEX_TYPE )
	{
		tmpFileName.append(".index");
		tmpFd = &ICDInfo->infoIndexFileFd;
	}
	else if(indexType == TIME_INDEX_TYPE)
	{
		tmpFileName.append( ".timeIndex");
		tmpFd = &ICDInfo->timeIndexFileFd;
	}
	else
	{
		REPORT(REPORT_ERROR, "write index file faild\n");
		return -1;
	}


	if(*tmpFd == 0)
	{
#ifndef _WIN32
		*tmpFd = open(tmpFileName.c_str(), O_CREAT | O_RDWR, OPEN_FILE_MODE);
#else
		*tmpFd = open(tmpFileName.c_str(), O_CREAT | O_RDWR | O_BINARY, OPEN_FILE_MODE);
#endif

		if(*tmpFd <= 0)
		{
			REPORT(REPORT_ERROR, "open ICDInfo file faild\\n");
			return -1;
		}
		lseek(*tmpFd, HERADER_LEN, SEEK_SET);
	}

	//get data file num
	unsigned int fileNo = ICDInfo->indexHeader.fileCount-1;
	if (write(*tmpFd, &fileNo, sizeof(fileNo)) == 0)
	{
		perror("error:");
		REPORT(REPORT_ERROR, "write index file : fileNo faild!!!\n");
		return -1;
	}

	//get data index
	if(ICDInfo->dataIndex == 0)
	{
		dataIndex = ICDInfo->lastDataIndex + HERADER_LEN - ICDInfo->lastDataLen;
	}
	else
	{
		dataIndex = ICDInfo->dataIndex + HERADER_LEN - ICDInfo->lastDataLen;
	}

	if (write(*tmpFd, &dataIndex, sizeof(dataIndex)) <= 0)
	{
		perror("error:");
		REPORT(REPORT_ERROR, "write index file : dataIndex faild!!!\n");
		return -1;
	}

	return 0;
}

static int  storageEmptyTimeIndex(TY_STORAGE_DATA_INFO *ICDInfo, TY_STORAGE_TIME *tmpEmptyICDInfo)
{
	unsigned int lastIndexTime = 0;
	unsigned int tmpCurrentIndexTime = 0;
	char fileName[256];
	int dataIndex = 0;
	int *tmpFd = NULL;

	memset(fileName, 0, sizeof(fileName));

	if(ICDInfo == NULL)
	{
		REPORT(REPORT_ERROR, "storageEmptyTimeIndex faild\n");
		return -1;
	}

	tmpFd = &ICDInfo->timeIndexFileFd;
	if(*tmpFd <= 0)//when you write timeIndex file first, you will not write empty index.
	{
		return 0;
	}

	//last time when you write time index file
	lastIndexTime = (tmpEmptyICDInfo->tv_sec - ICDInfo->indexHeader.firstPacketTime.tv_sec) / INDEX_DATA_TIME;

	//current time that you want to write time index file
	tmpCurrentIndexTime = (ICDInfo->indexHeader.endTime.tv_sec - ICDInfo->indexHeader.firstPacketTime.tv_sec) / INDEX_DATA_TIME;

	while(tmpCurrentIndexTime > lastIndexTime)
	{
		tmpEmptyICDInfo->tv_sec += INDEX_DATA_TIME;
		if(tmpEmptyICDInfo->tv_sec == ICDInfo->indexHeader.endTime.tv_sec)
		{
			break;
		}
		lastIndexTime = (tmpEmptyICDInfo->tv_sec - ICDInfo->indexHeader.firstPacketTime.tv_sec) / INDEX_DATA_TIME;

		/*write empty time index >>>>>>>*/
		//get data file num
		int fileNo = -1;
		if (write(*tmpFd, &fileNo, sizeof(fileNo)) <= 0)
		{
			perror("error:");
			REPORT(REPORT_ERROR, "write index file : fileNo faild!!!\n");
		}

		//get data index
		dataIndex = -1;
		if (write(*tmpFd, &dataIndex, sizeof(dataIndex)) <= 0)
		{
			perror("error:");
			REPORT(REPORT_ERROR, "write index file : dataIndex faild!!!\n");
		}
		/*write empty time index <<<<<<<<*/
	}

	return 0;
}

/*wite data file header*/
void writeDataHeader(TY_STORAGE_DATA_INFO *ICDInfo)
{
	if(ICDInfo == NULL)
	{
		REPORT(REPORT_ERROR, "writeDataHeader faild\n");
		return;
	}
	if(ICDInfo->dataFileFd <= 0)
	{
		REPORT(REPORT_ERROR, "writeDataHeader faild\n");
		return ;
	}
	lseek(ICDInfo->dataFileFd, 0, SEEK_SET);
	if (write(ICDInfo->dataFileFd, &ICDInfo->dataHeader, sizeof(ICDInfo->dataHeader)) <= 0)
	{

		perror("error:");
		REPORT(REPORT_ERROR, "write header in file failed !!!\n");
	}
}

/*write info index file and time index file header*/
void writeIndexHeader(TY_STORAGE_DATA_INFO *ICDInfo)
{
	if(ICDInfo == NULL)
	{
		REPORT(REPORT_ERROR, "writeIndexHeader faild\n");
		return;
	}
	if(ICDInfo->infoIndexFileFd <= 0)
	{
		REPORT(REPORT_ERROR, "writeIndexHeader faild\n");
		return;
	}

	if(ICDInfo->timeIndexFileFd <= 0)
	{
		REPORT(REPORT_ERROR, "writeIndexHeader faild\n");
		return;
	}
	//write header into ICD info index file
	lseek(ICDInfo->infoIndexFileFd, 0, SEEK_SET);
	if (write(ICDInfo->infoIndexFileFd, &ICDInfo->indexHeader, HERADER_LEN) <= 0)
	{
		perror("error:");
		REPORT(REPORT_ERROR, "write header into infoIndexfile failed !!!\n");
	}

	close(ICDInfo->infoIndexFileFd);

	ICDInfo->infoIndexFileFd = 0;

	//write header into ICD time index file
	lseek(ICDInfo->timeIndexFileFd, 0, SEEK_SET);
	if (write(ICDInfo->timeIndexFileFd, &ICDInfo->indexHeader, HERADER_LEN) <= 0)
	{
		perror("error:");
		REPORT(REPORT_ERROR, "write header into timeIndexfile failed !!!\n");
	}

	close(ICDInfo->timeIndexFileFd);
	ICDInfo->timeIndexFileFd = 0;

}


static void* storageThread(void * pParam)
{

	TY_STORAGE_DATA tmpData;
	TY_STORAGE_DATA_INFO *tmpInfo;

	CStorageManager *pManager = CStorageManager::getInstance();
	int ret  = 0;

	while (pManager->m_storageStatus == SERVICE_RUN)
	{
		//read from FIFO
		pthread_mutex_lock(&pManager->fifoLock);
		int cnt = pManager->serviceDataFIFO.size();
		pthread_mutex_unlock(&pManager->fifoLock);
		if(cnt == 0)
		{
#ifdef _WIN32
			Sleep(1);
#else
			usleep(1);
#endif
			continue;
		}
		while((cnt--) > 0)
		{
			pthread_mutex_lock(&pManager->fifoLock);
			memcpy(&tmpData, &pManager->serviceDataFIFO.front(), sizeof(TY_STORAGE_DATA));
			pManager->serviceDataFIFO.pop();
			pthread_mutex_unlock(&pManager->fifoLock);

			TY_ICD_STORAGE_MAP::iterator pos;

			pos = pManager->m_icdStorageMap.find(tmpData.uuid);
			if (pos == pManager->m_icdStorageMap.end())
			{
				//REPORT(REPORT_WARNING, "recv data is not in map uuid:%s\n", tmpData.uuid);
			}
			else
			{

				//REPORT(REPORT_WARNING, "recv data uuid:%s, usec:%d,chnID:%d ,dataType:%d\n", tmpData.uuid, tmpData.chnId, tmpData.usec,tmpData.dataType);
				//tmpInfo = findICDDataInfo(&pos->second, tmpData.uuid);
				tmpInfo = pos->second;
				if(pos->second == NULL)
				{
					REPORT(REPORT_WARNING, "recv data pos->second is empty:%s\n", tmpData.uuid);
					pos->second = new(TY_STORAGE_DATA_INFO);
					continue;
					/*

					pos->second->lastIndexTime.tv_sec = 65535;
					*/
				}
				//printf("ICDID:%s, chnID:%d\n",  tmpInfo->icdID, tmpInfo->chnID);
				ret = storageData(&tmpData, pos->second, tmpData.dataType);
				if(ret)
				{
					REPORT(REPORT_WARNING, "storage icd data error\n");
				}

				//write count index
				if( ((tmpInfo->indexHeader.ICDCount - 1) % INDEX_DATA_COUNT) == 0)
				{
					ret = storageIndex(tmpInfo,  tmpData.dataType, INFO_INDEX_TYPE);
					if(ret)
					{
						REPORT(REPORT_ERROR, "storage icd count Index error\n");
					}
				}

				//write time index
				if( (tmpInfo->lastIndexTime.tv_sec != tmpInfo->indexHeader.endTime.tv_sec)
						&& ((tmpInfo->indexHeader.endTime.tv_sec > tmpInfo->lastIndexTime.tv_sec) || tmpInfo->lastIndexTime.tv_sec == 65535) )
				{
					ret = storageEmptyTimeIndex(tmpInfo, &tmpInfo->lastIndexTime);
					if(ret)
					{
						REPORT(REPORT_ERROR, "storage icd empty time Index error\n");
					}

					if( ((tmpInfo->indexHeader.endTime.tv_sec - tmpInfo->indexHeader.firstPacketTime.tv_sec)
								% INDEX_DATA_TIME) == 0)
					{
						ret = storageIndex(tmpInfo, tmpData.dataType, TIME_INDEX_TYPE);
						if(ret)
						{
							REPORT(REPORT_ERROR, "storage icd time Index error\n");
						}
						tmpInfo->lastIndexTime.tv_sec = tmpInfo->indexHeader.endTime.tv_sec;
					}
				}

			}
		}//end while cnt

	}
	//printf("[%s:%s:%d]--> ---------->while over\n", __FILE__, __FUNCTION__, __LINE__);
	//write remain data into files
	for(TY_ICD_STORAGE_MAP::iterator pos = pManager->m_icdStorageMap.begin();
			pos != pManager->m_icdStorageMap.end(); pos++)
	{
		if( (pos->second->dataFileFd <= 0) || (pos->second->indexHeader.ICDCount <= 0))
		{
			continue;
		}

		if (pos->second->allLength > 0)
		{
			if (write(pos->second->dataFileFd, pos->second->maxDataBuff, pos->second->allLength) <= 0)
			{
				perror("error:");
				REPORT(REPORT_ERROR, "write remain ICD data to file\n");
			}
			pos->second->allLength = 0;
		}

		/*record data file size*/
		pos->second->allDataFileSize += pos->second->dataIndex + HERADER_LEN;

		unsigned int offset = lseek(pos->second->dataFileFd, 0, SEEK_CUR);
		if(offset == HERADER_LEN)//the last file data don't have data,remove it
		{
			close(pos->second->dataFileFd);

			/*remove invalid datafile*/
			char datafileName[256];
			string tmpFileName = pManager->fileName;
			sprintf(datafileName, "%s_%d.data%d", pos->second->icdID, pos->second->chnID, pos->second->indexHeader.fileCount-1);
			//sprintf(datafileNum, "%d.data%d", ICDInfo->sn, ICDInfo->indexHeader.fileCount-1);
			tmpFileName.append(datafileName);
			pos->second->indexHeader.fileCount-= 1;
			remove(tmpFileName.c_str());
		}
		else
		{
			//write header in data file
			writeDataHeader(pos->second);
			close(pos->second->dataFileFd);

		}
		//write header into ICD info index and time index
		writeIndexHeader(pos->second);

	}
	//printf("[%s:%s:%d]--> ---------->storageStop success\n", __FILE__, __FUNCTION__, __LINE__);

	return NULL;
}

#ifdef WIN32
static void * spaceWarnFunc(void *param)
{
	return NULL;
}
#else
static void * spaceWarnFunc(void *param)
{
	CStorageManager *pManager = (CStorageManager*) param;
	struct statfs diskInfo;

	unsigned long long blockSize;
	double freeDisk;

	while (pManager->m_storageStatus == SERVICE_RUN)
	{
		statfs(pManager->m_config.storagePath.c_str(), &diskInfo);
		blockSize = diskInfo.f_bsize;
		freeDisk = diskInfo.f_bavail * blockSize;
		freeDisk = freeDisk /1024/1024;
		pManager->m_config.freeDisk = freeDisk;
		if(freeDisk < pManager->m_config.spaceWarn * 1024)
		{
			REPORT(REPORT_ERROR, "WARN!!! Path : %s Space is lower than the spaceWarn limit(%d GB)!!! The storage will Stop !!! \n",
					pManager->m_config.storagePath.c_str(),
					pManager->m_config.spaceWarn);
			pManager->stopStorage();
			break;
		}
#ifdef _WIN32
		Sleep(500);
#else
		usleep(500000);
#endif
	}
	return NULL;
}
#endif

void CStorageManager::addSnStorageRange(int devId)
{

}



int CStorageManager::insertModelData(TY_STORAGE_DATA *data)
{

	static TY_STORAGE_DATA stroageData = {0};
	if(data == NULL)
	{
		REPORT(REPORT_ERROR, "insertData point is empty\n");
		return -1;
	}
	if(m_storageStatus == SERVICE_RUN)
	{
		if(m_storageRule.triggerFlag == 1)
		{
			string fullPath = data->uuid;
			TY_ICD_STORAGE_MAP::iterator iter = m_icdStorageMap.find(fullPath);
			if(iter == m_icdStorageMap.end())
			{
				TY_STORAGE_DATA_INFO* pIcdInfo = (TY_STORAGE_DATA_INFO*)new(TY_STORAGE_DATA_INFO);
				memset(pIcdInfo, 0, sizeof(TY_STORAGE_DATA_INFO));

				pIcdInfo->lastIndexTime.tv_sec = 65535;
				m_icdStorageMap.insert(pair<string, TY_STORAGE_DATA_INFO*>(fullPath, pIcdInfo));
				REPORT(REPORT_ERROR, "insertDataMap fullPath = %s \n",fullPath.c_str());
			}
			pthread_mutex_lock(&fifoLock);
			if(serviceDataFIFO.size() > MAX_FIFO_SIZE)
			{
				REPORT(REPORT_ERROR, "insert data fifo is full\n");
				pthread_mutex_unlock(&fifoLock);
				return -1;
			}
			serviceDataFIFO.push(*data);
			pthread_mutex_unlock(&fifoLock);
		}

	}

	return 0;
}

int CStorageManager::insertData(TY_STORAGE_DATA *data)
{
	if(data == NULL)
	{
		//REPORT(REPORT_ERROR, "insertData point is empty\n");
		return -1;
	}
	if(m_storageStatus == SERVICE_RUN)
	{
		if(m_storageRule.triggerFlag == 1)
		{
			pthread_mutex_lock(&fifoLock);
			if(serviceDataFIFO.size() > MAX_FIFO_SIZE)
			{
				REPORT(REPORT_ERROR, "insert data fifo is full\n");
				pthread_mutex_unlock(&fifoLock);
				return -1;
			}
			REPORT(REPORT_INFO, "------------------insertData->uuid:%s\n", data->uuid);
			serviceDataFIFO.push(*data);
			pthread_mutex_unlock(&fifoLock);
		}

	}

	return 0;
}

int CStorageManager::initStorage(const char* storeContent)
{


	storeXml = storeContent;


	//int ret = pasreConfigXml(storeXml);

   /*ret = pasreRuleConfigXml(storeContent);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "pasreRuleConfigXml error\n");
	}*/
	return 0;

}

int CStorageManager::startStorage(string caseID)
{
	struct timeval tmpTime;
	CStorageManager *pManager = CStorageManager::getInstance();
	int ret  = 0;


	ret = pasreConfigXml(storeXml, caseID);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "pasreConfigXml error\n");
		return -1;
	}
	if(m_storageName.empty())
	{
		REPORT(REPORT_ERROR, "storageName is empty\n");
		return -1;
	}

	time_t now;
	struct tm *timeNow;
	time(&now);
	timeNow = localtime(&now);

	if(m_storageStatus == SERVICE_STOP)
	{
		int status = 0;
		char cmd[256];
		m_config.storageName.clear();
		m_config.storageName = m_storageName;
		m_config.caseID = m_caseId;
		gettimeofday(&tmpTime, NULL);
		//m_config.storageStopTimeSec = tmpTime.tv_sec;
		m_config.storageStartTimeSec = tmpTime.tv_sec;

		/*get system time*/
		m_config.startTimeSec = tmpTime.tv_sec;
		m_config.startTimeUsec = tmpTime.tv_usec;
		/*mkdir*/
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "%s_%s_%d.%02d.%02d_%02d:%02d:%02d",
				m_config.storageName.c_str(),
				m_config.caseID.c_str(),
				timeNow->tm_year+1900,
				timeNow->tm_mon + 1,
				timeNow->tm_mday,
				timeNow->tm_hour,
				timeNow->tm_min,
				timeNow->tm_sec);
		m_config.storageName = cmd;
		sprintf(cmd, "mkdir -p \"%s/%s\"",m_config.storagePath.c_str(), m_config.storageName.c_str());
		system(cmd);

		/*get file name*/
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "%s/%s/", m_config.storagePath.c_str(), m_config.storageName.c_str());
		fileName.clear();
		fileName.append(cmd);
		int cnt = serviceDataFIFO.size();
		for(int i = 0; i < cnt; i++)
		{
			serviceDataFIFO.pop();
		}


		/*clear the data fifo*/
		TY_STORAGE_FIFO empty;
		serviceDataFIFO = empty;
		clearStorageDataInfo();

		status = pthread_attr_init(&threadAttr);
		if (status != 0)
		{
			REPORT(REPORT_ERROR, "init the pthread attr failed !!\n");
			return -1;
		}
		status = pthread_attr_setstacksize(&threadAttr, MAX_STACK);
		if (status != 0)
		{
			REPORT(REPORT_ERROR, "set stack in pthread failed !!!\n");
			return -1;
		}

		m_storageStatus = SERVICE_RUN;
		status = pthread_create(&threadId, //ID of Thread being created
				&threadAttr, //Default attributes--//
				storageThread, //Pointer to thread function
				NULL); //Argument for new thread... (void*) &m_pthreadParam
		if (status != 0)
		{
			REPORT(REPORT_ERROR, "thread for storage msg failed\n");
			return -1;
		}
		//printf("[%s:%s:%d]--> ---->>sotrage start threadId:%d\n", __FILE__, __FUNCTION__, __LINE__,threadId);
		(void) pthread_attr_destroy(&threadAttr);

		int result = pthread_create(&spaceWarnThreadId, NULL, spaceWarnFunc, this);
		if (result != 0)
		{
			REPORT(REPORT_ERROR, "create spaceWarnThreadId failed !!!");
			return -1;
		}
		pManager->getStoreInfo();
	}
	else
	{
		REPORT(REPORT_WARNING, "storage server has start!!\n");
	}


	return 0;

}


void CStorageManager::clearStorageDataInfo(void)
{

	for(TY_ICD_STORAGE_MAP::iterator pos = m_icdStorageMap.begin();
			pos != m_icdStorageMap.end(); pos++)
	{
		memset(pos->second, 0, sizeof(TY_STORAGE_DATA_INFO));
	}
	//m_icdStorageMap.clear();
}

void CStorageManager::deleteStorageDataInfo(void)
{

	for(TY_ICD_STORAGE_MAP::iterator pos = m_icdStorageMap.begin();
			pos != m_icdStorageMap.end(); pos++)
	{
		delete pos->second;
	}
	m_icdStorageMap.clear();
}

int CStorageManager::stopStorage()
{
	struct timeval tmpTime;
	
	gettimeofday(&tmpTime, NULL);
	m_config.storageStopTimeSec = tmpTime.tv_sec;
	m_config.stopTimeSec = tmpTime.tv_sec;
	m_config.stopTimeUsec = tmpTime.tv_usec;

	insertStorageXml();

	m_storageStatus = SERVICE_STOP;

#ifdef _WIN32
	if (0 != threadId.p)
#else
	if(threadId)
#endif
	{
		pthread_join(threadId, NULL);
#ifdef _WIN32
		threadId.p = 0;
#else
		threadId = 0;
#endif
		
	}

#ifdef _WIN32
	if (0 != spaceWarnThreadId.p)
#else
	if(spaceWarnThreadId)
#endif
	{
		pthread_join(spaceWarnThreadId, NULL);
#ifdef _WIN32
		spaceWarnThreadId.p = 0;
#else
		spaceWarnThreadId = 0;
#endif
	}

	deleteStorageDataInfo();
	return 0;
}

#ifdef _WIN32
char* linuxFilePathToWin(const char* srcPath,char* dstPath)
{
	sprintf(dstPath,"%s",srcPath);
	for (int i=0;i<256;i++)
	{
		if ((dstPath[i] == 47) || (dstPath[i] == 92))// 47 '/'
		{
			dstPath[i] = '\\\\';//92 '\\'
		}
	}

	return dstPath;
}

#endif

int CStorageManager::deleteAllStorageRecord()
{
	char cmd[256];
	if(m_config.storagePath.empty())
	{
		return -1;
	}
	sprintf(cmd, "rm -rf %s/*", m_config.storagePath.c_str());
	system(cmd);
	/*if( status != -1 && WIFEXITED(status) && (WEXITSTATUS(status) == 0) )
	{
		REPORT(REPORT_INFO, "deleteStorageRecord sucess !!\n");
		return 0;
	}
*/
	return 0;

}
int CStorageManager::deleteStorageRecord(string storageName)
{
	char cmd[256];
	int ret = 0;
	if(storageName.empty())
	{
		REPORT(REPORT_ERROR, "storageName is empty !!\n");
		return -1;
	}
	char filePath[256] = {0};
	string strXmlPath(m_config.storagePath);
	string dictionaryXmlPath;

	string storeRecordPath = strXmlPath + "/StoreRecordInfo.xml";
	TiXmlDocument mydoc(storeRecordPath.c_str());
	bool loadOk = mydoc.LoadFile();//�����ĵ�
	if (!loadOk)
	{
		return -1;
	}
	TiXmlElement* storeRecordElement = mydoc.RootElement();
	string  recordName = 0;
	for (TiXmlElement* pRecordElement = storeRecordElement->FirstChildElement(); pRecordElement != NULL; pRecordElement = pRecordElement->NextSiblingElement())
	{
		ret = GetValue(pRecordElement, "name", recordName);
		if(recordName == storageName)
		{
			storeRecordElement->RemoveChild(pRecordElement);
		}
	}
#ifdef _WIN32
	char winPath[256];
	sprintf(path, "%s/%s", m_config.storagePath.c_str(), storageName.c_str());
	linuxFilePathToWin(path,winPath);
	char str[120] ="RMDIR /S /Q "; 
	strcat(str,winPath);
	//strcat(str,path);
	strcpy(cmd,str);
	system(cmd);
#else
	sprintf(cmd, "rm -rf \"%s/%s\"", m_config.storagePath.c_str(), storageName.c_str());
	int status = system(cmd);
	if( status != -1 && WIFEXITED(status) && (WEXITSTATUS(status) == 0) )
	{
		REPORT(REPORT_INFO, "deleteStorageRecord sucess !!\n");
	}
	else
	{
		REPORT(REPORT_ERROR, "deleteStorageRecord faild !!\n");
		return -1;
	}

	DIR *dirp = opendir(m_config.storagePath.c_str());
	if(!dirp)
	{
		REPORT(REPORT_ERROR, "opendir faild !!\n");
		return -1;
	}
	struct dirent *dir;


	while( (dir = readdir(dirp)) != NULL )
	{
		//printf("dir name:%s\n",dir->d_name);
		if(strcmp(dir->d_name, storageName.c_str()) == 0)
		{
			REPORT(REPORT_ERROR, "the storage record still on disk !!\n");
			return -1;
		}
	}
	return 0;
#endif


}

int CStorageManager::getStorageStatus(TY_STORAGE_STATUS *info)
{
	TY_PACKET_INFO tmpPacketInfo;

	for(TY_STORAGE_ICD_MAP_MAP::iterator pos = m_storageICDMap.begin();
			pos != m_storageICDMap.end(); pos++)
	{
		for(TY_STORAGE_ICD_MAP::iterator posSn = pos->second.begin();
				posSn != pos->second.end(); posSn++)
		{
			if(posSn->second->indexHeader.ICDCount == 0)
			{
				continue;
			}
			tmpPacketInfo.count = posSn->second->indexHeader.ICDCount;
			tmpPacketInfo.size = posSn->second->allDataFileSize;
			tmpPacketInfo.startTime.tv_sec = posSn->second->indexHeader.firstPacketTime.tv_sec;
			tmpPacketInfo.startTime.tv_usec = posSn->second->indexHeader.firstPacketTime.tv_usec;
			tmpPacketInfo.stopTime.tv_sec = posSn->second->indexHeader.endTime.tv_sec;
			tmpPacketInfo.stopTime.tv_usec = posSn->second->indexHeader.endTime.tv_usec;
			info->icdInfo.push(tmpPacketInfo);

		}
	}


	info->status = m_storageStatus;
	info->storageStartTimeSec = m_config.storageStartTimeSec;
	info->storageStopTimeSec = m_config.storageStopTimeSec;

	info->startTimeSec = m_config.startTimeSec;
	info->startTimeUsec = m_config.startTimeUsec;
	info->stopTimeSec = m_config.stopTimeSec;
	info->stopTimeUsec = m_config.stopTimeUsec;
	/*if the storage server is stop,get the free disk*/
	if(m_storageStatus == SERVICE_STOP)
	{
#ifdef _WIN32
		m_config.freeDisk = getDiskCapacity();
#else

		struct statfs diskInfo;
		unsigned long long blockSize;
		double freeDisk;

		statfs(m_config.storagePath.c_str(), &diskInfo);
		blockSize = diskInfo.f_bsize;
		freeDisk = diskInfo.f_bavail * blockSize;
		freeDisk = freeDisk /1024/1024;
		m_config.freeDisk = freeDisk;
#endif
	}

	info->freeDisk = m_config.freeDisk;

	if(m_storageStatus == SERVICE_RUN)
	{
		info->statusMsg = "RUN";
	}
	else if(m_storageStatus == SERVICE_STOP)
	{
		info->statusMsg = "STOP";
	}

	info->IP = m_config.IP;
	info->busType = m_config.busType;

	info->path.append(m_config.storagePath.c_str());
	info->path.append("/");
	if(m_config.storageName.empty())
	{
		//REPORT(REPORT_ERROR, "storageName is empty\n");
		return 0;
	}
	info->path.append(m_config.storageName.c_str());
	info->path.append("/");
	info->path.append(m_config.storageServerName.c_str());

	return 0;
}


int getIndexFileHeader(char *fileName, TY_INDEX_HEADER_INFO *header)
{
	/*open index file*/
#ifndef _WIN32
	int fd = open(fileName, O_RDONLY);
#else
	int fd = open(fileName, O_RDONLY | O_BINARY);
#endif

	if(fd <= 0)
	{
		REPORT(REPORT_ERROR, "fileName :%s fd:%d \n", fileName, fd);
		REPORT(REPORT_ERROR, "getIndexFileHeader faild\n");
		perror("");
		return -1;
	}

	/*get ICD data len*/
	TY_INDEX_HEADER_INFO tmpHeader;
	int ret = read(fd, &tmpHeader, sizeof(TY_INDEX_HEADER_INFO));
	if(ret <= 0)
	{
		REPORT(REPORT_ERROR, "fileName :%s fd:%d \n", fileName, fd);
		REPORT(REPORT_ERROR, "read index header file\n");
		close(fd);

		perror("read error:");
		return -1;
	}
	memcpy(header, &tmpHeader, sizeof(TY_INDEX_HEADER_INFO));
	close(fd);

	return 0;
}

int AnalyseIndexFile(char *fileName, int startIndex, int mode,
					unsigned int *dataFileNo, unsigned int *dataFileIndex)
{
	unsigned int indexFileIndex = 0;

	if( (dataFileNo == NULL) || (dataFileIndex == NULL) )
	{
		return -1;
	}
	/*open index file*/
#ifndef _WIN32
	int fd = open(fileName, O_RDONLY);
#else
	int fd = open(fileName, O_RDONLY | O_BINARY);
#endif

	if(fd <= 0)
	{
		REPORT(REPORT_ERROR, "fileName :%s fd:%d \n", fileName, fd);
		return -1;
	}

	/*get ICD data len*/
	TY_INDEX_HEADER_INFO header;
	read(fd, &header, sizeof(TY_INDEX_HEADER_INFO));

	if(mode == ANALYSE_COUNT_INDEX)
	{
		indexFileIndex = HERADER_LEN + (startIndex / INDEX_DATA_COUNT) * 2 * 4;
	}
	else if(mode == ANALYSE_TIME_INDEX)
	{
		indexFileIndex = HERADER_LEN + (startIndex / INDEX_DATA_TIME) * 2 * 4;
	}

	unsigned int offset = lseek(fd, 0, SEEK_END);
	if(indexFileIndex >= offset)
	{
		REPORT(REPORT_ERROR, "AnalyseIndexFile error\n");
		close(fd);

		return -1;
	}
	lseek(fd, indexFileIndex, SEEK_SET);

	int indexFileBuffer[2];
	read(fd, indexFileBuffer, sizeof(indexFileBuffer));

	while(indexFileBuffer[0] == -1)
	{
		indexFileIndex -= 8;
		lseek(fd, indexFileIndex, SEEK_SET);
		read(fd, indexFileBuffer, sizeof(indexFileBuffer));
	}
	/*get the data file No and index by index file*/
	*dataFileNo = indexFileBuffer[0];
	*dataFileIndex = indexFileBuffer[1];
	close(fd);

	return 0;
}
int readFileData(int fd, TY_STORAGE_DATA *pData)
{
	int ret = 0;
	int currentIndex = 0;

	if(fd <= 0)
	{
		return 0;
	}
	/*read data header*/
	TY_STORAGE_DATA readData;
	memset(&readData, 0, sizeof(TY_STORAGE_DATA));
	ret = read(fd, &readData, sizeof(TY_STORAGE_DATA) - PAYLOAD_LEN);
	if(ret == 0)
	{
		printf("[%s:%s:%d]--> read fail, fd:%d, len:%d\n", __FILE__, __FUNCTION__, __LINE__,fd, sizeof(TY_STORAGE_DATA) - PAYLOAD_LEN);
		perror("read data header:");
		return 0;
	}

	/*read data*/
	ret = read(fd, readData.payload, readData.length);
	if(ret == 0)
	{
		perror("read data file:");
		return 0;
	}

	currentIndex = lseek(fd, 0, SEEK_CUR);
	if(currentIndex == -1)
	{
		perror("read data file:");
		return 0;
	}

	memcpy(pData, &readData, sizeof(TY_STORAGE_DATA));

	return currentIndex;

}

/*******************************************************************************
 * Function Name  : getDataByIndex
 * Description    : get storage data by index,you can get ICD data and channel data
 * 					by this function.
 * Input          : storagePath : server name of the requesting data
 *					channelSN : channel Sn
 *					ddsID : DDS Key
 *					startIndex : the start index of the requesting data
 *					stopIndex :  the stop index of the requesting data
 *					maxCount :  the max number of data that the buffer can hold,
 *								maxCount= sizeof(pData)/sizeof(TY_STORAGE_DATA)
 *					flag : the type of the requesting data,
 *							DATA_TYPE_ICD : get ICD data
 *							DATA_TYPE_SN : get channel data
 * Output         : pData : the requesting data
 * Return         : int :
 *					-1 : get data error
 *					> 0 : the real number of the requesting data
 *******************************************************************************/
int CStorageManager::getDataByIndex(string storageName, char* pName,
		unsigned int startIndex, unsigned int stopIndex, TY_STORAGE_DATA* pData, int maxCount)
{
	int ret = 0;

	char tmpPath[256];
	char tmpFileName[256];
	int currentDataCount = startIndex % INDEX_DATA_COUNT;

	if(startIndex == stopIndex)
	{
		return -1;
	}
	if(pData == NULL)
	{
		return -1;
	}

	/*open index file*/
	memset(tmpFileName, 0, sizeof(tmpFileName));
	memset(tmpPath, 0, sizeof(tmpPath));

	sprintf(tmpPath, "%s/%s/",
			m_config.storagePath.c_str(), storageName.c_str());



	sprintf(tmpFileName, "%s/%s.index", tmpPath, pName);


	unsigned int dataFileNo = 0;
	unsigned int dataFileIndex = 0;
	TY_INDEX_HEADER_INFO indexHeader;

	/*get index file header*/
	ret = getIndexFileHeader(tmpFileName, &indexHeader);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "getIndexFileHeader error\n");
		return -1;
	}

	if(startIndex > indexHeader.ICDCount-1)
	{
		REPORT(REPORT_ERROR, "startIndex larger than storage data count\n");
		return -1;
	}
	//printf("startindex:%d stopindex:%d count:%d \n", startIndex, stopIndex, indexHeader.ICDCount);
	if(stopIndex > indexHeader.ICDCount)
	{
		REPORT(REPORT_WARNING, "stopIndex larger than storage data count\n");
		stopIndex = indexHeader.ICDCount;
	}

	/*get the data file index with indexfile*/
	ret = AnalyseIndexFile(tmpFileName, startIndex, ANALYSE_COUNT_INDEX,
			&dataFileNo, &dataFileIndex);
	if(ret == -1)
	{
		REPORT(REPORT_ERROR, "AnalyseIndexFile error\n");
		return -1;
	}

	/*open data file*/
	memset(tmpFileName, 0, sizeof(tmpFileName));

	sprintf(tmpFileName, "%s/%s.data%d", tmpPath, pName, dataFileNo);



#ifndef _WIN32
	int fd = open(tmpFileName, O_RDONLY);
#else
	int fd = open(tmpFileName, O_RDONLY | O_BINARY);
#endif

	if(fd <= 0)
	{
		REPORT(REPORT_ERROR, "open data file error\n");
		return -1;
	}

	/*find the right data*/
	unsigned int maxIndex = lseek(fd, 0, SEEK_END);//get the data file max len

	{
		int dataCount = (maxIndex - dataFileIndex) / indexHeader.ICDDataLen;

		if( dataCount > currentDataCount)//the right is in current data file
		{
			dataFileIndex +=  indexHeader.ICDDataLen * currentDataCount;
			lseek(fd, dataFileIndex, SEEK_SET);
		}
		else if( dataCount <= currentDataCount )
		{

			while(dataCount <= currentDataCount)
			{
				currentDataCount -= dataCount;

				/*change the data file*/
				close(fd);

				fd = 0;
				dataFileNo++;
				if(dataFileNo > (indexHeader.fileCount -1))
				{
					REPORT(REPORT_ERROR, "the datafile is error\n");
					return -1;
				}
				memset(tmpFileName, 0, sizeof(tmpFileName));

				sprintf(tmpFileName, "%s/%s.data%d", tmpPath, pName, dataFileNo);


#ifndef _WIN32
				fd = open(tmpFileName, O_RDONLY);
#else
				fd = open(tmpFileName, O_RDONLY | O_BINARY);
#endif

				if(fd <= 0)
				{
					REPORT(REPORT_ERROR, "open data file error\n");
					return -1;
				}

				maxIndex = lseek(fd, 0, SEEK_END);
				dataCount = (maxIndex - HERADER_LEN) / indexHeader.ICDDataLen;

			}

			dataFileIndex = indexHeader.ICDDataLen * currentDataCount + HERADER_LEN;
			lseek(fd, dataFileIndex, SEEK_SET);
		}

	}


	/*read the right data and return it*/
	int tmpDataCount = 0;
	while(stopIndex - startIndex)
	{
		/*the buff provided by user is not enough*/
		tmpDataCount++;
		if(tmpDataCount > maxCount)
		{
			close(fd);
			REPORT(REPORT_ERROR, "the buff provided by user is not enough\n");
			return tmpDataCount-1;
		}

		TY_STORAGE_DATA readData;

		dataFileIndex = readFileData(fd, &readData);
		if(dataFileIndex == 0)
		{
			REPORT(REPORT_ERROR, ">>>---INdex----readFileData error\n");
			return tmpDataCount;
		}
		memcpy(pData++, &readData, sizeof(TY_STORAGE_DATA));

		startIndex++;
		/*change the data file*/
		if(maxIndex == dataFileIndex)
		{
			close(fd);

			fd = 0;
			dataFileNo++;

			if(dataFileNo > (indexHeader.fileCount -1))
			{
				return tmpDataCount;
			}

			memset(tmpFileName, 0, sizeof(tmpFileName));

			sprintf(tmpFileName, "%s/%s.data%d", tmpPath, pName, dataFileNo);


#ifndef _WIN32
			fd = open(tmpFileName, O_RDONLY);
#else
			fd = open(tmpFileName, O_RDONLY | O_BINARY);
#endif
			if(fd <= 0)
			{
				REPORT(REPORT_ERROR, "open data file error\n");
				return tmpDataCount;
			}
			maxIndex = lseek(fd, 0, SEEK_END);
			dataFileIndex = lseek(fd, HERADER_LEN, SEEK_SET);
		}

	}
	close(fd);

	return tmpDataCount;
}

/*compare time1 and time2
 * return:  1:time1 > time2
 * 			-1:time1 < time2
 * 			0 : time1 = time2
 */
int compareTime(unsigned int time1Sec, unsigned int time1Usec,
		unsigned int time2Sec, unsigned int time2Usec)
{
	long long sec1 = time1Sec;
	long long usec1 = time1Usec;
	long long sec2 = time2Sec;
	long long usec2 = time2Usec;

	if( ((sec1 - sec2)*1000000 + (usec1 - usec2)) > 0)
	{
		return 1;
	}
	else if( ((sec1 - sec2)*1000000 + (usec1 - usec2)) < 0)
	{
		return -1;
	}
	else if( ((sec1 - sec2)*1000000 + (usec1 - usec2)) == 0)
	{
		return 0;
	}
	return 0;
}
/*******************************************************************************
 * Function Name  : getDataByTime
 * Description    : get storage data by time,you can get ICD data and channel data
 * 					by this function.You should provied buffer for the requesting data
 * 					and the max number of data that the buffer can hold.The function
 * 					will return the real number of the requesting data and time of the
 * 					last data.You should call this function in a loop until
 * 					(startTime == stopTime) || (return value <= maxCount)
 * Input          : storagePath : server name of the requesting data
 *					channelSN : channel Sn
 *					ddsID : DDS Key
 *					startTime : the start time of the requesting data
 *					stopTime :  the stop time of the requesting data
 *					maxCount :  the max number of data that the buffer can hold,
 *								maxCount= sizeof(pData)/sizeof(TY_STORAGE_DATA)
 *					flag : the type of the requesting data,
 *							DATA_TYPE_ICD : get ICD data
 *							DATA_TYPE_SN : get channel data
 * Output         : pData : the requesting data
 * Return         : int :
 *					-1 : get data error
 *					> 0 : the real number of the requesting data
 *******************************************************************************/
int CStorageManager::getDataByTime(string storageName, char* pName, TY_STORAGE_TIME *startTime, TY_STORAGE_TIME *stopTime,
		TY_STORAGE_DATA* pData, int maxCount)
{
	int ret = 0;
	int fd = 0;
	char tmpFileName[256];
	char tmpPath[256];
	unsigned int maxIndex = 0;
	TY_STORAGE_DATA readData;
	unsigned int dataFileNo = 0;
	unsigned int dataFileIndex = 0;
	TY_INDEX_HEADER_INFO indexHeader;
	static unsigned int lastDataFileNo = 0;
	static unsigned int lastDataIndex = 0;
	int currentDataCount = 0;
	int changeTimeFlag = 0;

	/*open index file*/
	memset(tmpFileName, 0, sizeof(tmpFileName));
	memset(tmpPath, 0, sizeof(tmpPath));
	sprintf(tmpPath, "%s/%s/", m_config.storagePath.c_str(), storageName.c_str());


	sprintf(tmpFileName, "%s/%s.timeIndex", tmpPath, pName);


	/*get index file header*/
	ret = getIndexFileHeader(tmpFileName, &indexHeader);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "getIndexFileHeader error\n");
		return -1;
	}

	if( compareTime(startTime->tv_sec, startTime->tv_usec, indexHeader.firstPacketTime.tv_sec, indexHeader.firstPacketTime.tv_usec) == -1 )
	{
		startTime->tv_sec = indexHeader.firstPacketTime.tv_sec;
		startTime->tv_usec = indexHeader.firstPacketTime.tv_usec;
		REPORT(REPORT_ERROR, "start time is smaller than storage data firstPacketTime!\n");
	}

	if( compareTime(startTime->tv_sec, startTime->tv_usec, indexHeader.endTime.tv_sec, indexHeader.endTime.tv_usec) == 1 )
	{
		REPORT(REPORT_ERROR, "start time is larger than storage data endTime!\n");
		return -1;
	}

	if( compareTime(stopTime->tv_sec, stopTime->tv_usec, indexHeader.endTime.tv_sec, indexHeader.endTime.tv_usec) == 1 )
	{
		REPORT(REPORT_WARNING, "stop time is larger than storage data endTime!!\n");
		stopTime->tv_sec = indexHeader.endTime.tv_sec;
		stopTime->tv_usec = indexHeader.endTime.tv_usec;
	}

	/*if you call this function first time,you need analyse IndexFile and find the right data index.
	 *if the data request is not over,the function will record the right data index.
	 *The record index will be clear when the data request is over*/
	if( (lastDataFileNo == 0) && (lastDataIndex == 0) )
	{
		/*get the data file index with indexfile*/
		ret = AnalyseIndexFile(tmpFileName, startTime->tv_sec - indexHeader.firstPacketTime.tv_sec, ANALYSE_TIME_INDEX,
				&dataFileNo, &dataFileIndex);
		if(ret == -1)
		{
			REPORT(REPORT_ERROR, "AnalyseIndexFile error\n");
			return -1;
		}

		/*open data file*/
		memset(tmpFileName, 0, sizeof(tmpFileName));

		sprintf(tmpFileName, "%s/%s.data%d", tmpPath, pName, dataFileNo);


#ifndef _WIN32
		fd = open(tmpFileName, O_RDONLY );
#else
		fd = open(tmpFileName, O_RDONLY | O_BINARY);
#endif

		if(fd <= 0)
		{
			return -1;
		}

		maxIndex = lseek(fd, 0, SEEK_END);//get the data file max len
		/*find the start index data,read the data one by one*/
		memset(&readData, 0, sizeof(TY_STORAGE_DATA));
		lseek(fd, dataFileIndex, SEEK_SET);

		while( compareTime(startTime->tv_sec, startTime->tv_usec, readData.sec, readData.usec) == 1 )
		{
			dataFileIndex = readFileData(fd, &readData);

			/*change the data file*/
			if(dataFileIndex == maxIndex)
			{
				close(fd);

				fd = 0;
				dataFileNo++;
				if(dataFileNo > (indexHeader.fileCount -1))
				{
					REPORT(REPORT_ERROR, "the datafile is error\n");
					/*the last data is the right data*/

					if((compareTime(startTime->tv_sec, startTime->tv_usec, readData.sec, readData.usec) == -1)
						|| (compareTime(startTime->tv_sec, startTime->tv_usec, readData.sec, readData.usec) == 0)
						)
					{
						startTime->tv_sec = stopTime->tv_sec;
						startTime->tv_usec = stopTime->tv_usec;
						memcpy(pData++, &readData, sizeof(TY_STORAGE_DATA));
						return 1;
					}
					else
					{
						return -1;
					}
				}

				memset(tmpFileName, 0, sizeof(tmpFileName));
				sprintf(tmpFileName, "%s/%s.data%d", tmpPath, pName, dataFileNo);
#ifndef _WIN32
				fd = open(tmpFileName, O_RDONLY);
#else
				fd = open(tmpFileName, O_RDONLY | O_BINARY);
#endif
				if(fd <= 0)
				{
					REPORT(REPORT_ERROR, "open data file error\n");
					return -1;
				}

				maxIndex = lseek(fd, 0, SEEK_END);
				dataFileIndex = lseek(fd, HERADER_LEN, SEEK_SET);
			}
		}

		/*there is no data between startTime and stopTime*/
		if(compareTime(readData.sec, readData.usec, stopTime->tv_sec, stopTime->tv_usec) == 1)
		{
			close(fd);

			REPORT(REPORT_WARNING, "there is no data between startTime and stopTime\n");
			return 0;
		}


		/*when you found the right data index,you has read a right data*/
		if((readData.sec != 0) || (readData.usec != 0))//if your start time is zero,you has not read the right data.
		{
			maxCount--;
			currentDataCount++;
			memcpy(pData++, &readData, sizeof(TY_STORAGE_DATA));
		}


	}//end 	if( (lastDataFileNo == 0) && (lastDataIndex == 0) )
	else
	{
		/*open data file*/
		memset(tmpFileName, 0, sizeof(tmpFileName));

		sprintf(tmpFileName, "%s/%s.data%d", tmpPath, pName, lastDataFileNo);


#ifndef _WIN32
		fd = open(tmpFileName, O_RDONLY);
#else
		fd = open(tmpFileName, O_RDONLY | O_BINARY);
#endif

		if(fd <= 0)
		{
			return -1;
		}

		maxIndex = lseek(fd, 0, SEEK_END);//get the data file max len
		dataFileIndex = lseek(fd, lastDataIndex, SEEK_SET);
		dataFileNo = lastDataFileNo;
	}

	/*read the right data and return it*/
	while(maxCount--)//max read data count
	{
		dataFileIndex = readFileData(fd, &readData);
		if(dataFileIndex == 0)
		{
			REPORT(REPORT_WARNING, ">>--Time---read data file error\n");
			break;
		}
		/*the time of current data is error,throw away the current data*/
		if( compareTime(stopTime->tv_sec, stopTime->tv_usec, readData.sec, readData.usec) == -1 )
		{
			changeTimeFlag = 1;
			break;
		}
		else if( compareTime(stopTime->tv_sec, stopTime->tv_usec, readData.sec, readData.usec) == 0 )
		{
			changeTimeFlag = 1;
			memcpy(pData++, &readData, sizeof(TY_STORAGE_DATA));
			currentDataCount++;//real read data count
			break;
		}
		memcpy(pData++, &readData, sizeof(TY_STORAGE_DATA));
		currentDataCount++;//real read data count

		/*change the data file*/
		if(maxIndex == dataFileIndex)
		{
			close(fd);

			fd = 0;
			dataFileNo++;
			if(dataFileNo > (indexHeader.fileCount -1))
			{
				changeTimeFlag = 1;
				break;
			}

			memset(tmpFileName, 0, sizeof(tmpFileName));

			sprintf(tmpFileName, "%s/%s.data%d", tmpPath, pName, dataFileNo);


#ifndef _WIN32
			fd = open(tmpFileName, O_RDONLY);
#else
			fd = open(tmpFileName, O_RDONLY | O_BINARY);
#endif

			if(fd <= 0)
			{
				REPORT(REPORT_ERROR, "open data file error\n");
				return -1;
			}
			maxIndex = lseek(fd, 0, SEEK_END);
			dataFileIndex = lseek(fd, HERADER_LEN, SEEK_SET);
		}

	}

	close(fd);


	if(changeTimeFlag == 1 || currentDataCount < maxCount)//the function has got all data between start and stop time
	{
		startTime->tv_sec = stopTime->tv_sec;
		startTime->tv_usec = stopTime->tv_usec;
		lastDataFileNo = 0;
		lastDataIndex = 0;
	}
	else//the function has not got all data between start and stop time,so return time of the last data
	{
		startTime->tv_sec = readData.sec;
		startTime->tv_usec = readData.usec;
		//save the last dataFile No and dataIndex,
		//then you can read right data with it when you call this function next
		lastDataFileNo = dataFileNo;
		lastDataIndex = dataFileIndex;
	}

	return currentDataCount;
}
int CStorageManager::getIndexByTime(string storageName, char* pName, TY_STORAGE_TIME *startTime, TY_STORAGE_TIME *stopTime,int * startIndex,int * stopIndex)
{
#if 1

	int ret = 0;
	int fd = 0;
	int startDataCount = 0;

	char tmpFileName[256];
	char tmpPath[256];

	TY_STORAGE_DATA readData;
	unsigned int dataFileNo = 0;
	unsigned int dataFileIndex = 0;
	TY_INDEX_HEADER_INFO indexHeader;

	int currentDataCount = 0;

	/*open index file*/
	memset(tmpFileName, 0, sizeof(tmpFileName));
	memset(tmpPath, 0, sizeof(tmpPath));
	sprintf(tmpPath, "%s/%s/%s",
			m_config.storagePath.c_str(), storageName.c_str(), m_config.storageServerName.c_str());

	sprintf(tmpFileName, "%s/%s.timeIndex", tmpPath, pName);

	/*get index file header*/
	ret = getIndexFileHeader(tmpFileName, &indexHeader);
	if(ret != 0)
	{
		REPORT(REPORT_ERROR, "getIndexFileHeader error\n");
		return -1;
	}	

	if( compareTime(startTime->tv_sec, startTime->tv_usec, indexHeader.firstPacketTime.tv_sec, indexHeader.firstPacketTime.tv_usec) == -1 )
	{
		startTime->tv_sec = indexHeader.firstPacketTime.tv_sec;
		startTime->tv_usec = indexHeader.firstPacketTime.tv_usec;
		REPORT(REPORT_ERROR, "start time is smaller than storage data firstPacketTime!\n");
	}

	if( compareTime(startTime->tv_sec, startTime->tv_usec, indexHeader.endTime.tv_sec, indexHeader.endTime.tv_usec) == 1 )
	{
		startTime->tv_sec = indexHeader.firstPacketTime.tv_sec;
		startTime->tv_usec = indexHeader.firstPacketTime.tv_usec;
		REPORT(REPORT_ERROR, "start time is larger than storage data endTime!\n");
		return -1;
	}

	/*****************/
	if( compareTime(stopTime->tv_sec, stopTime->tv_usec, indexHeader.endTime.tv_sec, indexHeader.endTime.tv_usec) == -1 )
	{
		stopTime->tv_sec = indexHeader.endTime.tv_sec;
		stopTime->tv_usec = indexHeader.endTime.tv_usec;
		REPORT(REPORT_WARNING, "stop time is larger than storage data endTime!!\n");
	}
	/*****************/

	if( compareTime(stopTime->tv_sec, stopTime->tv_usec, indexHeader.endTime.tv_sec, indexHeader.endTime.tv_usec) == 1 )
	{
		REPORT(REPORT_WARNING, "stop time is larger than storage data endTime!!\n");
		stopTime->tv_sec = indexHeader.endTime.tv_sec;
		stopTime->tv_usec = indexHeader.endTime.tv_usec;
	}


	ret = AnalyseIndexFile(tmpFileName, startTime->tv_sec - indexHeader.firstPacketTime.tv_sec, ANALYSE_TIME_INDEX,
			&dataFileNo, &dataFileIndex);
	if(ret == -1)
	{
		REPORT(REPORT_ERROR, "AnalyseIndexFile error\n");
		return -1;
	}

	/*open data file*/
	memset(tmpFileName, 0, sizeof(tmpFileName));

	sprintf(tmpFileName, "%s/%s.data%d", tmpPath, pName, dataFileNo);


	fd = open(tmpFileName, O_RDONLY);
	if(fd <= 0)
	{
		return -1;
	}

	int dataPack = 0;
	int length = 0;
	memset(&readData, 0, sizeof(TY_STORAGE_DATA));
	lseek(fd, dataFileIndex, SEEK_SET);

	if(dataFileIndex != HERADER_LEN)
	{
		readFileData(fd, &readData);
		length = readData.length;

		lseek(fd, dataFileIndex, SEEK_SET);
		dataPack= ((lseek(fd,0,SEEK_CUR))- HERADER_LEN)/(sizeof(TY_STORAGE_DATA) - PAYLOAD_LEN+length);////sizeof(TY_STORAGE_DATA);

	}
	//printf("[%s:%s:%d]--> open filename:%s, datafileindex:%d\n", __FILE__, __FUNCTION__, __LINE__,tmpFileName,dataFileIndex);
	lseek(fd, dataFileIndex, SEEK_SET);
	while( compareTime(startTime->tv_sec, startTime->tv_usec, readData.sec, readData.usec) == 1 )
	{
		dataFileIndex = readFileData(fd, &readData);
		startDataCount++;//璇诲埌涓庣粰鐨勫紑濮嬫椂闂翠竴鑷磋鏁�
	}

	//	printf("startDataCount=%d\n",startDataCount);
	/*read the data*/
	while(1)// read data count
	{
		dataFileIndex = readFileData(fd, &readData);
		if(dataFileIndex == 0)
		{
			REPORT(REPORT_WARNING, "read data file error\n");
			break;
		}
		/*the time of current data is error,throw away the current data*/
		if( compareTime(stopTime->tv_sec, stopTime->tv_usec, readData.sec, readData.usec) == -1 )
		{
			break;
		}
		else if( compareTime(stopTime->tv_sec, stopTime->tv_usec, readData.sec, readData.usec) == 0 )
		{
			currentDataCount++;// read data count
			break;
		}
		currentDataCount++;// read data count
	}
	//	printf("currentDataCount=%d\n",currentDataCount);
	*startIndex = dataPack + startDataCount - 1;
	*stopIndex = *startIndex + currentDataCount + 1;
	//	printf("startIndex=%d\tstopIndex=%d\n",*startIndex,*stopIndex);
#endif

	close(fd);

	return 0;
}


/*corba::::*/
long tmpInitStorage(string storeContent)
{
	CStorageManager *pManager;
	pManager = CStorageManager::getInstance();
	pManager->m_config.storageServerName = "RK12001_429";
	pManager->m_config.busType = "429";
	int ret = 0;//pManager->initStorage(storeContent.c_str());


	return ret;
}

/*long tmpStartStorage(string storagePath)
{
	CStorageManager *pManager;
	pManager = CStorageManager::getInstance();
	pManager->startStorage();

	return 0;
}*/

long tmpStopStorage()
{
	CStorageManager *pManager;
	pManager = CStorageManager::getInstance();

	pManager->stopStorage();

	return 0;
}

long tmpGetStorageStatus(TY_STORAGE_STATUS *info)
{
	CStorageManager *pManager;
	pManager = CStorageManager::getInstance();

	pManager->getStorageStatus(info);

	return 0;
}
/*corba::::*/

void readData(string fileName)
{
#ifndef _WIN32
	int fd = open(fileName.c_str(), O_RDONLY);//
#else
	int fd = open(fileName.c_str(), O_RDONLY | O_BINARY);//
#endif

	if(fd == -1)
	{
		perror("open faild:");
	}
	char buffer[256];
	char *pTmp = buffer;
	memset(buffer, 0, sizeof(buffer));

	int ret = read(fd, buffer, 184);
	if(ret == 0)
	{
		printf("read error\n");
	}
	for(int i = 0; i < 184/4;i++)
	{
		printf("data[%d]:%x\n", i,*(unsigned int *)(pTmp+i*4));
	}
	close(fd);

}
int	main(int argc, char *argv[])
{

	//printf("sizeof : %d\n", sizeof(TY_STORAGE_DATA_INFO));
#if 0
	struct timeval timeBefore;
	struct timeval timeAfter;

	long long count = 1;
	TY_STORAGE_DATA data;
	CStorageManager *pManager;
	pManager = CStorageManager::getInstance();
	string storagePath = "Test( 1111111)";
	getchar();
	pManager->deleteStorageRecord("Test(2019-10-24-17-49-26)");

	int ret = tmpInitStorage("StorageConfig.xml");
	if(ret != 0)
	{
		printf("init error\n");
	}
	tmpStartStorage(storagePath);
	if(ret != 0)
	{
		printf("start error\n");
	}

	while(count)
	{
		data.id = 10001;
		data.src = 123456;
		data.sec = 0 + (count-1)/3;
		data.usec = 0 + count-1;
		data.length = 10;
		for(unsigned int i = 0;i < data.length; i++)
		{
			data.payload[i] = 0 + count-1 + i;
		}
		pManager->insertData(&data);
		//		data.src = 234567;
		//		pManager->insertData(&data);
		//getchar();
		usleep(1);

		if(count >= 1000)
		{
			break;
		}
		count++;
	}

	tmpStopStorage();
#if 0
	printf("ICD status\n");
	TY_STORAGE_STATUS info;
	tmpGetStorageStatus(&info);
	int cnt = info.icdInfo.size();
	for(int i =0;i < cnt;i++)
	{
		TY_PACKET_INFO packet;
		memcpy(&packet, &info.icdInfo.front(), sizeof(TY_PACKET_INFO));
		info.icdInfo.pop();
		printf(" count:%d, id:%d, size:%d, startTime:%d, stopTime:%d \n",
				packet.count,
				packet.id,
				packet.size,
				packet.startTime.tv_sec,
				packet.stopTime.tv_sec);
	}
	printf("SN status\n");
	cnt = info.channelInfo.size();
	for(int i =0;i < cnt;i++)
	{
		TY_PACKET_INFO packet;
		memcpy(&packet, &info.channelInfo.front(), sizeof(TY_PACKET_INFO));
		info.channelInfo.pop();
		printf(" count:%d, id:%d, size:%d, startTime:%d, stopTime:%d \n",
				packet.count,
				packet.id,
				packet.size,
				packet.startTime.tv_sec,
				packet.stopTime.tv_sec);
	}

	getchar();
#endif

	TY_STORAGE_DATA *tmpRecvBuff = new TY_STORAGE_DATA[1000];
	unsigned int readDataCount = 0;
#if 0
	/*read ICD data by index*/
	ret = pManager->getDataByIndex("Test", 123456, 10001 ,
			0, 412, tmpRecvBuff, 1000, DATA_TYPE_ICD);
	if(ret != -1)
	{
		for(int i = 0; i < ret;i++)
		{
			readDataCount++;
			printf("recv data time:%d s, %d us, data[0]:%d %d %d\n",
					tmpRecvBuff[i].sec,
					tmpRecvBuff[i].usec,
					tmpRecvBuff[i].payload[0],
					tmpRecvBuff[i].payload[1],
					tmpRecvBuff[i].payload[2]);
		}
	}
#else
	/*read ICD data by time*/

	memset(tmpRecvBuff, 0, sizeof(tmpRecvBuff));
	int maxCount = 100;
	TY_STORAGE_TIME startTime;
	TY_STORAGE_TIME stopTime;
	startTime.tv_sec = 0;
	startTime.tv_usec = 0;
	stopTime.tv_sec = 4000;
	stopTime.tv_usec = 0;
	ret = maxCount;
	while( (ret == maxCount) && ((startTime.tv_sec - stopTime.tv_sec)*1000000+ (startTime.tv_usec - stopTime.tv_usec)) )
	{
		ret = pManager->getDataByTime("Test", 123456, 10001,
				&startTime, &stopTime, tmpRecvBuff, maxCount, DATA_TYPE_SN);
		if(ret != -1)
		{
			for(int i = 0; i < ret;i++)
			{
				readDataCount++;
				printf("recv data time:%d s, %d us, data[0]:%d %d %d\n",
						tmpRecvBuff[i].sec,
						tmpRecvBuff[i].usec,
						tmpRecvBuff[i].payload[0],
						tmpRecvBuff[i].payload[1],
						tmpRecvBuff[i].payload[2]);
			}
		}
		else
		{
			break;
		}
	}
#endif
	printf("readDataCount:%d\n",readDataCount);

	getchar();
	pManager->deleteStorageRecord("Test");
#endif
	return 0;
}

void * storeStatus(void *param)
{
	CStorageManager *pManager = CStorageManager::getInstance();

	//for bind width
	TY_HR_TIME currentTime;
	TY_HR_TIME tmpTime;
	bzero(&currentTime, sizeof(TY_HR_TIME));
	bzero(&tmpTime, sizeof(TY_HR_TIME));
/*	long time_use = 0;
	long total_size_current = 0;
	long total_size_tmp = 0;
	double bandwidth = 0;
*/
	//for time storage
	long fromTimeTmp = 0;
	long toTimeTmp = 0;
	fromTimeTmp = pManager->m_storageRule.m_timeRule.fromSec;
	toTimeTmp = pManager->m_storageRule.m_timeRule.toSec;
	//printf("fromTimeTmp=%d toTImeTmp=%d\n",fromTimeTmp,toTimeTmp);
	pManager->toTimeManage.arriveTime(toTimeTmp);
	pManager->fromTimeManage.arriveTime(fromTimeTmp);

	pManager->pTimeManager->getNowTime(&currentTime);
	pManager->m_storageRule.timeStorage.startSec = currentTime.sec;

	usleep(100);//delay 100us to  wait
	while (pManager->storeInfoThreadFlag == 1)
	{
		pManager->pTimeManager->getNowTime(&currentTime);

		if (pManager->m_storageRule.m_type == MANUAL_CONTROL)
		{
			/*sprintf(
					status,
					"======Manually Storage State=====\nINFO:\t\t%sBand Width:\t%.2fKB/s\n",
					"start\n", bandwidth);
			pManager->m_storageStatusMsg = status;*/
			pManager->m_storageRule.triggerFlag = 1;
		}

		/*******TIME STORAGE STATUS*****/
		if (pManager->m_storageRule.m_type == TIMING_CONTROL)
		{
			//from time arrive or fromtime == 0 ; start storage
			if ((fromTimeTmp == 0)
					|| (pManager->fromTimeManage.timeArrivePara.flag
							== ARRIVEFLAG))
			{
				pManager->m_storageRule.timeStorage.startSec = currentTime.sec;
				pManager->m_storageRule.triggerFlag = 1;

			}
			//toTime arrvie or recv corba stop;stop storage
			if ((pManager->toTimeManage.timeArrivePara.flag == ARRIVEFLAG)
					|| (pManager->m_storageStatus == SERVICE_STOP))
			{
				pManager->m_storageRule.timeStorage.endSec = currentTime.sec;
				pManager->m_storageRule.triggerFlag = 0;
				//stop storage
				pManager->stopStorage();
				break;
			}
			if (pManager->fromTimeManage.timeArrivePara.flag == WAITFLAG)
			{
				/*sprintf(
						status,
						"=====Time Storage State=====\nFrom Time:\t%lds\nTo Time:\t\t%lds\nINFO:\t\t%ds left to start storing.\n",
						fromTimeTmp, toTimeTmp,
						(pManager->fromTimeManage.timeArrivePara.TimeDiff - 1));*/
			}
			else if (pManager->toTimeManage.timeArrivePara.flag == WAITFLAG)
			{
				/*sprintf(
						status,
						"=====Time Storage State=====\nFrom Time:\t%lds\nTo Time:\t\t%lds\nINFO:\t\t%ds left to stop storing.\nBand Width:\t %.2fKB/s\n",
						fromTimeTmp, toTimeTmp,
						(pManager->toTimeManage.timeArrivePara.TimeDiff - 1),
						bandwidth);*/
			}
			//pManager->m_storageStatusMsg = status;
		}

		sleep(1);//1s update cycle
	}

	REPORT(REPORT_INFO,"Thread store_status exit!\n");
	pthread_exit(NULL);
	return NULL;
}
int CStorageManager::clearStoreInfo()
{
	storeInfoThreadFlag = 0;
	m_storageRule.triggerFlag = 0;
	//m_storageMap.m_map.clear();//clear map for next start
#ifdef _WIN32
	if (storeInfoThreadId.p != 0)
#else
	if (storeInfoThreadId != 0)
#endif

	{
		pthread_join(storeInfoThreadId, NULL);
	}

	return 0;
}

int CStorageManager::getStoreInfo()
{
	/***** make dir for storage ******/
/*
	pathName = getDataFilePath(m_storageFileName.c_str());
	memcpy(storeRecords.path, pathName, sizeof(storeRecords.path));
	m_storageRule.triggerCount = 0;
*/
	CStorageManager *pManager = CStorageManager::getInstance();
	storeInfoThreadFlag = 1;//start store
	int result = pthread_create(&storeInfoThreadId, NULL, storeStatus, NULL);
	if (result != 0)
	{
		//err_msg("create store_status pthread failed !!!");
		REPORT(REPORT_ERROR,"create store_status pthread failed !!!");
		return -1;
	}

	return 0;
}
int readfileList(char *path, TY_RECORD_INFO* pList, int&listLen)
{
	DIR *dir;

	struct dirent *ptr;
	if((dir=opendir(path)))
	{
		return -1;
	}
	while((ptr=readdir(dir)) != NULL)
	{
		if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
		{
			continue;
		}
		else if(ptr->d_type == 4)
		{
			listLen++;
		}

	}

}

int CStorageManager::getRecordListNum(unsigned int &listLen)
{
	int ret = 0;
	string strXmlPath(m_config.storagePath);
	string dictionaryXmlPath;

	string storeRecordPath = strXmlPath + "/StoreRecordInfo.xml";
	TiXmlDocument mydoc(storeRecordPath.c_str());
	bool loadOk = mydoc.LoadFile();

	if (!loadOk)
	{
		listLen = 0;
		return 0;
	}
	TiXmlElement* storeRecordElement = mydoc.RootElement();
	listLen = 0;
	for (TiXmlElement* pRecordElement = storeRecordElement->FirstChildElement(); pRecordElement != NULL; pRecordElement = pRecordElement->NextSiblingElement())
	{
		listLen++;
	}
	return ret;
}
int CStorageManager::getRecordList(TY_RecordList* pList, unsigned int &listLen)
{
	int ret = 0;
	char filePath[256] = {0};
	string strXmlPath(m_config.storagePath);
	string dictionaryXmlPath;

	string storeRecordPath = strXmlPath + "/StoreRecordInfo.xml";
	REPORT(REPORT_ERROR,"path = %s  \n", storeRecordPath.c_str());
	TiXmlDocument mydoc(storeRecordPath.c_str());
	bool loadOk = mydoc.LoadFile();//�����ĵ�
	if (!loadOk)
	{
		listLen = 0;
		return 0;
	}
	TiXmlElement *recordElement = new TiXmlElement("storeRecords");
	TiXmlElement* storeRecordElement = mydoc.RootElement();
	int  storeNum = 0;
	for (TiXmlElement* pRecordElement = storeRecordElement->FirstChildElement(); pRecordElement != NULL; pRecordElement = pRecordElement->NextSiblingElement(), storeNum++)
	{

		string tmp;
		ret = GetValue(pRecordElement, "name", tmp);
		if(true == ret)
		{
			strcpy(pList[storeNum].name, tmp.c_str());
		}

		ret = GetValue(pRecordElement, "path", tmp);
		if(true == ret)
		{
			strcpy(pList[storeNum].path, tmp.c_str());
		}

		ret = GetValue(pRecordElement, "startTimeSec", tmp);
		if(true == ret)
		{
			pList[storeNum].startTime.tv_sec = atoi(tmp.c_str());
		}

		ret = GetValue(pRecordElement, "startTimeUsec", tmp);
		if(true == ret)
		{
			pList[storeNum].startTime.tv_usec = atoi(tmp.c_str());
		}
		
		ret = GetValue(pRecordElement, "stopTimeSec", tmp);
		if(true == ret)
		{
			pList[storeNum].stopTime.tv_sec = atoi(tmp.c_str());
		}
		ret = GetValue(pRecordElement, "stopTimeUsec", tmp);
		if(true == ret)
		{
			pList[storeNum].stopTime.tv_usec = atoi(tmp.c_str());
		}

		ret = GetValue(pRecordElement, "dataCount", tmp);
		if(true == ret)
		{
			pList[storeNum].dataCount = atoi(tmp.c_str());
		}

		ret = GetValue(pRecordElement, "dataSize", tmp);
		if(true == ret)
		{
			pList[storeNum].dataLength = atoi(tmp.c_str());
		}

		int icdNum = 0;
		for (TiXmlElement* pIcdElement = pRecordElement->FirstChildElement(); pIcdElement != NULL; pIcdElement = pIcdElement->NextSiblingElement(), icdNum++)
		{
			ret = GetValue(pIcdElement, "icdID", tmp);
			if(true == ret)
			{
				strcpy(pList[storeNum].icdInfo[icdNum].icdID , tmp.c_str());
			}
			ret = GetValue(pIcdElement, "startTimeSec", tmp);
			if(true == ret)
			{
				pList[storeNum].icdInfo[icdNum].startTime.tv_sec = atoi(tmp.c_str());
			}
			ret = GetValue(pIcdElement, "startTimeUsec", tmp);
			if(true == ret)
			{
				pList[storeNum].icdInfo[icdNum].startTime.tv_usec = atoi(tmp.c_str());
			}

			ret = GetValue(pIcdElement, "stopTimeSec", tmp);
			if(true == ret)
			{
				pList[storeNum].icdInfo[icdNum].stopTime.tv_sec = atoi(tmp.c_str());
			}

			ret = GetValue(pIcdElement, "stopTimeUsec", tmp);
			if(true == ret)
			{
				pList[storeNum].icdInfo[icdNum].stopTime.tv_usec = atoi(tmp.c_str());
			}
			ret = GetValue(pIcdElement, "dataCount", tmp);
			if(true == ret)
			{
				pList[storeNum].icdInfo[icdNum].dataCount = atoi(tmp.c_str());
			}

			ret = GetValue(pIcdElement, "dataSize", tmp);
			if(true == ret)
			{
				pList[storeNum].icdInfo[icdNum].dataLength = atoi(tmp.c_str());
			}
			ret = GetValue(pIcdElement, "dataType", tmp);
			if(true == ret)
			{
				pList[storeNum].icdInfo[icdNum].storageType = atoi(tmp.c_str());
			}
		}
		pList[storeNum].icdCount = icdNum;
	}
	listLen = storeNum;
	return ret;
}
int CStorageManager::insertStorageXml()
{
	int ret = 0;

	string strXmlPath(m_config.storagePath);
	string storeRecordPath = strXmlPath + "/StoreRecordInfo.xml";
	//TiXmlDocument mydoc(storeRecordPath.c_str());
	TiXmlDocument *writeXml = new TiXmlDocument;
	//mydoc.LoadFile();
	//bool loadOk = mydoc.LoadFile();//�����ĵ� //modify by wc 20231212 don't need to load file , This operation will cause a storage exception
	//if (!loadOk)
		TiXmlElement *storeRecordElement = new TiXmlElement("storeRecords");
		TiXmlElement *recordElement = new TiXmlElement("record");
		recordElement->SetAttribute("name", m_config.storageName.c_str());
		recordElement->SetAttribute("startTimeSec", m_config.startTimeSec);
		recordElement->SetAttribute("stopTimeSec", m_config.stopTimeSec);
		recordElement->SetAttribute("startTimeUsec", m_config.startTimeUsec);
		recordElement->SetAttribute("stopTimeUsec", m_config.stopTimeUsec);

		writeXml->LinkEndChild(storeRecordElement);
		storeRecordElement->LinkEndChild(recordElement);
		TY_ICD_STORAGE_MAP::iterator iter = m_icdStorageMap.begin();
		while(iter != m_icdStorageMap.end())
		{
			TiXmlElement *icdElement = new TiXmlElement("icdInfo");
			icdElement->SetAttribute("icdID", iter->second->icdID);
			icdElement->SetAttribute("dataType", iter->second->dataType);
			icdElement->SetAttribute("startTimeSec", iter->second->indexHeader.firstPacketTime.tv_sec);
			icdElement->SetAttribute("startTimeUsec", iter->second->indexHeader.firstPacketTime.tv_usec);
			icdElement->SetAttribute("stopTimeSec", iter->second->indexHeader.endTime.tv_sec);
			icdElement->SetAttribute("stopTimeUsec", iter->second->indexHeader.endTime.tv_usec);
			icdElement->SetAttribute("dataCount", iter->second->allLength);
			icdElement->SetAttribute("dataSize", iter->second->allDataFileSize);
			recordElement->LinkEndChild(icdElement);
			iter++;
		}
		if(writeXml->SaveFile(storeRecordPath.c_str()) == false)
		{
			printf("[%s:%s:%d]--> save storagerecodeinfo.xml fail\n", __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			printf("[%s:%s:%d]--> save storagerecodeinfo.xml success\n", __FILE__, __FUNCTION__, __LINE__);

		}
		
	
    return 0;
	/*																modify by wc 20231213 Repetitive code nonsense
	   TiXmlElement* storeRecordElement = mydoc.RootElement();
	   TiXmlElement *recordElement = new TiXmlElement("record");
	   recordElement->SetAttribute("name", m_config.storageName.c_str());
	   recordElement->SetAttribute("startTimeSec", m_config.startTimeSec);
	   recordElement->SetAttribute("stopTimeSec", m_config.stopTimeSec);
	   recordElement->SetAttribute("startTimeUsec", m_config.startTimeUsec);
	   recordElement->SetAttribute("stopTimeUsec", m_config.stopTimeUsec);
	   TY_ICD_STORAGE_MAP::iterator iter = m_icdStorageMap.begin();
	   while(iter != m_icdStorageMap.end())
	   {
	   TiXmlElement *icdElement = new TiXmlElement("icdInfo");
	   icdElement->SetAttribute("icdID", iter->second->icdID);
	//printf("icdID:%s\n", iter->second->icdID);
	icdElement->SetAttribute("dataType", iter->second->dataType);
	icdElement->SetAttribute("startTimeSec", iter->second->indexHeader.firstPacketTime.tv_sec);
	icdElement->SetAttribute("startTimeUsec", iter->second->indexHeader.firstPacketTime.tv_usec);
	icdElement->SetAttribute("stopTimeSec", iter->second->indexHeader.endTime.tv_sec);
	icdElement->SetAttribute("stopTimeUsec", iter->second->indexHeader.endTime.tv_usec);

	icdElement->SetAttribute("dataCount", iter->second->allLength);
	icdElement->SetAttribute("dataSize", iter->second->allDataFileSize);
	recordElement->LinkEndChild(icdElement);
	iter++;
	}
	storeRecordElement->LinkEndChild(recordElement);
	//writeXml->LinkEndChild(storeRecordElement);
	mydoc.SaveFile(storeRecordPath.c_str());
	*/
}
int CStorageManager::log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
		int lineNo, string msg)
{
	char tmp[1024];
	CLog *pLog = CLog::getInstance();
	sprintf(tmp, "StorageManager :CaseId:%s StorageName:%s\n%s\n",
			 m_caseId.c_str(), m_storageName.c_str(), msg.c_str());
	pLog->log(logType, fileName, funName, lineNo, tmp);
	return 0;
}
