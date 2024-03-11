#ifndef _STEPFACTORY_H
#define _STEPFACTORY_H
#pragma once
#include "BaseStep.h"

class CStepFactory
{
     
public:
    CStepFactory();
    ~CStepFactory();

public:
    static CBaseStep* createStep(string type);
};

#endif
