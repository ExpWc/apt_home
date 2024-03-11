/*
 * CStorageRule.h
 *
 *  Created on: Feb 28, 2014
 *      Author: root
 */

#ifndef CSTORAGERULE_H_
#define CSTORAGERULE_H_

#define MANUAL_CONTROL 1
#define TIMING_CONTROL 2
#define TRIGGER_CONTROL 3

#define  RELATION_BIG 0   //(>)
#define  RELATION_SMALL 1//(<)
#define RELATION_NOT_BIG 2//(<=)
#define RELATION_NOT_SMALL 3//(>=)
#define RELATION_EQUAL 4//==
#define RELATION_NOT_EQUAL 5//(!=)
#define TYPE_DOUBLE 0
#define TYPE_FLOAT 1
#define TYPE_INT 2
#define TYPE_UINT 3

#define MAX_RULE_NUM 100

#define TRIGGERSTOP 0
#define TRIGGERRUN 1

#include <string>
using namespace std;

typedef struct time_rule_config
{
    long fromSec;
    long toSec;
} TY_TIME_RULE_CONFIG;

typedef struct time_storage
{
    long int startSec;
    long int endSec;
} TY_TIME_STORAGE;

typedef struct trigger_rule_config
{
    int id;
    int len;
    int offset;
    int type;
    double dValue;
    int iValue;
    unsigned int uIntVaule;
    float fValue;
    int dataType;
    int relation;
} TY_TRIGGER_RULE_CONFIG;

typedef struct trigger_storage
{
    int triggerStartSec;
    int triggerEndSec;
} TY_TRIGGER_STORAGE;

class CStorageRule
{
public:
    CStorageRule();
    static CStorageRule *instance;
    static CStorageRule *
    getInstance();
    virtual
    ~CStorageRule();

    TY_TIME_RULE_CONFIG m_timeRule;
    int m_type;
    int m_orRuleNum;
    TY_TRIGGER_RULE_CONFIG m_orRule[MAX_RULE_NUM];

    TY_TIME_STORAGE timeStorage;
    TY_TRIGGER_STORAGE triggerStorage;

    int triggerCount;//statistics trigger times
    int conditionFlag;

    unsigned long timeConditionTrigger;//??
    int triggerFlag;
    unsigned long perTriggerNumber;
    char triggerRule[256];

    static int getRelation(string relation);
    static int getDataType(string relation);
    static void setData(TY_TRIGGER_RULE_CONFIG *config, string value);
    void judgeConditionTrigger(unsigned long id, char* tmpData);
    int decideOrRule(unsigned long id, int i, char *data);
    unsigned long long unpack(int start, int length, char *data);
    int doubleDataCmp(double firstData, int relation, double lastData);
};

#endif /* CSTORAGERULE_H_ */
