/*
 * CrcFiled.h
 *
 *  Created on: 2022-3-28
 *      Author: root
 */

#ifndef CRCFILED_H_
#define CRCFILED_H_
#include "BaseField.h"

typedef struct crcInfo
{
	unsigned int polynomial;
	unsigned int initValue;
	unsigned int xorOutput;
	bool reflectInput;
	bool reflectOutput;
}TY_CRCINFO;
typedef enum
{
    UNKNOWN=1, CRC8, CRC16, CRC32, SUM, SUM_1188, BCC, LRC, XNOR
}CRC_TYPE;
class CrcField :
    public  BaseField
{

public:
    CrcField(unsigned char _bitLen, unsigned int _idx, const char* type, const char* name ,const char* calcType, int m_startByte,
    		int m_endByte, TY_CRCINFO crcInfo);
    CrcField(unsigned char _bitLen, unsigned int _idx, const char* type,const char* name ,const char* calcType,
    		int m_startByte, int m_endByte);
    virtual int encode(unsigned char* pData, int length);
    virtual int decode(unsigned char* pData, int length);
//ZR>>
    virtual void setData(unsigned long long data);
    virtual void getData(double* data);
    virtual void setDoubleData(unsigned long long data);
    TY_CRCINFO m_crcInfo;
    string m_strCalcType;
    unsigned int m_startByte;
    unsigned int m_endByte;
//    int defaultvalue;
//    int initCrcValue;

    unsigned int crc16_8005(unsigned int regInit, unsigned char* msgBuf, unsigned int msgBufLen);
    unsigned int crc16_1021_F(unsigned int regInit, unsigned char* msgBuf, unsigned int msgBufLen);
    unsigned int crc16_1021_0(unsigned int regInit, unsigned char* msgBuf, unsigned int msgBufLen);

    int computeCrc(unsigned char *pBuf,unsigned int len);
    int compareCrc(unsigned char *pBuf,unsigned int len);
    int checkCustom(unsigned char *ptr, int len, unsigned int *checkRes, int *checkResLen, char *checkDllName);
    unsigned short crc16Check(unsigned char *ptr, unsigned int len, int crcInitValue);

    unsigned char reverse8(unsigned char data);
    unsigned short reverse16(unsigned short data);
    unsigned int reverse32(unsigned int data);
    unsigned int crcTypeTransition(string type);
    unsigned short crc16(unsigned char* addr, int num, TY_CRCINFO info) ;
    unsigned int crc32(unsigned char* pBuf, int num, TY_CRCINFO info);
    unsigned char crc8(unsigned char* pBuf, int len, TY_CRCINFO info);
    unsigned char checkSum(unsigned char* pBuf, int len);
    unsigned short checkSum_1188(unsigned char* pBuf, int len);

    unsigned char checkBCC(unsigned char* pBuf, int len);
    unsigned char checkLRC(unsigned char* pBuf, int len);
    unsigned char checkXNOR(unsigned char* pBuf, int len);
};


#endif /* CRCFILED_H_ */
