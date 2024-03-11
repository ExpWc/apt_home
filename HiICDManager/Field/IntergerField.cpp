#include "IntergerField.h"
#include <type_traits>

IntergerField::IntergerField(unsigned char _bitLen, unsigned int _idx, unsigned char _offset,const char* type, const char* name)
    :BaseField(_bitLen,_idx,_offset, type,name)
{
    switch (m_srcValue.type)
    {
    case V_CHAR:
    case V_SHORT:
    case V_INT:
    case V_LONG:
    case V_LONGLONG:
    case V_FLOAT:
    case V_DOUBLE:
    //case V_BOOL:
        m_isUnsigned = false;
        break;
    default:
        m_isUnsigned = true;
        break;
    }
}
/***********************************************************************
* ����������ݵ�ԭʼֵ�����ԭʼ���ݰ���
************************************************************************/
int IntergerField::encode(unsigned char* pData, int length)///getdata
{
	double valDouble;
    REPORT(REPORT_DEBUG,"m_bitOffset:%d, m_bitLen:%d, m_mask:%lld\n", m_bitOffset, m_bitLen, m_mask);
    int startByte = m_bitOffset / 8;
    int endByte = (m_bitLen + m_bitOffset) / 8;
    if ((m_bitLen + m_bitOffset) % 8 != 0)
    {
        endByte += 1;
    }
    if (m_isUnsigned)
    {
        unsigned long long tmpVal = ((unsigned long long)(m_srcValue.getULongLong() << m_bitOffset) & m_mask);
        for (int i = startByte; i < endByte; i++)
        {
            pData[m_byteOffset + i] |= ((char*)&tmpVal)[i];
        }
        memcpy(&valDouble, pData, 8);
		//REPORT(REPORT_ERROR, "====valDouble : %lf.  tmpVal: %lld, m_byteOffset:%d, endByte:%d\n", valDouble, tmpVal, m_byteOffset, endByte);
    }
    else
    {
        long long tmpVal = ((long long)(m_srcValue.getLongLong() << m_bitOffset) & m_mask);
        for (int i = startByte; i < endByte; i++)
        {
            pData[m_byteOffset + i] |= ((char*)&tmpVal)[i];
        }
        memcpy(&valDouble, pData, 8);
		//REPORT(REPORT_ERROR, "====valDouble : %lf.  tmpVal: %lld, m_byteOffset:%d, endByte:%d\n", valDouble, tmpVal, m_byteOffset, endByte);
    }
    return 0;
}


/***********************************************************************
* �������ԭʼ���ݰ��õ����ԭʼֵ
************************************************************************/
int IntergerField::decode(unsigned char* pData, int length)////set data, put data in the field
{
    unsigned long long lData = 0;

    int cpLen = length - m_byteOffset;
    if(cpLen > 8)
    {
        cpLen = 8;
    }
    memcpy(&lData, pData + m_byteOffset, cpLen);

    if (m_isUnsigned)
    {
        unsigned long long val = ((unsigned long long)(lData & m_mask)) >> m_bitOffset;
        m_srcValue.setULongLong(val);
    }
    else
    {
        long long val = ((long long)(lData & m_mask)) << m_shiftL >> m_shiftR;
        m_srcValue.setLongLong(val);
    }
    //printf("m_byteOffset %d cpLen %d 0x%llx\n",m_byteOffset,cpLen,lData);
    return 0;
}

//ZR>>
void IntergerField::setData(unsigned long long data)
{
    unsigned long long val = ((unsigned long long)(data & (m_mask >> m_bitOffset)));
    m_srcValue.setULongLong(val);
    //printf("-----------------setData value= %ld, data = %lld", val,data);
    //printf("--void IntergerField::setData(unsigned long long data)>>val %lld m_mask 0x%x m_bitOffset %d\n", m_srcValue.getULongLong(), m_mask, m_bitOffset);
}
void IntergerField::setDoubleData(unsigned long long data)
{	
	double rtuDoule = 0;
	double rtuLL = 0;
    double tmp = 0;
    memcpy(&tmp, &data, 8);
    //printf("--->tmp:%lf  ", tmp);
    tmp = (tmp - m_offsetValue) / m_lsb;//127/10 = 12.7
    //printf("--->tmp %lf m_lsb %lf \n", tmp, m_lsb);
    tmp += 0.5;
    unsigned long long tmpData = (unsigned long long)tmp;
    //printf("--->tmp %lf tmpData %lld m_lsb %lf \n",
    //    			tmp, tmpData, m_lsb);
    unsigned long long val = ((unsigned long long)(tmpData & (m_mask >> m_bitOffset)));//engineer to actual
	m_srcValue.setULongLong(val);
	rtuDoule = m_srcValue.getDouble();
	rtuLL = m_srcValue.getULongLong();
	//REPORT(REPORT_ERROR, "===tmp : %lf,====rtuDoule : %lf.val : %lld,rtuLL:%lld\n", tmp, rtuDoule, val,rtuLL);
	//printf("--void IntergerField::setDoubleData(unsigned long long data)>>tmp %lf tmpData %lld val %lld m_mask 0x%x m_bitOffset %d\n",
			//tmp, tmpData, m_srcValue.getULongLong(), m_mask, m_bitOffset);
}
void IntergerField::getData(double* data)
{
	double tmpDouble = 0;
	if (m_isUnsigned)
	{
		unsigned long long tmp = ((unsigned long long)(m_srcValue.getULongLong() << m_bitOffset) & m_mask) >> m_bitOffset;
		//printf("--uuid:%s IntergerField::getData(double* data)>>%llx m_mask 0x%x m_bitOffset %d\n",m_uuid.c_str(),m_srcValue.getULongLong(), m_mask, m_bitOffset);
		tmpDouble = (double)tmp * m_lsb + m_offsetValue;
	}
    else
    {
    	tmpDouble = ((m_srcValue.getLongLong() << (64-m_bitLen)))  >> (64-m_bitLen);
    	//printf("--uuid:%s IntergerField::getData(double* data)>>%llx m_mask 0x%x m_bitOffset %d\n",m_uuid.c_str(),m_srcValue.getLongLong(), m_mask, m_bitOffset);
    	tmpDouble = (double)tmpDouble * m_lsb + m_offsetValue;
    }

 //   printf("--tmpDouble %lf m_mask 0x%x m_bitOffset %d m_byteOffset %d\n", tmpDouble, m_mask, m_bitOffset, m_byteOffset);
	//REPORT(REPORT_ERROR, "--tmpDouble %lf m_mask 0x%x m_bitOffset %d m_byteOffset %d\n", tmpDouble, m_mask, m_bitOffset, m_byteOffset);
    *data = tmpDouble;
}
