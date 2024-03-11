#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <dlfcn.h>
#include "clientLib.h"
#include "parseXmlApi.h"
#include <string>
//#include <Windows.h>

using namespace std;

const char xmlFileName[] = "CaseBinding422.xml";
const char caseId[64] = "a0bedf01-fdf9-4ca1-acb4-898c3dc00bb3";
int main()
{
    init("192.168.1.120", "HiAutoTestServer1");
    
    cout<<"=======>>please input cmd(1:ICD; 2:chn; 3:case; 4:storage; 5:curve; 6:start; 7:caseStatus; 8:stepStatus; 9:subStepStatus; 10:stop; 11:getCardInfo 12:server; 13:quit; 14:stopStep):"<<endl;
    string cmd;
    int ret = 0;
    while (cin>>cmd)
    {
    	if (strcmp(cmd.c_str(), "1") == 0)//ICD
		{

			string str;
			//str = string("xml/IcdBlockInfo1109.xml");
			str = string(GetXmlData("bin/ICDInfo.xml"));
			//printf("===>>%s\n", str.c_str());
			setICD((char *)str.c_str());
			printf("===>>%d\n",ret);
		}
    	if (strcmp(cmd.c_str(), "2") == 0)//chn
		{

			//TY_SIM_RESCOURCE_INFO *pRes=new TY_SIM_RESCOURCE_INFO;xml/ChannelConfig1109.xml
			//char channelConfig[64] = "channel config\n";/home/V1.0.0/HardwareManager/ChannelConfig.xml
			ret = setChannelConfig(GetXmlData("bin/CardInfo.xml"));
			//ret = setChannelConfig("xml/ChannelConfig1109.xml");

			printf("===>>%d\n",ret);
		}
      if (strcmp(cmd.c_str(), "3") == 0)//case
        {

            string str;
            //str = string(GetXmlData(xmlFileName));
            str = string(GetXmlData("bin/CaseInfo.xml"));
            //printf("===>>%s\n", str.c_str());
            setCase((char *)str.c_str());
            
            printf("===>>%d\n",ret);
        }
      if (strcmp(cmd.c_str(), "4") == 0)//storage
        {

            string str;
            //str = string(GetXmlData(xmlFileName));
            str = string(GetXmlData("xmlzr/StorageInfo.xml"));
            //printf("===>>%s\n", str.c_str());
            setStorageConfig((char *)str.c_str());

            printf("===>>%d\n",ret);
        }
      if (strcmp(cmd.c_str(), "5") == 0)//curve
        {

            string str;
            //str = string(GetXmlData(xmlFileName));
            str = string(GetXmlData("xmlzr/CurveInfo.xml"));
            //printf("===>>%s\n", str.c_str());
            setCurveConfig((char *)str.c_str());

            printf("===>>%d\n",ret);
        }

      if (strcmp(cmd.c_str(), "6") == 0)//start
        {
            TY_TEST_EXECUTE testExecute;
            testExecute.testExecuteNum = 1;
            testExecute.interval = 0;
            memset(testExecute.caseExecute[0].caseID, 0, 64);
            sprintf(testExecute.caseExecute[0].caseID, caseId);
            testExecute.caseExecute[0].caseExecuteTimes = 1;
            testExecute.caseExecute[0].interval = 0;
            ret = start(testExecute);
            printf("---->%d\n", ret);
        }
        if (strcmp(cmd.c_str(), "7") == 0)//status
        {
            int num = 1;
            TY_CASE_STATUS caseStatus[1];
            sprintf(caseStatus[0].caseID, caseId);
            ret = getCaseStatus(caseStatus, num);
            printf("---->%d\n", ret);
        }
        if (strcmp(cmd.c_str(), "8") == 0)//statuss
        {
            int num = 1;
            TY_CASE_STATUS caseStatus[1];
            sprintf(caseStatus[0].caseID, caseId);
            ret = getCaseStatus(caseStatus, num);
            printf("---->caseStatus:%d\n", caseStatus[0].caseStepStatus);

            int targetTimes = 5;
            int times = 0;
            int runFlag = 1;
            //int stepStatus = -1;

            TY_STEP_STATUS stepStatusInfo[3];
            memcpy(stepStatusInfo[0].caseID, caseStatus[0].caseID, 64);
            memcpy(stepStatusInfo[1].caseID, caseStatus[0].caseID, 64);

			sprintf(stepStatusInfo[0].stepID, "b41c0efb-13c4-49eb-8c26-1934fc8b3057");
			sprintf(stepStatusInfo[1].stepID, "99a494ff-7b78-4892-a1fb-b092ac6f10e1");
            while(runFlag)
            {
                if(times == targetTimes)
                {
                    runFlag = 0;
                }
				// stepStatus = pCaseManager->getStepStatus(testExecute.caseExecute[0].caseID, stepID1);
				// printf("--->>>main stepStatus1:%d\n", stepStatus);
				ret = getStepStatus(stepStatusInfo, 2);
				printf("--->>>main stepStatus2:%d msg:%s attachInfo:%s\n",
						stepStatusInfo[0].caseStepStatus, stepStatusInfo[0].msg, stepStatusInfo[0].attachInfo);
				printf("--->>>main stepStatus2:%d msg:%s attachInfo:%s\n",
						stepStatusInfo[1].caseStepStatus, stepStatusInfo[1].msg, stepStatusInfo[0].attachInfo);

				ret = getCaseStatus(caseStatus, num);
				printf("--->>>main caseStatus:%d\n\n", caseStatus[0].caseStepStatus);
#ifdef WIN32
                Sleep(1000);
#else
                sleep(1);
#endif
                times++;
            }
            printf("---->%d\n", ret);
        }
        if (strcmp(cmd.c_str(), "9") == 0)//statuss
        {
            int num = 1;
            TY_CASE_STATUS caseStatus[1];
            sprintf(caseStatus[0].caseID, caseId);
            ret = getCaseStatus(caseStatus, num);
            printf("---->caseStatus:%d\n", caseStatus[0].caseStepStatus);

            int targetTimes = 5;
            int times = 0;
            int runFlag = 1;
            //int stepStatus = -1;

            TY_SUBSTEP_STATUS subStepStatusInfo[3];
            memcpy(subStepStatusInfo[0].caseID, caseStatus[0].caseID, 64);
            memcpy(subStepStatusInfo[1].caseID, caseStatus[0].caseID, 64);

			sprintf(subStepStatusInfo[0].stepID, "50d1da1b-47fc-48e1-9f18-014faeca9f5d");
			sprintf(subStepStatusInfo[1].stepID, "99a494ff-7b78-4892-a1fb-b092ac6f10e1");

			sprintf(subStepStatusInfo[0].subStepID, "50d1da1b-47fc-48e1-9f18-014faeca9f5e");
			sprintf(subStepStatusInfo[1].subStepID, "99a494ff-7b78-4892-a1fb-b092ac6f10e1");
            while(runFlag)
            {
                if(times == targetTimes)
                {
                    runFlag = 0;
                }
				// stepStatus = pCaseManager->getStepStatus(testExecute.caseExecute[0].caseID, stepID1);
				 printf("subStepStatusInfo[0].subStepID:%s\n", subStepStatusInfo[0].subStepID);
				ret = getSubStepStatus(subStepStatusInfo, 2);
				printf("--->>>main stepStatus2:%d msg:%s attachInfo:%s\n",
						subStepStatusInfo[0].caseStepStatus, subStepStatusInfo[0].msg, subStepStatusInfo[0].attachInfo);
				printf("--->>>main stepStatus2:%d msg:%s attachInfo:%s\n",
						subStepStatusInfo[1].caseStepStatus, subStepStatusInfo[1].msg, subStepStatusInfo[0].attachInfo);

				ret = getCaseStatus(caseStatus, num);
				printf("--->>>main caseStatus:%d\n\n", caseStatus[0].caseStepStatus);
#ifdef WIN32
                Sleep(1000);
#else
                sleep(1);
#endif
                times++;
            }
            printf("---->%d\n", ret);
        }
        if (strcmp(cmd.c_str(), "10") == 0)//stop
        {
            ret = stop();
            printf("---->%d\n", ret);
        }
        if (strcmp(cmd.c_str(), "11") == 0)//stop
        {
            ret = getCardsInfo();
            printf("---->%d\n", ret);
        }
        if (strcmp(cmd.c_str(), "12") == 0)//server
        {
            TY_SERVER_STATUS serverStatus[1];
            ret = getServerStatus(serverStatus);
            printf("---->serverStatus cardNum:%d \n", serverStatus[0].cardNum, serverStatus[0].serverRunStatus);
            printf("---->serverStatus id:%lld status:%d \n", serverStatus[0].cardStatus[0].id, serverStatus[0].cardStatus[0].status);
            printf("---->%d\n", ret);
        }
        if (strcmp(cmd.c_str(), "14") == 0)//stop
        {
            TY_STEP_INFO stepInfo[2];
            sprintf(stepInfo[0].caseID, "4433ace0-a2c7-4204-827e-c72456418c5d");
			      sprintf(stepInfo[0].stepID, "1bedfd8d-aec1-4817-ac51-b24689b2e262");
            ret = stopStep(stepInfo, 1);

            printf("---->%d\n", ret);
        }
        if (strcmp(cmd.c_str(), "15") == 0)//getModelVar
        {
            TY_MODEL_INFO modelInfo;
            modelInfo.num = 1;
            sprintf(modelInfo.fullPath[0], "test1/Gain");
            sprintf(modelInfo.modelName,, "test1");
            ret = getModelVar(&modelInfo, 1);
            printf("---->%d data:%lf\n", ret,modelInfo.data[0]);
        }
        if (strcmp(cmd.c_str(), "16") == 0)//setModelVar
        {
            TY_MODEL_INFO modelInfo;
            modelInfo.num = 1;
            sprintf(modelInfo.fullPath[0], "test1/Gain");
            sprintf(modelInfo.modelName,, "test1");
            modelInfo.data[0] = 1;
            ret = setModelVar(&modelInfo, 1);
            printf("---->%d data:%lf\n", ret,modelInfo.data[0]);
        }
        if (strcmp(cmd.c_str(), "17") == 0)//getModelParam
        {
            TY_MODEL_INFO modelInfo;
            modelInfo.num = 1;
            sprintf(modelInfo.fullPath[0], "test1/Shared Memory Set");
            sprintf(modelInfo.modelName,, "test1");
            modelInfo.data[0] = 1;
            ret = getModelParam(&modelInfo, 1);
            printf("---->%d data:%lf\n", ret,modelInfo.data[0]);
        }
        if (strcmp(cmd.c_str(), "18") == 0)//setModelParam
        {
            TY_MODEL_INFO modelInfo;
            modelInfo.num = 1;
            sprintf(modelInfo.fullPath[0], "test1/Shared Memory Set");
            sprintf(modelInfo.modelName,, "test1");
            modelInfo.data[0] = 1;
            ret = setModelParam(&modelInfo, 1);
            printf("---->%d data:%lf\n", ret,modelInfo.data[0]);
        }
        if (strcmp(cmd.c_str(), "13") == 0)//quit
        {
            ret = dispose();
            printf("---->%d\n", ret);
            break;
        }
        cout << "=======>>please input cmd(1:ICD; 2:chn; 3:case; 4:storage; 5:curve; 6:start; 7:caseStatus; 8:stepStatus; 9:subStepStatus; 10:stop; 11:getCardInfo 12:server; 13:quit):" << endl;
    }
    return 0;
}
