/*
 * Constant.h
 *
 *  Created on: 2022-4-15
 *      Author: root
 */
//甯稿��
#ifndef CONSTANT_H_
#define CONSTANT_H_

#include "CCurve.h"
#include "tinyxml.h"
class CConstant : public CBaseCurve
{
public:
	CConstant(TY_CURVE_PARAM *param,TiXmlElement* pXmlElem);
    ~CConstant();

public:


	virtual int createDataPoints();

	virtual int runNoOutput();

	/**************************************************************
	 *Function name: run
	*Description:	运行计算输出结果
	*Parameters:	NULL
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int run();


};
#endif /* CONSTANT_H_ */
