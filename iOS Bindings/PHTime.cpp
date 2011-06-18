/*
 *  PHTime.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHTime.h"
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <time.h>
#include <errno.h>
#include "PHMain.h"

double PHTime::getTime()
{
	uint64_t tm = mach_absolute_time();
	
	static mach_timebase_info_data_t    sTimebaseInfo;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
	
	return tm * (double)(sTimebaseInfo.numer / sTimebaseInfo.denom / 1000000000.0f);
}

void PHTime::sleep(double time)
{
	if (time==0)
		return;
	//double tm = getTime();
	//PHLog("getTime:%lf",tm);
	
	struct timespec s1,s2;
	s1.tv_nsec = (int)((time-(int)time)*1000000000);
	s1.tv_sec = (int)time;
	int res;
	while (res=nanosleep(&s1, &s2))
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
	
	/*double ttosleep = time;
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
	
	/*double actual =getTime()-tm;
	double err = actual-time;
	double deltaerr = err/time;
	if (fabs(deltaerr)>=0.5)
		PHLog("PHTime::sleep(%+lf) error:%+lf delta err:%+lf actual:%+lf",time,err,deltaerr,actual);*/
}