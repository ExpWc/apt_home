/*
 * testTime.h
 *
 *  Created on: Nov 21, 2014
 *      Author: root
 */

#ifndef TESTTIME_H_
#define TESTTIME_H_
#ifdef _WIN32
#else
#include <sys/time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>
#include "HRDATATYPE.h"
#include "hr_time.h"

#define WAITFLAG    0
#define ARRIVEFLAG  1

using namespace std;

class TimeManager
{
public:
    typedef struct timeType
    {
        HRINT64U sec;
        HRINT64U usec;
    }TY_TIME;
    typedef struct LTimeType
    {
        HRINT32S year;
        HRINT32S mouth;
        HRINT32S day;
        HRINT32S hour;
        HRINT32S min;
        HRINT32S sec;
        string LDate;
        string LTime;
        string LDateTime;
    }TY_LTIME;
    typedef struct timeArriveParas
    {
        TY_HR_TIME nowTime;
        HRINT32U TimeDiff;
        HRINT32U flag;
        HRINT32U interval;
        TimeManager *pThis;
    }TY_ARRIVEPARAS;
    TY_ARRIVEPARAS timeArrivePara;
    pthread_t arriveThreadId;
public:
    TimeManager();
    ~TimeManager();
    void getNowTime(TY_HR_TIME *NowTime);
    void getLTime(TY_LTIME *NowLTime);
    int arriveTime(int interval);
    int stopArrTime();
private:

};

#endif /* TESTTIME_H_ */
