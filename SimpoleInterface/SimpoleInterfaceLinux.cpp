//#include <iostream>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

#include <cpprest/json.h>
#include <map>
#include "SimpoleInterfaceLinux.h"
using namespace utility;                    
using namespace std;                        
using namespace web;                        
using namespace web::http;                  
using namespace web::http::client;          
using namespace concurrency::streams;       

map<string, string> gUrlToToken;
map<string, string> gProjectUuid;
map<string, string> gParentName;

 int measure(int x, int y)
{
	int z = y;
	while (x%y != 0)
	{
		z = x % y;
		x = y;
		y = z;
	}
	return z;
}

int typeExchang(string type, ModelDataVarTypeE& dataType)
{
	if (type == U("double"))
	{
		dataType = M_DOUBLE;
	}
	else if (type == U("float"))
	{
		dataType = M_FLOAT;
	}
	else if (type == U("unsigned char"))
	{
		dataType = M_UCHAR;
	}
	else if (type == U("char"))
	{
		dataType = M_CHAR;
	}
	else if (type == U("unsigned short"))
	{
		dataType = M_USHORT;
	}
	else if (type == U("short"))
	{
		dataType = M_SHORT;
	}
	else if (type == U("unsigned int"))
	{
		dataType = M_UINT;
	}
	else if (type == U("int"))
	{
		dataType = M_INT;
	}
	else if (type == U("bool"))
	{
		dataType = M_BOOL;
	}
	else//M_BOOL, M_CHAR, M_UCHAR, M_SHORT, M_USHORT, M_INT, M_UINT, M_FLOAT, M_DOUBLE
	{
		return -1;
	}
	return 0;
}

int login(const char* url)
{

	string urlStr = string(url);
	const char *urlPath = "/_api/login/1.0.0";

	http_client_config config;
	config.set_timeout(utility::seconds(10));

	http_client client(urlStr.c_str());
	//http_client client(U("http://192.168.1.121:10031"));

	uri_builder builder(urlPath);
	//uri_builder builder(U("/_api/login/1.0.0"));

	builder.append_query(U("password"), U("123456"));
	builder.append_query(U("username"), U("root"));
	builder.append_query(U("Content-Type"), U("application/json"));

	http_response response = client.request(methods::POST, builder.to_string()).get();
	if (response.status_code() == status_codes::OK)
	{
		try
		{
			const json::value &jv = response.extract_json().get();
			const web::json::object &jobj = jv.as_object();
			auto status = jobj.at(U("status")).as_string();

			if (status == U("error"))
			{
				string pingCmd = "chcp 936";
				setlocale(LC_ALL, "chs");
				auto msg = jobj.at(U("message")).as_string();
				cout <<  "Error message: "<< msg << endl;
			}
			else
			{
		     auto gToken = jobj.at(U("data")).as_string();
         if (gUrlToToken.find(urlStr) == gUrlToToken.end())
	       {
  					gUrlToToken.insert(pair<string, string>(urlStr, gToken));
	       }
				else
				{
					 gUrlToToken[urlStr] = gToken;
				}
				cout << "status: " <<  status  << " \n"<<  "gToken data: " << gToken << endl;
			}

		}
		catch (const std::exception &e)
		{
			cout << e.what() << endl;
		}
	}
	else
	{
		cout << "error" << endl;
	}

	return 0;

}

int getModelList(TY_MODEL_LIST* modelList, const char*url)
{
	string urlStr = string(url);
	string urlPath = "/_api/SimModel/TPOS/getModelList?";
	string strUrl = urlStr + urlPath;
 	string gToken;
  	gToken = gUrlToToken[urlStr];

	http_client_config config;
	config.set_timeout(utility::seconds(10));

	http_client client(strUrl.c_str());
	//http_client client(U("http://192.168.1.121:10031"));

	http_request req(methods::GET);

	req.headers().add(U("Content-Type"), U("application/json"));
	req.headers().add(U("x-access-token"), gToken.c_str());
	//
	http_response response = client.request(req).get(); 
	if (response.status_code() == status_codes::OK)
	{
		try
		{
			int num = 0;
			const json::value &jv = response.extract_json().get();
			const web::json::object &jobj = jv.as_object();

			auto status = jobj.at(U("status")).as_string();
			if (status == U("error"))
			{
				string pingCmd = "chcp 936";
				setlocale(LC_ALL, "chs");
				auto msg = jobj.at(U("message")).as_string();
				cout <<  "Error message: "<< msg << endl;
			}
			else
			{
				cout << "status: " <<  status << endl;

				auto dataArray = jobj.at(U("data")).as_array();
				size_t num = dataArray.size();
				modelList->modelNumber = num;
				for(int i = 0; i < num; i++)
				{
					auto modelName = dataArray[i].at(U("name")).as_string();
					//cout << "modelName: " << modelName << " modelNameLength:"<< strlen(modelName.c_str()) << "num:"<< num << endl; 
					//memcpy(modelList->modelName[i], modelName.c_str(), strlen(modelName.c_str()));
					strcpy(modelList->modelName[i], modelName.c_str());	
							
					auto uuid = dataArray[i].at(U("uuid")).as_string();
					//cout << "UUID: " << uuid << "uuidlen："<< sizeof(uuid.c_str()) << endl; 
					//memcpy(modelList->uuid[i], uuid.c_str(), strlen(uuid.c_str()));
					strcpy(modelList->uuid[i], uuid.c_str());	
				}
			}

		}
		catch (const std::exception &e)
		{
			cout << e.what() << endl;
		}
	}
	else
	{
		cout << "error" << endl;
	}

	return 0;

}

int getVariableListNum(TY_MODELNAME_VALUE modelNameValue, int& varNum, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;

		string urlPath(U("/_api/ModelVariable/TPOS/getModelVariableList"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		int num = modelNameValue.modelNumber;
		json::value obj;
		obj = json::value::array(num);	

		json::value obj_array;

		for (int i = 0; i < num; i++)
		{
			obj_array = json::value::string(string(modelNameValue.modelName[i]));
			obj[i] = obj_array;
		}

		req.set_body(obj.serialize());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				num = dataArray.size();
				varNum = num;
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "1Error message:" <<  msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" <<  msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
	
}

int getVariableList(TY_MODELNAME_VALUE modelNameValue, TY_VAR_DES *pList, int& varNum, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;

		string urlPath(U("/_api/ModelVariable/TPOS/getModelVariableList"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		int num = modelNameValue.modelNumber;
		json::value obj;
		obj = json::value::array(num);	//��ʼ������

		json::value obj_array;

		for (int i = 0; i < num; i++)
		{
			obj_array = json::value::string(string(modelNameValue.modelName[i]));
			obj[i] = obj_array;
		}

		req.set_body(obj.serialize());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				num = dataArray.size();
				for (int i = 0; i < num; i++)
				{
					string name = dataArray[i].at(U("name")).as_string();
					//memcpy(pList[i].paraName, name.c_str(), name.length());
					strcpy(pList[i].paraName, name.c_str());
					
					string uuid = dataArray[i].at(U("uuid")).as_string();
					//memcpy(pList[i].uuid, uuid.c_str(), uuid.length());
					strcpy(pList[i].uuid, uuid.c_str());

					string type = dataArray[i].at(U("type")).as_string();
				//	memcpy(pList[i].nodeType, type.c_str(), type.length());
					strcpy(pList[i].nodeType, type.c_str());
					typeExchang(type, pList[i].dataType);

					string varType = dataArray[i].at(U("varType")).as_string();
					char tmp[8] = { 0 };
					//memcpy(tmp, varType.c_str(), varType.length());
					strcpy(tmp, varType.c_str());
					pList[i].varType = atoi(tmp);

					string fullPath = dataArray[i].at(U("fullpath")).as_string();
					//memcpy(pList[i].fullPath, fullPath.c_str(), fullPath.length());
					strcpy(pList[i].fullPath, fullPath.c_str());
				}
				varNum = num;
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}

}

int getRamSampleInTimeWindow(TY_VAR_VALUE* pList, int num, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string urlPath(U("/_api/Common/TPOS/getRawSampleInRealTimePlus"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());
		json::value obj;
		for (int i = 0; i < num; i++)
		{
			json::value obj_array;
			obj_array[U("mode")] = json::value::string(U("sample"));
			obj_array[U("nodeType")] = pList[i].varType;//json::value::string(stringToWString(string(pList[i].nodeType)));
			obj_array[U("widgetUuid")] = json::value::string(U(""));
			obj_array[U("fullPath")] = json::value::string(string(pList[i].varPath));
			obj[i] = obj_array;
		}
		json::value obj_array;
		req.set_body(obj.serialize());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				size_t dataSize = dataArray.size();
				for (int i = 0; i < dataSize; i++)
				{
					//string tmp = dataArray[i].as_object().at(L"uuid").as_string();
					//wStringToChar(tmp, pList[i].uuid);
					
					auto queueArray = dataArray[i].as_object().at(U("queueValues")).as_array();
					auto array = queueArray[0].as_array();
					pList[i].queueValues[0].type = (ModelDataVarTypeE)dataArray[i].as_object().at(U("dataType")).as_integer();
					//pList[i].queueValues->value = array[1].as_double();

					//char valueStr[64];
					//memcpy(valueStr, pParaDes[i].value, sizeof(pParaDes[i].value));
					if (pList[i].queueValues[0].type <= M_UINT)
					{
						sprintf(pList[i].queueValues[0].value, "%d", array[1].as_integer(), 64);
					}
					else if (pList[i].queueValues[0].type == M_DOUBLE || pList[i].queueValues->type == M_FLOAT)
					{;
						sprintf(pList[i].queueValues[0].value, "%lf", array[1].as_double(), 64);
					}
					else
					{

					}
					/*
					string value = wStringToString(array[1].as_string());
					sprintf(pList[i].queueValues->value, "%s", value.c_str());
					*/
					//sprintf((char[])(pList[i].value), "%lf", value);
		
				}
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
}

inline int varMonitorConfig(string urlPath,TY_VAR_DES * pVars, int num, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		http_client_config config;
		string strUrl = urlStr + urlPath;
		config.set_timeout(utility::seconds(90));
		http_client client(strUrl.c_str(), config);
		http_request req(methods::PUT);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		json::value obj;
		for (int i = 0; i < num; i++)
		{
			json::value obj_array;
			
			obj_array[U("dataType")] = pVars[i].dataType;
			obj_array[U("fullpath")] = json::value::string(string(pVars[i].fullPath));//pVars[i].fullPath
			obj_array[U("varType")] = pVars[i].varType;
			obj[i] = obj_array;
		}
		req.set_body(obj.serialize());
		string_t str = obj.serialize();
		http_response response = client.request(req).get(); //request body
		try
		{
			if (response.status_code() == status_codes::OK)
			{
				const json::value& jv = response.extract_json().get();
				const web::json::object& job = jv.as_object();
				string status = job.at(U("status")).as_string();
				if (status == U("success"))
				{
					ret = 0;
				}
				else
				{
					const web::json::object& job = jv.as_object();
					setlocale(LC_ALL, "chs");
					string msg = job.at(U("message")).as_string();
					cout << "Error message:" << msg << endl;
					ret = -1;
				}
			}
			else
			{
				const json::value& jv = response.extract_json().get();
				const web::json::object& job = jv.as_object();
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		catch (const std::exception& e)
		{
			printf("Error exception :%s\n", e.what());
			return -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
	
}

int addVarMonitor(TY_VAR_DES *pVars, int num, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string urlPath(U("/_api/TPOS/addVarMonitor?"));
		return varMonitorConfig(urlPath, pVars, num, url);
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
	
}

int delMonitorVar(TY_VAR_DES *pVars, int num, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	int ret = 0;
	string urlPath(U("/_api/TPOS/delVarMonitor?"));
	return varMonitorConfig(urlPath, pVars, num,url);
}

int getParameterListNum(TY_MODELNAME_VALUE modelNameValue, int& paraNum, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;

		string urlPath(U("/_api/ModelParameter/TPOS/getModelParameterList"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		int num = modelNameValue.modelNumber;
		json::value obj;
		obj = json::value::array(num);	//��ʼ������

		json::value obj_array;
		for (int i = 0; i < num; i++)
		{
			obj_array = json::value::string(string(modelNameValue.modelName[i]));
			obj[i] = obj_array;
		}
		req.set_body(obj.serialize());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				paraNum = dataArray.size();
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}

}

int getParameterList(TY_MODELNAME_VALUE modelNameValue, TY_PARA_DES *pList, int& paraNum, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;

		string urlPath(U("/_api/ModelParameter/TPOS/getModelParameterList"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		int num = modelNameValue.modelNumber;
		json::value obj;
		obj = json::value::array(num);	//��ʼ������

		json::value obj_array;

		for (int i = 0; i < num; i++)
		{
			obj_array[i] = json::value::string(string(modelNameValue.modelName[i]));
		}
		obj = obj_array;

		req.set_body(obj.serialize());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				num = dataArray.size();
				for (int i = 0; i < num; i++)
				{
					string name = dataArray[i].at(U("name")).as_string();
					//memcpy(pList[i].paraName, name.c_str(), strlen(name.c_str()));
					strcpy(pList[i].paraName, name.c_str());

					string value = dataArray[i].at(U("value")).as_string();
					//strcpy(pList[i].value, (unsigned char*)value.c_str());
					memcpy(pList[i].value, value.c_str(), strlen(value.c_str()) + 1);
					//wStringToChar(value, pList[i].value);

					string type = dataArray[i].at(U("type")).as_string();
					typeExchang(type, pList[i].dataType);
					

					string fullPath = dataArray[i].at(U("fullpath")).as_string();
					//memcpy(pList[i].fullPath, fullPath.c_str(), strlen(fullPath.c_str()));
					strcpy(pList[i].fullPath, fullPath.c_str());
					//cout << "------get path1: " << fullPath.c_str() <<"\n len1:" << strlen(fullPath.c_str())  <<endl;
					//cout << "------get path2: " << pList[i].fullPath <<"\n len2:" << strlen(pList[i].fullPath)  <<endl;

					string uuid = dataArray[i].at(U("uuid")).as_string();
					//memcpy(pList[i].uuid, uuid.c_str(), strlen(uuid.c_str()));
					strcpy(pList[i].uuid, uuid.c_str());
				}
				paraNum = num;
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
	
}

int getParameterValue(TY_PARA_DES *pParaDes, int num, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;

		string urlPath(U("/_api/ModelParameter/TPOS/getParameters?"));

		string modelName = string(pParaDes[0].fullPath);
		modelName = U("modelName=") + modelName.substr(0,modelName.find(U("/")));
		string strUrl = urlStr + urlPath + modelName;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		json::value obj;
		obj = json::value::array(num);
		for (int i = 0; i < num; i++)
		{
			json::value obj_array;
			obj_array[U("nodeType")] = json::value::string(U("modelParameter"));
			obj_array[U("fullpath")] = json::value::string(string(pParaDes[i].fullPath));
			obj_array[U("value")] = json::value::string(U("0.0"));
			obj_array[U("widgetUuid")] = json::value::string(U("uuid"));
			obj[i] = obj_array;
		}	
		
		req.set_body(obj.serialize());
		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				int paramsNum = job.at(U("data")).as_array().size();
				if (paramsNum == num)
				{
					auto dataArray = job.at(U("data")).as_array();
					for (int i = 0; i < paramsNum; i++)
					{
					
						string paramValueWstr = dataArray[i].at(U("value")).as_string();

						char dataBuffer[256] ;
						memcpy(dataBuffer, paramValueWstr.c_str(), paramValueWstr.length());
						//strcpy(dataBuffer, paramValueWstr.c_str());	

						//double value = atof(dataBuffer);
						//memcpy(pParaDes[i].value, &value, 8);
						if (pParaDes[i].dataType == M_BOOL)
						{
							bool tmp = (bool)atoi(dataBuffer);
							memcpy(pParaDes[i].value, &tmp, sizeof(tmp));
							//strcpy(pParaDes[i].value, &tmp);	
						}
						else if (pParaDes[i].dataType == M_CHAR)
						{
							char tmp = (char)atoi(dataBuffer);
							memcpy(pParaDes[i].value, &tmp, sizeof(tmp));
							//strcpy(pParaDes[i].value, &tmp);	
						}
						else if (pParaDes[i].dataType == M_UCHAR)
						{
							unsigned char tmp = (unsigned char)atoi(dataBuffer);
							memcpy(pParaDes[i].value, &tmp, sizeof(tmp));
							//strcpy(pParaDes[i].value, &tmp);	
						}
						else if (pParaDes[i].dataType == M_SHORT)
						{
							short tmp = (short)atoi(dataBuffer);
							memcpy(pParaDes[i].value, &tmp, sizeof(tmp));
							//strcpy(pParaDes[i].value, &tmp);	
						}
						else if (pParaDes[i].dataType == M_USHORT)
						{
							unsigned short tmp = (unsigned short)atoi(dataBuffer);
							//strcpy(pParaDes[i].value, &tmp);	
							memcpy(pParaDes[i].value, &tmp, sizeof(tmp));
						}
						else if (pParaDes[i].dataType == M_INT)
						{
							int tmp = (int)atoi(dataBuffer);
							memcpy(pParaDes[i].value, &tmp, sizeof(tmp));
							//strcpy(pParaDes[i].value, &tmp);	
						}
						else if (pParaDes[i].dataType == M_FLOAT)
						{
							float tmp = (float)atof(dataBuffer);
							memcpy(pParaDes[i].value, &tmp, sizeof(tmp));
							//strcpy(pParaDes[i].value, &tmp);	
						}
						else if (pParaDes[i].dataType == M_DOUBLE)
						{
							double tmp = (float)atof(dataBuffer);
							memcpy(pParaDes[i].value, &tmp, sizeof(tmp));
							//strcpy(pParaDes[i].value, &tmp);	
						}
						else
						{
							memcpy(pParaDes[i].value, paramValueWstr.c_str(), paramValueWstr.length());
							//strcpy(pParaDes[i].value, paramValueWstr.c_str());	
						}
						//string typeWstr = dataArray[i].at(L"type").as_string();
						//typeExchang(typeWstr, pParaDes[i].dataType);

					}
				}
				
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
	
}

int setParameterValue(TY_PARA_DES *pParaDes, int num, const char*url)
{	
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string urlPath(U("/_api/ModelParameter/TPOS/setParameter?"));
		string modelName = string(pParaDes[0].fullPath);
		modelName = U("modelName=") + modelName.substr(0, modelName.find(U("/")));
		string strUrl = urlStr + urlPath + modelName;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		json::value obj;
		for (int i = 0; i < num; i++)
		{
			json::value obj_array;

			char valueStr[64];
			//memcpy(valueStr, pParaDes[i].value, sizeof(pParaDes[i].value));
			if (pParaDes[i].dataType <= M_UINT)
			{
				sprintf(valueStr, "%d", *(pParaDes[i].value));
			}
			else if (pParaDes[i].dataType == M_FLOAT)
			{
				sprintf(valueStr, "%f", *(pParaDes[i].value));
			} 
			else if (pParaDes[i].dataType == M_DOUBLE)
			{
				double valueTest = *((double*)pParaDes[i].value);
				sprintf(valueStr, "%lf", *((double*)pParaDes[i].value));
			}
			else
			{

			}
			string tmpStr = string(valueStr);
			obj_array[U("nodeType")] = json::value::string(U("modelParameter"));
			obj_array[U("value")] = json::value::string(string(valueStr));
			obj_array[U("fullpath")] = json::value::string(string(pParaDes[i].fullPath));
			obj[i] = obj_array;
		}
	
		req.set_body(obj.serialize());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}

}
int getCurrentProject(TY_PROJECT_VALUE* pProjectValue, const char* url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}

	try
	{
		int ret = 0;
		string urlPath(U("/_api/SimpoleProject/currentProject"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::GET);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		http_response response = client.request(req).get(); //request body

		if (response.status_code() == status_codes::OK)
		{

			int num = 0;
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();

			string status = job.at(U("status")).as_string();//�ж�status�Ƿ���ȷ����ȷ��ȡdata 
			if (status == U("error"))
			{
				string pingCmd = "chcp 936";
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
			else
			{
				char projectName[256] = { 0 };
				char projectUuid[256] = { 0 };
				auto dataObj = job.at(U("data")).as_object();
				auto uuid = job.at(U("data")).as_object().at(U("uuid")).as_string();
				if (gProjectUuid.find(strUrl) == gProjectUuid.end())
				{
					gProjectUuid.insert(pair<string, string>(strUrl, uuid));
					//strcpy(projectUuid, uuid.c_str());
					strcpy(pProjectValue->uuid, uuid.c_str());
				}
				else
				{
					gProjectUuid[strUrl] = uuid;
					//wStringToChar(uuid.c_str(), projectUuid);
					strcpy(pProjectValue->uuid, uuid.c_str());
				}

				auto name = job.at(U("data")).as_object().at(U("name")).as_string();
				if (gParentName.find(strUrl) == gParentName.end())
				{
					gParentName.insert(pair<string, string>(strUrl, name));		
					//wStringToChar(name.c_str(), projectName);
					strcpy(pProjectValue->name, name.c_str());
				}
				else
				{
					gParentName[strUrl] = name;
				
					//wStringToChar(name.c_str(), projectName);
					strcpy(pProjectValue->name, name.c_str());
				}
				pProjectValue->createdAt = job.at(U("data")).as_object().at(U("createdAt")).as_integer();
				pProjectValue->createdAt = job.at(U("data")).as_object().at(U("visitedDate")).as_integer();
				pProjectValue->status = job.at(U("data")).as_object().at(U("status")).as_bool();


				auto descriptionWStr = job.at(U("data")).as_object().at(U("description")).as_string();
				//wStringToChar(descriptionWStr.c_str(), pProjectValue->description);
				strcpy(pProjectValue->description, descriptionWStr.c_str());

				pProjectValue->visitedDate = job.at(U("data")).as_object().at(U("visitedDate")).as_number().to_int64();
				pProjectValue->status = job.at(U("data")).as_object().at(U("status")).as_bool();

				auto paramConfigurations = job.at(U("data")).as_object().at(U("paramConfigurations")).as_array();
				pProjectValue->paramConfigurationsNum = paramConfigurations.size();
				for (int i = 0; i < pProjectValue->paramConfigurationsNum; i++)
				{
					auto tmpWStr = paramConfigurations[i].as_string();
					//wStringToChar(tmpWStr.c_str(), pProjectValue->paramConfigurations[i]);
					strcpy(pProjectValue->paramConfigurations[i], tmpWStr.c_str());
				}

				auto testConfigurations = job.at(U("data")).as_object().at(U("testConfigurations")).as_array();
				pProjectValue->testConfigurationsNum = testConfigurations.size();
				for (int i = 0; i < pProjectValue->testConfigurationsNum; i++)
				{
					auto tmpWStr = testConfigurations[i].as_string();
					//wStringToChar(tmpWStr.c_str(), pProjectValue->testConfigurations[i]);
					strcpy(pProjectValue->testConfigurations[i], tmpWStr.c_str());
				}

				auto simModels = job.at(U("data")).as_object().at(U("simModels")).as_array();
				pProjectValue->simModelsNum = simModels.size();
				for (int i = 0; i < pProjectValue->simModelsNum; i++)
				{
					auto tmpWStr = simModels[i].as_string();
					//wStringToChar(tmpWStr.c_str(), pProjectValue->simModels[i]);
					strcpy(pProjectValue->simModels[i], tmpWStr.c_str());
				}

				auto storageRecords = job.at(U("data")).as_object().at(U("storageRecords")).as_array();
				pProjectValue->storageRecordsNum = storageRecords.size();
				for (int i = 0; i < pProjectValue->storageRecordsNum; i++)
				{
					auto tmpWStr = storageRecords[i].as_string();
					//wStringToChar(tmpWStr.c_str(), pProjectValue->storageRecords[i]);
					strcpy(pProjectValue->storageRecords[i], tmpWStr.c_str());
				}
				ret = 0;
			}

		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;

	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}

}
int getStorageRecordListNum(int& varNum, const char*url)
{
	string gToken;
	string projectUuidWStr;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	projectUuidWStr = gProjectUuid[urlStr];

	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;

		string urlPath(U("/Common/TPOS/getStorageRecordList?"));
		string strUrl = urlStr + urlPath+ projectUuidWStr;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				varNum = dataArray.size();
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}

}


int getStorageRecordList(TY_STORAGERECORD_VALUES *pList, int& varNum, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string urlPath(U("/Common/TPOS/getStorageRecordList"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		int num = varNum;
		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				num = dataArray.size();
				for (int i = 0; i < num; i++)
				{
					string uuid = dataArray[i].at(U("uuid")).as_string();
					//wStringToChar(uuid, pList[i].storageUuid);
					strcpy( pList[i].storageUuid, uuid.c_str());


					string name = dataArray[i].at(U("name")).as_string();
					//wStringToChar(name, pList[i].storageName);
					strcpy( pList[i].storageName, name.c_str());
				}
				varNum = num;
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}

}


int getStorageModeListNum(int& varNum, const char *modelUuid, const char*url)
{
	string gToken;
	string urlStr = string(url);
	string uuid = string(modelUuid);
	gToken = gUrlToToken[urlStr];

	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string urlPath(U("/Common/getStorageModelList/"));
		string strUrl = urlStr + urlPath + uuid;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				varNum = dataArray.size();
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}

}

int getStorageModeList(TY_STORAGEMODELIST *pList, int& varNum, const char *storageUuid, const char*url)
{
	string gToken;
	string urlStr = string(url);
	string uuid = string(storageUuid);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string urlPath(U("/Common/getStorageModelList/"));
		string strUrl = urlStr + urlPath + uuid;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		int dataSize = varNum;
		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataArray = job.at(U("data")).as_array();
				dataSize = dataArray.size();
				for (int i = 0; i < dataSize; i++)
				{
					auto varArray = dataArray[i].as_object().at(U("variables")).as_array();
					auto modelNameWStr = dataArray[i].as_object().at(U("modelName")).as_string();
					//wStringToChar(modelNameWStr, pList[i].modelName);
					strcpy( pList[i].modelName, modelNameWStr.c_str());

					size_t varSize = varArray.size();
					for (int j = 0; j < varSize; j++)
					{
						pList[i].length[j] = varArray[j].as_object().at(U("length")).as_integer();

						string name = varArray[j].as_object().at(U("name")).as_string();
						//wStringToChar(name, pList[i].name[j]);
						strcpy( pList[i].name[j], name.c_str());

						string type = varArray[j].as_object().at(U("type")).as_string();
						//wStringToChar(type, pList[i].type[j]);
						strcpy( pList[i].type[j], type.c_str());

						string uuid = varArray[j].as_object().at(U("uuid")).as_string();
						//wStringToChar(uuid, pList[i].uuid[j]);
						strcpy( pList[i].uuid[j], uuid.c_str());

						string varPath = varArray[j].as_object().at(U("varPath")).as_string();
						//wStringToChar(varPath, pList[i].varPath[j]);
						strcpy( pList[i].varPath[j], varPath.c_str());

						pList[i].varType[j] = varArray[j].as_object().at(U("varType")).as_integer();
					}
					pList[i].varNum = varSize;
				}
				varNum = dataSize;
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
}


int getRecordTimeRange(char* uuid, _RECORD_TIME& recordTime, const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;

		string urlPath(U("/getRecordTimeRange"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		json::value obj;
		obj[U("storageUuid")] = json::value::string(string(uuid));	//��ʼ������		
		json::value obj_array;
		req.set_body(obj.serialize());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataObj = job.at(U("data")).as_object();
				recordTime.startTime = dataObj.at(U("startTime")).as_number().to_uint64();
				recordTime.endTime = dataObj.at(U("endTime")).as_number().to_uint64();
				//auto test = dataObj.at(L"startTime").as_integer();
				//auto test1 = dataObj.at(L"endTime").as_integer();

				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
}

int getRamSampleRecorde(TY_RAMSAMPLE_VALUES ramSample, TY_VAR_VALUE* pList,  const char*url)
{
	string gToken;
	char startTime[128];
	char endTime[128];
	sprintf(startTime, "%lld", ramSample.recordTime.startTime);
	sprintf(endTime, "%lld", ramSample.recordTime.endTime);
	string startTimeWStr = string(startTime);
	string endTimeWStr = string(endTime);
	string storageIdWStr = string(ramSample.storageUuid);
	endTimeWStr = U("endTime=") + endTimeWStr;
	startTimeWStr = U("startTime=") + startTimeWStr;
	storageIdWStr = U("storageId=") + storageIdWStr;
	string urlStr = string(url);

	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string urlPath(U("/TPOS/getRamSampleRecorde?"));
		string strUrl = urlStr + urlPath + startTimeWStr + U("&") + endTimeWStr + U("&") + storageIdWStr;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());//v

		json::value obj;
		for (int i = 0; i < ramSample.varLength; i++)
		{
			json::value obj_array;
			obj_array[U("fullpath")] = json::value::string(string(ramSample.fullPath[i]));
			obj_array[U("varType")] =  1;
			obj[i] = obj_array;
		}
		json::value obj_array;
		req.set_body(obj.serialize());
		printf("----------------22\n");
		http_response response = client.request(req).get(); //request body
		printf("----------------33\n");
		if (response.status_code() == status_codes::OK)
		{
			printf("----------------44\n");
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			printf("----------------55\n");
			if (status == U("success"))
			{
				
				auto dataArray = job.at(U("data")).as_array();
				size_t dataSize = dataArray.size();
				for (int i = 0; i < dataSize; i++)
				{
					printf("----------------66\n");
					auto dataObj = dataArray[i].as_object();
					auto qneueArray = dataObj.at(U("queueValues")).as_array();
					size_t qneueSize = qneueArray.size();
					for (int j = 0; j < qneueSize; j++)
					{
						auto data = qneueArray[j].as_array();
						//pList[i].queueValues[j].value = data[1].as_double();
						sprintf(pList[i].queueValues[j].value, "%lf", data[1].as_double());
						pList[i].queueValues[j].time = data[0].as_number().to_uint64();
					}

					pList[i].sampleNum = dataArray[i].at(U("sampleNum")).as_integer();
					pList[i].startTime = dataArray[i].at(U("startTime")).as_integer();
					pList[i].stepUnitInNs = dataArray[i].at(U("stepUnitInNs")).as_integer();
					pList[i].varType = dataArray[i].at(U("varType")).as_integer();
					pList[i].queueLen = qneueSize;
				}
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "1Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "2Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
}


int deleteStorageRecord(const char* storageUuid, const char*url)
{
	string gToken;
	string urlStr = string(url);
	string storageUuidStr = string(storageUuid);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = -1;

		string urlPath(U("/Common/deleteStorageRecord/"));
		string strUrl = urlStr + urlPath + storageUuidStr;
		http_client client(strUrl.c_str());
		http_request req(methods::DEL);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());//v

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				auto dataWstr = job.at(U("data")).as_string();
				char dataBuffer[32];
				memset(dataBuffer, 0, 32);
				//wStringToChar(dataWstr, dataBuffer);
				strcpy(dataBuffer, dataWstr.c_str());
				if (strcmp(dataBuffer, "delete success"))
				{
					ret = 0;
				}
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}
}

//
int getResourceInfo(TY_SIM_RESCOURCE_INFO *pSimRes, const char*url)
{
    string gToken;
	string urlStr = string(url);

	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string urlPath(U("/_api/Common/getSimResourceInfo"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::GET);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());
		http_response response = client.request(req).get(); //request body

		if (response.status_code() == status_codes::OK)
		{
       cout << "--> status_codes::ok" << endl;
      const json::value &jv = response.extract_json().get();
      
			const web::json::object& jobj = jv.as_object();
			string status = jobj.at(U("status")).as_string();
			status = response.to_string();
			if (status == U("error"))
			{				
				string msg = jobj.at(U("message")).as_string();
				if (msg == U("10001"))
				{
					cout << "Error message:NO_MODEL\n" << endl;
					return 0;
				}
				cout << "Error message:" <<  msg << endl;
				ret = -1;
			}
			else
			{

				const web::json::object& data = jobj.at(U("data")).as_object();
				size_t timeResNamesNum = data.at(U("timeResNames")).as_array().size();
				pSimRes->timeResNum = data.at(U("timeResNum")).as_integer();
				if (pSimRes->timeResNum == timeResNamesNum)
				{
					auto dataArray = data.at(U("timeResNames")).as_array();
					for (int i = 0; i < timeResNamesNum; i++)
					{
						string timeResName = dataArray[i].as_string();
             			//memcpy(pSimRes->timeResNames[i], timeResName.c_str(), timeResName.length());
						strcpy(pSimRes->timeResNames[i], timeResName.c_str());	
						 
				
					}
				}
				size_t modelNamesNum = data.at(U("modelNames")).as_array().size();
				pSimRes->modelNum = data.at(U("modelNum")).as_integer();
				if (pSimRes->modelNum == modelNamesNum)
				{
					int modelNameIndex = 0;
					auto dataArray = data.at(U("modelNames")).as_array();
					for (int i = 0; i < modelNamesNum; i++)
					{
						string modelName = dataArray[i].as_string();
						if (modelName == U("storage") || modelName == U("test"))
						{
							pSimRes->modelNum--;
						}
						else
						{
                			//memcpy(pSimRes->modelNames[modelNameIndex], modelName.c_str(), modelName.length());
							strcpy(pSimRes->modelNames[modelNameIndex], modelName.c_str());	
						  	modelNameIndex++;
						}
					}
				}
				pSimRes->cpuNum = data.at(U("cpuNum")).as_integer();
				ret = 0;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string msg = job.at(U("message")).as_string();
      cout << "Error message:" << msg << endl;
			ret = -1;
		}

		return ret;
	}
	catch (const std::exception& e)
	{
    cout << "--->Error exception :" << e.what() <<endl;
		return -1;
	}
	return 0;
}

int getModelStatus(char* modelName, TY_MODEL_STATUS *pModelStatus, const char*url)
{
	string gToken;
	string modelNameStr;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = -1;
		if ((string)modelName == "")
		{
			cout << "no model exist" << endl;
      
			return -1;
		}

		string urlPath(U("/_api/Common/getModelsStatus?"));
		string strUrl = urlStr + urlPath;
		
		modelNameStr = string(modelName);

		strUrl = strUrl + U("modelsName=") + modelNameStr;
			
		http_client client(strUrl.c_str());
		http_request req(methods::GET);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());
		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();

			string status = job.at(U("status")).as_string();
			if (status == U("error"))
			{
				string pingCmd = "chcp 936";
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
        
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
			else
			{
				auto dataArray = job.at(U("data")).as_array();

				size_t num = dataArray.size();
				int i = num - 1;
				/*get name*/
				string tmpStr = dataArray[i].at(U("modelName")).as_string();
				pModelStatus->period = (long)dataArray[i].at(U("period")).as_integer();
				pModelStatus->autoWrite = (long)dataArray[i].at(U("autoWrite")).as_bool();
				pModelStatus->autoRead = (long)dataArray[i].at(U("autoRead")).as_bool();
				pModelStatus->overrunTimes = (long)dataArray[i].at(U("overrunTimes")).as_integer();
				pModelStatus->cpuIndex = (long)dataArray[i].at(U("cpuIndex")).as_integer();
				pModelStatus->maxRunTime = (long)dataArray[i].at(U("maxRunTime")).as_integer();
				pModelStatus->pid = (long)dataArray[i].at(U("pid")).as_integer();
				pModelStatus->frameCount = (long)dataArray[i].at(U("frameCount")).as_integer();
				strcpy(pModelStatus->modelName, tmpStr.c_str());
				pModelStatus->avgRunTime = (long)dataArray[i].at(U("avgRunTime")).as_integer();
				pModelStatus->maxDelayTime = (long)dataArray[i].at(U("maxDelayTime")).as_integer();
				pModelStatus->delayTime = (long)dataArray[i].at(U("delayTime")).as_integer();
				pModelStatus->modelRunStatus = (ModelRunStatusE)dataArray[i].at(U("modelRunStatus")).as_integer();
				pModelStatus->runTime = (long)dataArray[i].at(U("runTime")).as_integer();
				ret = 0;			
				
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		cout << "Error exception :%s" << e.what() << endl;
		return -1;
	}
	return 0;
}

int getModelListNum(int& modelNum, const char* url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];

	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		//json::value obj;
		string urlPath(U("/_api/SimModel/TPOS/getModelList?"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::GET);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());
		http_response response = client.request(req).get(); //request body

		if (response.status_code() == status_codes::OK)
		{

			int num = 0;
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();

			string status = job.at(U("status")).as_string();//�ж�status�Ƿ���ȷ����ȷ��ȡdata 
			if (status == U("error"))//modify by sh 0919
			{
				string pingCmd = "chcp 936";
				//_popen(pingCmd.c_str(), "rt");
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
			else
			{
				auto dataArray = job.at(U("data")).as_array();
				size_t num = dataArray.size();
				modelNum = num;
				ret = 0;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		printf("Error exception :%s\n", e.what());
		return -1;
	}

}


int loadModel(TY_SIM_CONFIG simConfig, const char*url)
{
	string gToken;
	string urlStr = string(url);

	string storageNameStr = string(simConfig.storageName);
	storageNameStr = U("storageName=") + storageNameStr;


	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	int ret = 0;
	try
	{
		string urlPath(U("/_api/TPOS/loadModel?"));
		string strUrl = urlStr + urlPath + storageNameStr;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);
		req.headers().add(U("x-access-token"), gToken.c_str());
		req.headers().add(U("Content-Type"), U("application/json"));

		json::value obj;

		json::value objsimConfig;
		objsimConfig[U("timeResName")] = json::value::string(string(simConfig.timeResName));
		int basePeriod = 0;
		if (simConfig.modelNum == 1)
		{
			basePeriod = simConfig.pModelConfigs[0].period;
		}
		else if (simConfig.modelNum > 1)
		{
			basePeriod = simConfig.pModelConfigs[0].period;
			for (int i = 0; i < simConfig.modelNum - 1; i++)
			{
				basePeriod = measure(basePeriod, simConfig.pModelConfigs[i + 1].period);
			}
		}
		objsimConfig[U("basePeriod")] = basePeriod;

		objsimConfig[U("stored")] = json::value::boolean(true);

		json::value objModels;
		objModels = json::value::array(simConfig.modelNum);
		for (int i = 0; i < simConfig.modelNum; i++)
		{
			json::value objModel;
			objModel[U("modelName")] = json::value::string(string(simConfig.pModelConfigs[i].modelName));
			objModel[U("periodMul")] = simConfig.pModelConfigs[i].period / basePeriod;
			objModel[U("cpuIndex")] = simConfig.pModelConfigs[i].cpuIndex;
			objModel[U("uuid")] = json::value::string(string(simConfig.pModelConfigs[i].uuid));
			json::value objVars;
			int num = simConfig.pModelConfigs[i].storageVarNum;
			int mulNum = simConfig.pModelConfigs[i].storageVarNum + simConfig.pModelConfigs[i].storageSignalNum;
			objVars = json::value::array(mulNum);//objVars.array(mulNum);

			for (int iVar = 0; iVar < num; iVar++)
			{
				json::value objArray;
				objArray[U("varPath")] = json::value::string(string(simConfig.pModelConfigs[i].storageVar[iVar]));

				objArray[U("varType")] = 0;//json::value::(stringToWString(string(modelNameValue.modelName[i])));
				objVars[iVar] = objArray;
			}
			for (int iVar = num; iVar < mulNum; iVar++)
			{
				json::value objArray;
				objArray[U("varPath")] = json::value::string(simConfig.pModelConfigs[i].storageSignal[iVar - num]);

				objArray[U("varType")] = 1;//var:0 signal:1
				objVars[iVar] = objArray;
			}
			objModel[U("variables")] = objVars;
			objModels[i] = objModel;
		}
		objsimConfig[U("models")] = objModels;
		obj[U("simConfig")] = objsimConfig;

		req.set_body(objsimConfig.serialize());

		http_response response = client.request(req).get(); //request body

		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				ret = 0;
			}
			else
			{
				string pingCmd = "chcp 936";
				//_popen(pingCmd.c_str(), "rt");
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string pingCmd = "chcp 936";
			//_popen(pingCmd.c_str(), "rt");
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		cout << "Error exception :" << e.what() << endl;
		return -1;
	}
	return 0;
}

int unloadModel(const char*url)
{
	string gToken;
	string urlStr = string(url);
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	int ret = 0;
	try
	{
		string urlPath(U("/_api/TPOS/unloadModel"));
		string strUrl = urlStr + urlPath;
		http_client client(strUrl.c_str());
		http_request req(methods::POST);
		req.headers().add(U("x-access-token"), gToken.c_str());
		req.headers().add(U("Content-Type"), U("application/json"));
		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		setlocale(LC_ALL, "chs");
		cout << "Error exception :" << e.what() << endl;
		return -1;
	}
	
}

int startModel(TY_MODELNAME_VALUE modelNameValue, const char*url)
{
	string gToken;
	string urlStr = string(url);
	string modelNameStr;
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}

	try
	{
		int ret = 0;
		string urlPath(U("/_api/TPOS/startModel?"));
		string strUrl = urlStr + urlPath;
		for (int i = 0; i < modelNameValue.modelNumber; i++)
		{
			string modelNameStr = string(modelNameValue.modelName[i]);
			if(modelNameStr.empty())
			{
				break;
			}
			modelNameStr = U("modelName=") + modelNameStr;
			if(i != (modelNameValue.modelNumber - 1))
				modelNameStr = modelNameStr + U("&");
			strUrl = strUrl + modelNameStr;
		}

		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		cout << "Error exception :" << e.what() << endl;
		return -1;
	}
	return 0;
}


int stopModel(TY_MODELNAME_VALUE modelNameValue, const char*url)
{
	string gToken;
	string urlStr = string(url);
	string modelNameStr;
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string modelNameStr = string(modelNameValue.modelName[0]);
		string urlPath(U("/_api/TPOS/stopModel?"));
		string strUrl = urlStr + urlPath;
		for (int i = 0; i < modelNameValue.modelNumber; i++)
		{
			string modelNameStr = string(modelNameValue.modelName[i]);
			if (modelNameStr.empty())
			{
				break;
			}
			modelNameStr = U("modelName=") + modelNameStr;
			if (i != (modelNameValue.modelNumber - 1))
				modelNameStr = modelNameStr + U("&");
			strUrl = strUrl + modelNameStr;
		}


		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		cout << "Error exception :" << e.what() << endl;
		return -1;
	}
  return 0;
}
int pauseModel(TY_MODELNAME_VALUE modelNameValue, const char*url)
{
    string gToken;
	string urlStr = string(url);
	string modelNameStr;
	gToken = gUrlToToken[urlStr];
	if (gToken == U(""))
	{
		return -1;
	}
	try
	{
		int ret = 0;
		string modelNameStr = string(modelNameValue.modelName[0]);
		string urlPath(U("/_api/TPOS/pauseModel?"));
		string strUrl = urlStr + urlPath;
		for (int i = 0; i < modelNameValue.modelNumber; i++)
		{
			string modelNameStr = string(modelNameValue.modelName[i]);
			if (modelNameStr.empty())
			{
				break;
			}
			modelNameStr = U("modelName=") + modelNameStr;
			if (i != (modelNameValue.modelNumber - 1))
				modelNameStr = modelNameStr + ("&");
			strUrl = strUrl + modelNameStr;
		}

		http_client client(strUrl.c_str());
		http_request req(methods::POST);

		req.headers().add(U("Content-Type"), U("application/json"));
		req.headers().add(U("x-access-token"), gToken.c_str());

		http_response response = client.request(req).get(); //request body
		if (response.status_code() == status_codes::OK)
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			string status = job.at(U("status")).as_string();
			if (status == U("success"))
			{
				ret = 0;
			}
			else
			{
				setlocale(LC_ALL, "chs");
				string msg = job.at(U("message")).as_string();
				cout << "Error message:" << msg << endl;
				ret = -1;
			}
		}
		else
		{
			const json::value& jv = response.extract_json().get();
			const web::json::object& job = jv.as_object();
			setlocale(LC_ALL, "chs");
			string msg = job.at(U("message")).as_string();
			cout << "Error message:" << msg << endl;
			ret = -1;
		}
		return ret;
	}
	catch (const std::exception& e)
	{
		cout << "Error exception :" << e.what() << endl;
		
		return -1;
	}
   return 0;
}




