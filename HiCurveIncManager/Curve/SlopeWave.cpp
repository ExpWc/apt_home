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

CSlopeWave::CSlopeWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem):CBaseCurve(param,pXmlElem)
{
	m_curveType = SLOPE_WAVE;
	m_pDataBuffer = new(double);
	createDataPoints();
}
CSlopeWave::~CSlopeWave()
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
int CSlopeWave::loadCurveConfig(const char* xmlStr)
{
	string tmp;

	GetValue((TiXmlElement*)xmlStr, "CurveID", m_curveID);
	GetValue((TiXmlElement*)xmlStr, "ICDID", m_icdID);
	GetValue((TiXmlElement*)xmlStr, "FieldID", m_fieldID);
	GetValue((TiXmlElement*)xmlStr, "CardType", m_cardType);

	GetValue((TiXmlElement*)xmlStr, "CardId", tmp);
	m_cardID = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "StartTime", tmp);
	startTime = atoi(tmp.c_str());

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

	GetValue((TiXmlElement*)xmlStr, "Phase", tmp);
	m_phase = atoi(tmp.c_str());
	return 0;
}
*/
int CSlopeWave::createDataPoints()
{
	m_slope = m_amplitude / m_excTime;

	return 0;
}

/**************************************************************
 *Function name: run
*Description:	���м���������
*Parameters:	NULL
*Return:		0 - success other-failed
***************************************************************/
int CSlopeWave::run()
{
	if(m_runFlag == CURVE_RUNNING)
	{
		if(m_runTime >= m_startTime)
		{
			if(m_runTime >= (m_excTime + m_startTime))
			{
				stop();
				return 0;
			}
			if(m_runTime >= m_startTime)
			{
				if(m_runTime % m_excInteral == 0)
				{
					m_outputValue = m_slope * (m_runTime - m_phase - m_startTime) + m_valueSkewing;
				}
			}
			output();
		}
		m_runTime = m_runTime + m_stepValue;


	}
	return 0;
}
int CSlopeWave::runNoOutput()
{
	if(m_runFlag == CURVE_RUNNING)
	{
		if(m_runTime >= m_startTime)
		{
			if(m_runTime >= (m_excTime + m_startTime))
			{
				stop();
				return 0;
			}
			if(m_runTime >= m_startTime)
			{
				if(m_runTime % m_excInteral == 0)
				{
					m_outputValue = m_slope * (m_runTime - m_phase - m_startTime) + m_valueSkewing;
				}
			}
		}
		m_runTime = m_runTime + m_stepValue;
	}
	return 0;
}
