/*
 * SineWave.h
 *
 *  Created on: 2022-4-15
 *      Author: root
 */
//sine鏇茬嚎
#ifndef SINEWAVE_H_
#define SINEWAVE_H_
#include "CCurve.h"
#include "tinyxml.h"
class CSineWave : public CBaseCurve
{
public:
	CSineWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem);
    ~CSineWave();

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

#endif /* SINEWAVE_H_ */
