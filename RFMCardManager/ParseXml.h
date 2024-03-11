#ifndef _H_XML_API
#define _H_XML_API
#include "tinyxml.h"
#include "RFMCardManager.h"
#include "HiReport.h"
#include "BaseRFMCard.h"

inline bool isValidAttribute(const char *res,const char* element,const char *attr)
{
	if(res == NULL)
	{
		REPORT(REPORT_ERROR,"Element %s cannot find attibute %s",element,attr);
		return false;
	}
	//REPORT(REPORT_INFO,"Element: %s %s=%s",element,attr,res);
	return true;
}

int pasreRouterXml(const char *pData, char *msg,CRFMCardManager *pManager);
int pasreInitXml(const char *pData, char *msg,CRFMCardManager *pManager);
#endif
