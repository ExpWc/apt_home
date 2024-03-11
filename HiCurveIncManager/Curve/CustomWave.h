/*
 * CustomWave.h
 *
 *  Created on: 2022-4-15
 *      Author: root
 */
//自定义波
#ifndef CUSTOMWAVE_H_
#define CUSTOMWAVE_H_
#include "CCurve.h"
#include "tinyxml.h"
class CCustomWave : public CBaseCurve
{
public:
	CCustomWave(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem);
    ~CCustomWave();

public:

    void parseCurveXml(const char* xmlStr);
    TY_CURVE_VECTOR m_subCurveVec;
	CBaseCurve* m_pCurve[MAX_CURVE_NUM];

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

#endif /* STACKINGWAVE_H_ */
