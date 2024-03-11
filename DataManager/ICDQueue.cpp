/*
 * ICDQueue.cpp
 *
 *  Created on: Nov 7, 2021
 *      Author: root
 */

#include "ICDQueue.h"
#include <string.h>
#include <stdio.h>

ICDQueue::ICDQueue(int queueDepth,int bufferSize)
{
	sem_init(&m_sem, 0, 1);
	m_depth = queueDepth;
	m_bufferSize = bufferSize;
}

ICDQueue::~ICDQueue()
{
	clear();
	sem_destroy(&m_sem);
}

int ICDQueue::push(char* pData,int length,int sec,int usec)
{
	int ret = 0;
	TY_ICD_PAKET packet;
	packet.pData = new char[m_bufferSize];
	//printf("=====file:%s,sfunction:%s line:%d,======data:%d, buffersize:%d\n",__FILE__,__FUNCTION__,__LINE__,*pData,m_bufferSize);
	if(m_bufferSize < length)
	{
		length = m_bufferSize;//pRecvPac->len
	}
	memcpy(packet.pData,pData,length);
	packet.length = length;
	packet.sec = sec;
	packet.usec = usec;
	sem_wait(&m_sem);

	if(m_queue.size()>m_depth)
	{
		TY_ICD_PAKET tmpPacket;
		tmpPacket = m_queue.front();
		delete tmpPacket.pData;
		m_queue.pop();
		ret = ERR_QUEUE_OVERFLOW;
	}
	m_queue.push(packet);
	sem_post(&m_sem);
	return ret ;
}
int ICDQueue::pop(char*pData,int &length,int *pSec,int *pUsec)
{
	int ret = 0;
	if(m_queue.size()<1)
	{
		ret = ERR_QUEUE_EMPTY;
		return ret;
	}
	sem_wait(&m_sem);
	TY_ICD_PAKET packet = m_queue.front();

	if(length<packet.length)
	{
		packet.length = length;
	}
	memcpy(pData,packet.pData,packet.length);
	*pSec = packet.sec;
	*pUsec = packet.usec;
	length = packet.length;
	delete packet.pData;
	m_queue.pop();
	sem_post(&m_sem);
	return ret;
}


void ICDQueue::clear()
{
	while(m_queue.size()>0)
	{
		TY_ICD_PAKET packet = m_queue.front();
		delete packet.pData;
		m_queue.pop();
	}
}
int ICDQueue::length()
{
	return m_queue.size();
}
int ICDQueue::getLastPacket(char*pData,int &length,long *pSec,long *pUsec)
{
	int ret = 0;
	if(m_queue.size()<1)
	{
		ret = ERR_QUEUE_EMPTY;
		return ret;
	}
	TY_ICD_PAKET packet = m_queue.back();
	if(length<packet.length)
	{
		packet.length = length;
	}
	memcpy(pData,packet.pData,packet.length);
	*pSec = packet.sec;
	*pUsec = packet.usec;
	length = packet.length;
	return ret;
}
