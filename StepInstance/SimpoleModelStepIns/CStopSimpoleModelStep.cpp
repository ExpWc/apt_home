#include "CStopSimpoleModelStep.h"
#include "ICDManager.h"
#include "CustomDlStep.h"
#include "ParamData.h"

#define PASS_LEN_4 4

int gSimpoleModelStatus;
CStopSimpoleModelStep::CStopSimpoleModelStep()
{

}
CStopSimpoleModelStep::~CStopSimpoleModelStep()
{
    
}

int CStopSimpoleModelStep::parse(TiXmlElement *pStepElement, CBaseStep* pStep)
{

    int ret = 0;
    bool bRet = false;
    int modelIndex = 0;
    TiXmlElement *pMdlInfoElement = NULL;
    TiXmlElement *pParamElement = NULL;

    /*parse router*/
    ret = CBaseSimpoleInterfaceStep::parseRouter(pStepElement);
    if(ret != 0)
    {
        return ret;
    }

    for(pMdlInfoElement = pStepElement->FirstChildElement(); pMdlInfoElement; pMdlInfoElement = pMdlInfoElement->NextSiblingElement())
    {
        string tmpModelName = "";
        bRet = GetValue(pMdlInfoElement, "ModelName", tmpModelName);
        if (!bRet)
        {
            REPORT(REPORT_ERROR, "Get ModelName failed\n");
            return -1;
        }
       
        strcpy(m_modelNameValue.modelName[modelIndex], tmpModelName.c_str());
        modelIndex++;
    }

    m_modelNameValue.modelNumber = modelIndex;
    REPORT(REPORT_INFO, "parse\n");
    return ret;
}

int CStopSimpoleModelStep::init()
{

    m_stepStatus = STATUS_READY;
    REPORT(REPORT_INFO, "init ok \n");
    return 0;
}


int CStopSimpoleModelStep::run()
{
    int ret = 0;
    m_stepStatus = STATUS_RUNNING;
   

    ret = stopModel(m_modelNameValue, m_addr);
    if(ret != 0)
    {
        REPORT(REPORT_ERROR, "stopModel faild\n");
    }
    gSimpoleModelStatus = 1;
    m_msg = "Success";
    m_stepStatus = STATUS_SUCCESS;
    REPORT(REPORT_INFO, "run\n");
    return 0;
}

string CStopSimpoleModelStep::getAttachInfo()
{
	
	//char data[32];
	//sprintf(data, "%lf", this->m_dataDouble);
	
   // string retString = string(data);
    string retString = "";
	return retString;
	//return (string)NULL;
}


