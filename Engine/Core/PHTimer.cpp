//
//  PHTimer.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHTimer.h"

void PHTimer::timePassed(ph_float timeElapsed)
{
    timeleft -= timeElapsed;
    if (timeleft<=0)
    {
        timerFired();
        if (!repeat)
            valid = false;
        else
            timeleft+=(((int)((-timeleft)/time))+1)*time;
    }
}

void PHTimer::timerFired()
{
    invocation.call(this);
}