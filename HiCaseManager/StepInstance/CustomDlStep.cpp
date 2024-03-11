#include "CustomDlStep.h"
#include "HiCommon.h"
#include "dlfcn.h"
#include "AutoTestInterface.h"
#include "ParamData.h"
#include "sched.h"

#include "HiMParaComLib.h"
#include "HiMGlobalVarLib.h"
#include "HiMVarComLib.h"

#define SIMPOLE_MODEL_FLAG  1
#define AUTOTEST_MODEL_FLAG   0

CCustomDlStep::CCustomDlStep()
{
	m_customExecuteThreadId = NULL;
}
CCustomDlStep::~CCustomDlStep()
{

}

TY_MODEL_CARD_MAP modelCard;

char modelName[256];
TY_MODEL_COM_INFO * pModelComHandle = NULL;
TY_MODEL_PARAP_COM_INFO *pParaComHandle = NULL;

int CCustomDlStep::parse(TiXmlElement *pSubStepElement, CBaseStep *pStep)
{
    bool bRet = false;
    int ret = 0;
    string tmp;
    /*release resource*/
    m_subStepInfo.inputVarInfo.clear();
    m_subStepInfo.outputVarInfo.clear();
    m_subStepInfo.paramInfo.clear();
    /*release resource*/

	CObjectData *pAutoTest = CObjectData::getInstance();

    m_pSubStepElement = pSubStepElement;
    bRet = GetValue(pSubStepElement, "Dll", m_dlName);
    if (!bRet)
    {
        m_dlName = "";
        REPORT(REPORT_ERROR, "Get Dll failed\n");
        ret = -1;
    }
    /*
    bRet = GetValue(pSubStepElement, "SubStepID", m_subStepID);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get StepID failed\n");
        ret = -1;
    }
	*/
    bRet = GetValue(pSubStepElement, "PeriodMs", tmp);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get PeriodMs failed\n");
        ret = -1;
    }
    m_periodMs = atoi(tmp.c_str());
    bRet = GetValue(pSubStepElement, "Times", tmp);
    if (!bRet)
    {
	    REPORT(REPORT_ERROR, "Get Times failed\n");
	    ret = -1;
    }
    m_times = atoi(tmp.c_str());



    bRet = GetValue(pSubStepElement, "ModelName", m_modelName); //add by chao.wu 20231008
    if (!bRet)
    {	
	    REPORT(REPORT_ERROR, "Get ModelName failed\n");
	    ret = -1;

    }

    bRet = GetValue(pSubStepElement, "modelSource", tmp); 
    if (!bRet)
    {	
	    m_modelSourceFlag = AUTOTEST_MODEL_FLAG;
	    REPORT(REPORT_ERROR, "Get ModelSource failed, default Autotest\n");
	    //ret = -1;
    }
    else
    {
	    printf( "Get ModelSource success %s\n", tmp.c_str());
	    if(tmp == "simpole")
	    {
		    m_modelSourceFlag = SIMPOLE_MODEL_FLAG;
		    ret = 0;
	    }
	    else
	    {
	            printf("=================================================m_modelSourceFlag :%d\n",m_modelSourceFlag);
		    m_modelSourceFlag = AUTOTEST_MODEL_FLAG;
	    }
    }
    REPORT(REPORT_INFO, "Get Simpole model name, m_modelSourceFlag:%d,m_time:%lld, period:%d, ret:%d\n", m_modelSourceFlag,m_times,m_periodMs,ret);	

    TiXmlElement *pParameterElement = NULL;
    TiXmlElement *pOutputElement = NULL;
    TiXmlElement *pInputElement = NULL;
    TiXmlElement *pICDElement = NULL;
    pParameterElement = pSubStepElement->FirstChildElement("parameter");
    if(pParameterElement == NULL)
    {
	    REPORT(REPORT_ERROR, "Not found parameter\n");
    }
    else
    {          
	    int paramNo = 0 ;
	    TiXmlElement *pParamElement = NULL;
	    for (pParamElement = pParameterElement->FirstChildElement(); pParamElement;
			    pParamElement = pParamElement->NextSiblingElement())
	    {

		    TY_PARAM_INFO paramVar;
		    string tmpName;
		    for (TiXmlAttribute* tmp = pParamElement->FirstAttribute(); tmp != NULL; tmp = tmp->Next())
		    {
			    if (strcmp(tmp->Name(), "Referenced") == 0)
			    {
				    paramVar.strParamName = tmp->Value();
			    }
			    else if (strcmp(tmp->Name(), "Name") == 0)
			    {
				    tmpName = tmp->Value();
			    }
			    else if (strcmp(tmp->Name(), "DataType") == 0)
			    {
				    paramVar.strParamType = tmp->Value();
			    }
			    else if (strcmp(tmp->Name(), "Length") == 0)
			    {
				    paramVar.uParamLength = atoi(tmp->Value());
			    }
			    else if (strcmp(tmp->Name(), "Offset") == 0)
			    {
				    paramVar.offset = atoi(tmp->Value());
			    }

			    else if (strcmp(tmp->Name(), "Value") == 0)
			    {
				    string value(tmp->Value());
				    //TY_VALUEFIELD Dvalue;
				    //Dvalue.iValue = atoi(tmp->Value());
				    //pAutoTest->setDigitValueByString(&Dvalue, paramVar.strParamType, value);
				    pAutoTest->setDigitValueByString(&paramVar.value, paramVar.strParamType, value);
				    REPORT(REPORT_DEBUG, "parse param value:%lf\n",paramVar.value.dValue);
			    }

		    }
		    paramVar.strParamName = paramVar.strParamName + "/" + tmpName;
		    m_subStepInfo.paramInfo.push_back(paramVar);

	    }
    }
    //////////////////////////////////output>>

    pOutputElement = pSubStepElement->FirstChildElement("output");
    if(pOutputElement == NULL)
    {
	    REPORT(REPORT_ERROR, "Not found output\n");
    }
    else
    {
	    int outputNo = 0;
	    TY_OUTPUT_VAR_MAP outputVarMap;
	    for (pICDElement = pOutputElement->FirstChildElement(); pICDElement;
			    pICDElement = pICDElement->NextSiblingElement())
	    {
		    TY_VAR_INFO outputVar;
		    for (TiXmlAttribute* pIcd = pICDElement->FirstAttribute(); pIcd != NULL; pIcd = pIcd->Next())
		    {
			    if (strcmp(pIcd->Name(), "ICDID") == 0)
			    {
				    outputVar.strIcdID = pIcd->Value();
			    }
			    else if (strcmp(pIcd->Name(), "CardType") == 0)
			    {
				    outputVar.strCardType = pIcd->Value();
			    }
			    else if (strcmp(pIcd->Name(), "CardID") == 0)
			    {
				    outputVar.cardID = atoi(pIcd->Value());
			    }
			    else if (strcmp(pIcd->Name(), "ChnID") == 0)
			    {
				    outputVar.chnID = atoi(pIcd->Value());
			    }
			    else if (strcmp(pIcd->Name(), "FieldID") == 0)
			    {
				    outputVar.strFieldID = pIcd->Value();
			    }
			    else if (strcmp(pIcd->Name(), "Name") == 0)
			    {
				    outputVar.strVarName = pIcd->Value();
			    }
			    else if (strcmp(pIcd->Name(), "Referenced") == 0)
			    {
				    outputVar.strReferenced = pIcd->Value();
			    }
			    else if (strcmp(pIcd->Name(), "DataType") == 0)
			    {
				    outputVar.strVarType = pIcd->Value();
			    }
		    }
		    outputNo++;
		    m_subStepInfo.outputVarInfo.push_back(outputVar);
	    }
    }
    //////////////////////////////////input>>
    pInputElement = pSubStepElement->FirstChildElement("input");
    if(pInputElement == NULL)
    {
	    REPORT(REPORT_ERROR, "Not found input\n");
    }
    else
    {

	    int inputNo = 0;
	    pICDElement = NULL;
	    for (pICDElement = pInputElement->FirstChildElement(); pICDElement; 
			    pICDElement = pICDElement->NextSiblingElement())
	    {
		    //TY_INPUT_VAR inputVar;
		    TY_VAR_INFO inputVar;
		    bool ret1 = GetValue(pICDElement, "BindType", inputVar.strBindType);
		    if(inputVar.strBindType == "Curve")
		    {
			    GetValue(pICDElement, "CurveID", inputVar.strCurveID);
			    GetValue(pICDElement, "VarName", inputVar.strVarName);

		    }
		    else if(inputVar.strBindType == "Constant")
		    {
			    GetValue(pICDElement, "Referenced", inputVar.strReferenced);
			    GetValue(pICDElement, "Name", inputVar.strVarName);
			    GetValue(pICDElement, "DataType", inputVar.strVarType);
			    string value;
			    GetValue(pICDElement, "Value", value);

			    // REPORT(REPORT_DEBUG, "success parse value: strVarName:%s,strVarType:%s, value:%s \n",inputVar.strVarName.c_str(),inputVar.strVarType.c_str(),value.c_str());

			    //pAutoTest->setDigitValueByString(&inputVar.value, inputVar.strParamType, value);
			    inputVar.value.dValue = atof(value.c_str());

		    }

		    else if(inputVar.strBindType == "ICD")
		    {
			    TY_ROUTER router;
			    for (TiXmlAttribute* pIcd = pICDElement->FirstAttribute(); pIcd != NULL; pIcd = pIcd->Next())
			    {

				    if(pIcd == NULL)
				    {
					    REPORT(REPORT_ERROR, " pTmp is NULL failed\n");
				    }
				    if (strcmp(pIcd->Name(), "ICDID") == 0)
				    {
					    inputVar.strIcdID = pIcd->Value();
					    router.icdID = pIcd->Value();
				    }
				    else if (strcmp(pIcd->Name(), "CardType") == 0)
				    {
					    inputVar.strCardType = pIcd->Value();

				    }
				    else if (strcmp(pIcd->Name(), "CardID") == 0)
				    {
					    inputVar.cardID = atoi(pIcd->Value());

				    }
				    else if (strcmp(pIcd->Name(), "ChnID") == 0)
				    {
					    inputVar.chnID = atoi(pIcd->Value());

				    }
				    else if (strcmp(pIcd->Name(), "FieldID") == 0)
				    {
					    inputVar.strFieldID = pIcd->Value();

				    }
				    else if (strcmp(pIcd->Name(), "Name") == 0)
				    {
					    inputVar.strVarName = pIcd->Value();

				    }
				    else if (strcmp(pIcd->Name(), "Referenced") == 0)
				    {
					    inputVar.strReferenced = pIcd->Value();

				    }
				    else if (strcmp(pIcd->Name(), "DataType") == 0)
				    {
					    inputVar.strVarType = pIcd->Value();
				    }

				    else if (strcmp(pIcd->Name(), "CurveID") == 0)
				    {
					    inputVar.strCurveID = pIcd->Value();
				    }
			    }

			    inputVar.ICDInfo.m_ICDID = router.icdID; 
			    inputVar.ICDInfo.m_fieldID = inputVar.strFieldID;
		    }
		    else
		    {

			    GetValue(pICDElement, "VarName", inputVar.strVarName);
			    GetValue(pICDElement, "DataType", inputVar.strVarType);
			    //pAutoTest->setDigitValueByString(&inputVar.value, inputVar.strParamType, value);

			    inputVar.value.dValue = 0.0;

			    //REPORT(REPORT_ERROR, "success parse value: strVarName:%s,strVarType:%s, value:%lf \n",inputVar.strVarName.c_str(),inputVar.strVarType.c_str(),inputVar.value.dValue);
		    }

		    inputNo++;
		    /*insert input parameter to input_vector*/
		    m_subStepInfo.inputVarInfo.push_back(inputVar);
	    }/*end of input parse*/
    }
    if(MANUAL_TEST_FLAG == 1)
    {
	    modelCard.insert(make_pair(m_dlName, this));
    }

    /*
       for(TY_MODEL_CARD_MAP::iterator iter = modelCard.begin(); iter!=modelCard.end(); iter++)
       {
       printf("[%s:%s:%d]-->modelMap have:%s,map len:%d\n",__FILE__,__FUNCTION__,__LINE__, iter->first.c_str(),modelCard.size());
       }
       */
    return ret;
}

void* threadSimpoleExecute(void *pParam)
{

	int times = 0;
	CCustomDlStep* pCustomDlStep = (CCustomDlStep*)pParam;
	sem_init(&pCustomDlStep->m_sem, 0, 0);
	pCustomDlStep->m_sem.__align = 0;

	//>>modify by wc 20231024 1
	strcpy(modelName, (char*)pCustomDlStep->m_modelName.c_str());

	/************************
	 * Initialize the model *
	 ************************/

	initVar("HiSim");

	//<< 1
	while((pCustomDlStep->m_statusFlag != STEP_STOP) && (pCustomDlStep->m_statusFlag != STEP_NULL)
			&& (pCustomDlStep->m_statusFlag != STEP_INIT))
	{
		if(pCustomDlStep->m_statusFlag == STEP_PAUSE)
		{
			usleep(1000);
			continue;
		}

		sem_wait(&pCustomDlStep->m_sem);

		updateInputData((char*)pCustomDlStep->m_modelName.c_str());	

		updateOutputData((char*)pCustomDlStep->m_modelName.c_str());
		// Save the last beat I/O data
		storeModelVar((char*)pCustomDlStep->m_modelName.c_str());
		times++;
	}
	clearVar();
	printf("<======= threadSimpoleExecute : end ======>");
	//storeModelVar((char*)pCustomDlStep->m_modelName.c_str());
}

void *threadCustomStepExecute(void *pParam)
{
	printf("[%s:%s:%d]--> wc customdl->threadthis:%x\n", __FILE__, __FUNCTION__, __LINE__,pParam);

	CCustomDlStep* pCustomDlStep = (CCustomDlStep*)pParam;
	pCustomDlStep->dlDoInitialization((char*)(pCustomDlStep->m_dlName.c_str()));

	int doRet = 0;
	//pCustomDlStep->m_times = 0;
	sem_init(& pCustomDlStep->m_sem, 0, 0);
	pCustomDlStep->m_sem.__align = 0;
	int tickNUm = 0;
	long long times = 0;
	//printf("--------------------customDlAddr:%p, pCustomDlStep->m_statusFlag:%d\n", pCustomDlStep,pCustomDlStep->m_statusFlag);
	while((pCustomDlStep->m_statusFlag != STEP_STOP) && (pCustomDlStep->m_statusFlag != STEP_NULL)
			&& (pCustomDlStep->m_statusFlag != STEP_INIT))
	{

		if(pCustomDlStep->m_statusFlag == STEP_PAUSE)
		{
			usleep(1000);
			continue;
		}
		//printf("[%s:%s:%d]--> 2pCustomDlStep->m_sem:%d, semAddr:%x, pmodelAddr:%x\n", __FILE__, __FUNCTION__, __LINE__,pCustomDlStep->m_sem,&pCustomDlStep->m_sem,pCustomDlStep);

		sem_wait(&pCustomDlStep->m_sem);
		tickNUm++;

		//REPORT(REPORT_ERROR, "=========ZR===stepValue:%d,tickNUm:%d\n",pCustomDlStep->m_stepValue,tickNUm);

		if(MANUAL_TEST_FLAG == 1)
		{
			if(tickNUm == 1)
			{

				doRet = pCustomDlStep->dlDoOneStep(times++);
				if(doRet == 0)
				{
					tickNUm = 0;
				}
				else if(doRet == 1)
				{
					pCustomDlStep->m_statusFlag = STEP_FAILED;
					REPORT(REPORT_INFO, "==================model stop return:%d====================\n", doRet);
				}
				else if(doRet == 2)
				{
					pCustomDlStep->m_statusFlag = STEP_STOP;
					REPORT(REPORT_INFO, "==================model stop return:%d====================\n", doRet);
				}
				//times = times++;
			}
		}
		else if(MANUAL_TEST_FLAG == 0)
		{
			if(tickNUm == pCustomDlStep->m_stepValue)
			{

				doRet = pCustomDlStep->dlDoOneStep(times++);
				if(doRet == 0)
				{
					tickNUm = 0;
				}
				else if(doRet == 1)
				{
					pCustomDlStep->m_statusFlag = STEP_FAILED;
					REPORT(REPORT_INFO, "==================model stop return:%d====================\n", doRet);
				}
				else if(doRet == 2)
				{
					pCustomDlStep->m_statusFlag = STEP_STOP;
					REPORT(REPORT_INFO, "==================model stop return:%d====================\n", doRet);
				}
				//times = times++;
			}
			/*
			   doRet = checkCurveStatus();
			   if(doRet == 0)
			   {
			   REPORT(REPORT_INFO, "==================curve time out====================\n");
			   pCustomDlStep->m_statusFlag = STEP_STOP;
			   }
			   */
		}
		if(pCustomDlStep->m_times <= (times*pCustomDlStep->m_periodMs)/1000)
		{
			REPORT(REPORT_INFO, "==================model time out set:%d real:%d====================\n",pCustomDlStep->m_times, (times*pCustomDlStep->m_periodMs)/1000);
			pCustomDlStep->m_statusFlag = STEP_STOP;
		}

		char msg[1024];
		pCustomDlStep->dlGetSubStepAttachInfo(msg);
		pCustomDlStep->m_attachInfo = string(msg);
		//printf("--------------------customDlAddr:%p, \n", pCustomDlStep);
		// printf("%s-----%s", pCustomDlStep->m_subStepID.c_str(), pCustomDlStep->m_attachInfo.c_str());
	}

	printf("<======= threadCustomStepExecute : end ======>");
	pCustomDlStep->dlDoDispose();
}
int CCustomDlStep::initLibrary()
{
	//load library>>

	char pathBuff[256] = {0};
	getServerPath(pathBuff);
	string dlPath(pathBuff);
	dlPath = dlPath + "../../lib/" + m_dlName;
	dlHandle = dlopen((char *)dlPath.c_str(), RTLD_NOW );//RTLD_LAZY
	if(dlHandle == NULL)
	{
		char errMsg[256];
		sprintf(errMsg, "Load %s failed %s.\n", m_dlName.c_str(), dlerror());
		
#ifdef AUTO_LOG

		STEPLOG(LOG_ERROR, errMsg);
#endif
		REPORT(REPORT_ERROR, errMsg);
		return -1;
	}

	dlDoInitialization = (dlDoInitializationTYPE)dlsym(dlHandle, "autoTestDoInitialization");
	if (dlDoInitialization == NULL)
	{
		REPORT(REPORT_ERROR, "Undefined reference autoTestDoInitialization.%s\n", dlerror());
		return -1;
	}
	printf("[%s:%s:%d]--> ------init \n", __FILE__, __FUNCTION__, __LINE__);
	dlDoOneStep = (dlDoOneStepTYPE)dlsym(dlHandle, "autoTestDoOneStep");
	if (dlDoOneStep == NULL)
	{
		REPORT(REPORT_ERROR, "Undefined reference autoTestDoOneStep.\n");
		return -1;
	}
	dlDoDispose = (dlDoDisposeTYPE)dlsym(dlHandle, "autoTestDoDispose");
	if (dlDoOneStep == NULL)
	{
		REPORT(REPORT_ERROR, "Undefined reference autoTestDoDispose.\n");
		return -1;
	}

	dlSetSubStepID = (dlSetSubStepIDTYPE)dlsym(dlHandle, "autoTestSetSubStepID");
	if (dlSetSubStepID == NULL)
	{
		REPORT(REPORT_ERROR, "Undefined reference autoTestSetSubStepID.\n");
		return -1;
	}


	dlGetSubStepAttachInfo = (dlGetSubStepAttachInfoTYPE)dlsym(dlHandle, "autoTestGetSubStepAttachInfo");
	if (dlGetSubStepAttachInfo == NULL)
	{
		REPORT(REPORT_ERROR, "Undefined reference getSubStepAttachInfo.\n");
		return -1;
	}

	return 0;
}
int CCustomDlStep::initICD()
{
	int ret = 0;
	for(unsigned int i = 0; i < m_subStepInfo.inputVarInfo.size(); i++)
	{
		//only double>>
		//TODO:type 
		unsigned long long data = 0;
		if(m_subStepInfo.inputVarInfo[i].strBindType == "ICD")
		{
			memcpy(&data, m_subStepInfo.inputVarInfo[i].value.value, 8);
			ret = m_subStepInfo.inputVarInfo[i].ICDInfo.setData(data);
			if(ret != 0)
			{
				REPORT(REPORT_ERROR, "set failed\n");
				ret = -1;
			}
		}

	}
	return 0;
}

int CCustomDlStep::init()
{
	int ret = 0;
	printf("[%s:%s:%d]-->   goto init->m_modelSourceFlag:%d\n", __FILE__, __FUNCTION__, __LINE__,m_modelSourceFlag);

	if(m_modelSourceFlag == 1)
	{
		m_statusFlag = STEP_INIT;
		return 0;
	}
	ret = initLibrary();
	if(ret != 0)
	{
		m_statusFlag = STEP_FAILED;
		return -1;
	}
	else
	{
		dlSetSubStepID((char*)m_subStepID.c_str());
		ret = initICD();
		if(ret != 0)
		{
			//////////////ret = -1;
			////////////m_statusFlag = STEP_FAILED;
		}
	}

	m_statusFlag = STEP_INIT;
	return 0;

}

int CCustomDlStep::simpoleRun()
{

	if(m_statusFlag == STEP_FAILED)
	{
		REPORT(REPORT_ERROR, "Init subStep failed. Can not run.\n");
		return -1;
	}
	//m_statusFlag = STEP_INIT;
	m_statusFlag = STEP_RUN;
	int threadStatus = pthread_create(&m_simpoledataThreadId, 
			NULL, threadSimpoleExecute, this);
	if (0 != threadStatus)
	{
		REPORT(REPORT_ERROR, "pthread_create failed.\n");
		return -1;
	}


	return 0;
}

int CCustomDlStep::run()
{
	if(m_statusFlag == STEP_FAILED)
	{
		REPORT(REPORT_ERROR, "Init subStep failed. Can not run.\n");
		return -1;
	}
	//m_statusFlag = STEP_INIT;
	m_statusFlag = STEP_RUN; //modify 20231029
	//printf("[%s:%s:%d]--> wc customdl->this:%x\n", __FILE__, __FUNCTION__, __LINE__,this);
	int threadStatus = pthread_create(&m_customExecuteThreadId, 
			NULL, threadCustomStepExecute, this);
	if (0 != threadStatus)
	{
		REPORT(REPORT_ERROR, "pthread_create failed.\n");
		return -1;
	}


	//REPORT(REPORT_INFO, "-------------wc4-----------------m_customExecuteThreadId: %ld, add:%p\n",m_customExecuteThreadId,this);
	// m_statusFlag = STEP_RUN;
	return 0;
}
int CCustomDlStep::stop()
{
	int ret = -1;
	m_statusFlag = STEP_STOP;
	if(m_statusFlag == STEP_STOP)
	{
		//REPORT(REPORT_INFO, "-------------wc1------------------stopModelsuccess!--, this:%p,m_customExecuteThreadId:%ld \n",
		//this,m_customExecuteThreadId);
		if(m_modelSourceFlag == 1)
		{
			if(m_simpoledataThreadId != NULL)
			{
				pthread_join(m_simpoledataThreadId, NULL);
				m_simpoledataThreadId = NULL;
				return 0;
			}
		}
		else
		{
			if(m_customExecuteThreadId != NULL )
			{
				//printf("[%s:%s:%d]--> pthreadjion-->m_customExecuteThreadId:%ld\n", __FILE__, __FUNCTION__, __LINE__,m_customExecuteThreadId);
				pthread_join(m_customExecuteThreadId, NULL);//modify by ZR 20230614
				m_customExecuteThreadId = NULL;

			}

			if(dlHandle != NULL)
			{
				dlclose(dlHandle );
				dlHandle = NULL;
			}
			ret = 0;
		}

	}
	//REPORT(REPORT_INFO, "-------------wc3------------------stopModelsuccess!--\n");
	return ret;
}
int CCustomDlStep::pause()
{
	m_statusFlag = STEP_PAUSE;
	return 0;
}

int CCustomDlStep::getStatus()
{
	return m_statusFlag;
}

int CCustomDlStep::setStatus(int status)
{
	m_statusFlag = status;
	return 0;
}

string CCustomDlStep::getAttachInfo()
{
	return m_attachInfo;
}
