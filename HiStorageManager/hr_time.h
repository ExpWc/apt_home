#ifndef HR_TIME_H
#define HR_TIME_H
#ifdef _WIN32
#include "global.h"
#else
#include <sys/time.h>
#endif

typedef struct _TY_HR_TIME
{
    long int sec;
    long int usec;
    inline _TY_HR_TIME & operator = (_TY_HR_TIME hr_time)
    {
         this->sec = hr_time.sec;
         this->usec = hr_time.usec;
         return *this;
    }
    inline _TY_HR_TIME & operator = (struct timeval sys_time)
    {
         this->sec = sys_time.tv_sec;
         this->usec = sys_time.tv_usec;
         return *this;
    }
    inline _TY_HR_TIME & operator -=(_TY_HR_TIME hr_time)
    {
        this->sec = this->sec-hr_time.sec;
        this->usec = this->usec-hr_time.usec;
        if(this->usec<0)
        {
            this->sec -=1;
            this->usec +=1000000;
        }
        return *this;
    }
    inline _TY_HR_TIME  operator -(_TY_HR_TIME hr_time)
    {
        _TY_HR_TIME tmp_time;
        tmp_time.sec = this->sec  -hr_time.sec;
        tmp_time.usec = this->usec -hr_time.usec;
        if(tmp_time.usec<0)
        {
            tmp_time.sec -=1;
            tmp_time.usec +=1000000;
        }
        return tmp_time;
    }
    inline _TY_HR_TIME & operator = (int usec)
    {
         this->sec = usec/1000000;
         this->usec = usec%1000000;
         return *this;
    }
    inline bool operator >(_TY_HR_TIME hr_time)
    {
         if(this->sec > hr_time.sec)
             return true;
         else if(this->sec < hr_time.sec )
             return false;
         else
         {
             if(this->usec > hr_time.usec )
                 return true;
             else
                 return false;
         }
    }
    inline bool operator <(_TY_HR_TIME hr_time)
    {
         if(this->sec < hr_time.sec)
             return true;
         else if(this->sec > hr_time.sec )
             return false;
         else
         {
             if(this->usec < hr_time.usec )
                 return true;
             else
                 return false;
         }
    }
}TY_HR_TIME;
#endif
