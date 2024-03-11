#pragma once

#include <iostream>
#include <map>
#include "BaseField.h"
#include "PublicInclude.h"

using namespace std;
//ZR>>
#define MAX_PAYLOAD_LEN 2048
// typedef struct
// {
// 	int icdID;
// 	int length;
// 	char payload[MAX_PAYLOAD_LEN];
// }TY_PACK_DATA;
//ZR<<

typedef map<string, BaseField*> TY_FIELD_MAP;
class CBlock
{
public:
	CBlock();
	~CBlock();
protected:
	std::string name;
	TY_FIELD_MAP m_fields;
	//int m_length;//byte

//ZR>>
public:
	string m_type;
	string m_ICDID;
	string m_name;
	int m_endianType;
	int m_length;//byte
	//int m_bitSize;
	unsigned char m_txBuffer[MAX_PAYLOAD_LEN];
	unsigned char m_rxBuffer[MAX_PAYLOAD_LEN];

	//TY_PACK_DATA m_txData;
	//TY_PACK_DATA m_rxData;
//ZR<<

public:
	int insertField(BaseField* pField);
	//TODO: delete field 
	int unpack(unsigned char* pData,int length);
	int pack(unsigned char* pData, int &length);
	BaseField* getField(string fieldName);

	int reset();//���Block
	void printFieldValue();

//ZR>>
public:
	int packTxICD();
	int getTxData(unsigned char* txData, int& length);
	int setRxData(unsigned char* rxData, int& length);//input
	int unpackRxICD();
//ZR<<	
};

