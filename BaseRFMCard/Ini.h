/************************** (C) COPYRIGHT 2011  **************************
 * File Name          : Ini.h
 * Author             : dzy of 
 * Version            : V1.0.0
 * Date               : 5/09/2011
 * Description        : This file provides  the method to read/write config.ini
 * Built with Linux g++
 ********************************************************************************
 *******************************************************************************/
#include <vector>
#include <string>
using namespace std;
class CIni
{
public:
    CIni();
    virtual ~CIni();
    bool Read(const char * cFileName);
    bool GetValue(const char * cSection, const char * cItem, int &iVal);
    bool GetValue(const char * cSection, const char * cItem, unsigned int &iVal);
    bool GetValue(const char * cSection, const char * cItem, long &iVal);
    bool GetValue(const char * cSection, const char * cItem, string &strVal);
    bool SetValue(const char * cSection, const char * cItem, const int iVal);
    bool SetValue(const char * cSection, const char * cItem, const unsigned int iVal);
    bool SetValue(const char * cSection, const char * cItem, const long iVal);
    bool SetValue(const char * cSection, const char * cItem, string strValTmp);
    bool Write(const char * cFileName);
    void Clear();
private:
    int FindSection(const char * cSection);
    bool IsSection(const int iSection);
    int InsertSection(const char * cSection);
    int FindItem(const int iSection, const char * cItem, string &strVal);
    vector<string> m_strList;
};

