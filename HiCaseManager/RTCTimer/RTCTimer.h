/*
 * RTCTimer.h
 *
 *  Created on: Dec 5, 2019
 *      Author: root
 */

#ifndef RTCTIMER_H_
#define RTCTIMER_H_

#define ERR_NONE 0
#define ERR_OPEN_FILE -1
#define ERR_RTC_IRQP_SET -2
#define ERR_RTC_PIE_ON -3
#define ERR_START -4
#define ERR_PARAMETER -5

typedef void* (*_OnTimerHandler)(void *);

int rtcInit(int interval,_OnTimerHandler pHandler, void* pParam);
int rtcStart();
int rtcStop();
int rtcDispose();

#endif /* RTCTIMER_H_ */
