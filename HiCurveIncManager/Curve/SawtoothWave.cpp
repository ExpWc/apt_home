#include "SawtoothWave.h"
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

CSawtoothWave::CSawtoothWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem):CBaseCurve(param,pXmlElem)
{
	m_curveType = SAWTOOTH_WAVE;
	createDataPoints();
}
CSawtoothWave::~CSawtoothWave()
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
int CSawtoothWave::loadCurveConfig(const char* xmlStr)
{

	string tmp;

	GetValue((TiXmlElement*)xmlStr, "CurveID", m_curveID);
	GetValue((TiXmlElement*)xmlStr, "ICDID", m_icdID);
	GetValue((TiXmlElement*)xmlStr, "FieldID", m_fieldID);
	GetValue((TiXmlElement*)xmlStr, "CardType", m_cardType);

	GetValue((TiXmlElement*)xmlStr, "CardId", tmp);
	m_cardID = atoi(tmp.c_str());

	/*
	GetValue((TiXmlElement*)xmlStr, "StartTime", tmp);
	m_startTime = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "ExcTime", tmp);
	m_excTime = atoi(tmp.c_str());

	GetValue((TiXmlElement*)xmlStr, "Amplitude", tmp);
	m_amplitude = atof(tmp.c_str());

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
int CSawtoothWave::createDataPoints()
{

	m_dataLen = m_period / m_excInteral;
	m_pDataBuffer = new double[m_dataLen];
	memset(m_pDataBuffer, m_dataLen, 0);
	double k = 0, b = 0;
	k = 2*m_amplitude / m_period;
	b = m_valueSkewing;
//	FILE* fd = fopen("/home/V1.0.0/HiCurveIncManager/SawtoothWave.txt", "w+");
	for(int x = -m_period/2 + m_excInteral, i = 0; x<=m_period/2; x+=m_excInteral, i++)
	{
		m_pDataBuffer[i] = k*x + b;
	}

	return 0;
}



/**************************************************************
 *Function name: run
*Description:	���м���������
*Parameters:	NULL
*Return:		0 - success other-failed
***************************************************************/
int CSawtoothWave::run()
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
				if(m_phase > 0)
				{
					m_outputValue = m_pDataBuffer[(int)((m_runTime - m_startTime + m_period - m_phase) / m_excInteral) % m_dataLen];
				}
				else
				{
					m_outputValue = m_pDataBuffer[(int)((m_runTime - m_startTime - m_phase) / m_excInteral) % m_dataLen];
				}
				output();
			}
		}
		m_runTime = m_runTime + m_stepValue;

	}
	return 0;
}
int CSawtoothWave::runNoOutput()
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
				if(m_phase > 0)
				{
					m_outputValue = m_pDataBuffer[(int)((m_runTime - m_startTime + m_period - m_phase) / m_excInteral) % m_dataLen];
				}
				else
				{
					m_outputValue = m_pDataBuffer[(int)((m_runTime - m_startTime - m_phase) / m_excInteral) % m_dataLen];
				}
			}
		}
		m_runTime = m_runTime + m_stepValue;

	}
	return 0;
}
