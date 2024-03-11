#pragma once
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#define MAX_CHAR_SIZE 2048
#define AUTO_LOG

#define LOG(type, description, args...) \
	CLog::getInstance()->log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)


typedef enum _log_type
{
    LOG_INFO = 0,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL,
    LOG_FATAL,
} TY_LOG_TYPE;

class CLog
{
public:
	CLog();
	~CLog();

public:
	static CLog* getInstance();
public:
	int setLogFileName(char * fileName);
	int log(char* info, ...);
	int log(const char *fileName, const char *funName, int lineNo, char* info, ...);
	int log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
			int lineNo, char* info, ...);
	FILE *openLogFile(const char *openType);
public:
	FILE * m_fd;
	char m_logFileName[512];

	static CLog* m_instance;
};



class CPacLog : public CLog
{
public:
	CPacLog();
	~CPacLog();

public:
	int pacLog(char* pData, int length, char* info, ...);
};
