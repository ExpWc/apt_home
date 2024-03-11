/*
 * SquareWave.h
 *
 *  Created on: 2022-4-15
 *      Author: root
 */
//鏂规尝
#ifndef SQUAREWAVE_H_
#define SQUAREWAVE_H_
#include "CCurve.h"
#include "tinyxml.h"
class CSquareWave : public CBaseCurve
{
public:
	CSquareWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem);
    ~CSquareWave();

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

#endif /* SQUAREWAVE_H_ */
