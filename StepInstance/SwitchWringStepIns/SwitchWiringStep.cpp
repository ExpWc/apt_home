#include "SwitchWiringStep.h"
#include "SwitchWiringInterface.h"

CBaseStep* CreateStep()
{
	return new CSwitchWiringStep();
}

CSwitchWiringStep::CSwitchWiringStep()
{
}
CSwitchWiringStep::~CSwitchWiringStep()
{
}
// FILE* fd = NULL;
int CSwitchWiringStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
    int ret = 0;
    ret = CBaseStep::parse(pStepElement, pStep);
    if (ret != 0)
    {
        //REPORT(REPORT_ERROR, "parse failed\n");
        return ret;
    }
    bool bRet = false;
    string tmpStr;
    bRet = GetValue(pStepElement, "Command", tmpStr);
    if (!bRet)
    {
        //REPORT(REPORT_ERROR, "Get Command failed\n");
        return -1;
    }
    m_command = tmpStr.c_str();

    bRet = GetValue(pStepElement, "Type", tmpStr);
    if (!bRet)
    {
        //REPORT(REPORT_ERROR, "Get Type failed\n");
        return -1;
    }
    m_type = tmpStr.c_str();

    //REPORT(REPORT_DEBUG, "parse m_type:%d\n", m_type);

    bRet = GetValue(pStepElement, "Value", tmpStr);
    if (!bRet)
    {
        //REPORT(REPORT_ERROR, "Get Value failed\n");
        return -1;
    }
    m_value = tmpStr.c_str();
   // REPORT(REPORT_DEBUG, "parse m_Value:%d\n", m_value);

	bRet = GetValue(pStepElement, "Projectuuid", tmpStr);
    if (!bRet)
    {
        //REPORT(REPORT_ERROR, "Get Value failed\n");
        return -1;
    }
    m_projectuuid = tmpStr.c_str();
   // REPORT(REPORT_DEBUG, "parse m_Value:%d\n", m_value);
    return 0;
}
int CSwitchWiringStep::init()
{
    m_stepStatus = STATUS_READY;
   // REPORT(REPORT_DEBUG, "init\n");
    return 0;
}

int CSwitchWiringStep::run()
{
    int ret = 0;
    m_stepStatus = STATUS_RUNNING;
    printf("==========CSwitchWiringStep====\n");
    string urlStr = string(URL);
    //http_response response = NULL;
    unsigned int status_code = 0;
   
    if(m_type.compare("cfg") == 0)
    {
		m_stepStatus = STATUS_SUCCESS;
		switchWiringByModel("192.168.1.121", "10011", m_value, m_projectuuid);
		ret = 0;
		#if 0
        string urlPath(U("/modelBased/switchSchemes/{"));
        string strUrl = urlStr + urlPath + m_value + "}";
        http_client_config config;
        //cout << "strUrl = " <<  strUrl << endl;
        config.set_timeout(utility::seconds(10));
        http_client client(strUrl.c_str());
        http_request request(methods::PUT);
        request.headers().add(U("Content-Type"), U("application/json"));
        response = client.request(request).get();
		#endif
    }
    else if (m_type.compare("icd") == 0)
    {
		#if 0
        string urlPath = "/modelBased/chns";
        //cout << m_value <<endl;
        string strUrl = urlStr + urlPath;
        http_client_config config;
        config.set_timeout(utility::seconds(10));
        http_client client(strUrl.c_str());
        // req.headers().add(U("x-access-token"), U("eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJhdWQiOiJyb290IiwiZXhwIjoxNjkwNDczNjcyLCJpYXQiOjE2OTAzODcyNzJ9.AhI_BP7RqrlvtEcPr1m7E1LW5083HTA2sXwgQO7OWck"));
        json::value jsonString = json::value::string(string(m_value));
        //json::value::string(string(m_value));
        // utility::string_t jsonString = m_value;
        http_request request(methods::PUT);
        request.headers().add(U("Content-Type"), U("application/json"));
        request.set_body(jsonString.serialize());
        client.request(request).then([](http_response response){  
        if(response.status_code() == status_codes::OK)  
        {     
        
            cout << "STATUS_OK" << endl;
        }  
        else  
        {  

            cout << "STATUS_ERROR" << endl;
        } } ).wait();
		#endif
		m_stepStatus = STATUS_SUCCESS;
		//switchWiringByIcd("192.168.1.121", "11011", "a1c7b09d-61af-4a77-b27e-b6d673c3f824", "a90454d0-ad7e-11eb-ab9b-a374d85afd31");
		//switchWiringByIcd("192.168.1.121", "11011", m_value, m_projectuuid);
		switchWiringByIcdJson("192.168.1.121", "11011", m_value, m_projectuuid);
        ret = 0;
    }
    else
    {
        m_stepStatus = STATUS_ERROR;
        string errorMsg = string("type is ") + m_type;
        SWITCHWIRINGLOG(LOG_ERROR, errorMsg);
        cout << "Command error" << endl;
        ret = -1;
    }

    return ret;
}
int CSwitchWiringStep::stop()
{
    m_stepStatus = STATUS_READY;
    return 0;
}
int CSwitchWiringStep::log(TY_LOG_TYPE logType, const char *fileName, const char *funName,
		int lineNo, string msg)
{
	CLog *pLog = CLog::getInstance();
	char tmp[1024];
	sprintf(tmp, "Switch wiring: command = %s type = :%s value:%s\n%s\n",
			 m_command.c_str(), m_type.c_str(), m_value.c_str(), m_projectuuid.c_str());
	pLog->log(logType, fileName, funName, lineNo, tmp);
	return 0;
}
