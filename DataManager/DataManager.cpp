/*
 * DataManager.cpp
 *
 *  Created on: Nov 7, 2021
 *      Author: root
 */
//#include "HiReport.h"
#include "DataManager.h"
#include "stdio.h"
DataManager::DataManager()
{
	// TODO Auto-generated constructor stub

}

DataManager::~DataManager()
{
	// TODO Auto-generated destructor stub
}

ICDQueue* DataManager::getQueue(string uuid)
{
	TY_ICD_QUEUE_MAP::iterator pos = m_map.find(uuid);
	if(pos == m_map.end())
	{
		return NULL;
	}
	return pos->second;
}
int DataManager::initQueue(string uuid,int maxDepth,int bufferSize)
{
	int ret =0;
	TY_ICD_QUEUE_MAP::iterator pos = m_map.find(uuid);
	if(pos == m_map.end())
	{
		ICDQueue *pQueue=new ICDQueue(maxDepth,bufferSize);
		m_map.insert(make_pair(uuid,pQueue));
		//printf("initQueue uuid %s \n", uuid.c_str());
	}
	else
	{
		ret = ERR_QUEUE_ALREADY_EXIST;
	}
	return ret;
}
int DataManager::insert(string uuid,char*pData,int length,int sec, int usec)
{
	int ret = 0;
	TY_ICD_QUEUE_MAP::iterator pos = m_map.find(uuid);
	if(pos == m_map.end())
	{
		ret = ERR_QUEUE_NOT_FOUND;
	}
	else
	{
		ret = pos->second->push(pData,length,sec,usec);
		//printf("=====file:%s,sfunction:%s line:%d,======pData:%d\n",__FILE__,__FUNCTION__,__LINE__,*pData);
	}
	return ret ;
}
int DataManager::reset()
{
	for(TY_ICD_QUEUE_MAP::iterator pos = m_map.begin();pos!=m_map.end();pos++)
	{
		if(pos->second != NULL)
		{
			ICDQueue* pQueue = (ICDQueue*)pos->second;
			pQueue->clear();
			delete pos->second;
			pos->second = NULL;
		}

	}
	m_map.clear();
	return 0;
}
