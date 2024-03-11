#ifndef __STORE_MODEL_VAR_H_
#define __STORE_MODEL_VAR_H_
#include <string>
#include <vector>
#include <map>

#include <stdio.h>
#include <stdlib.h>
using namespace std;

typedef struct _model_var
{
	string strName;
	string strType;
	double value;
}TY_MODEL_VAR;

typedef map<string, TY_MODEL_VAR> TY_MODEL_VAR_MAP;



#endif
