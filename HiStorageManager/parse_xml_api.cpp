#include "tinyxml.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "parse_xml_api.h"
using namespace std;

const char xml_root_name[] = "StoreConfig";
TiXmlDocument * pXmlManager = NULL;

bool GetValue(TiXmlElement *element, const char * name, std::string &strVal)
{
	bool ret = false;
	TiXmlAttribute *Attribute = NULL;
	for (Attribute = element->FirstAttribute(); Attribute; Attribute
			= Attribute->Next())
	{
		std::string tmp = Attribute->Name();
		if (tmp == name)
		{
			ret = true;
			strVal = Attribute->Value();
		}
	}
	return ret;
}
bool GetChild(TiXmlElement *pParent, TiXmlElement **pChild, const char * name,
		const char * value)
{
	bool ret = false;
	std::string strValue = std::string(value);
	for (*pChild = pParent->FirstChildElement(); *pChild; *pChild
			= (TiXmlElement *) (*pChild)->NextSiblingElement())
	{ //target>>
		std::string tmp;
		ret = GetValue(*pChild, name, tmp);
		if (ret)
		{
			if (tmp == strValue)
			{
//				DEBUG("Find Service:%s config\n", value);
				ret = true;
				break;
			}
			else
			{
				ret = false;
				continue;
			}

		}
		else
		{
//			err_msg("Can't service_name config\n");
			continue;
		}
	}
	return ret;
}
TiXmlDocument *load_xml(const char *pData, char flag)
{
	TiXmlDocument *pDoc;
	if (flag == FROM_FILE)
	{
		pDoc = new TiXmlDocument(pData);
		bool loadOkay = pDoc->LoadFile();
		if (loadOkay == false)
		{
			char errStr[256];
			sprintf(errStr, "File:%s is not right xml file\n", pData);

//			err_msg(errStr);
		}
	}
	else
	{
		pDoc = new TiXmlDocument();
		pDoc->Parse(pData, NULL);
	}

	return pDoc;
}
int add_xml(TiXmlDocument * pDoc, TiXmlDocument * pTarget,
		const char * firstChildStr)
{
	TiXmlNode* firstChild = 0;
	TiXmlNode* newFirstChild;
	TiXmlNode* rootNode;
	if ((pDoc == NULL) || (pTarget == NULL))
		return -1;
	newFirstChild = pTarget->FirstChild(firstChildStr);
	if (newFirstChild == NULL)
	{
		TiXmlElement firstChildElement(firstChildStr);
		pTarget->InsertEndChild((const TiXmlNode &) firstChildElement);//if no init node add
		newFirstChild = pTarget->FirstChild(firstChildStr);
//		DEBUG("create new\n");
	}
	rootNode = pDoc->FirstChild(xml_root_name);
	if (rootNode == NULL)
	{
		TiXmlElement rootElement(xml_root_name);
		pDoc->InsertEndChild((const TiXmlNode &) rootElement);//if no init node add
		rootNode = pDoc->FirstChild(xml_root_name);
	}
	firstChild = rootNode->FirstChild(firstChildStr);
	if (firstChild == NULL)
	{
		firstChild = rootNode->InsertEndChild(
				(const TiXmlNode &) *newFirstChild);//if no init node add
//		DEBUG("insert\n");
	}
	else
	{
		rootNode->ReplaceChild((TiXmlNode *) firstChild,
				(const TiXmlNode &) *newFirstChild);//if no FirstPersonTarget add directory
//		DEBUG("replace\n");
	}
	return 0;
}
int save_xml(TiXmlDocument * pDoc, const char * filename)
{
	bool bRet = pDoc->SaveFile(filename);
	if (bRet == true)
	{
//		DEBUG("Save file :%s success\n",filename);
	}
	else
	{
		char err_string[100];
//		err_msg("Save file :%s failed\n",filename);
		sprintf(err_string, "Save file :%s failed", filename);
		return -1;
	}
	return 0;
}

