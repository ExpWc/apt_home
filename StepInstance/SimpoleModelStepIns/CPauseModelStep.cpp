/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2024-03-09 00:13:09
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2024-03-09 00:18:06
 * @FilePath: /ATP_V1.0.0/StepInstance/SimpoleModelStepIns/CPauseModelStep.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CPauseModelStep.h"
#include "ICDManager.h"
#include "ParamData.h"

#define PASS_LEN_4 4


CPauseModelStep::CPauseModelStep()
{

}
CPauseModelStep::~CPauseModelStep()
{
    
}

int CPauseModelStep::parse(TiXmlElement *pStepElement, CBaseStep* pStep)
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

int CPauseModelStep::init()
{

    m_stepStatus = STATUS_READY;
    REPORT(REPORT_INFO, "init ok \n");
    return 0;
}


int CPauseModelStep::run()
{
    int ret = 0;
    m_stepStatus = STATUS_RUNNING;
   

    ret = pauseModel(m_modelNameValue, m_addr);
    if(ret != 0)
    {
        string errorMsg = "pauseModel faild, m_addr is ";
        errorMsg = errorMsg + string(m_addr);
        COMSTEPLOG(LOG_ERROR, errorMsg); 
        REPORT(REPORT_ERROR, "pauseModel faild\n");
    }

    m_msg = "Success";
    m_stepStatus = STATUS_SUCCESS;
    REPORT(REPORT_INFO, "run\n");
    return 0;
}

string CPauseModelStep::getAttachInfo()
{
	
	//char data[32];
	//sprintf(data, "%lf", this->m_dataDouble);
	
   // string retString = string(data);
    string retString = "";
	return retString;
	//return (string)NULL;
}


