#include "stdio.h"
#include "HiLog.h"

int main()
{
    int a = 1;
    double b = 2.1;
    char data[32] = {2};

    CLog *pLog = CLog::getInstance();
    CPacLog * pPacLog = new CPacLog();
#if 0
    pLog->log("12345678:%d %lf\n", a, b);
    pPacLog->pacLog(data, 32, "hihihihi:%d %lf\n", a, b);

    getchar();
    pLog->setLogFileName("./log.log");
    pLog->log("12345678:%d %lf\n", a, b);
    pLog->log("123456789999999\n");
   
    pPacLog->setLogFileName("./pacLog.log");
    pPacLog->pacLog(data, 32, "hihihihi1:%d %lf\n", a, b);
    pPacLog->pacLog(data, 32, "hihihihi2:%d %lf\n", a, b);

    //rewrite>>
    getchar();
    pLog->setLogFileName("./log.log");
    pLog->log("12345678:%d %lf\n", a, b);

    pPacLog->setLogFileName("./pacLog.log");
    pPacLog->pacLog(data, 32, "hihihihi:%d %lf\n", a, b);

#endif
    //stdout>>
    getchar();
    pLog->setLogFileName("std");
    pLog->log("12345678:%d %lf\n", a, b);

    pLog->setLogFileName("std");
    pPacLog->pacLog(data, 32, "hihihihi:%d %lf\n", a, b);

    delete pPacLog;
    return 0;
}
