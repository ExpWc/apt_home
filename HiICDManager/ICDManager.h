#ifndef __ICDMANAGER_H_
#define __ICDMANAGER_H_
#include "Block.h"

typedef map<string, CBlock*> TY_BLOCK_MAP;

typedef struct _icd_statistic_
{
	int blockNum;
	int blockNodeNum;
}TY_ICD_STATISTIC;

class CICDManager
{
public:
	CICDManager();
	~CICDManager();
private:
	static CICDManager* m_instance;
	TY_BLOCK_MAP m_blockMap;
	TY_ICD_STATISTIC m_ICDStatistic;

	int insertBlock(string icdID, CBlock* pBlock);

public:	
	static CICDManager* getInstance();
	int init(string caseXml);
	CBlock* getBlock(string icdID);
	void resetBlockBuffer();

	int dispose();

};
#endif
