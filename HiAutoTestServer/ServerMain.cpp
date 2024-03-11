/******************** (C) COPYRIGHT 2014********************
 * File Name          : ServerMain.cpp
 * Author             : wyk
 * Version            : V1.0.0
 * Date               : 05/05/2014
 * Description        : Main file of HiIOManager
 * Built with Linux g++
 *******************************************************************/
/**********************************************************************************************
 *   Revision History:
 *
 *   Version     Date         Initials       Descriptions
 *   --------    ---------    ------------   ---------------
 *   3.0.0       14/05/22     shen.zhang     1.accomplish the selfcheck function
 *					     					 2.add parse_command in main()
 ************************************************************************************************/
#include <signal.h>
#include "CorbaManager.h"
#include "service_control.h"
#include "HiReport.h"
#include "HardwareManager.h"
#include "HiLog.h"
#include "HiCommon.h"

#define DEBUG_INFO

#define SELF_CHECK_INFO_LEN 10240 // modify by wc 20240227
CorbaManager *pServerCorbaManager;
enum gServerRunFlag
{
	RUN, EXIT
};
gServerRunFlag gServerRunFlag = EXIT;
const char softwareVersion[] = "1.0.0";
const char c_configFileName[] = ".xml";
const char logFileName[] = ".log";
char ServerName[64] = "HiAutoTestServer1";
char CorbaIp[64] = "10.10.1.33";
char gHrpHomeEnv[512];//"use for storage the HRP_HOME env"
unsigned int gDebugFlag =1;
char setPath[64] = "AUTO_HOME";
char reportType[32] = "INFO"; //"ERROR" "DEBUG"
/*******************************************************************************
 * Function Name  : exit_signal
 * Description    : signal fun
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
static void exit_signal(int signo)
{
	if (signo == SIGINT)
	{
		gServerRunFlag = EXIT;
	}
	printf("\nStart to exit\n");
	return;
}
static int server_exit()
{
	gServerRunFlag = EXIT;
	//REPORT(REPORT_INFO, "server will stop\n");
	return 0;
}

/*******************************************************************************
 * Function Name  : server_start
 * Description    : start corba Create DDS node and use Healthmanager to broadcast
 *                  ior
 * Input          : None
 * Output         : None
 * Return         : success return 0;failed -1;
 *******************************************************************************/
int server_start()
{
    int ret = 0;
	gServerRunFlag = RUN;

    CLog *pLog = CLog::getInstance();
    char filePath[256];
	getServerPath(filePath);
	string fullPath(filePath);
	fullPath.append("caseLog.log");

    pLog->setLogFileName((char*)fullPath.c_str());
    pLog->log("================start server=====================\n");
    //CHardwareManager * pHardwareManager = new CHardwareManager();
    CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
    printf("Load hardware resource ...\n");
    string hardXmlPath(filePath);
    hardXmlPath.append("Hardware.xml");
    ret = pHardwareManager->loadResource((char*)hardXmlPath.c_str());


	pServerCorbaManager = new CorbaManager((const char *)CorbaIp,(const char *)ServerName);
	ret = pServerCorbaManager->start();
	if (ret == 0)
	{
		REPORT(REPORT_INFO, "Corba start success\n");

	}
	else
	{
		REPORT(REPORT_INFO, "Corba start faild\n");
		gServerRunFlag = EXIT;
		return -1;
	}

	return 0;
}
int server_stop()
{
	int ret = 0;
	if (pServerCorbaManager != NULL)
	{
		ret = pServerCorbaManager->stop();
		if (ret == 0)
		{
			REPORT(REPORT_INFO, "Corba stop success\n");
		}
		else
		{
			REPORT(REPORT_ERROR, "Corba stop failed\n");
			return -1;
		}
		delete pServerCorbaManager;
	}
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
	pHardwareManager->stop();

	return 0;
}
int server_selfcheck(char *info)
{
	CHardwareManager * pHardwareManager = CHardwareManager::getInstance();
    //sprintf(info,"%s","running ok\n");
	char tmpInfo[SELF_CHECK_INFO_LEN];
	pHardwareManager->selfcheck(tmpInfo);
	strcat(info, tmpInfo);
//	sprintf(info, "%s", tmpInfo);
	return 0;
}

/*******************************************************************************
 * Function Name  : parse_command
 * Description    : parse the main input and set the config
 * Input          : - int argc,char *argv[]
 * Output         : None
 * Return         : None
 *******************************************************************************/
static void parse_command(int argc, char *argv[])
{
	int option;
	char str_date[32];
	char str_time[32];
	while ((option = getopt(argc, argv, ":vdi:p:e:")) != -1)//getopt define in unistd.h
	{
		switch (option)
		{
			case 'i':
				printf(ServerName, "%s", optarg);
				sprintf(ServerName, "%s", optarg);
				break;
			case 'p':
				sprintf(CorbaIp, "%s", optarg);
				break;
			case 'd':
				gDebugFlag =0;
				break;
            case 'e':
            	sprintf(setPath, "%s", optarg);
            	break;
			case 'v':
				sprintf(str_date, "%s", __DATE__);
				sprintf(str_time, "%s", __TIME__);
				printf("software version :%s\ncompile date:\n%s %s\n",
						softwareVersion, str_date, str_time);
				exit(1);
				break;
#ifndef _WIN32
		case ':':
			fprintf(stderr, "Error: -%c without argument\n", optopt);
			exit(1);
			break;
		case '?':
			fprintf(stderr, "Error: Unknown argument %c\n", optopt);
			exit(1);
			break;
#endif
		default:
			exit(1);
		}
	}
}

/*******************************************************************************
 * Function Name  : main
 * Description    : main
 * Input          : None
 * Output         : None
 * Return         :
 *******************************************************************************/
int main(int argc, char *argv[])
{
	char service_command[64];
	if (argc > 1)
	{
		strcpy(service_command, argv[1]);
	}
	if (signal(SIGINT, &exit_signal) == SIG_ERR)//SIGUSR
	{
		printf("Couldn't register signal handler !\n");
		return -1;
	}
	signal(SIGCHLD,SIG_IGN);
	parse_command(argc, argv);
        /*get hrp_home env*/


	char *path;
	path = getenv(setPath);//"AUTO_HOME"
	if(path != NULL)
	{
		sprintf(gHrpHomeEnv,"%s",path);
	}
	else
	{
		sprintf(gHrpHomeEnv,"%s","/home/simpole/V1.0.0");
	}



	register_service(service_command, ServerName, gDebugFlag,
	  	server_start, server_exit, server_selfcheck);
#ifdef DEBUG_INFO
	reportInit("std",(char *)reportType,REPORT_FALSE,NULL);//"ERROR"
#else
	reportInit("std",(char *)"ERROR",REPORT_FALSE,NULL);
#endif
	while (gServerRunFlag == RUN)
	{
	  	sleep(1);
	}
	server_stop();
	delete_service();
	return 0;
}
