/*
 * CCurve.cpp
 *
 *  Created on: 2022-4-15
 *      Author: root
 */

#include "CCurve.h"
#include "Constant.h"
#include "CustomWave.h"
#include "SawtoothWave.h"
#include "SineWave.h"
#include "SlopeWave.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "HRTime.h"
#include "DataManager.h"
#include "HiLog.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "AutoTestInterface.h"
#include "HiReport.h"
#include "HiCommon.h"
#include "parse_xml_api.h"
#include "ParamData.h"
#include "HiMGlobalVarLib.h"
CBaseCurve::CBaseCurve()
{

}
CBaseCurve::CBaseCurve(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem)
{
	loadCurveConfig((char*)pXmlElem);
	m_runFlag = CURVE_STOP;
	m_runTime = 0;
	m_outputValue = 0;
	m_curveType = SINE_WAVE;
	m_amplitude    = param->amplitude;
	m_duty         = param->duty;
	m_excInteral   = param->excInteral;
	m_excTime      = param->excTime;
	m_period       = param->period;
	m_phase        = param->phase;

	m_startTime    = param->startTime;
	m_valueSkewing = param->valueSkewing;
//	m_slope        = param->slope;
}
CBaseCurve::~CBaseCurve()
{

}

int CBaseCurve::loadCurveConfig(const char* xmlStr)
{
	string tmp;
	TY_ICD_INFO icdinfo;
	GetValue((TiXmlElement*)xmlStr, "CurveID", m_curveID);
	for(TiXmlElement* icdElement = ((TiXmlElement*)xmlStr)->FirstChildElement(); icdElement != NULL; icdElement = icdElement->NextSiblingElement())
	{
		GetValue((TiXmlElement*)icdElement, "ICDID", icdinfo.icdID);
		GetValue((TiXmlElement*)icdElement, "FieldID", icdinfo.fieldID);
		GetValue((TiXmlElement*)icdElement, "CardType",icdinfo.cardType);
		GetValue((TiXmlElement*)icdElement, "CardID", tmp);
		icdinfo.cardID = atoi(tmp.c_str());
		GetValue((TiXmlElement*)icdElement, "ChnID", tmp);
		icdinfo.chnID = atoi(tmp.c_str());
		m_vecIcdinfo.push_back(icdinfo);
	}


	return 0;
}

int CBaseCurve::createDataPoints()
{


	return 0;
}

void *threadCurveExecute(void* param)
{
	CBaseCurve* pCurve = (CBaseCurve*)param;
   // while((pCurve->m_runFlag != CURVE_STOP) && (pCurve->m_runFlag != CURVE_ERROR) && (pCurve->m_runFlag != CURVE_INIT))
	while(pCurve->m_runFlag == CURVE_RUNNING)
	{
     /*   if(pCurve->m_runFlag == CURVE_PAUSE)
        {
            usleep(1000);
            continue;
        }
*/
        sem_wait(&(pCurve->m_sem));
        pCurve->run();
    }
    //pCurve->m_statusFlag = STEP_STOP;
    printf("<======= threadCurveExecute : end ======>\n");
	REPORT(REPORT_DEBUG, "\n ===========================curve close success%s=============================== \n", pCurve->m_curveID.c_str());
    //pCurve->dlDoDispose();
}

int CBaseCurve::start()
{
	m_runFlag = CURVE_RUNNING;
	sem_init(&m_sem, 0, 0);
	m_sem.__align = 0;
	m_runTime = 0;
	int threadStatus = pthread_create(&m_curveThreadId, NULL, threadCurveExecute, this);
	if (0 != threadStatus)
	{
		REPORT(REPORT_ERROR, "curve pthread_create failed.\n");
		return -1;
	}
	return 0;
}

int CBaseCurve::stop()
{

	m_runFlag = CURVE_STOP;
	m_runTime = 0;
	m_outputValue = 0;
	REPORT(REPORT_DEBUG, "\n ===========================close curve%s=============================== \n", m_curveID.c_str());
	//output();
	return 0;
}

int CBaseCurve::pause()
{
	m_runFlag = CURVE_PAUSE;
	return 0;
}
int CBaseCurve::getCurveStatus(CURVE_RUNING_STATUS *status)
{
	*status = m_runFlag;
	return 0;
}
int CBaseCurve::setStepValue(int stepValue)
{
	m_stepValue = stepValue;
    //REPORT(REPORT_INFO, "Step run m_stepStatus %d\n", m_stepStatus);
    return 0;
}


int CBaseCurve::run()
{

	return 0;
}
int CBaseCurve::runNoOutput()
{

	return 0;
}

void CBaseCurve::toString()
{
	printf("m_curveType:%d\n", m_curveType);
	printf("m_startTime:%d\n", m_startTime);
	printf("m_excTime:%d\n", m_excTime);
	printf("m_startTime:%d\n", m_startTime);
	printf("m_amplitude:%d\n", m_amplitude);
	printf("m_valueSkewing:%d\n", m_valueSkewing);
	printf("m_excInteral:%d\n", m_excInteral);
	printf("m_period:%d\n", m_period);

}
int CBaseCurve::output()
{
	CObjectData *pAutoTest = CObjectData::getInstance();
	TY_VALUEFIELD value;
	value.dValue = m_outputValue;
	if(pAutoTest == NULL)
	{
		REPORT(REPORT_ERROR, "Get CObjectData::getInstance() failed\n");
		return -1;
	}
	vector<TY_ICD_INFO>::iterator iter = m_vecIcdinfo.begin();

	for(; iter != m_vecIcdinfo.end(); iter++)
	{
		//printf("type:%s fieldID:%s value:%lf\n", m_curveID.c_str(), iter->fieldID.c_str(), m_outputValue);
		if(pAutoTest->setICDData(iter->icdID, iter->fieldID, value) != 0)
		{
			REPORT(REPORT_ERROR, "setICDData failed\n");
			return -1;
		}
		if(pAutoTest->sendICDChnData(iter->icdID, iter->cardType, iter->cardID) != 0)
		{
			REPORT(REPORT_ERROR, " sendICDChnData failed\n");
			return -1;
		}
	}
	return 0;

}

CCurveManager::CCurveManager()
{

}
CCurveManager::~CCurveManager()
{

}
/**************************************************************
 *Function name: start
*Description:	锟斤拷锟斤拷锟斤拷锟竭匡拷始锟斤拷锟�
*Parameters:	curveInfo : 锟斤拷锟斤拷锟斤拷息
curveNum 锟斤拷 锟斤拷锟斤拷锟斤拷锟斤拷
*Return:		0 - success other-failed
***************************************************************/
/*
int CCurveManager::start(TY_CURVE_INFO* curveInfo, int curveNum)
{
	if(curveInfo == NULL)
	{
		return -1;
	}
	for(int i =0; i<curveNum; i++)
	{
		string curveName;
		curveName = curveInfo[i].curveName;
		TY_CURVE_MAP::iterator iter = m_curveMap.find(curveName);
		if(iter != m_curveMap.end())
		{
			iter->second->m_runFlag = CURVE_RUNNING;
		}
	}

	return 0;
}
*/
/**************************************************************
 *Function name: stop
*Description:	锟斤拷锟斤拷锟斤拷锟斤拷停止锟斤拷锟�
*Parameters:	curveInfo : 锟斤拷锟斤拷锟斤拷息
curveNum 锟斤拷 锟斤拷锟斤拷锟斤拷锟斤拷
*Return:		0 - success other-failed
***************************************************************/
/*
int CCurveManager::stop(TY_CURVE_INFO* curveInfo, int curveNum)
{
	if(curveInfo == NULL)
	{
		return -1;
	}
	for(int i =0; i<curveNum; i++)
	{
		string curveName;
		curveName = curveInfo[i].curveName;

		TY_CURVE_MAP::iterator iter = m_curveMap.find(curveName);
		if(iter != m_curveMap.end())
		{
			iter->second->m_runFlag = CURVE_STOP;
		}
	}

	return 0;
}
*/
/**************************************************************
 *Function name: pause
*Description:	锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷停锟斤拷锟�
*Parameters:	curveInfo : 锟斤拷锟斤拷锟斤拷息
curveNum 锟斤拷 锟斤拷锟斤拷锟斤拷锟斤拷
*Return:		0 - success other-failed
***************************************************************/
/*
int CCurveManager::pause(TY_CURVE_INFO* curveInfo, int curveNum)
{
	if(curveInfo == NULL)
	{
		return -1;
	}
	for(int i =0; i<curveNum; i++)
	{
		string curveName;
		curveName = curveInfo[i].curveName;

		TY_CURVE_MAP::iterator iter = m_curveMap.find(curveName);
		if(iter != m_curveMap.end())
		{
			iter->second->m_runFlag = CURVE_PAUSE;
		}
	}
	return 0;
}
*/
/**************************************************************
 *Function name: run
*Description:	锟斤拷锟斤拷锟斤拷锟叫ｏ拷锟斤拷锟狡革拷锟斤拷锟斤拷锟竭硷拷锟斤拷锟斤拷锟街碉拷锟斤拷锟斤拷锟斤拷
*Parameters:	curveInfo : 锟斤拷锟斤拷锟斤拷息
curveNum 锟斤拷 锟斤拷锟斤拷锟斤拷锟斤拷
*Return:		0 - success other-failed
***************************************************************/
/*
int CCurveManager::run()
{
	TY_CURVE_MAP::iterator iter = m_curveMap.begin();
	for(; iter != m_curveMap.end(); iter++)
	{
		iter->second->run();
	}
	return 0;
}
*/
/**************************************************************
 *Function name: getCurveList
*Description:	锟斤拷取锟斤拷锟斤拷锟叫憋拷
*Parameters:	curveInfo : 锟斤拷锟斤拷锟斤拷息
curveNum 锟斤拷 锟斤拷锟斤拷锟斤拷锟斤拷
*Return:		0 - success other-failed
***************************************************************/
int CCurveManager::getCurveList(TY_CURVE_INFO *curveInfo, int curveNum)
{

	return 0;
}

/**************************************************************
 *Function name: getCurveStatus
*Description:	锟斤拷取锟斤拷锟斤拷状态
*Parameters:	curveInfo : 锟斤拷锟斤拷状态
curveNum 锟斤拷 锟斤拷锟斤拷锟斤拷锟斤拷
*Return:		0 - success other-failed
***************************************************************/
int CCurveManager::getCurveStatus(TY_CURVE_STATUS *curveInfo, int curveNum)
{
	return 0;
}
int CCurveManager::checkCurveStatus()
{
	CObjectData *pAutoTest = CObjectData::getInstance();

	char stepId[128] = {0};
	getPresentStep(stepId);
	string presentStepID = stepId;

	TY_CURVE_BIND_MAP::iterator iterBind = m_curveBindMap.find(presentStepID);
	if(iterBind != m_curveBindMap.end())
	{
		CURVE_RUNING_STATUS status;
		TY_CURVE_MAP::iterator iterCurve = iterBind->second.begin();
		for( ; iterCurve != iterBind->second.end(); iterCurve++)
		{
			iterCurve->second->getCurveStatus(&status);
			if(status != CURVE_STOP)
			{
				return (int)status;
			}
		}
	}
	return 0;
}

int CCurveManager::getCurveData(char *pCurveID, double *value)
{
	string strCurveID(pCurveID);
	CObjectData *pAutoTest = CObjectData::getInstance();
	TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	TY_CURVE_BIND_MAP::iterator iterBind = m_curveBindMap.find(iterCase->second.m_presentStepId);
	if(iterBind != m_curveBindMap.end())
	{
		TY_CURVE_MAP::iterator iterCurve = iterBind->second.find(strCurveID);
		if(iterCurve != iterBind->second.end())
		{
			(*value) = (iterCurve->second)->m_outputValue;
		}

	}

	return 0;
}

int CCurveManager::updateCurveInputData(char *modelName)
{
	char buff[64] = {0};
	sprintf(buff, "%s%s%s", "lib", modelName, ".so");
	string dllName(buff);
	string strModelName(modelName);
	int ret = 0;

	CObjectData *pAutoTest = CObjectData::getInstance();
	TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	TY_STEP_MAP::iterator iterStep = iterCase->second.m_StepMap.find(iterCase->second.m_presentStepId);

	if(iterStep != iterCase->second.m_StepMap.end())
	{
	//	printf("[%s:%s:%d]--> ret :%d,var.strBindType:%s\n", __FILE__, __FUNCTION__, __LINE__, ret,dllName.c_str());
		TY_INPUT_MAP::iterator iterInput = iterStep->second.inputMap.find(dllName);
		if(iterInput != iterStep->second.inputMap.end())
		{
			
			TY_INPUT_VAR_MAP::iterator iter = iterInput->second.begin();
			while(iter != iterInput->second.end())
			{
				TY_INPUT_VAR *var = &iter->second;
				double value = 0;

				if(var->strBindType == "Curve")
				{
					ret = getCurveData((char*)var->strCurveID.c_str(), &value);
#if 0
					printf("VarName:%s, value:%lf\n",var->strReferenced.c_str(), value);
#endif
                    if(ret == 0)
    				{
    				    var->value.dValue = value;
    					setVarDataByName(var->strReferenced.c_str(), (char*)&value);
    				}
				}
				

				iter++;
			}

		}
	}

}

CCurveManager * CCurveManager::instance = NULL;
/**************************************************************
 *Function name: getInstance
*Description:	锟斤拷CCurveManager
*Return:		CCurveManager - success NULL-failed
***************************************************************/
CCurveManager * CCurveManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new CCurveManager();
	}
	return instance;
}
/*
void printfParam(TY_CURVE_PARAM param)
{
	printf("param->curveType:%s\n", param.curveType);
	printf("param->startTime:%d\n", param.startTime);
	printf("param.excTime:%d\n", param.excTime);
	printf("param.startTime:%d\n", param.startTime);
	printf("param.amplitude:%d\n", param.amplitude);
	printf("param.valueSkewing:%d\n",param.valueSkewing);
	printf("param.excInteral:%d\n", param.excInteral);
	printf("param.period:%d\n", param.period);
}
*/
/**************************************************************
 *Function name: loadConfig
*Description:	锟斤拷锟斤拷xml
*Parameters:	xmlStr : xml锟侥硷拷
*Return:		0 - success other-failed
***************************************************************/

int CCurveManager::loadConfig(const char* xmlStr)
{
	int ret = 0;
	TiXmlDocument * pDoc = NULL;

	TY_CURVE_ATTRIBUTE_MAP tmpCurveAttributeMap;
	TY_ATTRIBUTE_CURVE tmpAttributeCurve;

	CCurveManager *pManager;
	pManager = CCurveManager::getInstance();
	CObjectData *pAutoTest = CObjectData::getInstance();

	/* load xml file */
	pManager->m_curveBindMap.clear();
	pManager->m_paramMap.clear();
	pAutoTest->m_curveInfoMap.clear();


	pDoc = load_xml(xmlStr, FROM_DATA);//FROM_FILE FROM_DATA
	if (pDoc == NULL)
	{
		REPORT(REPORT_ERROR, "load xml faild\n");
		return -1;
	}
	TiXmlElement* rootElement = pDoc->RootElement();

	TiXmlElement* pCurveList = rootElement->FirstChildElement("curveList");
	TiXmlElement* pCurveBind = rootElement->FirstChildElement("curveBind");
	for(TiXmlElement* pCurveXmlElem = pCurveList->FirstChildElement(); pCurveXmlElem != NULL; pCurveXmlElem = pCurveXmlElem->NextSiblingElement())
	{

		TY_CURVE_PARAM *pCurveParam = NULL;
		string curveID;
		string tmp;
		ret = GetValue(pCurveXmlElem, "CurveID", curveID);
		if(ret == false)
		{
			return -1;
		}

		ret = GetValue(pCurveXmlElem, "Type", tmp);
		if(ret == false)
		{
			return -1;
		}

		if(tmp == "CustomWave")
		{
			int subCurveNum = 0;
			for(TiXmlElement* pSubCurveElem = pCurveXmlElem->FirstChildElement(); pSubCurveElem != NULL; pSubCurveElem = pSubCurveElem->NextSiblingElement())
			{
				subCurveNum++;
			}
			pCurveParam = new TY_CURVE_PARAM[subCurveNum + 1];
			strcpy((*pCurveParam).curveType, tmp.c_str());

			(*pCurveParam).subCurveNum = subCurveNum;
			subCurveNum = 0;
			for(TiXmlElement* pSubCurveElem = pCurveXmlElem->FirstChildElement(); pSubCurveElem != NULL;
					pSubCurveElem = pSubCurveElem->NextSiblingElement(), subCurveNum++)
			{
				ret = GetValue(pSubCurveElem, "Type", tmp);
				if(ret == false)
				{
					return -1;
				}
				strcpy(pCurveParam[subCurveNum+1].curveType, tmp.c_str());

				GetValue(pSubCurveElem, "StartTime", tmp);
				pCurveParam[subCurveNum+1].startTime = atoi(tmp.c_str());

				GetValue(pSubCurveElem, "ExcTime", tmp);
				pCurveParam[subCurveNum+1].excTime = atoi(tmp.c_str());

				GetValue(pSubCurveElem, "Amplitude", tmp);
				pCurveParam[subCurveNum+1].amplitude = atof(tmp.c_str());

				GetValue(pSubCurveElem, "Skewing", tmp);
				pCurveParam[subCurveNum+1].valueSkewing = atof(tmp.c_str());

				GetValue(pSubCurveElem, "ExcInteral", tmp);
				pCurveParam[subCurveNum+1].excInteral = atoi(tmp.c_str());

				GetValue(pSubCurveElem, "Period", tmp);
				pCurveParam[subCurveNum+1].period = atoi(tmp.c_str());

				GetValue(pSubCurveElem, "Phase", tmp);
				pCurveParam[subCurveNum+1].phase = atoi(tmp.c_str());

				GetValue(pSubCurveElem, "Duty", tmp);
				pCurveParam[subCurveNum+1].duty = (double)atoi(tmp.c_str()) / 100;
			}
		}
		else
		{
			pCurveParam = new TY_CURVE_PARAM;
			strcpy((*pCurveParam).curveType, tmp.c_str());
		}


		GetValue(pCurveXmlElem, "StartTime", tmp);
		(*pCurveParam).startTime = atoi(tmp.c_str());

		GetValue(pCurveXmlElem, "ExcTime", tmp);
		(*pCurveParam).excTime = atoi(tmp.c_str());

		GetValue(pCurveXmlElem, "Amplitude", tmp);
		(*pCurveParam).amplitude = atof(tmp.c_str());

		GetValue(pCurveXmlElem, "Skewing", tmp);
		(*pCurveParam).valueSkewing = atof(tmp.c_str());

		GetValue(pCurveXmlElem, "ExcInteral", tmp);
		(*pCurveParam).excInteral = atoi(tmp.c_str());

		GetValue(pCurveXmlElem, "Period", tmp);
		(*pCurveParam).period = atoi(tmp.c_str());

		GetValue(pCurveXmlElem, "Phase", tmp);
		(*pCurveParam).phase = atoi(tmp.c_str());

		GetValue(pCurveXmlElem, "Duty", tmp);
		(*pCurveParam).duty = atof(tmp.c_str()) / 100;

		tmpAttributeCurve.m_excInteral = (*pCurveParam).excInteral;

		tmpCurveAttributeMap.insert(pair<string, TY_ATTRIBUTE_CURVE>(curveID, tmpAttributeCurve));
		m_paramMap.insert(pair<string, TY_CURVE_PARAM*>(curveID, pCurveParam));
		/*CBaseCurve *pCurve = CCurveFactory::createCurve(curveType, pCurveXmlElem);
		if(pCurve != NULL)
		{
			m_curveMap.insert(pair<string, CBaseCurve*>(curveID, pCurve));
		}*/
	}

	for(TiXmlElement* pStepXmlElem = pCurveBind->FirstChildElement(); pStepXmlElem != NULL; pStepXmlElem = pStepXmlElem->NextSiblingElement())
	{
		string stepID;
		ret = GetValue(pStepXmlElem, "StepID", stepID);
		if(ret == true)
		{
			TY_CURVE_MAP curveList;
			for(TiXmlElement* pCurveXmlElem = pStepXmlElem->FirstChildElement(); pCurveXmlElem != NULL; pCurveXmlElem = pCurveXmlElem->NextSiblingElement())
			{
				string curveID;

				ret = GetValue(pCurveXmlElem, "CurveID", curveID);
				if(ret == true)
				{
					TY_CURVE_PARAM_MAP::iterator iter = m_paramMap.find(curveID);
					if(iter != m_paramMap.end())
					{
						TY_CURVE_PARAM curveParam;
						curveParam = *(iter->second);
//						printfParam(curveParam);
						CBaseCurve *pCurve = CCurveFactory::createCurve(iter->second, pCurveXmlElem);
						if(pCurve != NULL)
						{
							curveList.insert(pair<string, CBaseCurve*>(curveID, pCurve));
						}

					}
				}
			}
			pAutoTest->m_curveInfoMap.insert(make_pair(stepID, tmpCurveAttributeMap));
			m_curveBindMap.insert(make_pair(stepID, curveList));
		}
	}
	return 0;

}

CBaseCurve* CCurveFactory::createCurve(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem)
{
	CBaseCurve *pCurve = NULL;
	//printf("param.curveType:%s\n",param->curveType);
	if(strcmp(param->curveType, "Constant") == 0)
	{

		pCurve = new CConstant(param, pXmlElem);
		pCurve->toString();
	}
	else if(strcmp(param->curveType,  "CustomWave") == 0)
	{

		pCurve = new CCustomWave(param, pXmlElem);
		pCurve->toString();
	}
	else if(strcmp(param->curveType,  "SawtoothWave") == 0)
	{

		pCurve = new CSawtoothWave(param, pXmlElem);
		pCurve->toString();
	}
	else if(strcmp(param->curveType,  "SineWave") == 0)
	{

		pCurve = new CSineWave(param, pXmlElem);
		pCurve->toString();
	}
	else if(strcmp(param->curveType,  "SlopeWave") == 0)
	{

		pCurve = new CSlopeWave(param, pXmlElem);
		pCurve->toString();
	}
	else if(strcmp(param->curveType,  "SquareWave") == 0)
	{

		pCurve = new CSquareWave(param, pXmlElem);
		pCurve->toString();
	}
	else if(strcmp(param->curveType,  "TriangleWave") == 0)
	{

		pCurve = new CTriangleWave(param, pXmlElem);
		pCurve->toString();
	}
	else
	{
		REPORT(REPORT_ERROR, "curve type input error : %s\n", param->curveType);
	}

	return pCurve;
}



