#include "CCurve.h"
#include "Constant.h"
#include "CustomWave.h"
#include "SawtoothWave.h"
#include "SineWave.h"
#include "SlopeWave.h"
#include "SquareWave.h"
#include "TriangleWave.h"
#include "parse_xml_api.h"

int main()
{

	char configPath[512] = "/home/V1.0.0/HiAutoTestServer/bin/CurveConfig.xml";
	CCurveManager *pCurveManager = CCurveManager::getInstance();
	pCurveManager->loadConfig(configPath);
	string stepId("50d1da1b-47fc-48e1-9f18-014faeca9f5d");
	TY_CURVE_BIND_MAP::iterator iter = pCurveManager->m_curveBindMap.find(stepId);
/*
	for(TY_CURVE_LIST::iterator it = iter->second.begin() ;it != iter->second.end(); it++)
	{
		(*it)->start();
	}

	for(int i = 0; i < 100; i++)
	{
		for(TY_CURVE_LIST::iterator it = iter->second.begin(); it != iter->second.end(); it++)
		{
			(*it)->run();
		}

	}
	for(TY_CURVE_LIST::iterator it = iter->second.begin(); it != iter->second.end(); it++)
	{
		(*it)->stop();
	}
*/
	return 0;
}
