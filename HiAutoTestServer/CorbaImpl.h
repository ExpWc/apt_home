#include "ace/pre.h"
#include "ace/Log_Msg.h"
#include "ace/post.h"

#include "HiAutoTestServerS.h"

#include "HiReport.h"

using namespace std;


enum RunningStatusE
{
	RUN_STATUS_NULL,
	RUN_STATUS_RUNNING,
	RUN_STATUS_PAUSE,
};


class CorbaImpl: public virtual POA_autoTest::HiAutoTest
{
public:
	CorbaImpl();
	~CorbaImpl();
public:

	autoTest::ReturnInfo startManualTest ();
	autoTest::ReturnInfo stopManualTest (); 

	autoTest::ReturnInfo startModel (const char* pData);
	autoTest::ReturnInfo stopModel (const char* modelName);
	autoTest::ReturnInfo getModelRunStatus (const char* modelName, ::CORBA::Long_out modelStatus);
	autoTest::ReturnInfo setICDData (const ::autoTest::TySetIcdInfo & icdInfo, ::CORBA::Long icdNum);
	autoTest::ReturnInfo sendICDData(const ::autoTest::TySendIcdInfo & icdInfo, ::CORBA::Long icdNum);
	autoTest::ReturnInfo getICDData(const ::autoTest::TyGetIcdInfo & icdInfoIn, ::autoTest::TyGetIcdInfo_out  icdInfo, ::CORBA::Long icdNum);
	autoTest::ReturnInfo getModelParam(const ::autoTest::TyModelInfo & modelInfoIn, ::autoTest::TyModelInfo_out  modelInfo, ::CORBA::Long modelNum);
	autoTest::ReturnInfo setModelParam(const ::autoTest::TyModelInfo & modelInfo, ::CORBA::Long modelNum);
	autoTest::ReturnInfo getModelVar(const ::autoTest::TyModelInfo & modelInfoIn, ::autoTest::TyModelInfo_out  modelInfo, ::CORBA::Long modelNum);
	autoTest::ReturnInfo setModelVar(const ::autoTest::TyModelInfo & modelInfo, ::CORBA::Long modelNum);
	autoTest::ReturnInfo optManualTestOneStep(const char * OneStep, ::autoTest::StepStatusSeq_out info);

/**/
	autoTest::ReturnInfo setCase (const char * caseData);
	autoTest::ReturnInfo setICD (const char * ICDInfo);
	autoTest::ReturnInfo setStorageConfig (const char * storageConfig);
	autoTest::ReturnInfo setCurveConfig (const char * curveConfig);
    autoTest::ReturnInfo setChannelConfig (const char * channelConfig);
	autoTest::ReturnInfo start (const ::autoTest::TestExecute & testExecute);
	autoTest::ReturnInfo stop (void);
  	autoTest::ReturnInfo stopStep (const ::autoTest::StepInfoSeq & stepInfo);
	autoTest::ReturnInfo pause (void);
	autoTest::ReturnInfo getCaseStatus (const ::autoTest::CaseStatusSeq & cardStatusDess,
	::autoTest::CaseStatusSeq_out cardStatusVals);
	autoTest::ReturnInfo getStepStatus (const ::autoTest::StepStatusSeq & stepStatusDess,
	    ::autoTest::StepStatusSeq_out stepStatusVals);
	autoTest::ReturnInfo getSubStepStatus (const ::autoTest::SubStepStatusSeq & subStepStatusDess,
	    ::autoTest::SubStepStatusSeq_out subStepStatusVals);
	autoTest::ReturnInfo getFieldData (const ::autoTest::FieldData & fieldDataDess,
			::autoTest::FieldData_out fieldDataVals);
//	autoTest::ReturnInfo getServerStatus (::autoTest::ServerStatus_out ServerStatusVal);
	autoTest::ReturnInfo getServerStatus(::autoTest::StatusInfo_out info);
	autoTest::ReturnInfo getCardsInfo(::autoTest::CardManagerSeq_out info);
	autoTest::ReturnInfo getExecutiveInfo (::autoTest::ExecutiveInfo_out info);

	autoTest::ReturnInfo getRecordListNum(::CORBA::Long_out listNum);
	autoTest::ReturnInfo getRecordList(::autoTest::RecordListSeq_out pList, ::CORBA::Long_out listLen);
	autoTest::ReturnInfo getDataByTime(const char *storageName, const ::autoTest::FieldIcdInfo &varInfo, const ::autoTest::TY_HI_TIME &startTime,
			const ::autoTest::TY_HI_TIME& stopTime, ::autoTest::StorageDataSeq_out pData, ::autoTest::HiTimeSeq_out pTime, CORBA::Long &maxCount);

	autoTest::ReturnInfo getDataByIndex(const char *storageName, const ::autoTest::FieldIcdInfo &varInfo, ::CORBA::Long startIndex, ::CORBA::Long stopIndex,
			::autoTest::StorageDataSeq_out pData, ::autoTest::HiTimeSeq_out pTime, CORBA::Long &maxCount);

	autoTest::ReturnInfo getIndexByTime(const char *storageName, const ::autoTest::FieldIcdInfo &varInfo, const ::autoTest::TY_HI_TIME& startTime, const ::autoTest::TY_HI_TIME& stopTime,
			 ::CORBA::Long_out startIndex, ::CORBA::Long_out stopIndex);

	autoTest::ReturnInfo getModelDataByTime(const char *storageName, const ::autoTest::ModelInfo &varInfo, const ::autoTest::TY_HI_TIME &startTime,
			const ::autoTest::TY_HI_TIME& stopTime, ::autoTest::StorageDataSeq_out pData, ::autoTest::HiTimeSeq_out pTime, CORBA::Long &maxCount);

	autoTest::ReturnInfo getModelDataByIndex(const char *storageName, const ::autoTest::ModelInfo &varInfo, ::CORBA::Long startIndex, ::CORBA::Long stopIndex,
			::autoTest::StorageDataSeq_out pData, ::autoTest::HiTimeSeq_out pTime, CORBA::Long &maxCount);

	autoTest::ReturnInfo getModelIndexByTime(const char *storageName, const ::autoTest::ModelInfo &varInfo, const ::autoTest::TY_HI_TIME& startTime, const ::autoTest::TY_HI_TIME& stopTime,
			 ::CORBA::Long_out startIndex, ::CORBA::Long_out stopIndex);


	autoTest::ReturnInfo deleteStorageRecord(const char *storageName);
	autoTest::ReturnInfo deleteAllStorageRecord();

	autoTest::ReturnInfo getIpInfo(CORBA::String_out ipInfo);
public:
	RunningStatusE m_caseExcuteRunFlag;

};
