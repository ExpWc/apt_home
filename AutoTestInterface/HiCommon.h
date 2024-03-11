/*
 * user.h
 *
 *  Created on: 2013-12-23
 *      Author: root
 */

#ifndef USER_H_
#define USER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <unistd.h>

inline void getExePath(char *pFilePath)
{
	//get app path>>
	char buf[1024] =
	{ 0 };
	int n;
	n = readlink("/proc/self/exe", buf, sizeof(buf));
	if ((n < 0) || (n > (int)sizeof(buf)))
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
#else
#include <WinSock2.h>
#define _WINSOCKAPI_
#include "windows.h"
#include <time.h>
#include "HiReport.h"
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
    { 0 };
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
	char tmp_name[MAX_PATH];
	str_name = getProcessName(fullName);

	strcpy(tmp_name, (char*)str_name);
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
inline void gettimeofday(struct timeval *tp, struct timezone * /* = 0 */)
{
	SYSTEMTIME lpSystemTime;
	time_t t;
	t = time(NULL);
	GetSystemTime(&lpSystemTime); 
	tp->tv_usec = lpSystemTime.wMilliseconds*1000;
	tp->tv_sec = t;
}
#endif


#endif /* USER_H_ */
