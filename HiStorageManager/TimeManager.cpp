/*
 * testTime.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: root
 */
#include "TimeManager.h"
#include <string.h>

TimeManager::TimeManager()
{
}

TimeManager::~TimeManager()
{

}
void TimeManager::getNowTime(TY_HR_TIME *NowTime)
{
	struct timeval currTime;
	gettimeofday(&currTime, NULL);
	NowTime->sec = currTime.tv_sec;
	NowTime->usec = currTime.tv_usec;
}
void TimeManager::getLTime(TY_LTIME *NowLTime)
{
	struct tm *pTime;
	time_t t = time(NULL);
	pTime = localtime(&t);

	HRINT8S Date[256];
	HRINT8S Time[256];

	NowLTime->sec = pTime->tm_sec;
	NowLTime->min = pTime->tm_min;
	NowLTime->hour = pTime->tm_hour;
	NowLTime->day = pTime->tm_mday;
	NowLTime->mouth = pTime->tm_mon + 1;
	NowLTime->year = pTime->tm_year + 1900;

	sprintf(Date, "%d-%02d-%02d", NowLTime->year, NowLTime->mouth,
			NowLTime->day);
	sprintf(Time, "%d-%02d-%02d", NowLTime->hour, NowLTime->min, NowLTime->sec);

	NowLTime->LDate = Date;
	NowLTime->LTime = Time;
	NowLTime->LDateTime.append(Date);
	NowLTime->LDateTime.append(Time);
}
static void *arrTime(void *param)
{
	TimeManager::TY_ARRIVEPARAS *timeArrPara;
	timeArrPara = (TimeManager::TY_ARRIVEPARAS *) param;
	TY_HR_TIME timeTmp;
	TY_HR_TIME timeCur;
	TimeManager *pThis = timeArrPara->pThis;
	memset(&timeTmp, 0, sizeof(TY_HR_TIME));
	timeArrPara->flag = WAITFLAG;
	while (timeTmp.sec != timeArrPara->interval)
	{
		pThis->getNowTime(&timeCur);
		timeTmp = timeCur - timeArrPara->nowTime;
		timeArrPara->TimeDiff = (timeArrPara->interval - timeTmp.sec);
	}
	timeArrPara->flag = ARRIVEFLAG;
	pthread_exit(NULL);
	return NULL;
}

int TimeManager::arriveTime(int interval)
{
	TY_HR_TIME NowTime;
	getNowTime(&NowTime);

	timeArrivePara.nowTime.sec = NowTime.sec;
	timeArrivePara.nowTime.usec = NowTime.usec;
	timeArrivePara.interval = interval;
	timeArrivePara.flag = WAITFLAG;
	timeArrivePara.pThis = this;
	HRINT32S result = pthread_create(&arriveThreadId, NULL, &arrTime,
			&timeArrivePara);
	if (result != 0)
	{
		printf("create arrive time pthread failed !!! \n");
		return -1;
	}
	else
	{
		//pthread_join(arriveThreadId,NULL);
		return 0;
	}
}

int TimeManager::stopArrTime()
{
	pthread_join(arriveThreadId, NULL);
	return 0;
}
