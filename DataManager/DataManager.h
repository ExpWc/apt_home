/*
 * DataManager.h
 *
 *  Created on: Nov 7, 2021
 *      Author: root
 */

#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include <map>
#include <string>
#include "ICDQueue.h"
using namespace std;

typedef map<string,ICDQueue*> TY_ICD_QUEUE_MAP;

class DataManager
{
public:
	DataManager();
	virtual ~DataManager();

	ICDQueue* getQueue(string uuid);
	int initQueue(string uuid,int maxDepth,int bufferSize);
	int insert(string uuid,char*pData,int length,int sec, int usec);
	int reset();

private:
	TY_ICD_QUEUE_MAP m_map;
};

#endif /* DATAMANAGER_H_ */
