
#include "CurveInterface.h"
#include "CCurve.h"


int updateCurveData(char *modelName)
{
    if(modelName == NULL)
    {
        printf("input modelName is NULL\n");
        return -1;
    }
    
    CCurveManager *pCurveManager = CCurveManager::getInstance();
    pCurveManager->updateCurveInputData(modelName);
    return 0;
}