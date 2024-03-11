#include "HiLog.h"

CPacLog::CPacLog()
{
	m_fd = NULL;
	memset(m_logFileName, 0, sizeof(m_logFileName));
}
CPacLog::~CPacLog()
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
int CPacLog::pacLog(char* pData, int length, char* info, ...)
{

	va_list args;
	va_start(args, info);
	char extendDes[MAX_CHAR_SIZE] = {0};
	int len = sizeof(extendDes);
	vsnprintf(extendDes, len - 1, info, args);
	extendDes[len - 1] = '\0';
	//open new file>>
	FILE* pFd = openLogFile("w+");
	//printf("int CPacLog::pacLog(char* pData, int length, char* info, ...)\n");
	struct timeval logTime;
	gettimeofday(&logTime, NULL);
	time_t tt = logTime.tv_sec;
	char dateTime[128] = {0};
	ctime_r(&tt, dateTime);
	dateTime[strlen(dateTime) - 1] = '\0';
	fprintf(pFd, "\033[31m[%s:%s:%d]%s(%d.%06d):\n %s\033[0m", __FILE__,
			__FUNCTION__, __LINE__, dateTime, logTime.tv_sec, logTime.tv_usec,
			extendDes);

	for (int i = 0; i < length; i++)
	{
		if (i % 16 == 0)
		{
			fprintf(pFd, "%04X\t", i);
		}
		fprintf(pFd, "%02X ", pData[i] & 0xFF);
		if ((i + 1) % 8 == 0)
		{
			fprintf(pFd, "\t");
			if ((i + 1) % 16 == 0)
			{
				fprintf(pFd, "\n");
			}
		}
//		if (i == (length - 1))
//		{
//			fprintf(pFd, "\n");
//		}
	}
	fprintf(pFd, "=========================================================================\n");

	va_end(args);

	return 0;
}
