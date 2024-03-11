#include "BaseField.h"

unsigned long long MASK_TABLE[] {
            1,3,7,0xf,0x1f,0x3f,0x7f,0xff,
            0x1ff,0x3ff,0x7ff,0xfff,0x1fff,0x3fff,0x7fff,0xffff,
            0x1ffff,0x3ffff,0x7ffff,0xfffff,0x1fffff,0x3fffff,0x7fffff,0xffffff,
            0x1ffffff,0x3ffffff,0x7ffffff,0xfffffff,0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff,
            0x1ffffffff,0x3ffffffff,0x7ffffffff,0xfffffffff,0x1fffffffff,0x3fffffffff,0x7fffffffff,0xffffffffff,
            0x1ffffffffff,0x3ffffffffff,0x7ffffffffff,0xfffffffffff,0x1fffffffffff,0x3fffffffffff,0x7fffffffffff,0xffffffffffff,
            0x1ffffffffffff,0x3ffffffffffff,0x7ffffffffffff,0xfffffffffffff,0x1fffffffffffff,0x3fffffffffffff,0x7fffffffffffff,0xffffffffffffff,
            0x1ffffffffffffff,0x3ffffffffffffff,0x7ffffffffffffff,0xfffffffffffffff,0x1fffffffffffffff,0x3fffffffffffffff,0x7fffffffffffffff,0xffffffffffffffff
};

BaseField::BaseField(unsigned char _bitLen, unsigned int _idx, unsigned char _offset, const char* type, const char* name)
{
    m_mask = (MASK_TABLE[_bitLen - 1] << _offset);
    m_byteOffset = _idx;
    m_bitLen = _bitLen;
    m_bitOffset = _offset;//����ƫ��
    m_shiftL = 64 - _offset - _bitLen;
    m_shiftR = 64 - _bitLen;
    m_srcValue.setLongLong(0);
    m_srcValue.type = Varaint::getType(type);
    m_name = name;

    m_lsb = 1;
    m_offsetValue = 0;
   // printf("=======m_byteOffset:%d, m_bitLen:%d,m_bitOffset:%d\n ", m_byteOffset, m_bitLen,m_bitOffset);
}


int BaseField::parseXML(const char * xmlData)
{
	//printf("int parseXML(const char * xmlData)\n");
	return 0;
}
