/*
 * CustomWave.cpp
 *
 *  Created on: 2022-4-15
 *      Author: root
 */
#include "CustomWave.h"
#include "SlopeWave.h"
#include "HRTime.h"
#include "DataManager.h"
#include "HiLog.h"
#include "AutoTestInterface.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "AutoTestInterface.h"
#include "HiReport.h"
#include "HiCommon.h"
#include "parse_xml_api.h"
CCustomWave::CCustomWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem):CBaseCurve(param,pXmlElem)
{
	m_curveType = CUSTOMW_WAVE;
	CBaseCurve *pCurve = NULL;
	for(int num = 0; num < param->subCurveNum; num++)
	{
		pCurve = CCurveFactory::createCurve(&param[num+1], pXmlElem);
		if(pCurve != NULL)
		{
			m_subCurveVec.push_back(pCurve);
		}
	}

	createDataPoints();
}
CCustomWave::~CCustomWave()
{

	delete m_pDataBuffer;
}

/**************************************************************
 *Function name: loadCurveConfig
*Description:	���������ļ�
*Parameters:	xml�ļ�
*Return:		0 - success other-failed
***************************************************************/
/*
int CCustomWave::loadCurveConfig(const char* xmlStr)
{

	string tmp;

	GetValue((TiXmlElement*)xmlStr, "CurveID", m_curveID);
	GetValue((TiXmlElement*)xmlStr, "ICDID", m_icdID);
	GetValue((TiXmlElement*)xmlStr, "FieldID", m_fieldID);
	GetValue((TiXmlElement*)xmlStr, "CardType", m_cardType);
	GetValue((TiXmlElement*)xmlStr, "CardId", m_cardID);
	m_cardID = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "StartTime", tmp);
	m_startTime = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "ExcTime", tmp);
	m_excTime = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "Amplitude", tmp);
	m_amplitude = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "Skewing", tmp);
	m_valueSkewing = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "ExcInteral", tmp);
	m_excInteral = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "Period", tmp);
	m_period = atoi(tmp.c_str());

	for(TiXmlElement* pSubCurve = ((TiXmlElement*)xmlStr)->FirstChildElement(); pSubCurve != NULL; pSubCurve =((TiXmlElement*)xmlStr)->NextSiblingElement())
	{
		string curveName;
		int ret = GetValue(pSubCurve, "Name", curveName);
		if(ret == false)
		{
			return -1;
		}

		string curveType;
		ret = GetValue(pSubCurve, "Type", curveType);
		if(ret == false)
		{
			return -1;
		}
		//CBaseCurve *pCurve = CCurveFactory::createCurve(curveType, pSubCurve);

		if(pCurve != NULL)
		{
			m_subCurveMap.insert(pair<string, CBaseCurve*>(curveName, pCurve));
		}

	}

	return 0;
}
*/
int CCustomWave::createDataPoints()
{
	m_dataLen = m_period / m_excInteral ;
	m_pDataBuffer = new double[m_dataLen];
	memset(m_pDataBuffer, m_dataLen, 0);

	TY_CURVE_VECTOR::iterator iter = m_subCurveVec.begin();
	for(; iter != m_subCurveVec.end(); iter++)
	{
		(*iter)->m_runFlag = CURVE_RUNNING;
		(*iter)->m_stepValue = 1;
	}
	int runTime = 0;
	for(; runTime < m_period; runTime++)
	{
		double output = 0;
		for(iter = m_subCurveVec.begin(); iter != m_subCurveVec.end(); iter++)
		{
			(*iter)->runNoOutput();
			output += (*iter)->m_outputValue;
		}


		if(runTime % m_excInteral == 0)
		{
			m_pDataBuffer[runTime / m_excInteral] = output;
			//printf("curve value:%lf\n", output);
		}
	}
	for(iter = m_subCurveVec.begin(); iter != m_subCurveVec.end(); iter++)
	{
		delete (*iter);
	}

	return 0;
}

/**************************************************************
 *Function name: run
*Description:	���м���������
*Parameters:	NULL
*Return:		0 - success other-failed
***************************************************************/
int CCustomWave::run()
{
	if(m_runFlag == CURVE_RUNNING)
	{
		if(m_runTime >= m_startTime)
		{
			if(m_runTime >= (m_excTime+m_startTime))
			{
				stop();
				return 0;
			}
			if(m_runTime % m_excInteral == 0)
			{
				m_outputValue = m_pDataBuffer[((m_runTime-m_startTime + m_phase) / m_excInteral)%m_dataLen];
				//printf("curveID:%s m_sem.__align:%d runTime:%d value:%lf\n", m_curveID.c_str(), m_sem.__align,m_runTime, m_outputValue);
				output();
			}
		}
		m_runTime = m_runTime + m_stepValue;
	}
	return 0;
}

int CCustomWave::runNoOutput()
{
	if(m_runFlag == CURVE_RUNNING)
	{
		if(m_runTime >= m_startTime)
		{
			if(m_runTime >= (m_excTime+m_startTime))
			{
				stop();
				return 0;
			}
			if(m_runTime % m_excInteral == 0)
			{
				m_outputValue = m_pDataBuffer[(m_runTime-m_startTime + m_phase)%m_dataLen];
			}
		}
		m_runTime = m_runTime + m_stepValue;
	}
	return 0;
}
