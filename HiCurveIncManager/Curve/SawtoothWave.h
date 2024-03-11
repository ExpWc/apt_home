/*
 * SawtoothWave.h
 *
 *  Created on: 2022-4-15
 *      Author: root
 */
//閿娇娉�
#ifndef SAWTOOTHWAVE_H_
#define SAWTOOTHWAVE_H_
#include "CCurve.h"
#include "tinyxml.h"
class CSawtoothWave : public CBaseCurve
{
public:
	CSawtoothWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem);
    ~CSawtoothWave();

public:
	virtual int createDataPoints();
	/**************************************************************
	 *Function name: run
	*Description:	运行计算输出结果
	*Parameters:	NULL
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int run();
	virtual int runNoOutput();
};


#endif /* SAWTOOTHWAVE_H_ */
