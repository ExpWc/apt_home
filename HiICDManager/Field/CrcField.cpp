#include "CrcField.h"
#include <iostream>
using namespace std;

CrcField::CrcField(unsigned char _bitLen, unsigned int _idx, const char* type, const char* name ,const char* calcType, int startByte,
		int endByte, TY_CRCINFO crcInfo)
    :BaseField(_bitLen,_idx, 0, type, name)
{
	m_strCalcType = calcType;
	m_startByte = startByte;
	m_endByte = endByte;
	m_crcInfo = crcInfo;

}
CrcField::CrcField(unsigned char _bitLen, unsigned int _idx, const char* type, const char* name ,const char* calcType, int startByte,
		int endByte)
    :BaseField(_bitLen,_idx, 0, type,name)
{
	m_strCalcType = calcType;
	m_startByte = startByte;
	m_endByte = endByte;

}


int CrcField::encode(unsigned char* pData, int length)
{
	 computeCrc(pData, length);
//    double val = m_srcValue.getDouble();
//    memcpy(pData + m_byteOffset, &val, sizeof(double));
    return 0;
}

int CrcField::decode(unsigned char* pData, int length)
{
	unsigned long long val = 0;
	int ret = -1;
	ret = compareCrc(pData, length);
	if(ret == 0)
	{
		memcpy(&val, pData + m_byteOffset, m_bitLen/8);
		m_srcValue.setULongLong(val);
	}

	return ret;
}


//ZR>>
void CrcField::setData(unsigned long long data)
{
    double tmp = 0;

    //memcpy(&tmp, &data, 8);
    //m_srcValue.setDouble(tmp);
}
void CrcField::setDoubleData(unsigned long long data)
{
    double tmp = 0;
    memcpy(&tmp, &data, 8);
    REPORT(REPORT_INFO, "tmp : %lf.\n", tmp);
    m_srcValue.setDouble(tmp);
}
void CrcField::getData(double* data)
{
    double tmp = m_srcValue.getDouble();
    *data = tmp;
}


unsigned char CrcField::reverse8(unsigned char data)
{
	unsigned char temp = 0;
    // �ֽڷ�ת
    for (unsigned char i = 0; i < 8; i++) {
        temp |= ((data >> i) & 0x01) << (7 - i);
    }
    return temp;
}

unsigned short CrcField::reverse16(unsigned short data)
{
	unsigned short temp = 0;
	for (unsigned char i = 0; i < 16; i++)
	{
		temp |= ((data >> i) & 0x0001) << (15 - i);
	}
	return temp;
}


unsigned int CrcField::reverse32(unsigned int data)
{
	unsigned int temp = 0;
	for (unsigned char i = 0; i < 32; i++)
	{
		temp |= ((data >> i) & 0x01) << (31 - i);
	}
	return temp;
}


unsigned char CrcField::crc8(unsigned char* pBuf, int len, TY_CRCINFO info)
{
	unsigned char  data;
	unsigned char  crc = (unsigned char)info.initValue; //��ʼֵ
	for (; len > 0; len--)
	{
		data = *pBuf++;
		if (info.reflectInput == true)
		{
			data = reverse8(data);
		}

        //��crc��ʼֵ���
		crc = crc ^ data;
		for (int i = 0; i < 8; i++)
		{
		//�����Ƴ���λΪ1�����ƺ������ʽ���
			if (crc & 0x80)
			{
				crc = (crc << 1) ^ (unsigned char)info.polynomial;
			}
			else
			{
				crc <<= 1;
			}
        }
    }
    //������������ת
	if (info.reflectOutput == true)
	{
		crc = reverse8(crc);
	}
    crc = crc ^ (unsigned char)info.xorOutput; //����������ֵ���
    return (crc);            //��������У��ֵ
}


unsigned short CrcField::crc16(unsigned char* pBuf, int len, TY_CRCINFO info)
{
	unsigned char  data;
	unsigned short crc = info.initValue; //��ʼֵ
	for (; len > 0; len--)
	{
		data = *pBuf++;
		if (info.reflectInput == true)
		{
			data = reverse8(data);
		}
		//��crc��ʼֵ��8λ���
		crc = crc ^ (data << 8);
		for (int i = 0; i < 8; i++)
		{
			//�����Ƴ���λΪ1�����ƺ������ʽ���
			if (crc & 0x8000)
			{
				crc = (crc << 1) ^ info.polynomial;
			}
			else
			{
				crc <<= 1;
			}
		}
	}
    //������������ת
    if (info.reflectOutput == true) {
        crc = reverse16(crc);
    }
    crc = crc ^ info.xorOutput; //����������ֵ���
    return (crc);            //��������У��ֵ
}

unsigned int CrcField::crc32(unsigned char* pBuf, int len, TY_CRCINFO info)
{
	unsigned char  data;
	unsigned int crcResult = info.initValue; //��ʼֵ

	for (; len > 0; len--)
	{
		data = *pBuf++;
		if (info.reflectInput == true)
		{
			data = reverse8(data);
		}
		// ��crc��ʼֵ��8λ���
		crcResult = crcResult ^ (data << 24);
		for (int i = 0; i < 8; i++)
		{
			// �����Ƴ���λΪ1�����ƺ������ʽ���
			if (crcResult & 0x80000000)
			{
				crcResult = (crcResult << 1) ^ info.polynomial;
			}
			else
			{
				crcResult <<= 1;
			}
		}
	}
	//������������ת
	if (info.reflectOutput == true)
	{
		crcResult = reverse32(crcResult);
	}
	crcResult = crcResult ^ info.xorOutput; //����������ֵ���
	return (crcResult);            //��������У��ֵ
}

unsigned char CrcField::checkSum(unsigned char* pBuf, int len)
{
	unsigned long checkResult = 0;
	for(int i = 0; i <len; i++)
	{
		checkResult+=pBuf[i];
	}
	if(checkResult > 0xff)
	{
		checkResult = ~checkResult;
		checkResult = checkResult +1;
	}

	return (unsigned char)checkResult;

}

unsigned short CrcField::checkSum_1188(unsigned char* pBuf, int len)
{
	//unsigned long checkResult = 0;
	unsigned short value = 0;
	unsigned short tmpValue;

   memcpy(&tmpValue, pBuf, 2);
   unsigned short checkResult = tmpValue;
   int length = len/2;
	for(int i =  1; i < length; i++)
	{
		memcpy(&tmpValue, pBuf + i * 2, 2);
		value = (unsigned short)((tmpValue >> (i % 16)) | (tmpValue << (16 - i % 16)));
		checkResult = (checkResult ^ value);
	}
	checkResult = (unsigned short)((checkResult << (length % 16)) | (checkResult >> (16 - length % 16)));
	return (unsigned short)checkResult;

}
unsigned char CrcField::checkBCC(unsigned char* pBuf, int len)
{
	unsigned char checkResult = 0;
	for(int i = 0; i < len; i++)
	{
		checkResult ^= pBuf[i];
	}
	return checkResult;
}
unsigned char CrcField::checkLRC(unsigned char* pBuf, int len)
{
	unsigned char checkResult = 0;
	for(int i = 0; i < len; i++)
	{
		checkResult += pBuf[i];
	}
	return (checkResult ^ 0xff) + 1;
}
unsigned char CrcField::checkXNOR(unsigned char* pBuf, int len)
{
	return 0;
}
unsigned int CrcField::crcTypeTransition(string type)
{
	if(type == "CRC16")
	{
		return CRC16;
	}
	else if(type == "CRC32")
	{
		return CRC32;
	}
	else if(type == "SUM")
	{
		return SUM;
	}
	else if(type == "SUM_1188")
	{
		return SUM_1188;
	}
	else if(type == "BCC")
	{
		return BCC;
	}
	else if(type == "LRC")
	{
		return LRC;
	}
	else if(type == "XNOR")
	{
		return XNOR;
	}
	else
	{
		return UNKNOWN;
	}
}
int CrcField::computeCrc(unsigned char *pBuf,unsigned int len)
{
	int ret = 0;
	unsigned long long crcResult;
	double tmp = 0;
	switch(crcTypeTransition(m_strCalcType))
    {
	case UNKNOWN:
	{
		ret = -1;
	}
	break;
	case CRC8:
	{
		crcResult = crc8(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+m_byteOffset, &crcResult, sizeof(char));
		ret = 0;
	}
	break;
	case CRC16:
	{
		crcResult = crc16(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+m_byteOffset, &crcResult, sizeof(short));
		ret = 0;
	}
	break;
	case CRC32:
	{
		crcResult = crc32(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+m_byteOffset, &crcResult, sizeof(long));
		ret = 0;
	}
	break;
	case SUM:
	{
		crcResult = checkSum(pBuf + m_startByte, m_endByte - m_startByte);
		memcpy(pBuf+m_byteOffset, &crcResult, sizeof(char));
		ret = 0;
	}
	break;

	case SUM_1188:
	{
		crcResult = checkSum_1188(pBuf + m_startByte, m_endByte - m_startByte);
		memcpy(pBuf+m_byteOffset, &crcResult, sizeof(short));
		ret = 0;
	}
	break;

	case BCC:
	{
		crcResult = checkBCC(pBuf + m_startByte, m_endByte - m_startByte);
		memcpy(pBuf+m_byteOffset, &crcResult, sizeof(char));
		ret = 0;
	}
	break;

	case LRC:
	{
		crcResult = checkLRC(pBuf + m_startByte, m_endByte - m_startByte);
		memcpy(pBuf+m_byteOffset, &crcResult, sizeof(char));
		ret = 0;
	}
	break;
	case XNOR:
	{
		ret = -1;
	}
	break;
   default:
        ret = -1;
   break;
    }
    return ret;

}

int CrcField::compareCrc(unsigned char *pBuf,unsigned int len)
{
	int ret = -1;
	unsigned int crcResult;
	unsigned int crcTmpValue;
	switch(crcTypeTransition(m_strCalcType))
    {
	case UNKNOWN:
	{
		ret = -1;
	}
	break;
	case CRC8:
	{
		crcResult = crc8(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+ m_byteOffset, &crcTmpValue, sizeof(char));
		if(crcResult == crcTmpValue)
		{
			ret = 0;
		}
	}
	break;
	case CRC16:
	{
		crcResult = crc8(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+ m_byteOffset, &crcTmpValue, sizeof(char));
		if(crcResult == crcTmpValue)
		{
			ret = 0;
		}
	}
	break;
	case CRC32:
	{
		crcResult = crc8(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+ m_byteOffset, &crcTmpValue, sizeof(char));
		if(crcResult == crcTmpValue)
		{
			ret = 0;
		}
	}
	break;
	case SUM:
	{
		crcResult = crc8(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+ m_byteOffset, &crcTmpValue, sizeof(char));
		if(crcResult == crcTmpValue)
		{
			ret = 0;
		}
	}
	break;

	case SUM_1188:
	{
		crcResult = crc8(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+ m_byteOffset, &crcTmpValue, sizeof(char));
		if(crcResult == crcTmpValue)
		{
			ret = 0;
		}
	}
	break;

	case BCC:
	{
		crcResult = crc8(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+ m_byteOffset, &crcTmpValue, sizeof(char));
		if(crcResult == crcTmpValue)
		{
			ret = 0;
		}
	}
	break;

	case LRC:
	{
		crcResult = crc8(pBuf + m_startByte, m_endByte - m_startByte, m_crcInfo);
		memcpy(pBuf+ m_byteOffset, &crcTmpValue, sizeof(char));
		if(crcResult == crcTmpValue)
		{
			ret = 0;
		}
	}
	break;
	case XNOR:
	{
		ret = -1;
	}
	break;
    default:
        ret = -1;
        break;
    }
    return ret;

}
