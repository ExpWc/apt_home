#include "SendStep.h"
#include "ICDManager.h"
#include "HardwareManager.h"

CBaseStep *CreateStep()
{
    return new CSendStep();
}

CSendStep::CSendStep()
{
}
CSendStep::~CSendStep()
{
}
int CSendStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
    int ret = 0;
    ret = CBaseStep::parse(pStepElement, pStep);
    if (ret != 0)
    {
        REPORT(REPORT_ERROR, "Get Command failed\n");
        return ret;
    }
    /*parse router*/
    ret = CBaseCommunicationStep::parseRouter(pStepElement);
    if (ret != 0)
    {
        REPORT(REPORT_ERROR, "Get Command failed\n");
        return ret;
    }
    REPORT(REPORT_DEBUG, "parse\n");
    return ret;
}
int CSendStep::init()
{
    int ret = 0;
    CICDManager *pICDManager = CICDManager::getInstance();
    if (pICDManager->getBlock(m_router.icdID) == NULL)
    {
        REPORT(REPORT_ERROR, "Get m_ICDID failed\n");
#ifdef AUTO_LOG
        // log>>
        m_msg = string("Not found ICDID:") + m_router.icdID;
        COMSTEPLOG(LOG_ERROR, m_msg);
        // log<<
#endif
        ret = -1;
        m_stepStatus = STATUS_ERROR;
    }

    m_stepStatus = STATUS_READY;
    REPORT(REPORT_DEBUG, "init\n");
    return 0;
}
int CSendStep::run()
{
    double var = 0;
    int ret = 0;
    m_stepStatus = STATUS_RUNNING;
    unsigned char txData[MAX_PAYLOAD_LEN];
    int length = 2048;
    // int length = MAX_PAYLOAD_LEN;
    int lengthRet = 0;
    CICDManager *pICDManager = CICDManager::getInstance();
    CBlock *pBlock = pICDManager->getBlock(m_router.icdID);
    if (pBlock == NULL)
    {
        REPORT(REPORT_ERROR, "Get m_ICDID failed\n");
#ifdef AUTO_LOG
        // log>>
        m_msg = string("Not found ICDID:") + m_router.icdID;
        COMSTEPLOG(LOG_ERROR, m_msg);
        // log<<
#endif
        m_stepStatus = STATUS_ERROR;
        ret = -1;
    }
    else
    {
        lengthRet = pBlock->getTxData(txData, length); // 3<<
        // memcpy(&var, txData, lengthRet);

        //	REPORT(REPORT_ERROR, "var:%lf, txData[0]:%02x, field lengthRet:%d\n",var,txData[0],lengthRet);

        // showPac((char*)txData, lengthRet);
    }

    /* printfFieldValue: this function can printf what you set ICD Data->analog/discrete/429/422/1553*/
    // pBlock->printFieldValue();
    CHardwareManager *pHardwareManager = CHardwareManager::getInstance();
    CCardManager *pCardManager = pHardwareManager->getCardManager(m_router.CardType);
    if (pCardManager == NULL)
    {
        REPORT(REPORT_ERROR, "===Get CardManager failed\n");
        m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
        // log>>
        m_msg = string("ERROR:Get CardManager failed:") + m_router.CardType;
        COMSTEPLOG(LOG_ERROR, m_msg);
        // log>>
#endif
        return -1;
    }

    CCard *pCard = pCardManager->getCard(m_router.CardID);
    if (pCard == NULL)
    {
        REPORT(REPORT_ERROR, "Get pCard failed(id:%d)\n", m_router.CardID);
#ifdef AUTO_LOG
        // log>>
        m_msg = string("Get pCard failed:") + m_router.CardType;
        char tmpCardID[32] = {0};
        sprintf(tmpCardID, "%d", m_router.CardID);
        m_msg = m_msg + ("  CardID =") + tmpCardID;

        COMSTEPLOG(LOG_ERROR, m_msg);
        // log>>
#endif
        m_stepStatus = STATUS_ERROR;
        return -1;
    }

    ret = pCard->writeData(m_router.icdID, (char *)txData, lengthRet);
    if (ret != 0)
    {
        REPORT(REPORT_ERROR, "ICD:%s write Data failed\n", m_router.icdID.c_str());
#ifdef AUTO_LOG
        // log>>
        m_msg = string("writeData failed:") + pCard->getMsg();
        m_msg = m_msg + string(" ICDID = ") + m_router.icdID;
        COMSTEPLOG(LOG_ERROR, m_msg);
        // log>>
#endif
        m_stepStatus = STATUS_ERROR;
        return -1;
    }
    REPORT(REPORT_INFO, "write success, icdICD:%s,cardID:%d,cardType:%s\n", m_router.icdID.c_str(), m_router.CardID, m_router.CardType.c_str());
    m_msg = "Success";
    m_stepStatus = STATUS_SUCCESS;
    // REPORT(REPORT_INFO, "run\n");
    return 0;
}
