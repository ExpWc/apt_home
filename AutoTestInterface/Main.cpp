#include "AutoTestInterface.h"
#include "AutoTestInterface.h"
int main()
{
	char* presentCase = "0b9fb83f-9c94-4b4f-b130-83acf3c8da94";
	char* presentStep = "6bdb3f1a-0b12-4b40-8aec-9df5aa91841e";
	parseCaseBinding("/home/V1.0.0/HiAutoTestServer/bin/");
	setPresentCase(presentCase);
	setPresentStep(presentStep);
	createDataDictionaryXml("/home/V1.0.0/HiAutoTestServer/bin/");

}
