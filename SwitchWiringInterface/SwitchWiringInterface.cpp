/*
 * switchWiring.cpp
 * 
 * Created on: Oct 17, 2023
 * Author: root
 */
 #include <cstdio>
 #include <iostream>
 #include <cstring>
 #include <cpprest/http_client.h>
 #include <cpprest/filestream.h>
 #include <cpprest/json.h>
 #include "SwitchWiringInterface.h"
 

 using namespace web;
 using namespace web::http;
 using namespace web::http::client;
 using namespace concurrency::streams;
 
 using namespace web::json;

 int switchWiringByModel(string ip, string port, string modelUuid, string projectUuid)
 {
	printf("enter switchWiringByModel\n"); 
	//string strUrl = "http://192.168.1.121:11011/api/v1/wire/modelBased/switchScheme/12765f16-15e2-409e-90dd-10e833525902?\
	proj_uuid=3404c760-0366-11ee-a3da-b94b2a0f124d";
	char strTmp[1024];
	sprintf(strTmp, "http://%s:%s/api/v1/wire/modelBased/switchScheme/%s?proj_uuid=%s",
	ip.c_str(), port.c_str(), modelUuid.c_str(), projectUuid.c_str());
	string strUrl(strTmp);
	
	http_response response = NULL;
	http_client_config config;
	
	config.set_timeout(utility::seconds(10));
	int ret = 0;
	http_client client(strUrl.c_str());
	http_request request(methods::PUT);
	request.headers().add(U("Content-Type"), U("application/json"));
	response = client.request(request).get();
	printf("response.status_code %d\n", response.status_code());
	if(response.status_code() == status_codes::OK)
	{
		const json::value &jv = response.extract_json().get();
		const web::json::object &job = jv.as_object();
		string status = job.at(U("data")).as_string();
		if(status == U("ok"))
		{
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
		const json::value &jv = response.extract_json().get();
		const web::json::object &job = jv.as_object();
		setlocale(LC_ALL, "chs");
		string msg = job.at(U("message")).as_string();
		cout << "Error message:" << msg << endl;
		ret = -1;
	}
	return ret;
 }
 int switchWiringByIcd(string ip, string port, string icdUuid, string projectUuid)
 {
	printf("enter switchWiringByIcd\n"); 
	string strUrl = "http://" + ip + ":" + port + "/api/v1/wire/modelBased/chns?project_uuid=" + projectUuid;
	
	http_response response = NULL;
	http_client_config config;
	
	config.set_timeout(utility::seconds(10));
	int ret = 0;
	http_client client(strUrl.c_str());
	http_request request(methods::PUT);
	request.headers().add(U("Content-Type"), U("application/json"));
	json::value obj;
	int num = 1;
	for(int i = 0; i < num; i++)
	{
		json::value obj_array;
		obj_array[U("status")] = json::value::string(U("1"));
		obj_array[U("type")] = json::value::string(U("nobus_analog"));//jstring(stringToWString(string(oList[i].nodeType)));
		obj_array[U("uuid")] = json::value::string(U(icdUuid));
		obj[i] = obj_array;
	}
	json::value obj_array;
	string jsonStr = obj.serialize();
	printf("%s\n", jsonStr.c_str());
	request.set_body(jsonStr);
	response = client.request(request).get();
	printf("response.status_code %d\n", response.status_code());
	if(response.status_code() == status_codes::OK)
	{
		const json::value& jv = response.extract_json().get();
		const web::json::object& job = jv.as_object();
		string status = job.at(U("data")).as_string();
		if(status == U("ok"))
		{
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
		cout << "Error message:" << msg << endl;
		ret = -1;
	}
	return ret;
 }
 int switchWiringByIcdJson(string ip, string port, string icdJson, string projectUuid)
 {
	printf("enter switchWiringByIcdJson\n"); 
	string strUrl = "http://" + ip + ":" + port + "/api/v1/wire/modelBased/chns?proj_uuid=" + projectUuid;
	
	http_response response = NULL;
	http_client_config config;
	
	config.set_timeout(utility::seconds(10));
	int ret = 0;
	http_client client(strUrl.c_str());
	http_request request(methods::PUT);
	request.headers().add(U("Content-Type"), U("application/json"));
	/*json::value obj;
	int num = 1;
	for(i = 0; i < num; i++)
	{
		json::value obj_array;
		obj_array[U("status")] = json::value::string(U("1"));
		obj_array[U("type")] = json::value::string(U("nobus_analog"));//jstring(stringToWString(string(pList[i].nodeType)));
		obj_array[U("uuid")] = json::value::string(U(icdUuid));
		obj[i] = obj_array;
	}
	json::value obj_array;
	string jsonStr = obj.serialize();*/
	string jsonStr = icdJson;
	printf("%s\n", jsonStr.c_str());
	request.set_body(jsonStr);
	response = client.request(request).get();
	printf("response.status_code %d\n", response.status_code());
	if(response.status_code() == status_codes::OK)
	{
		const json::value &jv = response.extract_json().get();
		const web::json::object &job = jv.as_object();
		string status = job.at(U("data")).as_string();
		if(status == U("ok"))
		{
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
		const json::value &jv = response.extract_json().get();
		const web::json::object &job = jv.as_object();
		setlocale(LC_ALL, "chs");
		string msg = job.at(U("message")).as_string();
		cout << "1Error message:" << msg << endl;
		ret = -1;
	}
	return ret;
 }
 
