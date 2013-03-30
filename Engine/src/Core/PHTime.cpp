/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHTime.h>

#ifdef PH_DARWIN
    #include <mach/mach.h>
    #include <mach/mach_time.h>
#endif

#include <time.h>
#include <sys/time.h>
#include <errno.h>

double PHTime::getTime()
{
#ifdef PH_DARWIN
    uint64_t tm = mach_absolute_time();
    
    static double ratio = 0;
    static uint64_t start;
    if (ratio == 0)
    {
        mach_timebase_info_data_t timebaseInfo;
        mach_timebase_info(&timebaseInfo);
        ratio = (timebaseInfo.numer / (timebaseInfo.denom * 1000000000.0));
        start = tm;
    }
    
    return (tm - start) * ratio;
#else
    static time_t is = 0;
    struct timespec tv;
    clock_gettime(CLOCK_REALTIME, &tv);     
    if (is == 0)
        is = tv.tv_sec;
    return (tv.tv_sec - is) + tv.tv_nsec * (1.0 / 1000000000.0);
#endif
}

void PHTime::sleep(float time)
{
	if (time==0)
		return;
	//float tm = getTime();
	//PHLog("getTime:%lf",tm);
	
	struct timespec s1,s2;
	s1.tv_nsec = (int)((time-(int)time)*1000000000);
	s1.tv_sec = (int)time;
	int res;
	while ((res=nanosleep(&s1, &s2)))
	{
		if (errno!=EINTR)
		{
			PHLog("nanosleep() error: %s",strerror(errno));
			break;
		} else {
			s1.tv_sec = s2.tv_sec;
			s1.tv_nsec = s2.tv_nsec;
		}
	}
	
	/*float ttosleep = time;
	while (usleep(ttosleep*1000000))
	{
		if (errno!=EINTR)
		{
			PHLog("usleep() error: %s",strerror(errno));
			break;
		}
		ttosleep = tm+time - getTime();
	}*/
	
	//while (getTime()<tm+time);
	
	//[NSThread sleepForTimeInterval:time];
	
	/*float actual =getTime()-tm;
	float err = actual-time;
	float deltaerr = err/time;
	if (fabs(deltaerr)>=0.5)
		PHLog("PHTime::sleep(%+lf) error:%+lf delta err:%+lf actual:%+lf",time,err,deltaerr,actual);*/
}
