#include "StoreModelVar.h"
#include "HiMGlobalVarLib.h"
#include "HiMParaComLib.h"
#include "ParamData.h"
#include "HiReport.h"
TY_MODEL_VAR_MAP gModelVarMap;
int showCase(char *modelName)
{
	int i = 0;
	CObjectData *pAutoTest = CObjectData::getInstance();
	REPORT(REPORT_ERROR, "=======\n");
	char buff[254] = {0};
	sprintf(buff, "%s%s%s", "lib", modelName, ".so");
	string dllName(buff);

	for(TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.begin(); iterCase !=pAutoTest->m_caseMap.end();iterCase++ )
	{
		REPORT(REPORT_ERROR, "1111111\n");
		for(TY_STEP_MAP::iterator iterStep = iterCase->second.m_StepMap.begin(); iterStep != iterCase->second.m_StepMap.end(); iterStep++)
		{
			REPORT(REPORT_ERROR, "===strStepID:%s\n",iterStep->first.c_str());
		}
	}
	return 0;
}

int storeModelVar(char *modelName)
{
	CObjectData *pAutoTest = CObjectData::getInstance();
	char buff[254] = {0};
	sprintf(buff, "%s%s%s", "lib", modelName, ".so");
	string dllName(buff);
	//int ret = showCase(modelName);
	TY_CASE_MAP::iterator iterCase = pAutoTest->m_caseMap.find(pAutoTest->m_preCaseId);
	TY_STEP_MAP::iterator iterStep = iterCase->second.m_StepMap.find(iterCase->second.m_presentStepId);
	if(iterStep != iterCase->second.m_StepMap.end())
	{
		TY_OUTPUT_MAP::iterator iterOutput = iterStep->second.outputMap.find(dllName);
		if(iterOutput != iterStep->second.outputMap.end())
		{
			TY_OUTPUT_VAR_MAP::iterator iter = iterOutput->second.begin();
			if(iter == iterOutput->second.end())
			{
				REPORT(REPORT_ERROR, "if(iter == iterOutput->second.end())\n");
			}
			while(iter != iterOutput->second.end())
			{
#if 0
				TY_OUTPUT_VAR var = iter->second;
				TY_MODEL_VAR_MAP::iterator modelVarIter = gModelVarMap.find(var.strVarName);
				if(modelVarIter != gModelVarMap.end())
				{
					gModelVarMap.erase(var.strVarName);
				}
				var.value.llValue = 0;

				double value;
				getVarDataByName(var.strVarName.c_str(), (char*)&value);
				TY_MODEL_VAR modelVar;
				modelVar.strName = var.strVarName;
				modelVar.strType = var.strVarType;
				modelVar.value = value;
				gModelVarMap.insert(pair<string, TY_MODEL_VAR>(modelVar.strName, modelVar));
#endif
#if 1 //>>modify by wc 20231103
				TY_OUTPUT_VAR var = iter->second;
				double value = 0;
				getVarDataByName(var.strReferenced.c_str(), (char*)&value);
				TY_MODEL_VAR_MAP::iterator modelVarIter = gModelVarMap.find(var.strReferenced.c_str());
				if(modelVarIter != gModelVarMap.end())
				{
					modelVarIter->second.value = value;
					//REPORT(REPORT_ERROR, "store output Name:%s value:%lf\n", var.strReferenced.c_str(), value);
				}
				else
				{
					TY_MODEL_VAR modelVar;
					modelVar.strName = var.strReferenced;
					modelVar.strType = var.strVarType;
					modelVar.value = value;
					gModelVarMap.insert(pair<string, TY_MODEL_VAR>(modelVar.strName, modelVar));
					//REPORT(REPORT_ERROR, "store output Name:%s value:%lf\n", var.strReferenced.c_str(), value);
				}
#endif
//				gModelVarMap[modelVar.strName] = modelVar;
				
				iter++;
			}
		}
		else
		{
			REPORT(REPORT_ERROR, "not find iterCase->second.m_StepMap\n");
		}

		TY_INPUT_MAP::iterator iterInput = iterStep->second.inputMap.find(dllName);
		if(iterInput != iterStep->second.inputMap.end())
		{
			TY_INPUT_VAR_MAP::iterator iter = iterInput->second.begin();
			map<string, int> icdMap;
			if(iter == iterInput->second.end())
			{
				//REPORT(REPORT_ERROR, "iter == iterInput->second.end()\n");//0902
			}
			while(iter != iterInput->second.end())
			{
				TY_INPUT_VAR var = iter->second;
				var.value.llValue = 0;
				double value;
				getVarDataByName(var.strReferenced.c_str(), (char*)&value);
				TY_MODEL_VAR modelVar;
				modelVar.strName = var.strReferenced;
				modelVar.strType = var.strVarType;
				modelVar.value = value;
				//gModelVarMap.insert(pair<string, TY_MODEL_VAR>(modelVar.strName, modelVar));
				gModelVarMap[modelVar.strName] = modelVar;
				//REPORT(REPORT_ERROR, "store input Name:%s value:%lf,pAutoTest->m_preCaseId.c_str():%s, iterCase->second.m_presentStepId:%s,pAutoTestAdd:%x\n", modelVar.strName.c_str(), value,pAutoTest->m_preCaseId.c_str(),iterCase->second.m_presentStepId.c_str(),pAutoTest);
				iter++;
			}
		}
		else
		{
			REPORT(REPORT_ERROR, "not find iterStep->second.inputMap\n");
		}
	}
	else
	{
		REPORT(REPORT_ERROR, "not find iterCase->second.m_StepMap:pAutoTest->m_preCaseId:%s, iterCase->second.m_presentStepId:%s,modelName:%s,pAutoTestADD:%x\n",pAutoTest->m_preCaseId.c_str(),iterCase->second.m_presentStepId.c_str(),modelName,pAutoTest);

		//REPORT(REPORT_ERROR, "not find iterCase->second.m_StepMap\n");
	}

}
//int getModelVar(char *varName, double *value)
int getModelVarInterface(char *varName, double *value)
{
	string strVarName = varName;
	TY_MODEL_VAR_MAP::iterator iter = gModelVarMap.find(strVarName);

	if(iter != gModelVarMap.end())
	{
		*value = iter->second.value;
		return 0;
	}
	return -1;
}

int clearModelVarMap()
{
	gModelVarMap.clear();
	return 0;
}
