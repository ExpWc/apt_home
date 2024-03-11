#pragma once
#include "BaseStep.h"
inline  void showPac(char* pData,int length)
{
	printf("=================char len:%d================\n",length);
	for(int i=0;i<length;i++)
	{
		if(i%16==0)
		{
			printf("%02X\t",i/16);
		}
		printf("%02X ",pData[i]&0xFF);
		if((i+1)%8==0)
		{
			printf("\t");
			if((i+1)%16==0)
			{
				printf("\n");
			}
		}
	}
}

inline void strPac(const char* pData,int length, char* info)
{
	char tmp[1024];
	char tmpValue = 0;
	sprintf(tmp, "=================%d================\n",length);
	strcat(info, tmp);
	for(int i=0;i<length;i++)
	{
		if(i%16==0)
		{
			sprintf(tmp,"%02X\t",i/16);
			strcat(info, tmp);
		}
		tmpValue = pData[i];
		sprintf(tmp, "%02X ",tmpValue&0xFF);
		strcat(info, tmp);
		if((i+1)%8==0)
		{
			sprintf(tmp, "\t");
			strcat(info, tmp);
			if((i+1)%16==0)
			{
				sprintf(tmp, "\n");
				strcat(info, tmp);
			}
		}
	}
}
//inline  void savePac(char* pData,int length)
//{
//	FILE* pFd = fopen("/tmp/data.txt","a+");
////	fprintf(pFd,"chn:%d count:%d packetlen:%d \n", pDmaHeader->channelNo, pDmaHeader->frameNum, pDmaHeader->packetLen);
//	for(int i=0;i<length;i++)
//	{
//		if(i%16==0)
//		{
//			fprintf(pFd,"%02X\t",i/16);
//		}
//		fprintf(pFd,"%02X ",pData[i]&0xFF);
//		if((i+1)%8==0)
//		{
//			fprintf(pFd,"\t");
//			if((i+1)%16==0)
//			{
//				fprintf(pFd,"\n");
//			}
//		}
//	}
//	fprintf(pFd,"\n");
//	fclose(pFd);
//}

inline void savePac(int card, int chn, char* pData, int length)
{
	FILE* pFd = fopen("./1553.txt", "a+");
	fprintf(pFd, "==============422===%d-%d %d================\n", card, chn,
			length);
	for (int i = 0; i < length; i++)
	{
		if (i % 16 == 0)
		{
			fprintf(pFd, "%02X\t", i / 16);
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
	}
	fprintf(pFd, "\n");
	fclose(pFd);
}

#define COMSTEPLOG(type, description, args...) \
	CBaseCommunicationStep::log(type, __FILE__, __FUNCTION__, __LINE__, description, ##args)

class CBaseCommunicationStep : public CBaseStep
{
public:
    CBaseCommunicationStep();
    ~CBaseCommunicationStep();

public:
    TY_ROUTER m_router;
    int parseRouter(TiXmlElement *pStepElement);
    int log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
			int lineNo, string msg);
};
