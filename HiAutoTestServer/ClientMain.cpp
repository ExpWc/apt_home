#include <iostream>
#include "ace/Get_Opt.h"
#include "HiAutoTestServerC.h"
#include "orbsvcs/Naming/Naming_Client.h"
#define DEBUG printf
using namespace std;
int service_id = 0;
int ACE_TMAIN(int argc, ACE_TCHAR *argv[])
{
    try
    {
        argc = argc;
        argv[0] = argv[0];
        string name;
        string ior;
        int argc = 2;
        char *argv[2];
        argv[0] = new char[100];
        argv[1] = new char[100];
        strcpy(argv[0],"-ORBInitRef");
        strcpy(argv[1],"NameService=iiop://192.168.1.122:2809/NameService");
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

        // Find the Naming Service
        CORBA::Object_var naming_obj = orb->resolve_initial_references(
                "NameService");

        CosNaming::NamingContext_var root = CosNaming::NamingContext::_narrow(
                naming_obj.in());
        if (CORBA::is_nil(root.in()))
        {
            printf( "Nil Naming Context reference err\n" );
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

        // Bind the example Naming Context, if necessary
        CosNaming::Name nName;
        nName.length(1);
        nName[0].id = CORBA::string_dup("HiAutoTestServer1");
        nName[0].kind = CORBA::string_dup("");


       // CORBA::Object_var tmp = orb->string_to_object(ior.c_str());
        autoTest::HiAutoTest_var corbaVar;
        printf("get ob start\n");
       // corbaVar = io::lightningTracker::_narrow(tmp.in());
        corbaVar = autoTest::HiAutoTest::_narrow(root->resolve(nName));
        printf("get ob success\n");
        if (CORBA::is_nil(corbaVar.in()))
        {
            ACE_ERROR_RETURN(
                    (LM_DEBUG, "Nil Test::Hello reference <>\n"),
                    1);
        }
        char projectName[256];
        sprintf(projectName,"%s","3539e900-a504-11e8-940e-d3bed3e31258");
        corbaVar->setCase(projectName);
        printf("LoadProject :%s Success\n",projectName);
        printf("wait cmd\n");
        string cmd;

        /*while (cin >> cmd)
        {

            fflush(stdout);
            if (strcmp(cmd.c_str(), "res") == 0)
            {

                simulator::SimResourceInfo_var resInfo;
                corbaVar->getResourceInfo(resInfo);
                printf("CoarVar::getCardsInfo CpuNum:%d modelCount:%d\n",resInfo->cpuNum,resInfo->modelNames.length());
                for(unsigned int i=0;i<resInfo->modelNames.length();i++)
                {
                    printf("modelName:%s\n",resInfo->modelNames[i].in());
                }
                for(unsigned int i=0;i<resInfo->timeResNames.length();i++)
                {
                    printf("modelName:%s\n",resInfo->timeResNames[i].in());
                }
            }
            if (strcmp(cmd.c_str(), "simconfig") == 0)
            {
                simulator::SimConig  *pSimConfig = new simulator::SimConig() ;
                pSimConfig->basePeriod =200;
                pSimConfig->timeResName = CORBA::string_dup("SoftTime");
               // pSimConfig->timeResName = CORBA::string_dup("/dev/hrtimer");
                pSimConfig->modelNum = 1;
                pSimConfig->modelConfigs.length( pSimConfig->modelNum);
                pSimConfig->modelConfigs[0].modelName =CORBA::string_dup("testType");
                pSimConfig->modelConfigs[0].periodMul =1;
                pSimConfig->modelConfigs[0].cpuIndex =2;
                pSimConfig->modelConfigs[0].storageVarOffsets.length(10 );
                for(int i=0;i<10;i++)
                {
                    pSimConfig->modelConfigs[0].storageVarOffsets[i] = 13*i;
                }
                pSimConfig->modelConfigs[0].storageVarOffsets[0] = 949;
                pSimConfig->modelConfigs[0].storageSignalOffsets.length(1);
                pSimConfig->modelConfigs[0].storageSignalOffsets[0] = 1516;

//                pSimConfig->modelConfigs[1].modelName =CORBA::string_dup("testSHM2");
//                pSimConfig->modelConfigs[1].periodMul =100;
//                pSimConfig->modelConfigs[1].cpuIndex =1;
//
//                pSimConfig->modelConfigs[2].modelName =CORBA::string_dup("vds_demo5");
//                pSimConfig->modelConfigs[2].periodMul =100;
//                pSimConfig->modelConfigs[2].cpuIndex =1;
                corbaVar->setSimConfig(*pSimConfig,"112233");

            }
            if (strcmp(cmd.c_str(), "clear") == 0)
            {
                corbaVar->clearSimConfig();

            }
            if (strcmp(cmd.c_str(), "start") == 0)
            {
                simulator::StrSeq modelNames;
                modelNames.length(1);
                modelNames[0] = CORBA::string_dup("testType2");
               // modelNames[1] = CORBA::string_dup("testSHM2");
               // modelNames[2] = CORBA::string_dup("vds_demo5");

                corbaVar->startSim(modelNames);

            }
            if (strcmp(cmd.c_str(), "stop") == 0)
            {
                simulator::StrSeq modelNames;
                modelNames.length(1);
                modelNames[0] = CORBA::string_dup("testSHM9");
               // modelNames[1] = CORBA::string_dup("testSHM2");
               // modelNames[2] = CORBA::string_dup("vds_demo5");
                corbaVar->stopSim(modelNames);

            }
            if (strcmp(cmd.c_str(), "status") == 0)
            {
                simulator::ServerStatus_var statusInfo;
                corbaVar->getServerStatus(statusInfo);
                printf("====>>total:%f \n",statusInfo->storageCapacity);
                printf("====>>use:%f \n",statusInfo->usedCapacity);
                printf("====>>free:%f \n",statusInfo->unusedCapacity);
                for(unsigned int i=0;i<statusInfo->modelStatuss.length();i++)
                {
                       printf("==>>modelName:%s\n",statusInfo->modelStatuss[i].modelName.in());
                       printf("pid            :%d \n",statusInfo->modelStatuss[i].pid);
                       printf("modelRunStatus :%d \n",statusInfo->modelStatuss[i].modelRunStatus);
                       printf("period      :%d \n",statusInfo->modelStatuss[i].period);
                       printf("frameCount     :%d \n",statusInfo->modelStatuss[i].frameCount);
                       printf("overrunTimes   :%d \n",statusInfo->modelStatuss[i].overrunTimes);
                       printf("cpuIndex       :%d \n",statusInfo->modelStatuss[i].cpuIndex);
                       printf("runTime        :%d \n",statusInfo->modelStatuss[i].runTime);
                       printf("avgRunTime     :%d \n",statusInfo->modelStatuss[i].avgRunTime);
                       printf("delayTime      :%d \n",statusInfo->modelStatuss[i].delayTime);
                       printf("maxDelayTime   :%d \n\n",statusInfo->modelStatuss[i].maxDelayTime);


                }

            }
            if(strcmp(cmd.c_str(), "setp") == 0)
            {
                simulator::ParameterDesSeq paras;
                paras.length(1);
                paras[0].modelParaOffset = 0;
                double dData =10;
                char cData[8];
                memcpy(cData,&dData,8);
                paras[0].value.length(8);
                for(int i=0;i<8;i++)
                {
                    paras[0].value[i] = cData[i];
                }
                corbaVar->setParameter("testSHM9",paras);
            }
            if(strcmp(cmd.c_str(), "getp") == 0)
            {
                simulator::ParameterDesSeq des;
                des.length(1);
                des[0].modelParaOffset = 0;
                des[0].modelParaLen = 8;
                simulator::ParameterDesSeq_var paras;
                corbaVar->getParameter("testSHM9",des, paras);
                for (unsigned int i = 0; i < paras->length(); i++)
                {
                    char cData[8];
                    for (int j = 0;j < 8; j++)
                    {
                        cData[j]= paras[i].value[j] ;
                    }
                    double dData;
                    memcpy(&dData,cData,8);
                    printf("====>>para:%d value:%f \n",paras[i].modelParaOffset,dData);
                }
            }
            if (strcmp(cmd.c_str(), "addv") == 0)
            {
                simulator::VarDesSeq des;
                des.length(1);
                des[0].modelName = CORBA::string_dup("testSHM9");
                des[0].varType = 0;
                des[0].modelVarOffset = 949;
                des[0].modelVarLen = 8;
                des[0].dataType = simulator::M_DOUBLE;
                des[0].timeWindowInNs = 50000000;
                corbaVar->addMonitorVar(des);
            }
            if (strcmp(cmd.c_str(), "getv") == 0)
            {
                simulator::VarDesSeq des;
                des.length(1);
                des[0].modelName =  CORBA::string_dup("testType2");
                des[0].varType = 1;
                des[0].modelVarOffset = 1516;
                des[0].modelVarLen = 2;
                des[0].dataType = simulator::M_SHORT;
                //des[0].dataType = simulator::M_DOUBLE;

//                des[1].modelName =  CORBA::string_dup("testSHM3");
//                des[1].varType = 0;
//                des[1].modelVarOffset = 1560;
//                des[1].modelStartFrameCount = 0;
               simulator::VarValueSeq_var values;
                corbaVar->getRawSamplesInTimeWindow(des,values);
                for (unsigned int i = 0; i < values->length(); i++)
                {
                    for(unsigned int j=0;j<values[i].value.length();j++)
                    {
                        short value;
                        memcpy(&value,values[i].value[j].get_buffer(),sizeof(value)) ;
                        printf("j%d offset:%d  valude:%d\n",j,values[i].modelVarOffset,value);
                    }

                }
            }
            if (strcmp(cmd.c_str(), "getvr") == 0)
            {
                simulator::VarDesSeq des;
                des.length(1);
                des[0].modelName =  CORBA::string_dup("testType2");
                des[0].varType = 1;
                des[0].modelVarOffset = 1516;
                des[0].modelVarLen = 2;
                des[0].dataType = simulator::M_SHORT;
                //des[0].dataType = simulator::M_DOUBLE;
                //
                ////                des[1].modelName =  CORBA::string_dup("testSHM3");
                ////                des[1].varType = 0;
                ////                des[1].modelVarOffset = 1560;
                ////                des[1].modelStartFrameCount = 0;
                simulator::VarValueSeq_var values;
                 corbaVar->getRawSamplesInRealTime(des,values);
                for (unsigned int i = 0; i < values->length(); i++)
                {
                          for(unsigned int j=0;j<values[i].value.length();j++)
                          {
                                  short value;
                                  memcpy(&value,values[i].value[j].get_buffer(),sizeof(value)) ;
                                  printf("j%d offset:%d  valude:%d\n",j,values[i].modelVarOffset,value);
                          }
                }
            }
            if (strcmp(cmd.c_str(), "smw") == 0)
            {
                static int flag = 1;
                simulator::VarDesSeq des;
                des.length(1);
                des[0].modelName =  CORBA::string_dup("testType2");
                des[0].varType = 1;
                des[0].modelVarOffset = 1516;
                if(flag == 0)
                {
                    flag = 1;
                    des[0].timeWindowInNs = 5000000;
                }
                else
                {
                    flag = 0;
                    des[0].timeWindowInNs = 1000000000;
                }


                corbaVar->setMonitorWindow(des);
            }
            if (strcmp(cmd.c_str(), "getvf") == 0)
            {
                simulator::VarDesSeq des;
                des.length(1);
                des[0].modelName =  CORBA::string_dup("testSHM9");
                des[0].varType = 0;
                des[0].modelVarOffset = 949;
                des[0].modelVarLen = 8;
                des[0].dataType = simulator::M_DOUBLE;

                corbaVar->pauseMonitor(des);

                simulator::VarValueTimeSeq_var rawSampleDataTime;

                corbaVar->getBufferTime(des, rawSampleDataTime);

                for (unsigned int i = 0; i < rawSampleDataTime->length(); i++)
                {

                        printf("startTime:%ld-%ld endTime:%ld-%ld\n",rawSampleDataTime[i].startTime.sec,rawSampleDataTime[i].startTime.nsec,
                                rawSampleDataTime[i].endTime.sec,rawSampleDataTime[i].endTime.nsec);

                }
                simulator::TY_HI_TIME startTime;
                startTime.sec = rawSampleDataTime[0].startTime.sec;
                startTime.nsec = rawSampleDataTime[0].startTime.nsec;
                simulator::TY_HI_TIME endTime;

                endTime.sec = rawSampleDataTime[0].startTime.sec;
                endTime.nsec = rawSampleDataTime[0].startTime.nsec+ 500000;

                simulator::VarValueSeq_var values;
                corbaVar->getRawSampleInTimeRange(startTime, endTime, des,
                        values);
                for (unsigned int i = 0; i < values->length(); i++)
                {
                    for (unsigned int j = 0; j < values[i].value.length(); j++)
                    {
                        double value;
                        memcpy(&value,values[i].value[j].get_buffer(),sizeof(value)) ;
                        printf("j%d offset:%d  valude:%f\n", j,
                                values[i].modelVarOffset, value);
                    }

                }
                corbaVar->restartMonitor(des);
            }
            if (strcmp(cmd.c_str(), "loads") == 0)
            {
                simulator::ReturnInfo info;
                //char cInfo[1024];
                info = corbaVar->loadStorageRecord("b0b4d9d0-a43e-11e8-ac74-edccb4967b4b");
                 printf("==>loadStorageRecord %d\n",info.status);

            }
            if (strcmp(cmd.c_str(), "dels") == 0)
            {
                simulator::ReturnInfo info;
                //char cInfo[1024];
                info = corbaVar->deleteStorageRecord("112233");
                 printf("==>deleteStorageRecord %d\n",info.status);

            }

            if (strcmp(cmd.c_str(), "gets") == 0)
            {
                simulator::ReturnInfo info;
                simulator::VarDesSeq des;
                des.length(1);
                des[0].modelName = CORBA::string_dup("testType2");
                des[0].varType = 1;
                des[0].dataType = simulator::M_SHORT;
                des[0].modelVarOffset = 1516;
                des[0].modelVarLen = 2;
                simulator::VarValueSeq_var values;
#if 0
                simulator::TY_HI_TIME startTime;
                simulator::TY_HI_TIME endTime;
                startTime.sec = 0;
                startTime.nsec=0;
                endTime.sec =1;
                endTime.nsec =5000000;
                info = corbaVar->getRecordVarValue("",startTime,endTime,des, values);
                for (unsigned int i = 0; i < values->length(); i++)
                {
                    for (unsigned int j = 0; j < values[i].value.length(); j++)
                    {

                        short value=0;
                        memcpy(&value,values[i].value[j].get_buffer(),sizeof(value)) ;
                        printf("j:%d offset:%d valude:%d\n",
                                j,values[i].modelVarOffset, value);
                    }

                }
#endif
                printf("==>getRecordVarValue %d\n",info.status);

            }
            printf("=======>>please input cmd:\n");
            if (strcmp(cmd.c_str(), "quit") == 0)
                break;

        }*/
        orb->destroy();
    } catch (const CORBA::Exception& ex)
    {
        ex._tao_print_exception("Exception caught:");
        return 1;
    }
    return 0;
}
