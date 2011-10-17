//
//  PHTimer.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHTIMER_H
#include "PHMain.h"

class PHTimer : public PHObject
{
private:
    bool valid;
    bool repeat;
    double time;
    double timeleft;
    double lastupd;
    PHInvocation invocation;
public:
    PHTimer() : valid(true), repeat(false), time(0), timeleft(0) {}
    
    void timePassed(double timeElapsed);
    void setTimeInterval(double tm) { time = tm; timeleft = tm; }
    double timeInterval() { return time; }
    void setTimeLeft(double tm) { timeleft = tm; }
    double timeLeft() { return timeleft; }
    bool repeats() { return repeat; }
    void setRepeats(bool rpt) { repeat = rpt; }
    void setLastUpdatedAt(double u) { lastupd = u; }
    double lastUpdatedAt() { return lastupd; }
    
    void setCallback(PHInvocation inv) { invocation = inv; }
    bool isValid() { return valid; } 
    void invalidate() { valid = false; }
    
    virtual void timerFired();
};

#endif