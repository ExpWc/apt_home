#include <unistd.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

#ifndef  DDS2_INTERFACE
#define DDS2_INTERFACE

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

typedef void* (*createSimulationComFunc)(const char* ,COM_MODE );
typedef int (*readMsgCommFunc)(void* ,TY_COMMON_MSG* ,int ,int*);
typedef int (*writeMsgCommFunc)(void* ,int ,TY_COMMON_MSG );
typedef void (*disposeSimulationComFunc)(void* );

int loadLibrary(const char* soName);
bool isValidFunc();
#endif

static createSimulationComFunc createSimulationCom;
static readMsgCommFunc readMsgComm;
static writeMsgCommFunc writeMsgComm;
static disposeSimulationComFunc disposeSimulationCom;

int loadLibrary(const char* soName)
{
    void *handle;  
	char *error;  
  
	handle = dlopen (soName, RTLD_LAZY);  
	if (!handle) 
    {  
		printf ("%s ", dlerror());  
	}  
    
    createSimulationCom = (createSimulationComFunc)dlsym(handle, "createSimulationCom");  
	if (createSimulationCom == NULL)  
    {  
        if ((error = dlerror()) != NULL)  
        {  
		    printf ("%s", dlerror());  
        }
        return -1;
	}

    readMsgComm = (readMsgCommFunc)dlsym(handle, "readMsgComm");  
	if (readMsgComm == NULL) 
    {  
		if ((error = dlerror()) != NULL)  
        {  
		    printf ("%s", dlerror());  
        }
        return -1;
	}  

    writeMsgComm = (writeMsgCommFunc)dlsym(handle, "writeMsgComm");  
	if (writeMsgComm == NULL) 
    {  
		if ((error = dlerror()) != NULL)  
        {  
		    printf ("%s", dlerror());  
        }
        return -1;
	}  

    disposeSimulationCom = (disposeSimulationComFunc)dlsym(handle, "disposeSimulationCom");  
	if (disposeSimulationCom == NULL)  
    {  
		if ((error = dlerror()) != NULL)  
        {  
		    printf ("%s", dlerror());  
        }
        return -1;
	}   

    return 0;
}

bool isValidFunc()
{
    if(createSimulationCom == NULL || readMsgComm == NULL || writeMsgComm == NULL || disposeSimulationCom == NULL)
    {
        return false;
    }
    return true;
}
int test1();
int main()
{
    int ret = 0;
    if(!isValidFunc())
    {
        char soName[512];
        char* pEnv = getenv("SIMPOLE_HOME");
        if (pEnv == NULL) 
        {
            return -1;
        }
        sprintf(soName,"%s/lib/libdds2Interface.so",pEnv);

        ret = loadLibrary(soName);
        if(ret != 0)
        {
            return -1;
        }
    }
    
    if(!isValidFunc())
    {
        return -2;
    }

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
    int txCount = 1;
    while(txCount--)
    {
        txMsg.payload[0]++;
    ret = writeMsgComm(pWrCom,ddsKey,txMsg);
        if(ret != 0)
        {
            printf("Write msg failed.(%d)\n",ret);
            return ret;
        }
       // printf("Send %d\n",txMsg.payload[0]);
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
