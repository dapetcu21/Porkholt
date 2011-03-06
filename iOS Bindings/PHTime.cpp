/*
 *  PHTime.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHTime.h"
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <time.h>
#include <errno.h>

double PHTime::getTime()
{
	uint64_t tm = mach_absolute_time();
	
	static mach_timebase_info_data_t    sTimebaseInfo;
    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }
	
    uint64_t nano = tm * sTimebaseInfo.numer / sTimebaseInfo.denom;
	return nano/1000000000.0f;	
}

void PHTime::sleep(double time)
{
	struct timespec s1,s2;
	s1.tv_nsec = ((long)((time-(long long)time)*1000000000))%1000000000;
	s1.tv_sec = (int)time;
	int res;
	while (res=nanosleep(&s1, &s2))
	{
		if (errno!=EINTR)
		{
			break;
		} else {
			s1=s2;
		}
	}
}