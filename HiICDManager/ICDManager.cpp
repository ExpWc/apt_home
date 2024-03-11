#include "ICDManager.h"
#include "XmlParserFactory.h"
#include "PublicInclude.h"

//#include <stddef.h>
////////
CICDManager::CICDManager()
{

}
CICDManager::~CICDManager()
{
	/*release resource*/
    TY_BLOCK_MAP::iterator it;
    for(it = m_blockMap.begin(); it != m_blockMap.end(); it++)
    {
        CBlock *pBlock = it->second;
		pBlock->reset();
        if(pBlock != NULL)
        {
            delete pBlock;
			pBlock = NULL;
        }
    }
    m_blockMap.clear();
    /*release resource*/
}

CICDManager* CICDManager::m_instance = NULL;
CICDManager* CICDManager::getInstance()
{
	if (m_instance == NULL)
	{
		m_instance = new CICDManager();
	}
	return m_instance;
}

int CICDManager::init(string caseXml)
{
	#if 1
	/*release resource*/
    TY_BLOCK_MAP::iterator it; //no release
    for(it = m_blockMap.begin(); it != m_blockMap.end(); ++it)
    {
        CBlock *pBlock = it->second;
		
		pBlock->reset();
        if(pBlock != NULL)
        {
            delete pBlock;
			pBlock = NULL;
			
        }
    }

    m_blockMap.clear();

    /*release resource*/

	bool bRet = false;
    string tmpType = "";
    string tmpId = "";
    string tmpStepNum = "";
    //int id = 0;
    //int stepNum = 0;
    int ret = 0;

    TiXmlNode* rootNode = NULL;
    TiXmlNode* icdNode = NULL;
    TiXmlElement *pBlockElement = NULL;
    TiXmlDocument * pInitXmlManager = NULL;
    pInitXmlManager = load_xml((const char *)caseXml.c_str(), FROM_DATA);
    if (pInitXmlManager == NULL)
    {
	    REPORT(REPORT_ERROR, "Xml is not valid\n");
	    return -1;
    }
    else
    {
	    rootNode = pInitXmlManager->RootElement();
    }
    if (rootNode == NULL)
    {
	    REPORT(REPORT_ERROR, "Init.xml can't find the node\n");
	    return -1;
    }
    else
    {
	    icdNode = rootNode->FirstChild("icdRoot");
    }

    if (icdNode == NULL)
    {
	    REPORT(REPORT_ERROR, "Init.xml can't find the node of icdNode\n");
	    return -1;
    }
    else
    {    
	    int nodeNum = 0;
	    pBlockElement = icdNode->FirstChildElement("blocks");
	    for (pBlockElement; pBlockElement != NULL; pBlockElement = pBlockElement->NextSiblingElement())
	    {
		    nodeNum ++;
		    tmpType = "";
		    bRet = GetValue(pBlockElement, "xsi:type", tmpType);
		    if (!bRet)
		    {
			    REPORT(REPORT_ERROR, "Init.xml didn't find the node of xsi:type.\n");
			    continue;
		    }
		    tmpId = "";
		    bRet = GetValue(pBlockElement, "uuid", tmpId);
		    if (!bRet)
		    {
			    REPORT(REPORT_ERROR, "Init.xml didn't find the node of uuid.\n");
			    continue;
		    }
		    //Block-------->>
		    CBlock *pBlock = getBlock(tmpId);
		    if(pBlock != NULL)
		    {
			    REPORT(REPORT_ERROR, "ICD has Init.\n");
			    continue;
		    }
		    else
		    {
			    CAbstractXmlParser * pXmlParser = CXmlParserFactory::createXmlParser(tmpType.c_str());
			    if(pXmlParser == NULL)
			    {
				    REPORT(REPORT_ERROR, "Create Xml Parser.\n");
				    continue;
			    }
			    CBlock * pBlock = new CBlock();
			    if(pBlock == NULL)
			    {
				    REPORT(REPORT_ERROR, "Create Block.\n");
				    continue;
			    }
			    ret = pXmlParser->parse(pBlockElement, pBlock);
			    if(ret != 0)
			    {
				    REPORT(REPORT_ERROR, "parse.\n");
				    continue;
			    }
			    ret = insertBlock(tmpId, pBlock);
			    if(ret != 0)
			    {
				    REPORT(REPORT_ERROR, "insert Block.\n");
				    continue;
			    }
			    //REPORT(REPORT_ERROR, "parse.tmpId:%s\n", tmpId.c_str());
		    }
		    //Block--------<<
	    }

	    //modify by wc 20231108 release memory
	    delete pInitXmlManager;
	    pInitXmlManager = NULL;

	    m_ICDStatistic.blockNodeNum = nodeNum;
	    m_ICDStatistic.blockNum = m_blockMap.size();
	    REPORT(REPORT_INFO,"m_blockMap %d\n", m_blockMap.size());
    }
#endif
    REPORT(REPORT_INFO, "ICD Manager init finish.\n");
    return 0;
}

CBlock* CICDManager::getBlock(string icdID)
{
	TY_BLOCK_MAP::iterator pos = m_blockMap.find(icdID);
	if(pos == m_blockMap.end())
	{
		return NULL;
	}
	return pos->second;
}

int CICDManager::insertBlock(string icdID, CBlock* pBlock)
{
	TY_BLOCK_MAP::iterator pos = m_blockMap.find(icdID) ;
	if(pos == m_blockMap.end())
	{
		m_blockMap.insert(std::make_pair(icdID, pBlock));
		return 0;
	}
	return -1;
}
void CICDManager::resetBlockBuffer()
{
	TY_BLOCK_MAP::iterator it;
	for(it = m_blockMap.begin(); it != m_blockMap.end(); it++)
	{
		CBlock *pBlock = it->second;
		memset(pBlock->m_txBuffer, 0, MAX_PAYLOAD_LEN);
		memset(pBlock->m_rxBuffer, 0, MAX_PAYLOAD_LEN);
	}
}
