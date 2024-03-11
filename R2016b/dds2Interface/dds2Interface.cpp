#include "dds2Interface.h"
#include "CommunicationFactory.h"
#include <sys/time.h>

using namespace com::ceasw::network::simulation;

void* createSimulationCom(const char* partition,COM_MODE mode)
{
    ISimulationComm* com = CommunicationFactory::createSimulationComm(partition, (CommMode)mode);
    return com;
}


int readMsgComm(void* pCom,TY_COMMON_MSG* pMsg,int ddsKey,int *pCount)
{
    SimulationDataSeq dataSeq;
    int count = *pCount;
    if(pCom == NULL)
    {
        return -1;
    }
	int ret = ((ISimulationComm*)pCom)->read(10, dataSeq);
    if(ret != 0) 
    {
        *pCount = 0;
        return -2;
    }
    else
    {
        int readLen = dataSeq.length();
        if(readLen >  count)
        {
            readLen = count;
        }
        else
        {
            *pCount = readLen;
        }

        for(int i=0;i<readLen;i++)
        {
            pMsg[i].payloadLen = dataSeq[i].payloads.rawdata().length();
            unsigned char * pBuffer = dataSeq[i].payloads.rawdata().get_buffer();
            memcpy(pMsg[i].payload,pBuffer,pMsg[i].payloadLen);
        }
    }	
    return 0;
}

int writeMsgComm(void* pCom,int ddsKey,TY_COMMON_MSG txMsg)
{
    if(pCom == NULL)
    {
        return -1;
    }
    SimulationData data;

    SimulationType::STIMULATE_Info stiInfo;
    stiInfo.flag = 1;
    //RawdataType payload;
    char *mask = (char*) malloc(txMsg.payloadLen);
    stiInfo.payload_mask.replace(txMsg.payloadLen,txMsg.payloadLen,(DDS::Octet*)mask);
    //istiInfo.payload_mask.rawdata(payload);
    
    struct timeval tval;
    gettimeofday(&tval, NULL);

    data.id = ddsKey;
    data.info.stimulateInfo(stiInfo);
    data.src = 0;
    data.sec = tval.tv_sec;
    data.usec = tval.tv_usec;
    RawdataType payload;
    payload.replace(txMsg.payloadLen, txMsg.payloadLen, (DDS::Octet*)txMsg.payload);
    data.payloads.rawdata(payload);
    int ret = ((ISimulationComm*)pCom)->write(data);
    if(ret != 0)
    {
        return -2;
    }
    delete mask;
    return 0;
}

void disposeSimulationCom(void* pCom)
{
    if(pCom!=NULL)
    {
        ((ISimulationComm*)pCom)->dispose();
        pCom = NULL;
    }
}
