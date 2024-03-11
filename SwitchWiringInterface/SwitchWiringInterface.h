/*
 * switchWiring.h
 *
 * Created on: Oct 17, 2023
 * Author: root
 */
#ifndef SWITCHWIRING_H_
#define SWITCHWIRING_H_

#include <iostream>

using namespace std;

//#define DLL_EXPORT __attribute((visibility("default")))

extern "C"
{
	 int switchWiringByModel(string ip, string port, string modelUuid, string projectUuid);
	 int switchWiringByIcd(string ip, string port, string icdUuid, string projectUuid);
	 int switchWiringByIcdJson(string ip, string port, string icdJson, string projectUuid);

	//DLL_EXPORT int switchWiringByModel(string ip, string port, string modelUuid, string projectUuid);
	//DLL_EXPORT int switchWiringByIcd(string ip, string port, string icdUuid, string projectUuid);
	//DLL_EXPORT int switchWiringByIcdJson(string ip, string port, string icdJson, string projectUuid);
};

#endif/*SWITCHWIRING_H_*/
