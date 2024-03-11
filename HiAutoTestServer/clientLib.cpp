// clientLib.cpp : Defines the exported functions for the DLL application.
/*
 * clientLib.cpp
 *
 *  Created on: Sep 21, 2021
 *  Author: root
 */
#include <iostream>
#include <string>
#include <stdio.h>
#include "clientLib.h"
#include "HiReport.h"
#include "HiAutoTestServerC.h"
#include "orbsvcs/Naming/Naming_Client.h"
#include "orbsvcs/CosNamingC.h"

using namespace std;

typedef map<string, autoTest::HiAutoTest_var> TY_IP_CORBAVAL_MAP;
TY_IP_CORBAVAL_MAP my_ip_corbaval_map;

static CORBA_RUN_FLAG m_RunFlag = CORBA_EXIT;
autoTest::HiAutoTest_var corbaVar = NULL;
CORBA::ORB_var orb = NULL;
char gNameServerIp[32] = {0};
char gNameServerName[64] = {0};
bool gReconnectionFlag = false;
#ifdef WIN32
HANDLE reconnectionThreadId;
#else
pthread_t reconnectionThreadId;
#endif
#define DEBUG 1

#ifdef WIN32
unsigned long __stdcall reconnectionThread(void *pParam)
#else
void *reconnectionThread(void *pParam)
#endif
{
	int ret = 0;
	while (gReconnectionFlag == true)
	{
		ret = init(gNameServerIp, gNameServerName);
		if (ret == 0)
		{
			gReconnectionFlag = false;
		}
#ifdef WIN32
		Sleep(10);
#else
		usleep(10000);
#endif
	}
	return NULL;
}

int reconnectionCorbaVar(const char *exInfo)
{
	char exStr[256];
	sprintf(exStr, "%s", "system exception, ID 'IDL:omg.org/CORBA/TRANSIENT:1.0'\nOMG minor code (2), described as 'No usable profile in IOR.', completed = NO\n");

	if (strcmp(exInfo, exStr) == 0)
	{
		if (gReconnectionFlag == true)
		{
			return SERVER_DOWN;
		}
		gReconnectionFlag = true;
#ifdef WIN32
		reconnectionThreadId = CreateThread(NULL, NULL, reconnectionThread, NULL, NULL, NULL);
#else
		pthread_create(&reconnectionThreadId, NULL, reconnectionThread, NULL);
#endif
		return SERVER_DOWN;
	}
	return -1;
}

int init(const char *pNameServerIp, const char *pNameServerName)
{
	int count = 0;
test:
	try
	{
		sprintf(gNameServerIp, "%s", pNameServerIp);
		sprintf(gNameServerName, "%s", pNameServerName);
		int argc = 2;
		char *argv[2];
		string name;
		string ior;
		argv[0] = new char[100];
		argv[1] = new char[100];
		strcpy(argv[0], "-ORBInitRef");
		char tmpC[100];
		sprintf(tmpC, "NameService=iiop://%s:2809/NameService", pNameServerIp);
		strcpy(argv[1], tmpC);
		printf(argv[1]);
		orb = CORBA::ORB_init(argc, argv);
		printf("\n%s\n", pNameServerName);
		printf("ORB initialized\n");
		//get the root naming context
		CORBA::Object_var naming_obj = orb->resolve_initial_references(
			"NameService");
		CosNaming::NamingContext_var root = CosNaming::NamingContext::_narrow(
			naming_obj.in());
		if (CORBA::is_nil(root.in()))
		{
			printf("Nil Naming Context reference err\n");
			corbaVar = NULL;
			return 1;
		}

		::CosNaming::BindingList_var bl;
		::CosNaming::BindingIterator_var bi;
		root->list(3, bl, bi);

		::CosNaming::Binding *pBinding;
		pBinding = bl->get_buffer();
		for (unsigned int i = 0; i < bl->length(); i++)
		{
			printf("nameService:%s\n", pBinding[i].binding_name[0].id.in());
		}
		//bind the Messager object
		CosNaming::Name nName;
		nName.length(1);
		nName[0].id = CORBA::string_dup(pNameServerName);
		nName[0].kind = CORBA::string_dup("");
		corbaVar = autoTest::HiAutoTest::_narrow(root->resolve(nName));
		printf("get ob success\n");
		m_RunFlag = CORBA_RUN;
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		corbaVar = NULL;
		return -1;
	}
	return 0;
}

int dispose()
{
	int count = 0;
test:
	try
	{
		if (m_RunFlag == CORBA_EXIT)
		{
			return 0;
		}
		if (orb != NULL)
		{
			orb->shutdown(false);
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		return -1;
	}
	return 0;
}

int setICD(const char *ICDInfo)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->setICD(ICDInfo);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setCase(const char *caseData)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->setCase(caseData);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setChannelConfig(const char *channelConfig)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->setChannelConfig(channelConfig);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setCurveConfig(const char *pCurveConfig)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->setCurveConfig(pCurveConfig);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int start(TY_TEST_EXECUTE testExecute)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		printf("---lib---start \n");
		autoTest::TestExecute testExecuteCorba;
		testExecuteCorba.testExecuteNum = testExecute.testExecuteNum;
		testExecuteCorba.interval = testExecute.interval;
		testExecuteCorba.caseExecute.length(testExecute.testExecuteNum);
		for (int i = 0; i < testExecute.testExecuteNum; i++)
		{

			testExecuteCorba.caseExecute[i].caseExecuteTimes = (short)testExecute.caseExecute[i].caseExecuteTimes;
			testExecuteCorba.caseExecute[i].interval = testExecute.caseExecute[i].interval;
			testExecuteCorba.caseExecute[i].caseID = CORBA::string_dup(testExecute.caseExecute[i].caseID);
			printf("---lib---start caseExecuteTimes:%d interval:%d caseID:%s\n", testExecuteCorba.caseExecute[i].caseExecuteTimes,
				   testExecuteCorba.caseExecute[i].interval, testExecuteCorba.caseExecute[i].caseID.in());
		}
		rInfo = corbaVar->start(testExecuteCorba);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int stop()
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->stop();
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
int stopStep(TY_STEP_INFO *stepInfo, int num)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::StepInfoSeq StepInfoSeqCorba;
		StepInfoSeqCorba.length(num);
		//printf("---lib---num:%d\n", num);
		for (int i = 0; i < num; i++)
		{
			StepInfoSeqCorba[i].caseID = CORBA::string_dup(stepInfo[i].caseID);
			StepInfoSeqCorba[i].stepID = CORBA::string_dup(stepInfo[i].stepID);
		}
		rInfo = corbaVar->stopStep(StepInfoSeqCorba);
		//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pCaseStatus[i].caseStepStatus);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
//mannual test begin111111111111111111111111111111111111111111111111111111
int startManualTest()
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->startManualTest();
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int stopManualTest()
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->stopManualTest();
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int startModel(const char *pData)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		printf("==============go into start Model_API=======================\n");
		rInfo = corbaVar->startModel(pData);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int stopModel(char *modelName)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{

		rInfo = corbaVar->stopModel(modelName);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getModelRunStatus(char *modelName, int *modelStatus)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
again:
	try
	{
		printf("==============go into getModelStatus_API=======================\n");
		::CORBA::Long pModelStatus = 0;
		rInfo = corbaVar->getModelRunStatus(modelName, pModelStatus);
		*modelStatus = pModelStatus;
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			count++;
			goto again;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setICDData(TY_SET_ICD_INFO *icdInfo, int icdNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TySetIcdInfo TySetIcdCorba;
		TySetIcdCorba.length(icdNum);
		for (int i = 0; i < icdNum; i++)
		{
			int num = icdInfo[i].fieldNum;
			TySetIcdCorba[i].icdID = CORBA::string_dup(icdInfo[i].icdID);
			TySetIcdCorba[i].fieldID.length(num);
			TySetIcdCorba[i].data.length(num);
			TySetIcdCorba[i].fieldNum = num;
			printf("-----------setData fieldNUm = %d\n", num);
			for (int j = 0; j < num; j++)
			{
				char tmpVal[128];
				memcpy(tmpVal, icdInfo[i].fieldID[j], 128);
				TySetIcdCorba[i].fieldID[j] = CORBA::string_dup(tmpVal);
				//TySetIcdCorba[i].fieldID[j] = CORBA::string_dup(icdInfo[i].fieldID[j]);
				TySetIcdCorba[i].data[j] = icdInfo[i].data[j];
			}
		}

		rInfo = corbaVar->setICDData(TySetIcdCorba, icdNum);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int sendICDData(TY_SEND_ICD_INFO *icdInfo, int icdNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		printf("-=----------go into send ICDData\n");
		autoTest::TySendIcdInfo TySendIcdCorba;
		TySendIcdCorba.length(icdNum);
		for (int i = 0; i < icdNum; i++)
		{
			TySendIcdCorba[i].cardID = icdInfo[i].cardID;
			TySendIcdCorba[i].icdID = CORBA::string_dup(icdInfo[i].icdID);
			TySendIcdCorba[i].cardType = CORBA::string_dup(icdInfo[i].cardType);

			//			printf("-----cardID = %d, icdID = %s, cardType = %s\n",TySendIcdCorba[i].cardID,
			//				TySendIcdCorba[i].icdID,TySendIcdCorba[i].cardType );
		}

		rInfo = corbaVar->sendICDData(TySendIcdCorba, icdNum);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getICDData(TY_GET_ICD_INFO *icdInfo, int icdNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyGetIcdInfo TyGetIcdCorba;
		autoTest::TyGetIcdInfo_var TyGetIcdCorba_out;

		TyGetIcdCorba.length(icdNum);

		for (int i = 0; i < icdNum; i++)
		{
			TyGetIcdCorba[i].fieldNum = icdInfo[i].fieldNum;
			TyGetIcdCorba[i].cardID = icdInfo[i].cardID;
			TyGetIcdCorba[i].cardType = CORBA::string_dup(icdInfo[i].cardType);
			TyGetIcdCorba[i].icdID = CORBA::string_dup(icdInfo[i].icdID);
			TyGetIcdCorba[i].fieldID.length(icdInfo[i].fieldNum);
			TyGetIcdCorba[i].data.length(icdInfo[i].fieldNum);
			for (int j = 0; j < icdInfo[i].fieldNum; j++)
			{
				char tmpVal[128];
				memcpy(tmpVal, icdInfo[i].fieldID[j], 128);
				TyGetIcdCorba[i].fieldID[j] = CORBA::string_dup(icdInfo[i].fieldID[j]);
				//TyGetIcdCorba[i].data[j] = icdInfo[i].data[j];
			}
		}
		rInfo = corbaVar->getICDData(TyGetIcdCorba, TyGetIcdCorba_out, icdNum);
		for (int i = 0; i < icdNum; i++)
		{
			for (int j = 0; j < icdInfo[i].fieldNum; j++)
			{
				icdInfo[i].data[j] = (double)TyGetIcdCorba_out[i].data[j];
			}
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getModelParam(TY_MODEL_INFO *modelInfo, int modelNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyModelInfo TyModelCorba;
		autoTest::TyModelInfo_var TyModelCorba_out;
		TyModelCorba.length(modelNum);
		for (int i = 0; i < modelNum; i++)
		{
			TyModelCorba[i].num = modelInfo[i].num;
			TyModelCorba[i].modelName = CORBA::string_dup(modelInfo[i].modelName);
			TyModelCorba[i].fullPath.length(modelInfo[i].num);
			TyModelCorba[i].data.length(modelInfo[i].num);
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				TyModelCorba[i].fullPath[j] = CORBA::string_dup(modelInfo[i].fullPath[j]);
			}
		}
		rInfo = corbaVar->getModelParam(TyModelCorba, TyModelCorba_out, modelNum);
		for (int i = 0; i < modelNum; i++)
		{
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				//memcpy(modelInfo[i].fullPath[j], (char *)TyModelCorba_out[i].fullPath[j],strlen(TyModelCorba[i].fullPath[j]));
				modelInfo[i].data[j] = (double)TyModelCorba_out[i].data[j];
			}
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setModelParam(TY_MODEL_INFO *modelInfo, int modelNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyModelInfo TyModelCorba;
		TyModelCorba.length(modelNum);
		for (int i = 0; i < modelNum; i++)
		{
			TyModelCorba[i].num = modelInfo[i].num;
			TyModelCorba[i].modelName = CORBA::string_dup(modelInfo[i].modelName);
			TyModelCorba[i].fullPath.length(modelInfo[i].num);
			TyModelCorba[i].data.length(modelInfo[i].num);
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				TyModelCorba[i].fullPath[j] = CORBA::string_dup(modelInfo[i].fullPath[j]);
				TyModelCorba[i].data[j] = modelInfo[i].data[j];
			}
		}
		rInfo = corbaVar->setModelParam(TyModelCorba, modelNum);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setModelVar(TY_MODEL_INFO *modelInfo, int modelNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyModelInfo TyModelCorba;
		TyModelCorba.length(modelNum);
		for (int i = 0; i < modelNum; i++)
		{
			TyModelCorba[i].num = modelInfo[i].num;
			TyModelCorba[i].modelName = CORBA::string_dup(modelInfo[i].modelName);
			TyModelCorba[i].fullPath.length(modelInfo[i].num);
			TyModelCorba[i].data.length(modelInfo[i].num);
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				TyModelCorba[i].fullPath[j] = CORBA::string_dup(modelInfo[i].fullPath[j]);
				TyModelCorba[i].data[j] = modelInfo[i].data[j];
			}
		}
		rInfo = corbaVar->setModelVar(TyModelCorba, modelNum);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int optManualTestOneStep(const char *oneStep, TY_STEP_STATUS *pStepStatus)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::StepStatusSeq_var StepStatusValues;

		rInfo = corbaVar->optManualTestOneStep(oneStep, StepStatusValues);
		for (int i = 0; i < 1; i++)
		{
			pStepStatus[i].caseStepStatus = (CaseStepStatusE)StepStatusValues[i].caseStepStatus;
			sprintf(pStepStatus[i].msg, "%s", StepStatusValues[i].msg.in());
			sprintf(pStepStatus[i].attachInfo, "%s", StepStatusValues[i].attachInfo.in());
			//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pStepStatus[i].caseStepStatus);
		}
		//rInfo = corbaVar->???bu
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getModelVar(TY_MODEL_INFO *modelInfo, int modelNum)
{
	int count = 0;
	//printf("==============go into getModelVar_API=======================\n");
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyModelInfo TyModelCorba;
		autoTest::TyModelInfo_var TyModelCorba_out;
		TyModelCorba.length(modelNum);
		printf("=============== start get input struct=====================================\n");
		for (int i = 0; i < modelNum; i++)
		{
			TyModelCorba[i].num = modelInfo[i].num;
			TyModelCorba[i].modelName = CORBA::string_dup(modelInfo[i].modelName);
			TyModelCorba[i].fullPath.length(modelInfo[i].num);
			TyModelCorba[i].data.length(modelInfo[i].num);
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				TyModelCorba[i].fullPath[j] = CORBA::string_dup(modelInfo[i].fullPath[j]);
				TyModelCorba[i].data[j] = modelInfo[i].data[j];
			}
		}
		printf("===============struct swap success,go into getModelVar_API=======================\n");
		rInfo = corbaVar->getModelVar(TyModelCorba, TyModelCorba_out, modelNum);

		for (int i = 0; i < modelNum; i++)
		{
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				//memcpy(modelInfo[i].fullPath[j], (char *)TyModelCorba_out[i].fullPath[j],strlen(TyModelCorba[i].fullPath[j]));
				modelInfo[i].data[j] = (double)TyModelCorba_out[i].data[j];
			}
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

//11111
int getCaseStatus(TY_CASE_STATUS *pCaseStatus, int num)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::CaseStatusSeq CaseStatusCorba;
		CaseStatusCorba.length(num);
		//printf("---lib---num:%d\n", num);
		autoTest::CaseStatusSeq_var CaseStatusValues;
		for (int i = 0; i < num; i++)
		{
			CaseStatusCorba[i].caseID = CORBA::string_dup(pCaseStatus[i].caseID);
		}
		rInfo = corbaVar->getCaseStatus(CaseStatusCorba, CaseStatusValues);
		for (int i = 0; i < num; i++)
		{
			pCaseStatus[i].caseStepStatus = (CaseStepStatusE)CaseStatusValues[i].caseStepStatus;
			//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pCaseStatus[i].caseStepStatus);
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getSubStepStatus(TY_SUBSTEP_STATUS *pSubStepStatus, int num)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//printf("---lib---SubStepStatus\n");
		autoTest::SubStepStatusSeq SubStepStatusCorba;
		SubStepStatusCorba.length(num);
		//printf("---lib---num:%d\n", num);
		autoTest::SubStepStatusSeq_var SubStepStatusValues;
		for (int i = 0; i < num; i++)
		{
			SubStepStatusCorba[i].caseID = CORBA::string_dup(pSubStepStatus[i].caseID);
			SubStepStatusCorba[i].stepID = CORBA::string_dup(pSubStepStatus[i].stepID);
			SubStepStatusCorba[i].subStepID = CORBA::string_dup(pSubStepStatus[i].subStepID);
		}
		rInfo = corbaVar->getSubStepStatus(SubStepStatusCorba, SubStepStatusValues);
		for (int i = 0; i < num; i++)
		{
			pSubStepStatus[i].caseStepStatus = (CaseStepStatusE)SubStepStatusValues[i].caseStepStatus;
			sprintf(pSubStepStatus[i].msg, "%s", SubStepStatusValues[i].msg.in());
			sprintf(pSubStepStatus[i].attachInfo, "%s", SubStepStatusValues[i].attachInfo.in());
			//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pSubStepStatus[i].caseStepStatus);
		}
		//rInfo = corbaVar->???bu
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getStepStatus(TY_STEP_STATUS *pStepStatus, int num)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{

		autoTest::StepStatusSeq StepStatusCorba;
		StepStatusCorba.length(num);

		autoTest::StepStatusSeq_var StepStatusValues;
		for (int i = 0; i < num; i++)
		{
			StepStatusCorba[i].caseID = CORBA::string_dup(pStepStatus[i].caseID);
			StepStatusCorba[i].stepID = CORBA::string_dup(pStepStatus[i].stepID);
		}
		rInfo = corbaVar->getStepStatus(StepStatusCorba, StepStatusValues);
		for (int i = 0; i < num; i++)
		{
			pStepStatus[i].caseStepStatus = (CaseStepStatusE)StepStatusValues[i].caseStepStatus;
			sprintf(pStepStatus[i].msg, "%s", StepStatusValues[i].msg.in());
			sprintf(pStepStatus[i].attachInfo, "%s", StepStatusValues[i].attachInfo.in());
			//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pStepStatus[i].caseStepStatus);
		}
		//rInfo = corbaVar->???bu
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getFieldData(TY_FIELD_DATA *pFieldData, int num)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{

		autoTest::FieldData FieldDataCorba;
		autoTest::FieldData_var FieldDataValue;
		rInfo = corbaVar->getFieldData(FieldDataCorba, FieldDataValue);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
/*
int getServerStatus(TY_SERVER_STATUS *pServerStatus)
{
	printf("1\n");
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		autoTest::ServerStatus_var ServerStatusInfo;
		rInfo = corbaVar->getServerStatus(ServerStatusInfo);
		pServerStatus->id = ServerStatusInfo->id;
		pServerStatus->serverRunStatus = (ServerRunStatusE)ServerStatusInfo->serverRunStatus;
		pServerStatus->cardNum = ServerStatusInfo->cardStatus.length();
		for (unsigned int i = 0; i < pServerStatus->cardNum; i++)
		{
			pServerStatus->cardStatus[i].status = ServerStatusInfo->cardStatus[i].status;
			pServerStatus->cardStatus[i].id = ServerStatusInfo->cardStatus[i].id;
		}

	} catch (const CORBA::Exception& ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
*/

int getServerStatus(TY_SERVER_STATUS *pServerStatus)
{
	//printf("1\n");
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::ServerStatus_var ServerStatusInfo;
		autoTest::StatusInfo_var statusInfo;
		rInfo = corbaVar->getServerStatus(statusInfo);
		pServerStatus->id = ServerStatusInfo->id;
		pServerStatus->serverRunStatus = (ServerRunStatusE)ServerStatusInfo->serverRunStatus;
		pServerStatus->cardNum = ServerStatusInfo->cardStatus.length();
		for (unsigned int i = 0; i < pServerStatus->cardNum; i++)
		{
			pServerStatus->cardStatus[i].status = ServerStatusInfo->cardStatus[i].status;
			pServerStatus->cardStatus[i].id = ServerStatusInfo->cardStatus[i].id;
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getCardsInfo()
{
	//printf("1\n");
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::CardManagerSeq_var cardManagerinfo;
		rInfo = corbaVar->getCardsInfo(cardManagerinfo);
		int cardManagerNum = 0;
		for (unsigned int i = 0; i < cardManagerinfo->length(); i++)
		{
			cardManagerNum++;
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getExecutiveInfo(TY_ExecutiveInfo *pInfo)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::ExecutiveInfo_var executiveInfo;
		//printf("%s:%s:%d---------------\n",__FILE__,__FUNCTION__,__LINE__);
		rInfo = corbaVar->getExecutiveInfo(executiveInfo);
		sprintf(pInfo->preCaseID, executiveInfo->preCaseID.in());

		sprintf(pInfo->preStepID, executiveInfo->preStepID.in());

		pInfo->runStatus = (CaseStepStatusE)executiveInfo->runStatus;

		pInfo->failNum = executiveInfo->failNum;

		pInfo->successNum = executiveInfo->successNum;

		pInfo->caseRunTimes = executiveInfo->caseRunTimes;
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setStorageConfig(const char *pStorageConfig)
{
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		rInfo = corbaVar->setStorageConfig(pStorageConfig);
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getRecordListNum(int *pListNum)
{
	autoTest::ReturnInfo rInfo;
	::CORBA::Long listNum = 0;
	rInfo = corbaVar->getRecordListNum(listNum);
	*pListNum = listNum;
	return rInfo.status;
}
int getRecordList(TY_RecordList *pList, int *listLen)
{
	autoTest::ReturnInfo rInfo;
	::CORBA::Long retListLen = *listLen;
	::autoTest::RecordListSeq_var recordList;
	rInfo = corbaVar->getRecordList(recordList, retListLen);

	for (int listNum = 0; listNum < retListLen; listNum++)
	{
		pList[listNum].startTime.tv_usec = recordList[listNum].startTime.usec;
		pList[listNum].startTime.tv_sec = recordList[listNum].startTime.sec;
		pList[listNum].stopTime.tv_usec = recordList[listNum].stopTime.usec;
		pList[listNum].stopTime.tv_sec = recordList[listNum].stopTime.sec;
		pList[listNum].dataCount = recordList[listNum].dataCount;
		pList[listNum].dataLength = recordList[listNum].dataLength;
		pList[listNum].icdCount = recordList[listNum].icdCount;

		string tmp = recordList[listNum].name.in();
		sprintf(pList[listNum].name, "%s", tmp.c_str());
		tmp = recordList[listNum].path.in();
		sprintf(pList[listNum].path, "%s", tmp.c_str());
		if (recordList[listNum].icdCount > 256)
		{
			recordList[listNum].icdCount = 256;
		}
		for (int icdNum = 0; icdNum < recordList[listNum].icdCount; icdNum++)
		{
			pList[listNum].icdInfo[icdNum].dataCount = recordList[listNum].icdInfo[icdNum].dataCount;
			pList[listNum].icdInfo[icdNum].dataLength = recordList[listNum].icdInfo[icdNum].dataLength;
			pList[listNum].icdInfo[icdNum].startTime.tv_sec = recordList[listNum].icdInfo[icdNum].startTime.sec;
			pList[listNum].icdInfo[icdNum].startTime.tv_usec = recordList[listNum].icdInfo[icdNum].startTime.usec;
			pList[listNum].icdInfo[icdNum].stopTime.tv_sec = recordList[listNum].icdInfo[icdNum].stopTime.sec;
			pList[listNum].icdInfo[icdNum].stopTime.tv_usec = recordList[listNum].icdInfo[icdNum].stopTime.usec;
			pList[listNum].icdInfo[icdNum].storageType = recordList[listNum].icdInfo[icdNum].dataType;
			tmp = recordList[listNum].icdInfo[icdNum].icdID;
			sprintf(pList[listNum].icdInfo[icdNum].icdID, "%s", tmp.c_str());
		}
	}

	*listLen = retListLen;
	return rInfo.status;
}
int getDataByTime(char *storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, TY_DATA **pData,
				  TY_STORAGE_TIME *pTime, int *maxCount)
{
	autoTest::ReturnInfo rInfo;

	::autoTest::FieldIcdInfo varInfoTmp;
	::autoTest::TY_HI_TIME startTimeTmp;
	::autoTest::TY_HI_TIME stopTimeTmp;
	::autoTest::StorageDataSeq_var dataTmp;
	::autoTest::HiTimeSeq_var time;
	varInfoTmp.ICDID = CORBA::string_dup(varInfo.ICDID);
	varInfoTmp.caseID = CORBA::string_dup(varInfo.caseID);
	varInfoTmp.cardID = varInfo.cardID;
	varInfoTmp.channelID = varInfo.channelID;
	varInfoTmp.fieldNum = varInfo.fieldNum;

	startTimeTmp.sec = startTime.tv_sec;
	startTimeTmp.usec = startTime.tv_usec;

	stopTimeTmp.sec = stopTime.tv_sec;
	stopTimeTmp.usec = stopTime.tv_usec;
	double tmpValue = 0;
	varInfoTmp.fieldID.length(varInfo.fieldNum);
	for (int fieldNum = 0; fieldNum < varInfo.fieldNum; fieldNum++)
	{
		varInfoTmp.fieldID[fieldNum] = CORBA::string_dup(varInfo.fieldID[fieldNum]);
	}

	rInfo = corbaVar->getDataByTime(storageName, varInfoTmp, startTimeTmp, stopTimeTmp, dataTmp, time, *maxCount);

	if (rInfo.status != -1)
	{
		for (int dateNum = 0; dateNum < *maxCount; dateNum++)
		{
			pTime[dateNum].tv_sec = time[dateNum].sec;
			pTime[dateNum].tv_usec = time[dateNum].usec;
			for (int fieldNum = 0; fieldNum < varInfo.fieldNum; fieldNum++)
			{
				tmpValue = dataTmp[fieldNum].data[dateNum];
				memcpy(pData[fieldNum][dateNum].payload, (char *)&tmpValue, 8);
			}
		}
	}

	return rInfo.status;
}
int getDataByIndex(char *storageName, TY_FIELD_ICD_INFO varInfo, int startIndex, int stopIndex, TY_DATA **pData, TY_STORAGE_TIME *pTime, int *maxCount)
{
	autoTest::ReturnInfo rInfo;
	::autoTest::FieldIcdInfo varInfoTmp;
	::autoTest::StorageDataSeq_var dataTmp;
	::autoTest::HiTimeSeq_var time;
	varInfoTmp.ICDID = CORBA::string_dup(varInfo.ICDID);
	varInfoTmp.caseID = CORBA::string_dup(varInfo.caseID);
	varInfoTmp.cardID = varInfo.cardID;
	varInfoTmp.channelID = varInfo.channelID;
	varInfoTmp.fieldNum = varInfo.fieldNum;

	double tmpValue = 0;
	varInfoTmp.fieldID.length(varInfo.fieldNum);
	for (int fieldNum = 0; fieldNum < varInfo.fieldNum; fieldNum++)
	{
		varInfoTmp.fieldID[fieldNum] = CORBA::string_dup(varInfo.fieldID[fieldNum]);
	}

	rInfo = corbaVar->getDataByIndex(storageName, varInfoTmp, startIndex, stopIndex, dataTmp, time, *maxCount);

	if (rInfo.status != -1)
	{
		for (int dateNum = 0; dateNum < *maxCount; dateNum++)
		{
			pTime[dateNum].tv_sec = time[dateNum].sec;
			pTime[dateNum].tv_usec = time[dateNum].usec;
			for (int fieldNum = 0; fieldNum < varInfo.fieldNum; fieldNum++)
			{
				tmpValue = dataTmp[fieldNum].data[dateNum];
				memcpy(pData[fieldNum][dateNum].payload, (char *)&tmpValue, 8);
			}
		}
	}

	return rInfo.status;
}
int getIndexByTime(char *storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, int *startIndex, int *stopIndex)
{
	autoTest::ReturnInfo rInfo;

	//printf("---lib---getIndexByTime 1\n");
	::autoTest::FieldIcdInfo varInfoTmp;
	::autoTest::TY_HI_TIME startTimeTmp;
	::autoTest::TY_HI_TIME stopTimeTmp;
	::autoTest::HiTimeSeq_var time;
	::CORBA::Long startIndexTmp = 0;
	::CORBA::Long stopIndexTmp = 0;
	//printf("---lib---getIndexByTime 2 ICDID%s\n", varInfo.ICDID);

	varInfoTmp.ICDID = CORBA::string_dup(varInfo.ICDID);
	//printf("---lib---getIndexByTime 3 caseID%s\n", varInfo.caseID);
	varInfoTmp.caseID = CORBA::string_dup(varInfo.caseID);

	varInfoTmp.cardID = varInfo.cardID;
	varInfoTmp.channelID = varInfo.channelID;
	varInfoTmp.fieldNum = varInfo.fieldNum;

	startTimeTmp.sec = startTime.tv_sec;
	startTimeTmp.usec = startTime.tv_usec;

	stopTimeTmp.sec = stopTime.tv_sec;
	stopTimeTmp.usec = stopTime.tv_usec;
	//printf("---lib---getIndexByTime 4 fieldNum%d\n",varInfo.fieldNum);
	/*	varInfoTmp.fieldID.length(varInfo.fieldNum);
	for(int fieldNum = 0; fieldNum<varInfo.fieldNum; fieldNum++)
	{
		varInfoTmp.fieldID[fieldNum] = CORBA::string_dup(varInfo.fieldID[fieldNum]);
	}
*/
	rInfo = corbaVar->getIndexByTime(storageName, varInfoTmp, startTimeTmp, stopTimeTmp, startIndexTmp, stopIndexTmp);

	*startIndex = startIndexTmp;
	*stopIndex = stopIndexTmp;

	//rInfo = corbaVar->setStorageConfig(pStorageConfig);
	return rInfo.status;
}
int deleteStorageRecord(char *storageName)
{
	autoTest::ReturnInfo rInfo;
	rInfo = corbaVar->deleteStorageRecord(storageName);
	return rInfo.status;
}
int deleteAllStorageRecord()
{
	autoTest::ReturnInfo rInfo;
	rInfo = corbaVar->deleteAllStorageRecord();
	return rInfo.status;
}

int getModelDataByTime(char *storageName, TY_MODEL_VAR_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, TY_DATA **pData, TY_STORAGE_TIME *pTime, int *maxCount)
{

	autoTest::ReturnInfo rInfo;
	::autoTest::ModelInfo varInfoTmp;
	::autoTest::TY_HI_TIME startTimeTmp;
	::autoTest::TY_HI_TIME stopTimeTmp;
	::autoTest::StorageDataSeq_var dataTmp;
	::autoTest::HiTimeSeq_var time;
	varInfoTmp.modelName = CORBA::string_dup(varInfo.modelName);
	varInfoTmp.varNum = varInfo.varNum;

	startTimeTmp.sec = startTime.tv_sec;
	startTimeTmp.usec = startTime.tv_usec;

	stopTimeTmp.sec = stopTime.tv_sec;
	stopTimeTmp.usec = stopTime.tv_usec;
	double tmpValue = 0;
	varInfoTmp.varName.length(varInfo.varNum);
	for (int varNum = 0; varNum < varInfo.varNum; varNum++)
	{
		varInfoTmp.varName[varNum] = CORBA::string_dup(varInfo.varName[varNum]);
	}

	rInfo = corbaVar->getModelDataByTime(storageName, varInfoTmp, startTimeTmp, stopTimeTmp, dataTmp, time, *maxCount);

	if (rInfo.status != -1)
	{
		for (int dateNum = 0; dateNum < *maxCount; dateNum++)
		{
			pTime[dateNum].tv_sec = time[dateNum].sec;
			pTime[dateNum].tv_usec = time[dateNum].usec;
			for (int varNum = 0; varNum < varInfo.varNum; varNum++)
			{
				tmpValue = dataTmp[varNum].data[dateNum];
				memcpy(pData[varNum][dateNum].payload, (char *)&tmpValue, 8);
			}
		}
	}
	return 0;
}
int getModelDataByIndex(char *storageName, TY_MODEL_VAR_INFO varInfo, int startIndex, int stopIndex, TY_DATA **pData, TY_STORAGE_TIME *pTime, int *maxCount)
{
	autoTest::ReturnInfo rInfo;
	::autoTest::ModelInfo varInfoTmp;
	::autoTest::StorageDataSeq_var dataTmp;
	::autoTest::HiTimeSeq_var time;
	varInfoTmp.modelName = CORBA::string_dup(varInfo.modelName);
	varInfoTmp.varNum = varInfo.varNum;
	double tmpValue = 0;
	varInfoTmp.varName.length(varInfo.varNum);
	for (int varNum = 0; varNum < varInfo.varNum; varNum++)
	{
		varInfoTmp.varName[varNum] = CORBA::string_dup(varInfo.varName[varNum]);
	}

	rInfo = corbaVar->getModelDataByIndex(storageName, varInfoTmp, startIndex, stopIndex, dataTmp, time, *maxCount);

	if (rInfo.status != -1)
	{
		for (int dateNum = 0; dateNum < *maxCount; dateNum++)
		{
			pTime[dateNum].tv_sec = time[dateNum].sec;
			pTime[dateNum].tv_usec = time[dateNum].usec;
			for (int varNum = 0; varNum < varInfo.varNum; varNum++)
			{
				tmpValue = dataTmp[varNum].data[dateNum];
				memcpy(pData[varNum][dateNum].payload, (char *)&tmpValue, 8);
			}
		}
	}

	return rInfo.status;
}
int getModelIndexByTime(char *storageName, TY_MODEL_VAR_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, int *startIndex, int *stopIndex)
{
	autoTest::ReturnInfo rInfo;

	//printf("---lib---getIndexByTime 1\n");
	::autoTest::ModelInfo varInfoTmp;
	::autoTest::TY_HI_TIME startTimeTmp;
	::autoTest::TY_HI_TIME stopTimeTmp;
	::autoTest::HiTimeSeq_var time;
	::CORBA::Long startIndexTmp = 0;
	::CORBA::Long stopIndexTmp = 0;
	//printf("---lib---getIndexByTime 2 ICDID%s\n", varInfo.ICDID);

	varInfoTmp.modelName = CORBA::string_dup(varInfo.modelName);
	varInfoTmp.varNum = varInfo.varNum;
	printf("%s:%s:%d----", __FILE__, __FUNCTION__, __LINE__);
	//printf("---lib-varInfoTmp.modelName:%s, varInfoTmp.varNum:%d\n", varInfoTmp.modelName, varInfoTmp.varNum);

	startTimeTmp.sec = startTime.tv_sec;
	startTimeTmp.usec = startTime.tv_usec;

	stopTimeTmp.sec = stopTime.tv_sec;
	stopTimeTmp.usec = stopTime.tv_usec;

	rInfo = corbaVar->getModelIndexByTime(storageName, varInfoTmp, startTimeTmp, stopTimeTmp, startIndexTmp, stopIndexTmp);

	*startIndex = startIndexTmp;
	*stopIndex = stopIndexTmp;

	//rInfo = corbaVar->setStorageConfig(pStorageConfig);
	return rInfo.status;
	return 0;
}
int getIpInfo(char *pIpInfo)
{
	autoTest::ReturnInfo rInfo;
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		printf("enter int getIpInfo(char *pIpInfo) try\n");
		//rInfo = corbaVar->getIpInfo(pIpInfo);
		//printf("pIpInfo:%s\n", pIpInfo);
		CORBA::String_var ipInfo;
		rInfo = corbaVar->getIpInfo(ipInfo);
		char ip[64];
		int len = strlen(ipInfo);
		memset(ip, 0, 64);
		memcpy(ip, ipInfo, len);
		//printf("ipInfo:%s\n", ipInfo);
		printf("ip:%s\n", ip);
		printf("after corbaVar->getIpInfo\n");
		sprintf(pIpInfo, "%s", ip);
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
int getNodeInfo(TY_NODE_INFO *pNodeInfo, int *pCnt)
{
	try
	{
		printf("enter getNodeInfo\n");
		char *pNameServerIp;
		pNameServerIp = new char[32];
		sprintf(pNameServerIp, "%s", "192.168.1.121");
		//sprintf(pNameServerIp, "%s", "192.168.0.121");
		//sprintf(pNameServerIp, "%s", "192.168.0.120");
		int argc = 2;
		char *argv[2];
		string ipInfo;
		argv[0] = new char[100];
		argv[1] = new char[100];
		strcpy(argv[0], "-ORBInitRef");
		char tmpC[100];
		sprintf(tmpC, "NameService=iiop://%s:2809/NameService", pNameServerIp);
		strcpy(argv[1], tmpC);
		printf(argv[1]);
		orb = CORBA::ORB_init(argc, argv);
		printf("ORB initialized\n");
		//get the root naming context
		CORBA::Object_var naming_obj = orb->resolve_initial_references("NameService");
		CosNaming::NamingContext_var root = CosNaming::NamingContext::_narrow(naming_obj.in());
		if (CORBA::is_nil(root.in()))
		{
			printf("Nil Naming Context reference err\n");
			corbaVar = NULL;
			return 1;
		}

		::CosNaming::BindingList_var bl;
		::CosNaming::BindingIterator_var bi;
		root->list(3, bl, bi);

		::CosNaming::Binding *pBinding;
		pBinding = bl->get_buffer();
		printf("bl->length():%d\n", bl->length());
		*pCnt = bl->length();
		for (unsigned int i = 0; i < bl->length(); i++)
		{
			printf("nameService:%s\n", pBinding[i].binding_name[0].id.in());
			//bind the Messager object
			CosNaming::Name nName;
			nName.length(1);
			nName[0].id = CORBA::string_dup(pBinding[i].binding_name[0].id.in());
			nName[0].kind = CORBA::string_dup("");
			corbaVar = autoTest::HiAutoTest::_narrow(root->resolve(nName));
			printf("get ob success\n");
			m_RunFlag = CORBA_RUN;
			autoTest::ReturnInfo rInfo;
			CORBA::String_var ipInfo;
			rInfo = corbaVar->getIpInfo(ipInfo);
			char ip[64];
			char serverName[64];
			int len = strlen(ipInfo);
			int serverNameLen = strlen(pBinding[i].binding_name[0].id.in());
			memset(ip, 0, 64);
			memcpy(ip, ipInfo, len);
			memset(serverName, 0, 64);
			memcpy(serverName, pBinding[i].binding_name[0].id.in(), serverNameLen);
			//printf("ipInfo:%s\n", ipInfo);
			printf("ip:%s\n", ip);
			printf("serverName:%s\n", serverName);
			sprintf(pNodeInfo[i].serverName, "%s", serverName);
			sprintf(pNodeInfo[i].ip, "%s", ip);
			printf("after corbaVar->getIpInfo\n");
			TY_IP_CORBAVAL_MAP::iterator pos;
			pos = my_ip_corbaval_map.find(ip);
			if (pos == my_ip_corbaval_map.end())
			{
				my_ip_corbaval_map.insert(make_pair(ip, corbaVar));
			}
			printf("after insert map\n");
		}
		//corbaVar->getIpInfo(ipInfo);
		//printf("ipInfo:%s\n", ipInfo.c_str());
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		corbaVar = NULL;
		return -1;
	}
	return 0;
}
int setNodeICD(const char *pIp, const char *pICDInfo)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	else
	{
		printf("not find ip -> corbaVar in ip_corbaval_map\n");
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		rInfo = corbaVar->setICD(pICDInfo);
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
int setNodeChannelConfig(const char *pIp, const char *pChannelConfig)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	else
	{
		printf("not find ip -> corbaVar in ip_corbaval_map\n");
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		rInfo = corbaVar->setChannelConfig(pChannelConfig);
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
int setNodeCase(const char *pIp, const char *pCaseData)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	else
	{
		printf("not find ip -> corbaVar in ip_corbaval_map\n");
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		rInfo = corbaVar->setCase(pCaseData);
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
int startNode(const char *pIp, TY_TEST_EXECUTE testExecute)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	else
	{
		printf("not find ip -> corbaVar in ip_corbaval_map\n");
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		printf("---lib---start \n");
		autoTest::TestExecute testExecuteCorba;
		testExecuteCorba.testExecuteNum = testExecute.testExecuteNum;
		testExecuteCorba.interval = testExecute.interval;
		testExecuteCorba.caseExecute.length(testExecute.testExecuteNum);
		for (int i = 0; i < testExecute.testExecuteNum; i++)
		{

			testExecuteCorba.caseExecute[i].caseExecuteTimes = (short)testExecute.caseExecute[i].caseExecuteTimes;
			testExecuteCorba.caseExecute[i].interval = testExecute.caseExecute[i].interval;
			testExecuteCorba.caseExecute[i].caseID = CORBA::string_dup(testExecute.caseExecute[i].caseID);
			printf("---lib---start caseExecuteTimes:%d interval:%d caseID:%s\n", testExecuteCorba.caseExecute[i].caseExecuteTimes,
				   testExecuteCorba.caseExecute[i].interval, testExecuteCorba.caseExecute[i].caseID.in());
		}
		rInfo = corbaVar->start(testExecuteCorba);
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
int getNodeStepStatus(const char *pIp, TY_STEP_STATUS *pStepStatus, int num)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	else
	{
		printf("not find ip -> corbaVar in ip_corbaval_map\n");
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{

		autoTest::StepStatusSeq StepStatusCorba;
		StepStatusCorba.length(num);

		autoTest::StepStatusSeq_var StepStatusValues;
		for (int i = 0; i < num; i++)
		{
			StepStatusCorba[i].caseID = CORBA::string_dup(pStepStatus[i].caseID);
			StepStatusCorba[i].stepID = CORBA::string_dup(pStepStatus[i].stepID);
		}
		rInfo = corbaVar->getStepStatus(StepStatusCorba, StepStatusValues);
		for (int i = 0; i < num; i++)
		{
			pStepStatus[i].caseStepStatus = (CaseStepStatusE)StepStatusValues[i].caseStepStatus;
			sprintf(pStepStatus[i].msg, "%s", StepStatusValues[i].msg.in());
			sprintf(pStepStatus[i].attachInfo, "%s", StepStatusValues[i].attachInfo.in());
			//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pStepStatus[i].caseStepStatus);
		}
		//rInfo = corbaVar->???bu
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
int getNodeCaseStatus(const char *pIp, TY_CASE_STATUS *pCaseStatus, int num)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	else
	{
		printf("not find ip -> corbaVar in ip_corbaval_map\n");
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		autoTest::CaseStatusSeq CaseStatusCorba;
		CaseStatusCorba.length(num);
		printf("---lib---num:%d\n", num);
		autoTest::CaseStatusSeq_var CaseStatusValues;
		for (int i = 0; i < num; i++)
		{
			CaseStatusCorba[i].caseID = CORBA::string_dup(pCaseStatus[i].caseID);
		}
		rInfo = corbaVar->getCaseStatus(CaseStatusCorba, CaseStatusValues);
		for (int i = 0; i < num; i++)
		{
			pCaseStatus[i].caseStepStatus = (CaseStepStatusE)CaseStatusValues[i].caseStepStatus;
			printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pCaseStatus[i].caseStepStatus);
		}
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int stopNode(const char *pIp)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->stop();
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setNodeCurveConfig(const char *pIp, const char *pCurveConfig)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->setCurveConfig(pCurveConfig);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}
int stopNodeStep(const char *pIp, TY_STEP_INFO *stepInfo, int num)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::StepInfoSeq StepInfoSeqCorba;
		StepInfoSeqCorba.length(num);
		//printf("---lib---num:%d\n", num);
		for (int i = 0; i < num; i++)
		{
			StepInfoSeqCorba[i].caseID = CORBA::string_dup(stepInfo[i].caseID);
			StepInfoSeqCorba[i].stepID = CORBA::string_dup(stepInfo[i].stepID);
		}
		rInfo = corbaVar->stopStep(StepInfoSeqCorba);
		//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pCaseStatus[i].caseStepStatus);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeSubStepStatus(const char *pIp, TY_SUBSTEP_STATUS *pSubStepStatus, int num)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//printf("---lib---SubStepStatus\n");
		autoTest::SubStepStatusSeq SubStepStatusCorba;
		SubStepStatusCorba.length(num);
		//printf("---lib---num:%d\n", num);
		autoTest::SubStepStatusSeq_var SubStepStatusValues;
		for (int i = 0; i < num; i++)
		{
			SubStepStatusCorba[i].caseID = CORBA::string_dup(pSubStepStatus[i].caseID);
			SubStepStatusCorba[i].stepID = CORBA::string_dup(pSubStepStatus[i].stepID);
			SubStepStatusCorba[i].subStepID = CORBA::string_dup(pSubStepStatus[i].subStepID);
		}
		rInfo = corbaVar->getSubStepStatus(SubStepStatusCorba, SubStepStatusValues);
		for (int i = 0; i < num; i++)
		{
			pSubStepStatus[i].caseStepStatus = (CaseStepStatusE)SubStepStatusValues[i].caseStepStatus;
			sprintf(pSubStepStatus[i].msg, "%s", SubStepStatusValues[i].msg.in());
			sprintf(pSubStepStatus[i].attachInfo, "%s", SubStepStatusValues[i].attachInfo.in());
			//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pSubStepStatus[i].caseStepStatus);
		}
		//rInfo = corbaVar->???bu
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeCardsInfo(const char *pIp)
{
	//printf("1\n");
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::CardManagerSeq_var cardManagerinfo;
		rInfo = corbaVar->getCardsInfo(cardManagerinfo);
		int cardManagerNum = 0;
		for (unsigned int i = 0; i < cardManagerinfo->length(); i++)
		{
			cardManagerNum++;
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeExecutiveInfo(const char *pIp, TY_ExecutiveInfo *pInfo)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::ExecutiveInfo_var executiveInfo;
		//printf("%s:%s:%d---------------\n",__FILE__,__FUNCTION__,__LINE__);
		rInfo = corbaVar->getExecutiveInfo(executiveInfo);
		sprintf(pInfo->preCaseID, executiveInfo->preCaseID.in());

		sprintf(pInfo->preStepID, executiveInfo->preStepID.in());

		pInfo->runStatus = (CaseStepStatusE)executiveInfo->runStatus;

		pInfo->failNum = executiveInfo->failNum;

		pInfo->successNum = executiveInfo->successNum;

		pInfo->caseRunTimes = executiveInfo->caseRunTimes;
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setNodeStorageConfig(const char *pIp, const char *pStorageConfig)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
	try
	{
		rInfo = corbaVar->setStorageConfig(pStorageConfig);
	}
	catch (const CORBA::Exception &ex)
	{
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeRecordListNum(const char *pIp, int *pListNum)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	::CORBA::Long listNum = 0;
	rInfo = corbaVar->getRecordListNum(listNum);
	*pListNum = listNum;
	return rInfo.status;
}

int getNodeRecordList(const char *pIp, TY_RecordList *pList, int *listLen)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	::CORBA::Long retListLen = *listLen;
	::autoTest::RecordListSeq_var recordList;
	rInfo = corbaVar->getRecordList(recordList, retListLen);

	for (int listNum = 0; listNum < retListLen; listNum++)
	{
		pList[listNum].startTime.tv_usec = recordList[listNum].startTime.usec;
		pList[listNum].startTime.tv_sec = recordList[listNum].startTime.sec;
		pList[listNum].stopTime.tv_usec = recordList[listNum].stopTime.usec;
		pList[listNum].stopTime.tv_sec = recordList[listNum].stopTime.sec;
		pList[listNum].dataCount = recordList[listNum].dataCount;
		pList[listNum].dataLength = recordList[listNum].dataLength;
		pList[listNum].icdCount = recordList[listNum].icdCount;

		string tmp = recordList[listNum].name.in();
		sprintf(pList[listNum].name, "%s", tmp.c_str());
		tmp = recordList[listNum].path.in();
		sprintf(pList[listNum].path, "%s", tmp.c_str());
		if (recordList[listNum].icdCount > 256)
		{
			recordList[listNum].icdCount = 256;
		}
		for (int icdNum = 0; icdNum < recordList[listNum].icdCount; icdNum++)
		{
			pList[listNum].icdInfo[icdNum].dataCount = recordList[listNum].icdInfo[icdNum].dataCount;
			pList[listNum].icdInfo[icdNum].dataLength = recordList[listNum].icdInfo[icdNum].dataLength;
			pList[listNum].icdInfo[icdNum].startTime.tv_sec = recordList[listNum].icdInfo[icdNum].startTime.sec;
			pList[listNum].icdInfo[icdNum].startTime.tv_usec = recordList[listNum].icdInfo[icdNum].startTime.usec;
			pList[listNum].icdInfo[icdNum].stopTime.tv_sec = recordList[listNum].icdInfo[icdNum].stopTime.sec;
			pList[listNum].icdInfo[icdNum].stopTime.tv_usec = recordList[listNum].icdInfo[icdNum].stopTime.usec;
			pList[listNum].icdInfo[icdNum].storageType = recordList[listNum].icdInfo[icdNum].dataType;
			tmp = recordList[listNum].icdInfo[icdNum].icdID;
			sprintf(pList[listNum].icdInfo[icdNum].icdID, "%s", tmp.c_str());
		}
	}

	*listLen = retListLen;
	return rInfo.status;
}

int getNodeDataByTime(const char *pIp, char *storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, TY_DATA **pData,
					  TY_STORAGE_TIME *pTime, int *maxCount)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	::autoTest::FieldIcdInfo varInfoTmp;
	::autoTest::TY_HI_TIME startTimeTmp;
	::autoTest::TY_HI_TIME stopTimeTmp;
	::autoTest::StorageDataSeq_var dataTmp;
	::autoTest::HiTimeSeq_var time;
	varInfoTmp.ICDID = CORBA::string_dup(varInfo.ICDID);
	varInfoTmp.caseID = CORBA::string_dup(varInfo.caseID);
	varInfoTmp.cardID = varInfo.cardID;
	varInfoTmp.channelID = varInfo.channelID;
	varInfoTmp.fieldNum = varInfo.fieldNum;

	startTimeTmp.sec = startTime.tv_sec;
	startTimeTmp.usec = startTime.tv_usec;

	stopTimeTmp.sec = stopTime.tv_sec;
	stopTimeTmp.usec = stopTime.tv_usec;
	double tmpValue = 0;
	varInfoTmp.fieldID.length(varInfo.fieldNum);
	for (int fieldNum = 0; fieldNum < varInfo.fieldNum; fieldNum++)
	{
		varInfoTmp.fieldID[fieldNum] = CORBA::string_dup(varInfo.fieldID[fieldNum]);
	}

	rInfo = corbaVar->getDataByTime(storageName, varInfoTmp, startTimeTmp, stopTimeTmp, dataTmp, time, *maxCount);

	if (rInfo.status != -1)
	{
		for (int dateNum = 0; dateNum < *maxCount; dateNum++)
		{
			pTime[dateNum].tv_sec = time[dateNum].sec;
			pTime[dateNum].tv_usec = time[dateNum].usec;
			for (int fieldNum = 0; fieldNum < varInfo.fieldNum; fieldNum++)
			{
				tmpValue = dataTmp[fieldNum].data[dateNum];
				memcpy(pData[fieldNum][dateNum].payload, (char *)&tmpValue, 8);
			}
		}
	}

	return rInfo.status;
}

int getNodeDataByIndex(const char *pIp, char *storageName, TY_FIELD_ICD_INFO varInfo, int startIndex, int stopIndex, TY_DATA **pData, TY_STORAGE_TIME *pTime, int *maxCount)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	::autoTest::FieldIcdInfo varInfoTmp;
	::autoTest::StorageDataSeq_var dataTmp;
	::autoTest::HiTimeSeq_var time;
	varInfoTmp.ICDID = CORBA::string_dup(varInfo.ICDID);
	varInfoTmp.caseID = CORBA::string_dup(varInfo.caseID);
	varInfoTmp.cardID = varInfo.cardID;
	varInfoTmp.channelID = varInfo.channelID;
	varInfoTmp.fieldNum = varInfo.fieldNum;

	double tmpValue = 0;
	varInfoTmp.fieldID.length(varInfo.fieldNum);
	for (int fieldNum = 0; fieldNum < varInfo.fieldNum; fieldNum++)
	{
		varInfoTmp.fieldID[fieldNum] = CORBA::string_dup(varInfo.fieldID[fieldNum]);
	}

	rInfo = corbaVar->getDataByIndex(storageName, varInfoTmp, startIndex, stopIndex, dataTmp, time, *maxCount);

	if (rInfo.status != -1)
	{
		for (int dateNum = 0; dateNum < *maxCount; dateNum++)
		{
			pTime[dateNum].tv_sec = time[dateNum].sec;
			pTime[dateNum].tv_usec = time[dateNum].usec;
			for (int fieldNum = 0; fieldNum < varInfo.fieldNum; fieldNum++)
			{
				tmpValue = dataTmp[fieldNum].data[dateNum];
				memcpy(pData[fieldNum][dateNum].payload, (char *)&tmpValue, 8);
			}
		}
	}

	return rInfo.status;
}
int getNodeIndexByTime(const char *pIp, char *storageName, TY_FIELD_ICD_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, int *startIndex, int *stopIndex)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	//printf("---lib---getIndexByTime 1\n");
	::autoTest::FieldIcdInfo varInfoTmp;
	::autoTest::TY_HI_TIME startTimeTmp;
	::autoTest::TY_HI_TIME stopTimeTmp;
	::autoTest::HiTimeSeq_var time;
	::CORBA::Long startIndexTmp = 0;
	::CORBA::Long stopIndexTmp = 0;
	//printf("---lib---getIndexByTime 2 ICDID%s\n", varInfo.ICDID);

	varInfoTmp.ICDID = CORBA::string_dup(varInfo.ICDID);
	//printf("---lib---getIndexByTime 3 caseID%s\n", varInfo.caseID);
	varInfoTmp.caseID = CORBA::string_dup(varInfo.caseID);

	varInfoTmp.cardID = varInfo.cardID;
	varInfoTmp.channelID = varInfo.channelID;
	varInfoTmp.fieldNum = varInfo.fieldNum;

	startTimeTmp.sec = startTime.tv_sec;
	startTimeTmp.usec = startTime.tv_usec;

	stopTimeTmp.sec = stopTime.tv_sec;
	stopTimeTmp.usec = stopTime.tv_usec;
	//printf("---lib---getIndexByTime 4 fieldNum%d\n",varInfo.fieldNum);
	/*	varInfoTmp.fieldID.length(varInfo.fieldNum);
	for(int fieldNum = 0; fieldNum<varInfo.fieldNum; fieldNum++)
	{
		varInfoTmp.fieldID[fieldNum] = CORBA::string_dup(varInfo.fieldID[fieldNum]);
	}
*/
	rInfo = corbaVar->getIndexByTime(storageName, varInfoTmp, startTimeTmp, stopTimeTmp, startIndexTmp, stopIndexTmp);

	*startIndex = startIndexTmp;
	*stopIndex = stopIndexTmp;

	//rInfo = corbaVar->setStorageConfig(pStorageConfig);
	return rInfo.status;
}

int getNodeModelDataByTime(const char *pIp, char *storageName, TY_MODEL_VAR_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, TY_DATA **pData, TY_STORAGE_TIME *pTime, int *maxCount)
{

	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	::autoTest::ModelInfo varInfoTmp;
	::autoTest::TY_HI_TIME startTimeTmp;
	::autoTest::TY_HI_TIME stopTimeTmp;
	::autoTest::StorageDataSeq_var dataTmp;
	::autoTest::HiTimeSeq_var time;
	varInfoTmp.modelName = CORBA::string_dup(varInfo.modelName);
	varInfoTmp.varNum = varInfo.varNum;

	startTimeTmp.sec = startTime.tv_sec;
	startTimeTmp.usec = startTime.tv_usec;

	stopTimeTmp.sec = stopTime.tv_sec;
	stopTimeTmp.usec = stopTime.tv_usec;
	double tmpValue = 0;
	varInfoTmp.varName.length(varInfo.varNum);
	for (int varNum = 0; varNum < varInfo.varNum; varNum++)
	{
		varInfoTmp.varName[varNum] = CORBA::string_dup(varInfo.varName[varNum]);
	}

	rInfo = corbaVar->getModelDataByTime(storageName, varInfoTmp, startTimeTmp, stopTimeTmp, dataTmp, time, *maxCount);

	if (rInfo.status != -1)
	{
		for (int dateNum = 0; dateNum < *maxCount; dateNum++)
		{
			pTime[dateNum].tv_sec = time[dateNum].sec;
			pTime[dateNum].tv_usec = time[dateNum].usec;
			for (int varNum = 0; varNum < varInfo.varNum; varNum++)
			{
				tmpValue = dataTmp[varNum].data[dateNum];
				memcpy(pData[varNum][dateNum].payload, (char *)&tmpValue, 8);
			}
		}
	}
	return 0;
}
int getNodeModelDataByIndex(const char *pIp, char *storageName, TY_MODEL_VAR_INFO varInfo, int startIndex, int stopIndex, TY_DATA **pData, TY_STORAGE_TIME *pTime, int *maxCount)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	::autoTest::ModelInfo varInfoTmp;
	::autoTest::StorageDataSeq_var dataTmp;
	::autoTest::HiTimeSeq_var time;
	varInfoTmp.modelName = CORBA::string_dup(varInfo.modelName);
	varInfoTmp.varNum = varInfo.varNum;
	double tmpValue = 0;
	varInfoTmp.varName.length(varInfo.varNum);
	for (int varNum = 0; varNum < varInfo.varNum; varNum++)
	{
		varInfoTmp.varName[varNum] = CORBA::string_dup(varInfo.varName[varNum]);
	}

	rInfo = corbaVar->getModelDataByIndex(storageName, varInfoTmp, startIndex, stopIndex, dataTmp, time, *maxCount);

	if (rInfo.status != -1)
	{
		for (int dateNum = 0; dateNum < *maxCount; dateNum++)
		{
			pTime[dateNum].tv_sec = time[dateNum].sec;
			pTime[dateNum].tv_usec = time[dateNum].usec;
			for (int varNum = 0; varNum < varInfo.varNum; varNum++)
			{
				tmpValue = dataTmp[varNum].data[dateNum];
				memcpy(pData[varNum][dateNum].payload, (char *)&tmpValue, 8);
			}
		}
	}

	return rInfo.status;
}

int getNodeModelIndexByTime(const char *pIp, char *storageName, TY_MODEL_VAR_INFO varInfo, TY_STORAGE_TIME startTime, TY_STORAGE_TIME stopTime, int *startIndex, int *stopIndex)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	//printf("---lib---getIndexByTime 1\n");
	::autoTest::ModelInfo varInfoTmp;
	::autoTest::TY_HI_TIME startTimeTmp;
	::autoTest::TY_HI_TIME stopTimeTmp;
	::autoTest::HiTimeSeq_var time;
	::CORBA::Long startIndexTmp = 0;
	::CORBA::Long stopIndexTmp = 0;
	//printf("---lib---getIndexByTime 2 ICDID%s\n", varInfo.ICDID);

	varInfoTmp.modelName = CORBA::string_dup(varInfo.modelName);
	varInfoTmp.varNum = varInfo.varNum;
	printf("%s:%s:%d----", __FILE__, __FUNCTION__, __LINE__);
	//printf("---lib-varInfoTmp.modelName:%s, varInfoTmp.varNum:%d\n", varInfoTmp.modelName, varInfoTmp.varNum);

	startTimeTmp.sec = startTime.tv_sec;
	startTimeTmp.usec = startTime.tv_usec;

	stopTimeTmp.sec = stopTime.tv_sec;
	stopTimeTmp.usec = stopTime.tv_usec;

	rInfo = corbaVar->getModelIndexByTime(storageName, varInfoTmp, startTimeTmp, stopTimeTmp, startIndexTmp, stopIndexTmp);

	*startIndex = startIndexTmp;
	*stopIndex = stopIndexTmp;

	//rInfo = corbaVar->setStorageConfig(pStorageConfig);
	return rInfo.status;
	return 0;
}

int deleteNodeStorageRecord(const char *pIp, char *storageName)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	rInfo = corbaVar->deleteStorageRecord(storageName);
	return rInfo.status;
}

int deleteNodeAllStorageRecord(const char *pIp)
{
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	rInfo = corbaVar->deleteAllStorageRecord();
	return rInfo.status;
}

int startNodeManualTest(const char *pIp)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->startManualTest();
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int stopNodeManualTest(const char *pIp)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		rInfo = corbaVar->stopManualTest();
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int startNodeModel(const char *pIp, const char *pData)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		printf("==============go into start Model_API=======================\n");
		rInfo = corbaVar->startModel(pData);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int stopNodeModel(const char *pIp, char *modelName)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{

		rInfo = corbaVar->stopModel(modelName);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeModelRunStatus(const char *pIp, char *modelName, int *modelStatus)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
again:
	try
	{
		printf("==============go into getModelStatus_API=======================\n");
		::CORBA::Long pModelStatus = 0;
		rInfo = corbaVar->getModelRunStatus(modelName, pModelStatus);
		*modelStatus = pModelStatus;
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			count++;
			goto again;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setNodeICDData(const char *pIp, TY_SET_ICD_INFO *icdInfo, int icdNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TySetIcdInfo TySetIcdCorba;
		TySetIcdCorba.length(icdNum);
		for (int i = 0; i < icdNum; i++)
		{
			int num = icdInfo[i].fieldNum;
			TySetIcdCorba[i].icdID = CORBA::string_dup(icdInfo[i].icdID);
			TySetIcdCorba[i].fieldID.length(num);
			TySetIcdCorba[i].data.length(num);
			TySetIcdCorba[i].fieldNum = num;
			printf("-----------setData fieldNUm = %d\n", num);
			for (int j = 0; j < num; j++)
			{
				char tmpVal[128];
				memcpy(tmpVal, icdInfo[i].fieldID[j], 128);
				TySetIcdCorba[i].fieldID[j] = CORBA::string_dup(tmpVal);
				//TySetIcdCorba[i].fieldID[j] = CORBA::string_dup(icdInfo[i].fieldID[j]);
				TySetIcdCorba[i].data[j] = icdInfo[i].data[j];
			}
		}

		rInfo = corbaVar->setICDData(TySetIcdCorba, icdNum);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int sendNodeICDData(const char *pIp, TY_SEND_ICD_INFO *icdInfo, int icdNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		printf("-=----------go into send ICDData\n");
		autoTest::TySendIcdInfo TySendIcdCorba;
		TySendIcdCorba.length(icdNum);
		for (int i = 0; i < icdNum; i++)
		{
			TySendIcdCorba[i].cardID = icdInfo[i].cardID;
			TySendIcdCorba[i].icdID = CORBA::string_dup(icdInfo[i].icdID);
			TySendIcdCorba[i].cardType = CORBA::string_dup(icdInfo[i].cardType);

			//			printf("-----cardID = %d, icdID = %s, cardType = %s\n",TySendIcdCorba[i].cardID,
			//				TySendIcdCorba[i].icdID,TySendIcdCorba[i].cardType );
		}

		rInfo = corbaVar->sendICDData(TySendIcdCorba, icdNum);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeICDData(const char *pIp, TY_GET_ICD_INFO *icdInfo, int icdNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyGetIcdInfo TyGetIcdCorba;
		autoTest::TyGetIcdInfo_var TyGetIcdCorba_out;

		TyGetIcdCorba.length(icdNum);

		for (int i = 0; i < icdNum; i++)
		{
			TyGetIcdCorba[i].fieldNum = icdInfo[i].fieldNum;
			TyGetIcdCorba[i].cardID = icdInfo[i].cardID;
			TyGetIcdCorba[i].cardType = CORBA::string_dup(icdInfo[i].cardType);
			TyGetIcdCorba[i].icdID = CORBA::string_dup(icdInfo[i].icdID);
			TyGetIcdCorba[i].fieldID.length(icdInfo[i].fieldNum);
			TyGetIcdCorba[i].data.length(icdInfo[i].fieldNum);
			for (int j = 0; j < icdInfo[i].fieldNum; j++)
			{
				char tmpVal[128];
				memcpy(tmpVal, icdInfo[i].fieldID[j], 128);
				TyGetIcdCorba[i].fieldID[j] = CORBA::string_dup(icdInfo[i].fieldID[j]);
				//TyGetIcdCorba[i].data[j] = icdInfo[i].data[j];
			}
		}
		rInfo = corbaVar->getICDData(TyGetIcdCorba, TyGetIcdCorba_out, icdNum);
		for (int i = 0; i < icdNum; i++)
		{
			for (int j = 0; j < icdInfo[i].fieldNum; j++)
			{
				icdInfo[i].data[j] = (double)TyGetIcdCorba_out[i].data[j];
			}
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeModelParam(const char *pIp, TY_MODEL_INFO *modelInfo, int modelNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyModelInfo TyModelCorba;
		autoTest::TyModelInfo_var TyModelCorba_out;
		TyModelCorba.length(modelNum);
		for (int i = 0; i < modelNum; i++)
		{
			TyModelCorba[i].num = modelInfo[i].num;
			TyModelCorba[i].modelName = CORBA::string_dup(modelInfo[i].modelName);
			TyModelCorba[i].fullPath.length(modelInfo[i].num);
			TyModelCorba[i].data.length(modelInfo[i].num);
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				TyModelCorba[i].fullPath[j] = CORBA::string_dup(modelInfo[i].fullPath[j]);
			}
		}
		rInfo = corbaVar->getModelParam(TyModelCorba, TyModelCorba_out, modelNum);
		for (int i = 0; i < modelNum; i++)
		{
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				//memcpy(modelInfo[i].fullPath[j], (char *)TyModelCorba_out[i].fullPath[j],strlen(TyModelCorba[i].fullPath[j]));
				modelInfo[i].data[j] = (double)TyModelCorba_out[i].data[j];
			}
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setNodeModelParam(const char *pIp, TY_MODEL_INFO *modelInfo, int modelNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyModelInfo TyModelCorba;
		TyModelCorba.length(modelNum);
		for (int i = 0; i < modelNum; i++)
		{
			TyModelCorba[i].num = modelInfo[i].num;
			TyModelCorba[i].modelName = CORBA::string_dup(modelInfo[i].modelName);
			TyModelCorba[i].fullPath.length(modelInfo[i].num);
			TyModelCorba[i].data.length(modelInfo[i].num);
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				TyModelCorba[i].fullPath[j] = CORBA::string_dup(modelInfo[i].fullPath[j]);
				TyModelCorba[i].data[j] = modelInfo[i].data[j];
			}
		}
		rInfo = corbaVar->setModelParam(TyModelCorba, modelNum);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeModelVar(const char *pIp, TY_MODEL_INFO *modelInfo, int modelNum)
{
	int count = 0;
	//printf("==============go into getModelVar_API=======================\n");
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyModelInfo TyModelCorba;
		autoTest::TyModelInfo_var TyModelCorba_out;
		TyModelCorba.length(modelNum);
		printf("=============== start get input struct=====================================\n");
		for (int i = 0; i < modelNum; i++)
		{
			TyModelCorba[i].num = modelInfo[i].num;
			TyModelCorba[i].modelName = CORBA::string_dup(modelInfo[i].modelName);
			TyModelCorba[i].fullPath.length(modelInfo[i].num);
			TyModelCorba[i].data.length(modelInfo[i].num);
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				TyModelCorba[i].fullPath[j] = CORBA::string_dup(modelInfo[i].fullPath[j]);
				TyModelCorba[i].data[j] = modelInfo[i].data[j];
			}
		}
		printf("===============struct swap success,go into getModelVar_API=======================\n");
		rInfo = corbaVar->getModelVar(TyModelCorba, TyModelCorba_out, modelNum);

		for (int i = 0; i < modelNum; i++)
		{
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				//memcpy(modelInfo[i].fullPath[j], (char *)TyModelCorba_out[i].fullPath[j],strlen(TyModelCorba[i].fullPath[j]));
				modelInfo[i].data[j] = (double)TyModelCorba_out[i].data[j];
			}
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int setNodeModelVar(const char *pIp, TY_MODEL_INFO *modelInfo, int modelNum)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		//autoTest::SqFieldID SqFieldCorba;
		autoTest::TyModelInfo TyModelCorba;
		TyModelCorba.length(modelNum);
		for (int i = 0; i < modelNum; i++)
		{
			TyModelCorba[i].num = modelInfo[i].num;
			TyModelCorba[i].modelName = CORBA::string_dup(modelInfo[i].modelName);
			TyModelCorba[i].fullPath.length(modelInfo[i].num);
			TyModelCorba[i].data.length(modelInfo[i].num);
			for (int j = 0; j < modelInfo[i].num; j++)
			{
				TyModelCorba[i].fullPath[j] = CORBA::string_dup(modelInfo[i].fullPath[j]);
				TyModelCorba[i].data[j] = modelInfo[i].data[j];
			}
		}
		rInfo = corbaVar->setModelVar(TyModelCorba, modelNum);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int optNodeManualTestOneStep(const char *pIp, const char *oneStep, TY_STEP_STATUS *pStepStatus)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::StepStatusSeq_var StepStatusValues;

		rInfo = corbaVar->optManualTestOneStep(oneStep, StepStatusValues);
		for (int i = 0; i < 1; i++)
		{
			pStepStatus[i].caseStepStatus = (CaseStepStatusE)StepStatusValues[i].caseStepStatus;
			sprintf(pStepStatus[i].msg, "%s", StepStatusValues[i].msg.in());
			sprintf(pStepStatus[i].attachInfo, "%s", StepStatusValues[i].attachInfo.in());
			//printf("  ---lib---getCaseStatus-->caseStepStatus:%d\n", pStepStatus[i].caseStepStatus);
		}
		//rInfo = corbaVar->???bu
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeFieldData(const char *pIp, TY_FIELD_DATA *pFieldData, int num)
{
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{

		autoTest::FieldData FieldDataCorba;
		autoTest::FieldData_var FieldDataValue;
		rInfo = corbaVar->getFieldData(FieldDataCorba, FieldDataValue);
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}

int getNodeServerStatus(const char *pIp, TY_SERVER_STATUS *pServerStatus)
{
	//printf("1\n");
	int count = 0;
	autoTest::ReturnInfo rInfo;
	autoTest::HiAutoTest_var corbaVar = NULL;
	TY_IP_CORBAVAL_MAP::iterator pos;
	pos = my_ip_corbaval_map.find(pIp);
	if (pos != my_ip_corbaval_map.end())
	{
		corbaVar = pos->second;
	}
	if (corbaVar == NULL)
	{
		printf("Corba is not init\n");
		return -1;
	}
test:
	try
	{
		autoTest::ServerStatus_var ServerStatusInfo;
		autoTest::StatusInfo_var statusInfo;
		rInfo = corbaVar->getServerStatus(statusInfo);
		pServerStatus->id = ServerStatusInfo->id;
		pServerStatus->serverRunStatus = (ServerRunStatusE)ServerStatusInfo->serverRunStatus;
		pServerStatus->cardNum = ServerStatusInfo->cardStatus.length();
		for (unsigned int i = 0; i < pServerStatus->cardNum; i++)
		{
			pServerStatus->cardStatus[i].status = ServerStatusInfo->cardStatus[i].status;
			pServerStatus->cardStatus[i].id = ServerStatusInfo->cardStatus[i].id;
		}
	}
	catch (const CORBA::Exception &ex)
	{
		if (count < 5)
		{
			//			Sleep(10);
			count++;
			goto test;
		}
		ex._tao_print_exception("Exception caught:");
		rInfo.status = reconnectionCorbaVar(ex._info().c_str());
		return rInfo.status;
	}
	return rInfo.status;
}