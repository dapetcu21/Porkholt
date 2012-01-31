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
    ph_float time;
    ph_float timeleft;
    ph_float lastupd;
    bool cboninvalidate;
    PHInvocation invocation;
public:
    PHTimer() : valid(true), repeat(false), time(0), timeleft(0), cboninvalidate(false) {}
    ~PHTimer() { invalidate(); }
    
    void timePassed(ph_float timeElapsed);
    void setTimeInterval(ph_float tm) { time = tm; timeleft = tm; }
    ph_float timeInterval() { return time; }
    void setTimeLeft(ph_float tm) { timeleft = tm; }
    ph_float timeLeft() { return timeleft; }
    bool repeats() { return repeat; }
    void setRepeats(bool rpt) { repeat = rpt; }
    void setLastUpdatedAt(ph_float u) { lastupd = u; }
    ph_float lastUpdatedAt() { return lastupd; }
    bool callsBackOnInvalidate() { return cboninvalidate; }
    void setCallsBackOnInvalidate(bool b) { cboninvalidate = b; }
    
    void setCallback(const PHInvocation & inv) { invocation = inv; }
    bool isValid() { return valid; } 
    void invalidate() { if (cboninvalidate&&valid) timerFired(); valid = false; }
    
    virtual void timerFired();
};

#endif