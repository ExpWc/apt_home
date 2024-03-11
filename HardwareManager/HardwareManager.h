#pragma once
#include <stdio.h>
#include <unistd.h>
#include <map>
#include <vector>
#include <string>
//#include "Ini.h"
#include "CCard.h"
#include "tinyxml.h"
typedef struct _hardware_config
{
    //unsigned int devID;
    unsigned int cardNum;
    string cardManagerLibName;
    string cardLibName;
    string busType;
}TY_HARDWARE_CONFIG;


typedef map<string, CCardManager*> TY_CARD_MANAGER_MAP;

class CHardwareManager
{
public:
    CHardwareManager();
    ~CHardwareManager();

private:
    static CHardwareManager* m_instance;

    vector<TY_HARDWARE_CONFIG> m_config;
    string m_configFileName;

    int createHWInstance(TY_HARDWARE_CONFIG hwConfig,TiXmlElement *pCardElement);
    int insertCardManagerMap(string busType, CCardManager* pCardManager);
    int initCardInstance();
public:
    static CHardwareManager* getInstance();
    int loadResource(char* configFileName);
    int loadConfig(const char* configFileName);
    int start();
    int stop();
    int start(string busType);
    int stop(string busType);
    int selfcheck(char * info);
    int log(string msg);

    TY_CARD_MANAGER_MAP m_typeCardManagerMap;
    CCardManager* getCardManager(string busType);
    int deleteCardInstance();
};

