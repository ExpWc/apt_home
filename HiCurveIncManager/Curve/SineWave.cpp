#include "SineWave.h"
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
#define PI 3.1415926535



CSineWave::CSineWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem):CBaseCurve(param,pXmlElem)
{

	m_curveType = SINE_WAVE;

	createDataPoints();
}
CSineWave::~CSineWave()
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
int CSineWave::loadCurveConfig(const char* xmlStr)
{
	string tmp;

	GetValue((TiXmlElement*)xmlStr, "CurveID", m_curveID);
	GetValue((TiXmlElement*)xmlStr, "ICDID", m_icdID);
	GetValue((TiXmlElement*)xmlStr, "FieldID", m_fieldID);
	GetValue((TiXmlElement*)xmlStr, "CardType", m_cardType);

	GetValue((TiXmlElement*)xmlStr, "CardId", tmp);
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

	GetValue((TiXmlElement*)xmlStr, "Phase", tmp);
	m_phase = atoi(tmp.c_str());
	return 0;

}
*/

int CSineWave::createDataPoints()
{
	m_dataLen = m_period / m_excInteral;
	m_pDataBuffer = new double[m_dataLen];
	memset(m_pDataBuffer, m_dataLen, 0);
//	FILE* fd = fopen("/home/V1.0.0/HiCurveIncManager/SineWave.txt", "w+");
	for(int i = 0; i<m_dataLen; i++)
	{
		m_pDataBuffer[i] = m_amplitude* sin(2*PI/m_period*((i+1)*m_excInteral -  m_phase)) + m_valueSkewing;
//		fprintf(fd, "%f\n", m_pDataBuffer[i]);
		//printf("%f\n", m_pDataBuffer[i]);
	}
//	fclose(fd);

	return 0;
}

/**************************************************************
 *Function name: run
*Description:	���м���������
*Parameters:	NULL
*Return:		0 - success other-failed
***************************************************************/
int CSineWave::run()
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
				m_outputValue = m_pDataBuffer[((m_runTime-m_startTime) / m_excInteral)%m_dataLen];
				output();
			}
		}
		m_runTime = m_runTime + m_stepValue;
	}
	return 0;
}
int CSineWave::runNoOutput()
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
				m_outputValue = m_pDataBuffer[((m_runTime-m_startTime) / m_excInteral)%m_dataLen];
			}
		}
		m_runTime = m_runTime + m_stepValue;
	}
	return 0;
}
