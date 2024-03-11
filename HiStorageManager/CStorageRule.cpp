/*
 * CStorageRule.cpp
 *
 *  Created on: Feb 28, 2014
 *      Author: root
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CStorageRule.h"

CStorageRule::CStorageRule()
{
	m_type = MANUAL_CONTROL;
	m_orRuleNum = 0;

	memset(&m_timeRule, 0, sizeof(m_timeRule));
	memset(&timeStorage, 0, sizeof(timeStorage));
	memset(&triggerStorage, 0, sizeof(triggerStorage));
	memset(&triggerRule, 0, sizeof(triggerRule));

	triggerCount = 0;
	conditionFlag = TRIGGERRUN;
	timeConditionTrigger = 0;
	triggerFlag = 0;
	perTriggerNumber = 0;

}

CStorageRule::~CStorageRule()
{
	// TODO Auto-generated destructor stub
}
int CStorageRule::getRelation(string relation)
{
	printf("relation.c_str():%s \n", relation.c_str());
	if (strcmp(relation.c_str(), "<") == 0) // data < condition data;
	{
		return RELATION_SMALL;
	}
	else if (strcmp(relation.c_str(), ">") == 0) // data > condition data;
	{
		return RELATION_BIG;
	}
	else if (strcmp(relation.c_str(), "==") == 0) // data = condition data;
	{
		return RELATION_EQUAL;
	}
	else if (strcmp(relation.c_str(), "!=") == 0) //data != condition data;
	{
		return RELATION_NOT_EQUAL;
	}
	else if (strcmp(relation.c_str(), ">=") == 0)// data >= condition data;
	{
		return RELATION_NOT_SMALL;
	}
	else if (strcmp(relation.c_str(), "<=") == 0) //data <= condition data;
	{
		return RELATION_NOT_BIG;
	}
	else
	{
		return 0;
	}
}

/*
 * Description: compare the firstData and lastData;
 * parameter:
 *              firstData ---
 *              relation  ---
 *              lastData  ---
 *
 * return:      0 or 1;
 */
int CStorageRule::doubleDataCmp(double firstData, int relation, double lastData)
{
	if (relation == RELATION_SMALL) // data < condition data;
	{
		printf("RELATION_SMALL\n");
		if (firstData < lastData)
			return 1;
		else
			return 0;
	}
	else if (relation == RELATION_BIG) //data > condition data;
	{
		printf("RELATION_BIG\n");
		if (firstData > lastData)
			return 1;
		else
			return 0;
	}
	else if (relation == RELATION_EQUAL) //  data == condition data;
	{
		printf("RELATION_EQUAL\n");
		if (firstData == lastData)
			return 1;
		else
			return 0;
	}
	else if (relation == RELATION_NOT_EQUAL) //data != condition data;
	{
		printf("RELATION_NOT_EQUAL\n");
		if (firstData != lastData)
			return 1;
		else
			return 0;
	}
	else if (relation == RELATION_NOT_SMALL) //data >= condition data;
	{
		printf("RELATION_NOT_SMALL\n");
		if (firstData >= lastData)
			return 1;
		else
			return 0;
	}
	else if (relation == RELATION_NOT_BIG) //data <= condition data;
	{
		printf("RELATION_NOT_BIG\n");
		if (firstData <= lastData)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}

int CStorageRule::getDataType(string relation)
{
	if (strcmp(relation.c_str(), "double") == 0) // data < condition data;
	{
		return TYPE_DOUBLE;
	}
	else if (strcmp(relation.c_str(), "float") == 0) // data > condition data;
	{
		return TYPE_FLOAT;
	}
	else if (strcmp(relation.c_str(), "int") == 0) // data = condition data;
	{
		return TYPE_INT;
	}
	else if (strcmp(relation.c_str(), "unsigned int") == 0) //data != condition data;
	{
		return TYPE_UINT;
	}
	else
	{
		return TYPE_DOUBLE;
	}
}
void CStorageRule::judgeConditionTrigger(unsigned long id, char* tmpData)
{
	int compareResult = 0;

	/***** orRule estimate ******/
	for (int i = 0; i <= m_orRuleNum; i++)
	{
		compareResult = decideOrRule(id, i, tmpData);
		//        printf("judgeConditionTrigger====%d compareResult:%d\n",i,compareResult);
		/****** if all the one orRule is run right so set the condition flag stop*****/
		if (compareResult == 1)
		{
			conditionFlag = TRIGGERSTOP;
			//if one id appease the orRule , start waiting
			triggerCount++;
		}
	}
}
int CStorageRule::decideOrRule(unsigned long id, int i, char *data)
{
	unsigned long long lastData = 0;
	int result = 0;
	/***if get from DDS id == xml id , then judge the orRule ****/
	if (id == m_orRule[i].id)
	{
		int offsetXml = m_orRule[i].offset;
		int length = m_orRule[i].len;

		lastData = unpack(offsetXml, length, data);//finded data
		if (m_orRule[i].dataType == TYPE_DOUBLE)
		{
			double dataMove = *(double*) &lastData;
			double getValue = (double) (m_orRule[i].dValue);
			result = doubleDataCmp(dataMove, m_orRule[i].relation, getValue);
			sprintf(triggerRule, "%ld-%d-%f", id, offsetXml, dataMove);
		}
		else if (m_orRule[i].dataType == TYPE_FLOAT)
		{
			float dataMove = *(float*) &lastData;
			float getValue = (float) (m_orRule[i].fValue);
			result = doubleDataCmp(dataMove, m_orRule[i].relation, getValue);
			sprintf(triggerRule, "%ld-%d-%f", id, offsetXml, dataMove);
		}
		/****** int type compare 4 byte *****/
		else if (m_orRule[i].dataType == TYPE_INT)
		{
			int dataMove = *(int *) &lastData;
			int getData = (int) (m_orRule[i].iValue);
			if (((getData >> 31) & (0x01)) == 1)
			{
				getData &= (0xffffffff >> (32 - length));
			}
			result = doubleDataCmp(dataMove, m_orRule[i].relation, getData);
			printf(
					"dataMove:%d getData:%d m_orRule[i].iValue:0x%x result:%d \n",
					dataMove, getData, m_orRule[i].iValue, result);
			sprintf(triggerRule, "%ld-%d-%d", id, offsetXml, dataMove);
		}

		/***** unsigned int compare 4 byte ******/
		else if (m_orRule[i].dataType == TYPE_UINT)
		{
			unsigned int dataMove = *(unsigned int*) &lastData;
			unsigned int getData = (unsigned int) (m_orRule[i].uIntVaule);
			result = doubleDataCmp(dataMove, m_orRule[i].relation, getData);
			sprintf(triggerRule, "%ld-%d-%d", id, offsetXml, dataMove);
		}
	}//end id = filter_id[id]

	return result;
}

unsigned long long CStorageRule::unpack(int start, int length, char *data)
{
	unsigned long long gotData = 0;

	unsigned char byte_off = start / 8;
	unsigned char bit_off = start % 8;
	if ((length < 64) || ((length == 64) & (length % 8 == 0)) == 0)
	{
		unsigned long long value = (*(unsigned long long*) (data + byte_off))
				>> bit_off;
		long i = -1;
		long long mask = i << length;
		gotData = value & ~mask;

		return gotData;
	}
	else
	{
		unsigned long long value = (*(unsigned long long*) (data + byte_off))
				>> bit_off;
		gotData = value;

		unsigned char highByte = (*(unsigned char*) (data + byte_off + 8));
		highByte = highByte << (8 - bit_off);
		unsigned char *dataTmp = ((unsigned char*) (&gotData)) + 7;
		*dataTmp = (*dataTmp) | highByte;
	}
	return gotData;
}

CStorageRule * CStorageRule::instance = NULL;
CStorageRule*
CStorageRule::getInstance()
{
	if (instance == NULL)
	{
		instance = new CStorageRule();
	}
	return instance;
}
