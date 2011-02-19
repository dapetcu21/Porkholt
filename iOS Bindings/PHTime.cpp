/*
 *  PHTime.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/18/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHTime.h"
#import <mach/mach.h>
#import <mach/mach_time.h>

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