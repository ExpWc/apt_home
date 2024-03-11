#include "HiLog.h"

const char *logTypeText[] =
{ "INFO", "WARNING", "ERROR", "CRITICAL", "FATAL" };

CLog::CLog()
{
	m_fd = NULL;
	memset(m_logFileName, 0, sizeof(m_logFileName));
}
CLog::~CLog()
{
	if (m_fd != NULL)
	{
		if (m_fd != stdout)
		{
			fclose(m_fd);
		}
		m_fd = NULL;
	}
}
CLog* CLog::m_instance = NULL;
CLog* CLog::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new CLog();
		printf("m_instance = new CLog():%p", m_instance);
	}
	printf("m_instance:%p", m_instance);
	return m_instance;
}

FILE* CLog::openLogFile(const char *openType)
{
	//char name[256];
	//FILE *file = NULL;
	//printf("m_fd %p\n", m_fd);

 	if (m_fd == NULL)
	{
		if ((strcmp(m_logFileName, "std") == 0) || (m_logFileName[0] == '\0'))
		{
			m_fd = stdout;
		}
		else
		{
			m_fd = fopen(m_logFileName, openType);
		}
	}

	return m_fd;
}

int CLog::setLogFileName(char * fileName)
{
	if (m_fd != NULL)
	{
		if (m_fd != stdout)
		{
			fclose(m_fd);
		}
		m_fd = NULL;
	}
	sprintf(m_logFileName, "%s", fileName);
}

int CLog::log(char* info, ...)
{
#ifdef AUTO_LOG
	va_list args;
	va_start(args, info);
	char extendDes[MAX_CHAR_SIZE] = {0};
	int len = sizeof(extendDes);
	vsnprintf(extendDes, len - 1, info, args);
	extendDes[len - 1] = '\0';

	//increment write>>
	FILE* pFd = openLogFile("a+");

	struct timeval logTime;
	gettimeofday(&logTime, NULL);
	time_t tt = logTime.tv_sec;
	char dateTime[128] = {0};
	ctime_r(&tt, dateTime);
	dateTime[strlen(dateTime) - 1] = '\0';
//	fprintf(pFd, "\033[31m[%s:%s:%d]%s(%d.%06d):\n %s\n\033[0m", __FILE__,
//			__FUNCTION__, __LINE__, dateTime, logTime.tv_sec, logTime.tv_usec,
//			extendDes);
	fprintf(pFd, "[%s:%s:%d]%s(%d.%06d):\n %s\n", __FILE__,
				__FUNCTION__, __LINE__, dateTime, logTime.tv_sec, logTime.tv_usec,
				extendDes);
	va_end(args);
	fflush(pFd);
#endif
	return 0;
}


int CLog::log(const char *fileName, const char *funName, int lineNo, char* info, ...)
{
#ifdef AUTO_LOG
	va_list args;
	va_start(args, info);
	char extendDes[MAX_CHAR_SIZE] = {0};
	int len = sizeof(extendDes);
	vsnprintf(extendDes, len - 1, info, args);
	extendDes[len - 1] = '\0';

	//increment write>>
	FILE* pFd = openLogFile("a+");

	struct timeval logTime;
	gettimeofday(&logTime, NULL);
	time_t tt = logTime.tv_sec;
	char dateTime[128] = {0};
	ctime_r(&tt, dateTime);
	dateTime[strlen(dateTime) - 1] = '\0';
//	fprintf(pFd, "\033[31m[%s:%s:%d]%s(%d.%06d):\n %s\n\033[0m", __FILE__,
//			__FUNCTION__, __LINE__, dateTime, logTime.tv_sec, logTime.tv_usec,
//			extendDes);
	fprintf(pFd, "[%s:%s:%d]%s(%d.%06d):\n %s\n", fileName, funName, lineNo,
			dateTime, logTime.tv_sec, logTime.tv_usec, extendDes);
	va_end(args);
	fflush(pFd);
#endif
	return 0;
}


int CLog::log(TY_LOG_TYPE logType, const char *fileName, const char *funName, int lineNo, char* info, ...)
{
#ifdef AUTO_LOG
	va_list args;
	va_start(args, info);
	char extendDes[MAX_CHAR_SIZE] = {0};
	int len = sizeof(extendDes);
	vsnprintf(extendDes, len - 1, info, args);
	extendDes[len - 1] = '\0';

	//increment write>>
	FILE* pFd = openLogFile("a+");

	struct timeval logTime;
	gettimeofday(&logTime, NULL);
	time_t tt = logTime.tv_sec;
	char dateTime[128] = {0};
	ctime_r(&tt, dateTime);
	dateTime[strlen(dateTime) - 1] = '\0';

	fprintf(pFd, "[%s:%s:%d]%s(%d.%06d):\n%s: %s\n", fileName, funName, lineNo,
			dateTime, logTime.tv_sec, logTime.tv_usec, logTypeText[logType], extendDes);
	va_end(args);
	fflush(pFd);
#endif
	return 0;
}
