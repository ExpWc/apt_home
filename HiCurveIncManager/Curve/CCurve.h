/*
 * CCurve.h
 *
 *  Created on: 2022-4-15
 *      Author: root
 */

#ifndef CCURVE_H_
#define CCURVE_H_

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <vector>
#include "tinystr.h"
#include "tinyxml.h"
#include "AutoTestInterface.h"
#include <semaphore.h>
using namespace std;



#ifdef _WIN32
#define CLASS_EXPORT __declspec(dllexport)
#else
#define CLASS_EXPORT
#endif
#define MAX_CURVE_NUM 20

enum CURVE_RUNING_STATUS
{
	CURVE_STOP   = 0,
	CURVE_PAUSE  = 1,
	CURVE_INIT  = 2,
	CURVE_RUNNING  = 3,
	CURVE_ERROR  = 4,

};
enum CURVE_TYPE
{
	CONSTAN            = 0,  //常值
	CUSTOMW_WAVE       = 1,  //自定义
	SAWTOOTH_WAVE      = 2,  //锯齿波
	SINE_WAVE          = 3,  //sine
	SLOPE_WAVE         = 4,  //斜坡
	SQUARE_WAVE        = 5,  //方波
	TRIANGLE_WAVE      = 6,  //三角波
};

typedef struct sub_curve_param
{
	char curveType[64];
    int excInteral;//激励间隔 ：ms
	int startTime;//开始时间
	int excTime;//激励时间 ：s
	double amplitude;//幅值
	double valueSkewing;//偏移
    int period;//周期 ：ms
    int phase;//相位
	int duty;
	double slope;
}TY_SUB_CURVE_PARAM;

typedef struct curve_param
{
	char curveType[64];
    int excInteral;//激励间隔 ：ms
	int startTime;//开始时间
	int excTime;//激励时间 ：s
	double amplitude;//幅值
	double valueSkewing;//偏移
    int period;//周期 ：ms
    int phase;//相位
	double duty;

	int subCurveNum;
}TY_CURVE_PARAM;

typedef struct icd_info
{
	string icdID;
	string fieldID;
	string cardType;
	int cardID;
	int chnID;
}TY_ICD_INFO;

class CLASS_EXPORT CBaseCurve
{
public:
	CBaseCurve();
	CBaseCurve(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem);

	~CBaseCurve();
public:

	string m_curveID;
	CURVE_TYPE m_curveType;
	vector<TY_ICD_INFO> m_vecIcdinfo;
	int m_startTime;//开始时间
	int m_excTime;//激励时间 ：s
	double m_amplitude;//幅值
	double m_valueSkewing;//偏移
    int m_excInteral;//激励间隔 ：ms
    int m_period;//周期 ：ms
	int m_runTime;//运行时间 ：ms
	double m_outputValue;//输出值
	double *m_pDataBuffer;
	double m_slope;
	int m_dataLen;
	double m_duty;
	int m_phase;
	int m_stepValue;
	CURVE_RUNING_STATUS m_runFlag;
    pthread_t m_curveThreadId;
    sem_t m_sem;
	/**************************************************************
	 *Function name: loadCurveConfig
	*Description:	加载配置文件
	*Parameters:	xml文件
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int loadCurveConfig(const char* xmlStr);

	virtual int createDataPoints();

	/**************************************************************
	 *Function name: start
	*Description:	开始运行曲线
	*Parameters:	NULL
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int start();

	/**************************************************************
	 *Function name: stop
	*Description:	停止运行曲线
	*Parameters:	NULL
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int stop();

	/**************************************************************
	 *Function name: pause
	*Description:	暂停运行曲线
	*Parameters:	NULL
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int pause();

	/**************************************************************
	 *Function name: run
	*Description:	运行计算输出结果
	*Parameters:	NULL
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int run();
	virtual int runNoOutput();
	/**************************************************************
	 *Function name: output
	*Description:	输出计算结果
	*Parameters:	NULL
	*Return:		0 - success other-failed
	***************************************************************/
	virtual int output();	

	virtual void toString();
	virtual int setStepValue(int stepValue);
	virtual int getCurveStatus(CURVE_RUNING_STATUS *status);

};
typedef map<string, CBaseCurve*> TY_CURVE_MAP;
typedef vector<CBaseCurve*> TY_CURVE_VECTOR;
typedef struct _curve_info_
{
	char curveName[64];
	CURVE_TYPE curveType;

}TY_CURVE_INFO;

typedef struct _curve_status_
{
	char curveName[64];
	CURVE_TYPE curveType;
	CURVE_RUNING_STATUS runFlag;
}TY_CURVE_STATUS;

typedef map<string, TY_CURVE_PARAM*> TY_CURVE_PARAM_MAP;

typedef map<string, TY_CURVE_MAP> TY_CURVE_BIND_MAP;

class CLASS_EXPORT CCurveManager
{
public:
	CCurveManager();
	~CCurveManager();
public:
	static CCurveManager *instance;//static

	string CurveFunName;
	/**************************************************************
	 *Function name: start
	*Description:	控制曲线开始输出
	*Parameters:	curveInfo : 曲线信息
	curveNum ： 曲线数量
	*Return:		0 - success other-failed
	***************************************************************/
	int start(TY_CURVE_INFO* curveInfo, int curveNum);

	/**************************************************************
	 *Function name: stop
	*Description:	控制曲线停止输出
	*Parameters:	curveInfo : 曲线信息
	curveNum ： 曲线数量
	*Return:		0 - success other-failed
	***************************************************************/
	int stop(TY_CURVE_INFO* curveInfo, int curveNum);

	/**************************************************************
	 *Function name: pause
	*Description:	控制曲线暂停输出
	*Parameters:	curveInfo : 曲线信息
	curveNum ： 曲线数量
	*Return:		0 - success other-failed
	***************************************************************/
	int pause(TY_CURVE_INFO* curveInfo, int curveNum);

	/**************************************************************
	 *Function name: run
	*Description:	曲线运行（控制各个曲线计算输出值并输出）
	*Parameters:	curveInfo : 曲线信息
	curveNum ： 曲线数量
	*Return:		0 - success other-failed
	***************************************************************/
	int run();

	/**************************************************************
	 *Function name: getCurveList
	*Description:	获取曲线列表
	*Parameters:	curveInfo : 曲线信息
	curveNum ： 曲线数量
	*Return:		0 - success other-failed
	***************************************************************/
	int getCurveList(TY_CURVE_INFO *curveInfo, int curveNum);

	/**************************************************************
	 *Function name: getCurveStatus
	*Description:	获取曲线状态
	*Parameters:	curveInfo : 曲线状态
	curveNum ： 曲线数量
	*Return:		0 - success other-failed
	***************************************************************/
	int getCurveStatus(TY_CURVE_STATUS *curveInfo, int curveNum);

	/**************************************************************
	 *Function name: getInstance
	*Description:	获CCurveManager
	*Return:		CCurveManager - success NULL-failed
	***************************************************************/

	static CCurveManager* getInstance();

	/**************************************************************
	 *Function name: loadConfig
	*Description:	解析xml
	*Parameters:	xmlStr : xml文件
	*Return:		0 - success other-failed
	***************************************************************/
	int loadConfig(const char* xmlStr);

	/**************************************************************
	 *Function name: insertCurveMap
	*Description:	插入曲线map
	*Parameters:	xmlStr : xml文件
	*Return:		0 - success other-failed
	***************************************************************/
	int insertCurveMap(string curveConfigXml);

	/**************************************************************
	 *Function name: run
	*Description:	检测曲线运行状态
	*Return:		0 - success other-failed
	***************************************************************/
	int checkCurveStatus(); // add by wc 20240219
	
	int getCurveData(char *pCurveID, double *value); // add by wc 20240219

	int updateCurveInputData(char *modelName); // add by wc 20240219
	
	TY_CURVE_PARAM_MAP m_paramMap;
	TY_CURVE_BIND_MAP m_curveBindMap;
};

class CLASS_EXPORT CCurveFactory
{
public:
	CCurveFactory();
	~CCurveFactory();
public:
	static CBaseCurve* createCurve(TY_CURVE_PARAM *param, TiXmlElement* pXmlElem);

};
#endif /* CCURVE_H_ */
