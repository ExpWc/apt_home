#pragma once
#include <iostream>
#include <stdio.h>
using namespace std;
typedef enum
{
    V_UNKNOWN=1, V_BOOL, V_CHAR, V_UCHAR, V_SHORT, V_USHORT, V_INT, V_UINT, V_LONG, V_ULONG, V_LONGLONG, V_ULONGLONG, V_FLOAT, V_DOUBLE
}VARAINT_TYPE;
class Varaint
{
private:
    union
    {
        bool bVal;
        char cVal;
        unsigned char ucVal;
        short sVal;
        unsigned short usVal;
        int iVal;
        unsigned int uiVal;
        long lVal;
        unsigned long ulVal;
        long long llVal;
        unsigned long long ullVal;
        float fVal;
        double dbVal;
    }m_val;
public:
    VARAINT_TYPE type;
    void setBool(bool val)
    {
        m_val.llVal = val;
    }
    void setChar(char val)
    {
        m_val.llVal = val;
    }
    void setShort(short val)
    {
        m_val.llVal = val;
    }
    void setInt(int val)
    {
        m_val.llVal = val;
    }
    void setLong(long val)
    {
        m_val.llVal = val;
    }
    void setLongLong(long long val)
    {
        m_val.llVal = val;
    }
    void setUChar(unsigned char val)
    {
        m_val.ullVal = val;
    }
    void setUShort(unsigned short val)
    {
        m_val.ullVal = val;
    }
    void setUInt(unsigned int val)
    {
        m_val.ullVal = val;
    }
    void setULong(unsigned long val)
    {
        m_val.ullVal = val;
    }
    void setULongLong(unsigned long long val)
    {
        m_val.ullVal = val;
    }
    void setFloat(float val)
    {
        m_val.fVal = val;
    }
    void setDouble(double val)
    {
        m_val.dbVal = val;
    }

    bool getBool()
    {
        return m_val.bVal;
    }
    char getChar()
    {
        return m_val.cVal;
    }
    short getShort()
    {
        return m_val.sVal;
    }
    int getInt()
    {
        return m_val.iVal;
    }
    long getLong()
    {
        return m_val.lVal;
    }
    long long getLongLong()
    {
        return m_val.llVal;
    }
    unsigned char getUChar()
    {
        return m_val.ucVal;
    }
    unsigned short getUShort()
    {
        return m_val.usVal;
    }
    unsigned int getUInt()
    {
        return  m_val.uiVal;
    }
    unsigned long getULong()
    {
        return m_val.ulVal;
    }
    unsigned long long getULongLong()
    {
        return m_val.ullVal;
    }
    float getFloat()
    {
        return m_val.fVal;
    }
    double getDouble()
    {
        return  m_val.dbVal;
    }



    /**************************************
    *���������ַ���������������
    **************************************/
    static VARAINT_TYPE getType(string type)
    {
        if (type == "bool")
        {
            return V_BOOL;
        }
        else if (type == "char")
        {
            return V_CHAR;
        }
        else if (type == "unsigned char")
        {
            return V_UCHAR;
        }
        else if (type == "short")
        {
            return V_SHORT;
        }
        else if (type == "unsigned short")
        {
            return V_USHORT;
        }
        else if (type == "int")
        {
            return V_INT;
        }
        else if (type == "unsigned int")
        {
            return V_UINT;
        }
        else if (type == "long")
        {
            return V_LONG;
        }
        else if (type == "unsigned long")
        {
            return V_ULONG;
        }
        else if (type == "long long")
        {
            return V_LONGLONG;
        }
        else if (type == "unsigned long long")
        {
            return V_ULONGLONG;
        }
        else if (type == "float")
        {
            return V_FLOAT;
        }
        else if (type == "double")
        {
            return V_DOUBLE;
        }
        return V_UNKNOWN;
    }
};

