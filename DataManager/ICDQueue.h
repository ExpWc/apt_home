/*
 * ICDQueue.h
 *
 *  Created on: Nov 7, 2021
 *      Author: root
 */

#ifndef ICDQUEUE_H_
#define ICDQUEUE_H_

#include <queue>
#include <semaphore.h>
#include "ICDQueue.h"

using namespace std;

typedef struct _icdPacket
{
	char* pData;
	int length;
	int sec;
	int usec;
}TY_ICD_PAKET;

typedef queue<TY_ICD_PAKET> TY_PACKET_QUEUE;

#define ERR_QUEUE_OVERFLOW -1
#define ERR_QUEUE_EMPTY -2
#define ERR_QUEUE_ALREADY_EXIST -3
#define ERR_QUEUE_NOT_FOUND -4


class ICDQueue
{
public:
	ICDQueue(int queueDepth,int bufferSize);
	virtual ~ICDQueue();
	int push(char* pData,int length,int sec,int usec);
	int pop(char*pData,int &length,int *pSec,int *pUsec);
	void clear();
	int length();
	int getLastPacket(char*pData,int &length,long *pSec,long *pUsec);
private:
	int m_depth;
	int m_bufferSize;
	sem_t m_sem;
	TY_PACKET_QUEUE m_queue;

};

#endif /* ICDQUEUE_H_ */
