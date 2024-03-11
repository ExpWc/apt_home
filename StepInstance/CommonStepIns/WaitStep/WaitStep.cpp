#include "WaitStep.h"
#include <errno.h>

#define PRINT 0

CBaseStep *CreateStep()
{
    return new CWaitStep();
}

CWaitStep::CWaitStep()
{
}
CWaitStep::~CWaitStep()
{
}

// FILE* fd = NULL;
void myUsleep(unsigned long usec)
{
    struct timeval tv;
    tv.tv_sec = usec / 1000000;
    tv.tv_usec = usec % 1000000;

    int err;
    do
    {
        err = select(0, NULL, NULL, NULL, &tv);
    } while (err < 0 && errno == EINTR);
}

int CWaitStep::parse(TiXmlElement *pStepElement, CBaseStep *pStep)
{
    int ret = 0;
    ret = CBaseStep::parse(pStepElement, pStep);
    if (ret != 0)
    {
        REPORT(REPORT_ERROR, "parse failed\n");
        return ret;
    }
    bool bRet = false;
    string tmpStr;
    bRet = GetValue(pStepElement, "timeMS", tmpStr);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get timeS failed\n");
        return -1;
    }
    m_timeMS = atoi(tmpStr.c_str());
    bRet = GetValue(pStepElement, "timeUS", tmpStr);
    if (!bRet)
    {
        REPORT(REPORT_ERROR, "Get timeUS failed\n");
        return -1;
    }
    m_timeUS = atoi(tmpStr.c_str());
    //  REPORT(REPORT_ERROR, "++++++++++++++++++++++++++=parse m_timeMS:%d\n", m_timeMS);
    return 0;
}
int CWaitStep::init()
{
    m_stepStatus = STATUS_READY;
    //  REPORT(REPORT_DEBUG, "init\n");
    return 0;
}

inline void msleep(unsigned int ms)
{
    struct timespec ts = {
        ms / 1000,
        (ms % 1000) * 1000 * 1000};
    while ((-1 == nanosleep(&ts, &ts)) && (EINTR == errno))
        ;
}

int CWaitStep::run()
{
    m_stepStatus = STATUS_RUNNING;
#if PRINT
    struct timeval t_old;
    struct timeval t_new;
#endif

    int timeTmpUs = 0;
    int timeS = 0;
    int loop = 0;
#if PRINT
    memset(&t_old, 0, sizeof(struct timeval));
    gettimeofday(&t_old, NULL);
#endif

    // myUsleep(m_timeMS * 1000 + m_timeUS);

    timeS = m_timeMS / 1000;
    timeTmpUs = m_timeMS % 1000 * 1000 + m_timeUS;
    REPORT(REPORT_INFO, "run -----------------------------start,nowTImeS:%d:%d, timeS:%d\n", timeS, timeTmpUs, m_timeMS);
    while (m_stepStatus == STATUS_RUNNING && loop < timeS)
    {
        myUsleep(1000000);
        // printf("[%s:%s:%d]--> wait time : %d,timeS:%d\n", __FILE__, __FUNCTION__, __LINE__, loop, timeS);
        loop++;
    }
    myUsleep(timeTmpUs);

    m_stepStatus = STATUS_SUCCESS;
    m_msg = "Success";
#ifdef AUTO_LOG
    // log>>
    char tmp[32] = {0};
    sprintf(tmp, "%d", timeTmpUs);
    printf(" tmp = %s us = %d \n", tmp, timeTmpUs);
    m_msg = string("timeTmpUs is:") + string(tmp);
    STEPLOG(LOG_INFO, m_msg);
    // log(m_msg);
    // log<<
#endif
    // REPORT(REPORT_INFO, "--------WaitStep--------run over\n");
    return 0;
}
int CWaitStep::stop()
{
    m_stepStatus = STATUS_READY;
    return 0;
}
