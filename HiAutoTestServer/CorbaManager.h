/*
 * CorbaManager.h
 *
 *  Created on: Mar 30, 2012
 *      Author: root
 */

#ifndef CORBAMANAGER_H_
#define CORBAMANAGER_H_
#include "ace/Get_Opt.h"
#include "ace/OS_NS_stdio.h"
#include "CorbaImpl.h"
using namespace std;
typedef  CorbaImpl CorbaImplClass;
typedef  autoTest::HiAutoTest_var InterFaceVar;
typedef  autoTest::HiAutoTest InterFaceClass;
class CorbaManager
{
	friend void * cobra_run_pthread(void *pParam);
public:
    //CorbaManager(const char * str_eth);
    CorbaManager(const char * nameServerIp, const char * serverName);
    CorbaManager();
    virtual ~CorbaManager();
public:
    int start();
    int stop();
    CORBA::String_var getIor();
private:
    int corba_run();
private:
    CORBA::String_var m_ior;
    string m_strEth;
    string m_corbaIp;
    string m_serverName;
    pthread_t m_pThreadCorbaRun;
    int getSip(const char *dev, string &cobar_ip);
};

#endif /* CORBAMANAGER_H_ */
