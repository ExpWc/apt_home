#ifndef _DDS2_INTERFACE_H
#define _DDS2_INTERFACE_H

#define MAX_DDS_PAYLOAG_LEN 2048

typedef enum _ComMode
{
	COM_MODE_RO, COM_MODE_WO, COM_MODE_RW
}COM_MODE;

typedef struct _commonMsg
{
    int sec;
    int usec;
    int payloadLen;
    char payload[MAX_DDS_PAYLOAG_LEN];
}TY_COMMON_MSG;


#ifdef __cplusplus
    extern "C"
    {
#endif


void* createSimulationCom(const char* partition,COM_MODE mode);
int readMsgComm(void* pCom,TY_COMMON_MSG* pMsg,int ddsKey,int *pCount);
int writeMsgComm(void* pCom,int ddsKey,TY_COMMON_MSG txMsg);
void disposeSimulationCom(void* pCom);



#ifdef __cplusplus
    };
#endif

#endif
