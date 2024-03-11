#include "ReceiveStep.h"
#include "ICDManager.h"
#include "HardwareManager.h"
#include <stdio.h>

CBaseStep *CreateStep()
{
    return new CReceiveStep();
}

CReceiveStep::CReceiveStep()
{
}
CReceiveStep::~CReceiveStep()
{
}
int CReceiveStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
    int ret = 0;
    ret = CBaseStep::parse(pStepElement, pStep);
    if (ret != 0)
    {
        return ret;
    }
    /*parse router*/
    ret = CBaseCommunicationStep::parseRouter(pStepElement);
    if (ret != 0)
    {
        return ret;
    }
    REPORT(REPORT_DEBUG, "parse\n");
    return ret;
}
int CReceiveStep::init()
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

int CReceiveStep::run()
{
    m_stepStatus = STATUS_RUNNING;
    int ret = 0;

    CHardwareManager *pHardwareManager = CHardwareManager::getInstance();
    unsigned char rxData[MAX_PAYLOAD_LEN];
    int length = 500; // MAX_PAYLOAD_LEN;//TODO:modyfi
    struct timeval t;
    CCardManager *pCardManager = pHardwareManager->getCardManager(m_router.CardType);
    if (pCardManager == NULL)
    {
        REPORT(REPORT_ERROR, "Get pCard failed\n");
        m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
        // log>>
        m_msg = string("CardType not found:") + m_router.CardType;
        COMSTEPLOG(LOG_ERROR, m_msg);
        // log>>
#endif
        return -1;
    }
    else
    {
        CCard *pCard = pCardManager->getCard(m_router.CardID);
        if (pCard == NULL)
        {
            REPORT(REPORT_ERROR, "Get pCard failed(id:%d)\n", m_router.CardID);
            m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
            // log>>
            char tmpCardID[32] = {0};
            sprintf(tmpCardID, "%d", m_router.CardID);
            m_msg = string("Get pCard failed:") + m_router.CardType;
            m_msg = m_msg + string(" CardID is ") + tmpCardID;
            COMSTEPLOG(LOG_ERROR, m_msg);
            // log>>
#endif
            return -1;
        }
        else
        {
            memset(rxData, 0, 2048);
            if (0 != pCard->readData(m_router.icdID, (char *)rxData, length, &t))
            {
                REPORT(REPORT_ERROR, "uuid %s read failed.\n", m_router.icdID.c_str());
                m_stepStatus = STATUS_ERROR;
#ifdef AUTO_LOG
                // log>>
                m_msg = string("CardType not found:") + pCard->getMsg();
                m_msg = m_msg + string(" UUID read Failed : UUID is") + m_router.icdID;
                COMSTEPLOG(LOG_ERROR, m_msg);
                // log>>
#endif
                return -1;
            }
            //			printf("icdID:%s CardType:%s ChnID:%d \nTime %d-%d length %d\n",m_router.icdID.c_str(), m_router.CardType.c_str(), m_router.ChnID, t.tv_sec,t.tv_usec, length);
            //            showPac((char*)rxData,length);
            //            savePac(m_router.CardID, m_router.ChnID, (char*)rxData, length);
        }
    }

    CICDManager *pICDManager = CICDManager::getInstance();
    CBlock *pBlock = pICDManager->getBlock(m_router.icdID);
    if (pBlock == NULL)
    {
        REPORT(REPORT_ERROR, "Get m_ICDID failed\n");
        ret = -1;
        m_stepStatus = STATUS_ERROR;
    }
    else
    {
        pBlock->setRxData(rxData, length);

        // printf("ICDManager unpackRxICD >>>>>00000000000\n");
    }
    m_msg = "Success";
    m_stepStatus = STATUS_SUCCESS;
    // REPORT(REPORT_INFO, "run\n");
    return 0;
}
