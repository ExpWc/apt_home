#pragma once


#ifdef WIN32

unsigned long __stdcall threadRecvFromSil(void *pParam);
unsigned long __stdcall threadRecvFromHil(void *pParam);
unsigned long __stdcall threadRecvFromIil(void *pParam);
unsigned long __stdcall sendDataToDDS(void *pParam);

#else
void* threadRecvFromSil(void *pParam);
void* threadRecvFromHil(void *pParam);
void* threadRecvFromIil(void *pParam);
void* sendDataToDDS(void *pParam);
#endif
