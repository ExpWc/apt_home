/*
 * user.h
 *
 *  Created on: 2013-12-23
 *      Author: root
 */

#ifndef USER_H_
#define USER_H_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <time.h>
#include "HiReport.h"
//#define __MINGW32__
#else
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>
#endif
#define TYPE_NUM 15
#define MAX_MSG_LEN  4096	//modified by ping.song
#define MAX_STACK (64*1024*8)
#define SIZEINT   sizeof(int)
#define SIZEUINT  sizeof(unsigned int)
#define SIZELONG  sizeof(long)
#define SIZEULONG sizeof(unsigned long)


enum TY_BUS_TYPE
{
	BUS429 = 0,
	BUS717,
	BUS708,
	BUS1553,
	BUS422,
	BUSAFDX,
	BUSCAN,
	BUSRFM,
	BUSNOBUS,
	BUSIMBADB,
	BUSETH,
	BUSHDLC,
	BUSHIL,
	BUSSIL,
	BUSFLEXRAY
};
#ifdef _WIN32
inline void sleep(int sec)
{
	Sleep(sec*1000);
}
/*
 * Sets timeval* to current time of day. tv_usec contains only millisec
 * resolution.
 */
inline void gettimeofday(struct timeval* tp, void* timezone)
{
	SYSTEMTIME lpSystemTime;
	time_t t;
	t = time(NULL);
	GetSystemTime(&lpSystemTime);
	tp->tv_usec = lpSystemTime.wMilliseconds*1000;
	tp->tv_sec = t;
}
/*******************************************************************************
 * Function Name  : getProcessPath
 * Description    : get the proc  path
 * Input          : None
 * Output         : char *path:the proc path: /home// .ex
 * Return         : char * the path.
 *******************************************************************************/
inline char * getServerPath(char *path)
{
	TCHAR fullPath[MAX_PATH] =
	{	0};
	GetModuleFileName(NULL,fullPath,MAX_PATH);
	char tmp = '\\';
	char* str_name;
	char tmp_name[MAX_PATH];

	strcpy(tmp_name, (char*)fullPath);
	str_name = strrchr(tmp_name, tmp);
	if (NULL != str_name)
	{
		*(str_name + 1) = '\0';
		strcpy(path, tmp_name);
	}
	else
	{
		return NULL;
	}
	return path;
}

/*******************************************************************************
 * Function Name  : getServerName
 * Description    : get the proc  name(without .exe)
 * Input          : None
 * Output         : char * name:the proc name: exe .ex
 * Return         : char * the name.
 *******************************************************************************/
inline char* getExePath(char* name)
{
	char fullName[MAX_PATH];
	char tmp = '.';
	char *str_name;
	char *str_name1;
	char tmp_name[MAX_PATH];
	TCHAR fullPath[MAX_PATH] =
	{	0};
	GetModuleFileName(NULL,fullPath,MAX_PATH);
	//char tmp1 = '\\';

	//strcpy(tmp_name, (char*)fullPath);
	//str_name1 = strrchr(tmp_name, tmp1);
	//if (NULL != str_name1)
	//{
	//	*(str_name1 + 1) = '\0';
	//	strcpy(fullName, tmp_name);
	//}
	//str_name = getProcessName(fullName);

	strcpy(tmp_name, (char*)fullPath);
	str_name = strrchr(tmp_name, tmp);
	if (NULL != str_name)
	{
		*(str_name) = '\0';
		strcpy(name, tmp_name );
	}
	else
	{
		return NULL;
	}

}

inline int getDiskCapacity()
{
	unsigned int disk_room = 0;
	int MB = 1024*1024;
	char dName[4] = "D:";
	ULARGE_INTEGER nFreeBytesAvailabel;
	ULARGE_INTEGER nTotalNumberOfBytes;
	ULARGE_INTEGER nTotalNumberOfFreeBtes;
	int ret = 0;

	if(GetDiskFreeSpaceEx(dName,
					&nFreeBytesAvailabel,
					&nTotalNumberOfBytes,
					&nTotalNumberOfFreeBtes) != 0)
	{
		disk_room = nTotalNumberOfFreeBtes.QuadPart/MB;
	}
	else
	{
		return -1;
	}
	return disk_room;
}
#else
inline void getExePath(char *pFilePath)
{
	//get app path>>
	char buf[1024] =
	{ 0 };
	int n;
	n = readlink("/proc/self/exe", buf, sizeof(buf));
	if ((n < 0) || (n > (int) sizeof(buf)))
	{
		sprintf(pFilePath, "%s", "/TEST");//use "/"
		return;
	}
	sprintf(pFilePath, "%s", buf);
	return;
}
inline char * getServerPath(char* path)
{

	char name[256];
	getExePath(name);
	char tmp = '/';
	char* str_name;
	char tmp_name[256];
	if (name == NULL)
		return NULL;
	strcpy(tmp_name, name);
	str_name = strrchr(tmp_name, tmp);
	if (NULL != str_name)
	{
		*(str_name + 1) = '\0';
		strcpy(path, tmp_name);
	}
	else
	{
		return NULL;
	}
	return path;
}
inline int getDiskCapacity()
{
	struct statfs diskInfo;
	statfs("/storage", &diskInfo);
	//    if(diskInfo.f_bavail)
	unsigned long long blockSize = diskInfo.f_bsize;
	unsigned long long blockCount = diskInfo.f_blocks;
	unsigned long long totolSize = blockSize * blockCount;
	totolSize = totolSize >> 20;
	unsigned long long freeDisk = diskInfo.f_bavail * blockSize;
	freeDisk = freeDisk >> 20;
	return freeDisk;
}
#endif

#endif /* USER_H_ */
