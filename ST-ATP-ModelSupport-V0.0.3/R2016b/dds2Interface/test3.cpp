#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include "dds2Interface.h"

int main()
{
    int ret = 0;
    /*Create dds com*/
    void* pRdCom = createSimulationCom("SimTx-100002-0",COM_MODE_RO);
    void* pWrCom = createSimulationCom("SimTx-100002-0",COM_MODE_WO);

    /*Write dds message*/
    int ddsKey = 100002;
    TY_COMMON_MSG txMsg;
    txMsg.payloadLen = 4;
    txMsg.sec = 12345;
    txMsg.usec = 56789;
    for(int i=0;i<txMsg.payloadLen;i++)
    {
        txMsg.payload[i] = i+1;
    }
    ret = writeMsgComm(pWrCom,ddsKey,txMsg);
    int txCount = 10000;
    while(txCount--)
    {
        if(ret != 0)
        {
            printf("Write msg failed.(%d)\n",ret);
            return ret;
        }

        usleep(10000);
    }   
    /*Read dds message*/
    TY_COMMON_MSG rxMsg;
    int rCount = 1;
    ret = readMsgComm(pRdCom, &rxMsg,ddsKey,&rCount);
    if(ret != 0)
    {
        printf("Read msg failed.(%d)\n",ret);
        return ret;
    }
    printf("Read msg:\n");
    for(int i=0;i<rxMsg.payloadLen;i++)
    {
        printf("%x ",rxMsg.payload[i]);
    }
    printf("\n");

    /*Dispose dds com*/
    disposeSimulationCom(pRdCom);
    disposeSimulationCom(pWrCom);
    return 0;
}
