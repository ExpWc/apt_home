/*
 * RTCTimer.c
 *
 *  Created on: Dec 5, 2019
 *      Author: root
 */
#include <linux/rtc.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include "RTCTimer.h"
 #include <time.h>

int gFd = 0;
pthread_t gThreadHandle;
pthread_t gThreadHandleRecvSig;
static int gRunFlag = 0;
_OnTimerHandler gTimerHandler = NULL;
void* gParam = NULL;
sem_t gSem;

void* timerThread(void* pParam)
{
	unsigned long data = 0;
	gSem.__align = 0;
	while (gRunFlag == 1)
	{
		if (read(gFd, &data, sizeof(unsigned long)) < 0)
		{
		}
		sem_post(&gSem);
	}
	return NULL;
}

void* recvSigThread(void* pParam)
{
	while(gRunFlag == 1)
	{
		struct timespec now;
		struct timespec wait_time =
		{	tv_sec:1,tv_nsec:0};
		struct timespec reltime;
		clock_gettime(CLOCK_REALTIME, &now);
		reltime.tv_sec = now.tv_sec + wait_time.tv_sec;
		reltime.tv_nsec = now.tv_nsec;
		sem_timedwait(&gSem, &reltime);
		if(gTimerHandler != NULL)
		{
			gTimerHandler(gParam);
		}
	}
	return NULL;
}

int rtcInit(int interval,_OnTimerHandler pHandler, void* pParam)
{
	int fd = open("/dev/rtc", O_RDONLY);
	if (fd < 0)
	{
		perror("Open rtc");
		return ERR_OPEN_FILE;
	}
	if(interval<=0)
	{
		return ERR_PARAMETER;
	}
	int freq = 1000000 / interval;
	/*Set the freq */
	if (ioctl(fd, RTC_IRQP_SET, freq) < 0)
	{
		perror("ioctl(RTC_IRQP_SET)");
		close(fd);
		return ERR_RTC_IRQP_SET;
	}
	/* Enable periodic interrupts */
	if (ioctl(fd, RTC_PIE_ON, 0) < 0)
	{
		perror("ioctl(RTC_PIE_ON)");
		close(fd);
		return ERR_RTC_PIE_ON;
	}
	gFd = fd;
	gTimerHandler = pHandler;
	gParam = pParam;
	sem_init(&gSem,0,0);
	return ERR_NONE;
}

int rtcStart()
{
	gRunFlag = 1;
	int ret = pthread_create(&gThreadHandle, NULL, timerThread, NULL);
	if (ret != 0)
	{
		gRunFlag = 0;
		return ERR_START;
	}
	pthread_create(&gThreadHandleRecvSig, NULL, recvSigThread, NULL);
	return ERR_NONE;
}
int rtcWait()
{
	unsigned long data = 0;
	if (read(gFd, &data, sizeof(unsigned long)) < 0)
	{
	}
}
int rtcStop()
{
	gRunFlag = 0;
	pthread_join(gThreadHandle,NULL);
	return ERR_NONE;
}
int rtcDispose()
{
	/* Disable periodic interrupts */
	int ret = ioctl(gFd, RTC_PIE_OFF, 0);
	if(ret != 0)
	{
		close(gFd);
		return ERR_RTC_PIE_ON;
	}
	close(gFd);
	return 0;
}
#ifdef TEST
void OnTimer(void )
{
	printf("=========\n");
}
int main()
{
	rtcInit(1000000,OnTimer);
	rtcStart();
	getchar();
	rtcStop();
	getchar();
	rtcDispose();
	return 0;
}
#endif
