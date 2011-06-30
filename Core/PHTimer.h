//
//  PHTimer.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHTIMER_H
#include "PHObject.h"

class PHTimer : public PHObject
{
private:
    bool valid;
    bool repeat;
    double time;
    double timeleft;
    double lastupd;
    PHCallback cb;
    PHObject * target;
    void * ud;
public:
    PHTimer() : valid(true), repeat(false), time(0), timeleft(0), cb(NULL), target(NULL), ud(NULL) {}
    
    void timePassed(double timeElapsed);
    void setTimeInterval(double tm) { time = tm; timeleft = tm; }
    double timeInterval() { return time; }
    void setTimeLeft(double tm) { timeleft = tm; }
    double timeLeft() { return timeleft; }
    bool repeats() { return repeat; }
    void setRepeats(bool rpt) { repeat = rpt; }
    void setLastUpdatedAt(double u) { lastupd = u; }
    double lastUpdatedAt() { return lastupd; }
    
    void setCallback(PHObject * trg, PHCallback call, void * usd) { target = trg; cb = call; ud=usd; }
    bool isValid() { return valid; } 
    void invalidate() { valid = false; }
    
    virtual void timerFired();
};

#endif