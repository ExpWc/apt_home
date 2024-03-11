#include "CUnLoadModelStep.h"
#include "ICDManager.h"
#include "ParamData.h"
#include "CStorageManager.h"

#define PASS_LEN_4 4


CUnLoadModelStep::CUnLoadModelStep()
{

}
CUnLoadModelStep::~CUnLoadModelStep()
{
    
}

int CUnLoadModelStep::parse(TiXmlElement *pStepElement, CBaseStep* pStep)
{

    int ret = 0;
    bool bRet = false;
    TiXmlElement *pChnInfoElement = NULL;
    TiXmlElement *pParamElement = NULL;

    /*parse router*/
    ret = CBaseSimpoleInterfaceStep::parseRouter(pStepElement);
    if(ret != 0)
    {
        return ret;
    }

    REPORT(REPORT_INFO, "parse\n");
    return ret;
}

int CUnLoadModelStep::init()
{

    m_stepStatus = STATUS_READY;
    REPORT(REPORT_INFO, "init ok \n");
    return 0;
}

int CUnLoadModelStep::run()
{
     m_stepStatus = STATUS_RUNNING;
    int ret = unloadModel(m_addr);
    if(ret != 0)
    {
        REPORT(REPORT_INFO, "run faild \n");
    }

    m_msg = "Success";
    m_stepStatus = STATUS_SUCCESS;
    REPORT(REPORT_INFO, "run\n");
    return 0;
}

string CUnLoadModelStep::getAttachInfo()
{
	
	//char data[32];
	//sprintf(data, "%lf", this->m_dataDouble);
	
   // string retString = string(data);
    string retString = "";
	return retString;
	//return (string)NULL;
}


