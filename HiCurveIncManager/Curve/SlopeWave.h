/*
 * SlopeWave.h
 *
 *  Created on: 2022-4-15
 *      Author: root
 */
//斜坡
#ifndef SLOPEWAVE_H_
#define SLOPEWAVE_H_
#include "CCurve.h"
#include "tinyxml.h"
class CSlopeWave : public CBaseCurve
{
public:
	CSlopeWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem);
    ~CSlopeWave();

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

#endif /* SLOPEWAVE_H_ */
