/*
 * TriangleWare.h
 *
 *  Created on: 2022-4-15
 *      Author: root
 */
//涓夎娉�
#ifndef TRIANGLEWARE_H_
#define TRIANGLEWARE_H_
#include "CCurve.h"
#include "tinyxml.h"
class CTriangleWave : public CBaseCurve
{
public:
	CTriangleWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem);
    ~CTriangleWave();

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
#endif /* TRIANGLEWARE_H_ */
