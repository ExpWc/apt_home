/*
 * CorbaManager.cpp
 *
 *  Created on: Mar 30, 2012
 *      Author: root
 */
#include <net/ethernet.h>
#include <arpa/inet.h>//for inet_addr
#include "orbsvcs/Naming/Naming_Client.h"
#include "HiReport.h"
#include "CorbaManager.h"
extern char gHrpHomeEnv[512];
CORBA::ORB_var m_orb;
CosNaming::NamingContext_var gNameRoot;
enum CORBA_RUN_FLAG
{
    CORBA_RUN, CORBA_EXIT
};
static CORBA_RUN_FLAG m_RunFlag = CORBA_EXIT;
/*******************************************************************************
 * Function Name  : service_start
 * Description    : start cobar service and broadcast the ior form dds(HealthManager)
 * Input          : - int argc,char *argv[]
 * Output         : None
 * Return         : int :return 0 when success ,other failed
 *******************************************************************************/
int CorbaManager::corba_run()
{
    int argc = 5;
    char *argv[6];
    char tmpC[100];
    argv[0] = new char[100];
    argv[1] = new char[100];
    argv[2] = new char[100];
    argv[3] = new char[100];
    argv[4] = new char[100];
    argv[5] = new char[100];
    string str_iiop = "-ORBListenEndpoints iiop://";
    str_iiop += m_corbaIp;
    strcpy(argv[4],str_iiop.c_str());
    strcpy(argv[0],"-ORBSvcConf");
    sprintf(tmpC,"%s/HiAutoTestServer/bin/svc.conf",gHrpHomeEnv);
    strcpy(argv[1],tmpC); /*cobar config file .allow corba server mutilthread*/
    strcpy(argv[2],"-ORBInitRef");
    sprintf(tmpC,"NameService=iiop://%s:2809/NameService",m_corbaIp.c_str());
    strcpy(argv[3],tmpC);/*connect to corba naming service*/
    try
    {
        // start corba service and get the ior
        m_orb = CORBA::ORB_init(argc, argv);
        CORBA::Object_var poa_object = m_orb->resolve_initial_references(
                "RootPOA");

        PortableServer::POA_var root_poa = PortableServer::POA::_narrow(
                poa_object.in());
        if (CORBA::is_nil(root_poa.in()))
            ACE_ERROR_RETURN((LM_ERROR, " (%P|%t) Panic: nil RootPOA\n"), 1);
        PortableServer::POAManager_var poa_manager = root_poa->the_POAManager();
        poa_manager->activate();
        CorbaImplClass *corbaImpl = 0;
        ACE_NEW_RETURN(corbaImpl, CorbaImplClass(), 1);
        PortableServer::ServantBase_var owner_transfer(corbaImpl);
        PortableServer::ObjectId_var id = root_poa->activate_object(
                corbaImpl);
        CORBA::Object_var object = root_poa->id_to_reference(id.in());
        InterFaceVar corbaVar = InterFaceClass::_narrow(
                object.in());
        CORBA::String_var ior = m_orb->object_to_string(corbaVar.in());
        m_ior = ior;

        // Find the Naming Service
        CORBA::Object_var naming_obj = m_orb->resolve_initial_references(
                "NameService");

        gNameRoot = CosNaming::NamingContext::_narrow(
                naming_obj.in());
        if (CORBA::is_nil(gNameRoot.in()))
        {
            printf( "Nil Naming Context reference err\n" );
            return 1;
        }


        // Bind the example Naming Context, if necessary
        CosNaming::Name name;
        name.length(1);
        name[0].id = CORBA::string_dup(m_serverName.c_str());
        name[0].kind = CORBA::string_dup("");

        gNameRoot->rebind(name, corbaVar);

        REPORT(REPORT_INFO,"===>> [[%s]] start success\n",m_serverName.c_str());
        m_RunFlag = CORBA_RUN;
        m_orb->run();

		usleep(1000);

        root_poa->destroy(1, 1);
        m_orb->destroy();
    } catch (const CORBA::Exception& ex)
    {
        ex._tao_print_exception("Exception caught:");
        //REPORT(REPORT_ERROR,"corba failed\n");
        return 1;
    }
    //delete argv[0];
    m_RunFlag = CORBA_EXIT;
    return 0;
}

void * cobra_run_pthread(void *pParam)
{
    CorbaManager *manager = (CorbaManager *) pParam;
    manager->corba_run();
    return NULL;
}

CorbaManager::CorbaManager(const char * nameServerIp,const char * serverName)
{
    m_ior = "NULL";
    if(nameServerIp != NULL)
    {
        m_corbaIp = string(nameServerIp);
    }
    else
    {
        m_corbaIp = "10.10.1.40";
    }
    if(serverName != NULL)
    {
        m_serverName = string(serverName);
    }
    else
    {
        m_serverName = "SimulatorServer";
    }
}
CorbaManager::CorbaManager()
{
    m_ior = "NULL";
    m_strEth = "eth0";
}
CorbaManager::~CorbaManager()
{
}
int CorbaManager::start()
{
    if (m_RunFlag == CORBA_RUN)
    {
       // REPORT(REPORT_INFO,"The Corba server have started!\n");
        return 0;
    }
    int pthread_status;
    pthread_status = pthread_create(&m_pThreadCorbaRun, NULL,
            cobra_run_pthread, (void*) this);
    if (pthread_status != 0)
    {
        return -1;
    }
    for(int i = 0;i<2000;i++)
    {
        if(m_RunFlag == CORBA_RUN)
        {
            break;
        }
        usleep(200000);
    }
    if(m_RunFlag != CORBA_RUN)
    {
        pthread_cancel(m_pThreadCorbaRun);
        m_RunFlag = CORBA_EXIT;
        return -1;
    }
	
    return 0;
}
int CorbaManager::stop()
{
    if (m_RunFlag == CORBA_EXIT)
    {
        //REPORT(REPORT_INFO,"The Corba server have stop!\n");
        return 0;
    }

    // Bind the example Naming Context, if necessary
    CosNaming::Name name;
    name.length(1);
    name[0].id = CORBA::string_dup(m_serverName.c_str());
    name[0].kind = CORBA::string_dup("");
    gNameRoot->unbind(name);
    m_orb->shutdown(false);

	pthread_join(m_pThreadCorbaRun, NULL);

    
    return 0;
}
CORBA::String_var CorbaManager::getIor()
{
    return m_ior;
}
/*******************************************************************************
 * Function Name  : getSrcIp
 * Description    : 获取源ip地址;
 * Input          : -ip地址
 * Output         : NONE
 * Return         : 成功返回0，失败返回1;
 *******************************************************************************/
int CorbaManager::getSip(const char *dev, string &cobar_ip)
{
    struct ifreq ifr;
    int sockfd;
    unsigned long srcIp;
    struct in_addr tmp_addr;

    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    {
        //REPORT(REPORT_ERROR,"create socket error\n");
        return -1;
    }
    strcpy(ifr.ifr_name, dev);
    if (ioctl(sockfd, SIOCGIFADDR, &ifr))
    {
        //REPORT(REPORT_ERROR,"ioctl error!\n");
        return -1;
    }
    memcpy(&srcIp, ifr.ifr_addr.sa_data + 2, 4);
    tmp_addr.s_addr = srcIp;
    cobar_ip.assign(inet_ntoa(tmp_addr));
    close(sockfd);
    return 0;
}
