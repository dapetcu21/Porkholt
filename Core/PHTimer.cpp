//
//  PHTimer.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHMain.h"
#include "PHTimer.h"

void PHTimer::timePassed(double timeElapsed)
{
    timeleft -= timeElapsed;
    if (timeleft<=0)
    {
        timerFired();
        if (!repeat)
            invalidate();
        else
            timeleft+=floor((-timeleft)/time)*time;
    }
}

void PHTimer::timerFired()
{
    if (cb && target)
        (target->*cb)(this,ud);
}