#pragma once

/*锟斤拷锟斤拷锟酵诧拷锟斤拷锟斤拷Flag锟斤拷志锟斤拷锟斤拷锟斤拷锟斤拷锟街碉拷锟斤拷锟叫≈�*/
#define MAX_LIMIT_FLAG		0x01
#define MIN_LIMIT_FLAG		0x02

/*支锟街的诧拷锟斤拷锟斤拷锟酵ｏ拷锟斤拷锟叫诧拷锟斤拷锟斤拷锟斤拷支锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷*/
#define CHAR_ARRAY_TYPE		  ("char_array")
#define DOUBLE_TYPE			  ("double")
#define FLOAT_TYPE			  ("float")
#define LONG_TYPE			  ("long")
#define SHORT_TYPE			  ("short")
#define CHAR_TYPE			  ("char")
#define UCHAR_TYPE			  ("unsigned char")
#define USHORT_TYPE			  ("unsigned short")
#define ULONG_TYPE			  ("unsigned long")
#define UINT_TYPE			  ("unsigned int")
#define INT_TYPE			  ("int")
#define BOOL_TYPE			  ("bool")
#define ENUM_TYPE			  ("enum")
#define PARAM_TYPE			  ("parameter")
#define INPUT_ICD_TYPE		  ("input")
#define OUTPUT_ICD_TYPE		  ("output")

#include <string>
#include <vector>
#include <map>
#include <tinyxml.h>
#include "AutoTestInterface.h"
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN	256
typedef char CHAR_UNIT;
typedef CHAR_UNIT	CHAR_ARRAY[MAX_STRING_LEN];
#endif



#ifndef _ttof
#ifndef _UNICODE
	#define _ttof atof
#else
	#define _ttof atof
#endif
#endif

using namespace std;







typedef struct _input_field
{
	string strFieldID;
	string strVarName;
}TY_INPUT_FIELD;

typedef struct _output_field
{
	string strFieldID;
	string strVarName;
	string strVarType;
	TY_VALUEFIELD varValue;
}TY_OUTPUT_FIELD;

typedef struct _input_value
{
	string strIcdID;
	string strCardType;
	unsigned int cardID;
	unsigned int chnID;
	vector<TY_INPUT_FIELD> vecIcdField;
}TY_INPUT_VALUE;

typedef struct _output_value
{
	string strIcdID;
	string strCardType;
	unsigned int cardID;
	unsigned int chnID;
	vector<TY_OUTPUT_FIELD>  vecIcdField;
}TY_OUTPUT_VALUE;

typedef struct _param_value
{
	string strParamName;
	string strParamType;
	string strReferenced;
	TY_VALUEFIELD value;;
	unsigned int uParamLength;
	unsigned int offset;
}TY_PARAM_VALUE;


class CValueArray
{
public:
	string m_strValueType;
	vector<TY_INPUT_VALUE>  m_vecIcdInputValue;
	vector<TY_OUTPUT_VALUE>  m_vecIcdOutputValue;
	vector<TY_PARAM_VALUE>  m_vecParamValue;
}; 


class CSubStep
{

public:
	CSubStep();
	~CSubStep();
	string m_strStepID;
	string m_strDllName;
	vector<CValueArray> m_vecValueArray;
    string m_msg;
    string m_attachInfo;
    int m_stepStatus;
    TiXmlElement* m_pSubStepElement;
	void getValue(string &strStepID, string &strDllName);
	int getSendICDByName(string strICDName, string& strICDID, string& strCardType, unsigned int& cardID, string& strValueType, TY_VALUEFIELD& value);
	int getReceICDByName(string strICDName, string& strICDID, string& strCardType, unsigned int& cardID);
	int getParamValueByName(string strParamName, string& strParamType, TY_VALUEFIELD& value);
};

typedef map<string, CSubStep> MAPVectSubStep;


typedef struct _template_value
{
	string strSubStepID;
	string strDllName;
	string strAttachInfo;
	vector<TY_INPUT_VALUE>  vecInputValue;
	vector<TY_OUTPUT_VALUE>  vecOutputValue;
	vector<TY_PARAM_VALUE>  vecParamValue;
}TY_TEMPLATE_VALUE;

typedef struct _output_var
{
	string strReferenced;
	string strVarName;
	string strIcdID;
	string strCurveID;
	string strCardType;
	unsigned int cardID;
	unsigned int chnID;
	string strFieldID;
	string strVarType;
	TY_VALUEFIELD value;
}TY_OUTPUT_VAR;

typedef struct _input_var
{
	string strReferenced;
	string strVarName;
	string strBindType;
	string strCurveID;
	string strIcdID;
	string strCardType;
	unsigned int cardID;
	unsigned int chnID;
	string strFieldID;
	string strVarType;
	TY_VALUEFIELD value;
}TY_INPUT_VAR;

typedef struct _attribute_model
{
	string strDllName;
	string strPeriodMs;
	string strTimes;
	string strModelSource;
	string strTerminalType;

}TY_ATTRIBUTE_MODEL;

typedef struct _attribute_curve
{
	int m_excInteral;
}TY_ATTRIBUTE_CURVE;

//by Var Name
typedef map<string, TY_OUTPUT_VAR> TY_OUTPUT_VAR_MAP;
typedef map<string, TY_INPUT_VAR> TY_INPUT_VAR_MAP;
typedef map<string, TY_PARAM_VALUE> TY_PARAM_VAR_MAP;

//by dllname
typedef map<string, TY_OUTPUT_VAR_MAP> TY_OUTPUT_MAP;
typedef map<string, TY_INPUT_VAR_MAP> TY_INPUT_MAP;
typedef map<string, TY_PARAM_VAR_MAP> TY_PARAM_MAP;
typedef map<string, TY_ATTRIBUTE_MODEL> TY_ATTRIBUTE_MAP;

//by curveID
typedef map<string, TY_ATTRIBUTE_CURVE> TY_CURVE_ATTRIBUTE_MAP;
//by stepID
typedef map<string, TY_CURVE_ATTRIBUTE_MAP> TY_CURVE_INFO_MAP;

typedef struct _step
{
	string strCommand;
	string strStepID;
	string strSubCmd;
	string strStepName;
	string strType;
	string strTimes;
	string strPeriodMs;

	TY_INPUT_MAP inputMap;
	TY_OUTPUT_MAP outputMap;
	TY_PARAM_MAP paramMap;
	TY_ATTRIBUTE_MAP modelAttributeMap;
	void getValue(string &strStepID, string &strStepName, long long &periodMs);
}TY_STEP;
//by step id
typedef map<string, TY_STEP> TY_STEP_MAP;
class CCaseRoot
{
public:
	CCaseRoot();
	~CCaseRoot();
	string m_strCaseID;
	string m_strName;
	string m_strStepNum;
	void getValue(string &strCaseID, string &strName, string &strStepNum);
	string m_presentStepId;
	TY_STEP_MAP m_StepMap;
	int m_tickNum;
};


class CStep
{
public:
        CStep();
        ~CStep();
        string m_strCommand;
        string m_strStepID;
        string m_strStepName;
        string m_strType;
        string m_strTimes;
        string m_strPeriodMs;
        MAPVectSubStep m_mapVectSubStep;
        void getValue(string &strStepID, string &strStepName, long long &periodMs);
};

/*
typedef struct _var_info
{
	vector<TY_INPUT_VAR>  vecInputVar;
	vector<TY_OUTPUT_VAR>  vecOutputVar;
}TY_VAR_INFO;
*/
//typedef map<string, TY_VAR_INFO> TY_VAR_INFO_MAP;
typedef map<string, CCaseRoot> TY_CASE_MAP;
class CObjectData
{

public:
	CObjectData();
	~CObjectData();
	TY_CASE_MAP m_caseMap;
	TY_CURVE_INFO_MAP m_curveInfoMap;
	string m_preCaseId;
	//TY_VAR_INFO_MAP varInfoMap;
	static CObjectData* m_instance;
	static CObjectData* getInstance();
	int serverInit();

	void parseSubStepXMLData(TiXmlElement* subStep, CSubStep *pSubStep);
	int parseXMLData(TiXmlElement* pSubStep, TY_TEMPLATE_VALUE* value);
	int parseXMLData(string strXmlPath);

	void getParamData(TiXmlElement* parameter, TY_PARAM_VALUE& param);
	void getInputData(TiXmlElement* icdXml, TY_INPUT_VALUE &icd);
	void getOutputData(TiXmlElement* icdXml, TY_OUTPUT_VALUE &icd);

	int setICDData(string ICDID, string fieldID, TY_VALUEFIELD value);
	int getICDData(string ICDID, string fieldID, TY_VALUEFIELD& value);
	int sendICDChnData(string icdID, string cardType, unsigned int cardID);
	int receiveICDChnData(string icdID, string cardType, unsigned int cardID);
	int resetICDData();

	int getInfoByDllName(string strDllName,  string& strStepID, string& strStepName, string& strSubStepID, long long& pPeriodMs, TiXmlElement* m_pSubStepElement);
	void getDigitValueByDouble(TY_VALUEFIELD valueField,string strType, double *value);
	void setDigitValueByDouble(TY_VALUEFIELD* valueField,string strType, double value);

	int getPositionByParamName(vector<TY_PARAM_VALUE> value, int& position);
	int getPositionByOutputFieldName(vector<TY_OUTPUT_VALUE> value, int& icdPosition, int& fieldPosition);
	int getPositionByInputFieldName(vector<TY_INPUT_VALUE> value , int& icdPosition, int& fieldPosition);

	void setDigitValueByString(TY_VALUEFIELD*valueField,string strType,string strValue);
	void eraseObject(string stepId);
private:
	int getSize(string strType);


};

