/************************** (C) COPYRIGHT 2011  **************************
 * File Name          : Ini.cpp
 * Author             : dzy of 
 * Version            : V1.0.0
 * Date               : 5/09/2011
 * Description        : This file provides  the method to read/write config.ini
 * Built with Linux g++
 ********************************************************************************
 *******************************************************************************/
#include "Ini.h"
#include <fstream>
#include <string.h>
#include <stdlib.h>
CIni::CIni()
{
}

CIni::~CIni()
{
    Clear();
}
bool CIni::Read(const char * cFileName)
{
    Clear();
    char buf[1024];
    std::ifstream ifs(cFileName);
    while (ifs.good())
    {
        ifs.getline(buf, 1023);
        string str(buf);
        m_strList.push_back(str);
    }
    return true;
}
int CIni::FindSection(const char * cSection)
{
    int t, max = m_strList.size();
    char tmp[256];
    memset(tmp, 0, 256);
    sprintf(tmp, "[%s]", cSection);//add Enter;
    string strSection(tmp);
    for (t = 0; t < max; t++)
    {
        if (m_strList.at(t) == strSection)
            return t;
    }

    return -1;
}
bool CIni::IsSection(const int iSection)
{
    if (iSection >= 0 && iSection < (int) m_strList.size())
    {
        string strItem = m_strList.at(iSection);
        int len = strItem.size();
        if (len > 2 && strItem[1] == '[' && strItem[len - 1] == ']')
            return true;
    }
    return false;
}
int CIni::FindItem(const int iSection, const char * cItem, string &strVal)
{
    int max = m_strList.size(), t;
    string strItem(cItem), strLook;
    strItem += " = ";
    int iLen = strItem.size();

    for (t = iSection; t < max; t++)
    {
        if (!IsSection(t))
        {
            strLook = m_strList.at(t);
            if ((int) strLook.size() >= iLen)
            {
                string strTest;
                strTest.assign(strLook, 0, iLen);
                if (strTest == strItem)
                {
                    if ((int) strLook.size() == iLen)
                        strVal = "";
                    else
                        strVal.assign(strLook, iLen, strLook.size() - iLen);
                    return t;
                }
            }
        }
        else
            return -1;
    }
    return -1;
}
bool CIni::GetValue(const char * cSection, const char * cItem, int &iVal)
{
    int idx = FindSection(cSection);
    if (idx >= 0)
    {
        string strVal;
        if (FindItem(idx + 1, cItem, strVal) > 0)
        {
            iVal = (int) atoi(strVal.c_str());
            return true;
        }
    }
    return false;
}
bool CIni::GetValue(const char * cSection, const char * cItem, unsigned int &iVal)
{
    int idx = FindSection(cSection);
    if (idx >= 0)
    {
        string strVal;
        if (FindItem(idx + 1, cItem, strVal) > 0)
        {
            iVal = (int) atoi(strVal.c_str());
            return true;
        }
    }
    return false;
}
bool CIni::GetValue(const char * cSection, const char * cItem, long &iVal)
{
    int idx = FindSection(cSection);
    if (idx >= 0)
    {
        string strVal;
        if (FindItem(idx + 1, cItem, strVal) > 0)
        {
            iVal = (long) atol(strVal.c_str());
            return true;
        }
    }
    return false;
}
bool CIni::GetValue(const char * cSection, const char * cItem, string &strVal)
{
    int idx = FindSection(cSection);
    if (idx >= 0)
    {
        if (FindItem(idx + 1, cItem, strVal) > 0)
        {
            return true;
        }
    }
    return false;
}
int CIni::InsertSection(const char * cSection)
{
    if (!cSection)
        return -1;
    int idx = FindSection(cSection);
    if (idx < 0)
    {
        char tmp[256];
        memset(tmp, 0, 256);
        sprintf(tmp, "[%s]", cSection);//add Enter;
        string strSection(tmp);

        m_strList.push_back(strSection);
        idx = m_strList.size() - 1;
    }
    return idx;
}
bool CIni::SetValue(const char * cSection, const char * cItem, const int iVal)
{
    int idx = InsertSection(cSection);
    if (idx >= 0)
    {
        string unused;
        int iIdx = FindItem(idx + 1, cItem, unused);
        char tmp[256];
        memset(tmp, 0, 256);
        sprintf(tmp, "%s = %d", cItem, iVal);
        string strVal(tmp);
        if (iIdx >= 0)
            m_strList[iIdx] = strVal;
        else
        {
            vector<string>::iterator pos;
            pos = m_strList.begin();
            for (int i = 0; i < idx + 1; i++)
                pos++;
            m_strList.insert(pos, strVal);
        }
        return true;
    }
    return false;
}
bool CIni::SetValue(const char * cSection, const char * cItem, const unsigned int iVal)
{
    int idx = InsertSection(cSection);
    if (idx >= 0)
    {
        string unused;
        int iIdx = FindItem(idx + 1, cItem, unused);
        char tmp[256];
        memset(tmp, 0, 256);
        sprintf(tmp, "%s = %d", cItem, iVal);
        string strVal(tmp);
        if (iIdx >= 0)
            m_strList[iIdx] = strVal;
        else
        {
            vector<string>::iterator pos;
            pos = m_strList.begin();
            for (int i = 0; i < idx + 1; i++)
                pos++;
            m_strList.insert(pos, strVal);
        }
        return true;
    }
    return false;
}
bool CIni::SetValue(const char * cSection, const char * cItem, const long iVal)
{
    int idx = InsertSection(cSection);
    if (idx >= 0)
    {
        string unused;
        int iIdx = FindItem(idx + 1, cItem, unused);
        char tmp[256];
        memset(tmp, 0, 256);
        sprintf(tmp, "%s = %ld", cItem, iVal);
        string strVal(tmp);
        if (iIdx >= 0)
            m_strList[iIdx] = strVal;
        else
        {
            vector<string>::iterator pos;
            pos = m_strList.begin();
            for (int i = 0; i < idx + 1; i++)
                pos++;
            m_strList.insert(pos, strVal);
        }
        return true;
    }
    return false;
}
bool CIni::SetValue(const char * cSection, const char * cItem, string strValTmp)
{
    int idx = InsertSection(cSection);
    if (idx >= 0)
    {
        string unused;
        int iIdx = FindItem(idx + 1, cItem, unused);
        char tmp[256];
        memset(tmp, 0, 256);
        sprintf(tmp, "%s = %s", cItem, strValTmp.c_str());
        string strVal(tmp);
        if (iIdx >= 0)
            m_strList[iIdx] = strVal;
        else
        {
            vector<string>::iterator pos;
            pos = m_strList.begin();
            for (int i = 0; i < idx + 1; i++)
                pos++;
            m_strList.insert(pos, strVal);
        }
        return true;
    }
    return false;
}
bool CIni::Write(const char * cFileName)
{
    ofstream ofs(cFileName);
    int t, max = m_strList.size();
    for (t = 0; t < max; t++)
    {
        string strTmp = m_strList[t];
       // if (strTmp != "")
        if (max != t+1)
        {
            ofs << m_strList[t] << "\n";
        }
        else
        {
            ofs << m_strList[t];
        }
    }
    return true;
}
void CIni::Clear()
{
    m_strList.clear();
}
/*int main()
 {
 int iPort;
 CIni iniFile;
 iniFile.Read("config.ini");
 bool ret = iniFile.GetValue("0","a",iPort);
 if(ret)
 printf("the port is :%d\n",iPort);
 ret = iniFile.SetValue("0","rtaddr",1);
 if(ret)
 iniFile.Write("config.ini");
 iniFile.Clear();
 }*/
