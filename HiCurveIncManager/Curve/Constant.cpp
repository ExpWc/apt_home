#include "tinyxml.h"
#include "Constant.h"
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
CConstant::CConstant(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem):CBaseCurve(param,pXmlElem)
{
	m_curveType = CONSTAN;

	createDataPoints();

}
CConstant::~CConstant()
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
int CConstant::loadCurveConfig(const char* xmlStr)
{

	string tmp;

	GetValue((TiXmlElement*)xmlStr, "CurveID", m_curveID);
	GetValue((TiXmlElement*)xmlStr, "ICDID", m_icdID);
	GetValue((TiXmlElement*)xmlStr, "FieldID", m_fieldID);
	GetValue((TiXmlElement*)xmlStr, "CardType", m_cardType);
	GetValue((TiXmlElement*)xmlStr, "CardID", tmp);
	m_cardID = atoi(tmp.c_str());


	GetValue((TiXmlElement*)xmlStr, "StartTime", tmp);
	m_startTime = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "ExcTime", tmp);
	m_excTime = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "Amplitude", tmp);
	m_amplitude = atoi(tmp.c_str());


	return 0;
}
*/
int CConstant::createDataPoints()
{
	m_dataLen = 1;
	m_pDataBuffer = new(double);
	m_pDataBuffer[0] = m_amplitude;
	return 0;
}


/**************************************************************
 *Function name: run
*Description:	���м���������
*Parameters:	NULL
*Return:		0 - success other-failed
***************************************************************/
int CConstant::run()
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

			m_outputValue = *m_pDataBuffer;
			output();

		}
		m_runTime = m_runTime + m_stepValue;
	}
	return 0;
}

int CConstant::runNoOutput()
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
			m_outputValue = *m_pDataBuffer;
		}
		m_runTime = m_runTime + m_stepValue;
	}
	return 0;
}
