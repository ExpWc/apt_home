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
inline int stringConvertType(string value, string type, unsigned long long & setValue)
{
    
    int varLen = 8;
    memset(&setValue, 0, varLen);
    if((type == "double")||(type == "float"))
    {
        double data = atof(value.c_str());
        varLen = 8;

        memcpy(&setValue, &data, varLen);
        //REPORT(REPORT_INFO, "data %lf value.c_str() %s m_setValue 0x%llx\n",
        //		data, value.c_str(), setValue);
    }
    else if(type == "float")
    {
        float data = (float)atof(value.c_str());
        varLen = 4;
        memcpy(&setValue, &data, varLen);
    }
    else if((type == "int")||(type == "unsigned int"))
    {
        int data = atoi(value.c_str());
        varLen = 4;
        memcpy(&setValue, &data, varLen);
    }
    else if((type == "short")||(type == "unsigned short"))
    {
        short data = (short)atoi(value.c_str());
        varLen = 2;
        memcpy(&setValue, &data, varLen);
    }
    else if((type == "char")||(type == "unsigned char"))
    {
        char data = (char)atoi(value.c_str());
        varLen = 1;
        memcpy(&setValue, &data, varLen);
    }
    else if(type == "bool")
    {
        bool data = (char)atoi(value.c_str());
        varLen = 1;
        memcpy(&setValue, &data, varLen);
    }
    else if((type == "long")||(type == "unsigned long"))
    {
        long data = atol(value.c_str());
        varLen = 8;
        memcpy(&setValue, &data, varLen);
    }
    else if((type == "long long")||(type == "unsigned long long"))
    {
        long long data = atol(value.c_str());
        varLen = 8;
        memcpy(&setValue, &data, varLen);
    }
    else
    {
        //REPORT(REPORT_ERROR, "Type is inconsistent\n");
        return -1;
    }
    
    return 0;
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
