/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHTIMER_H
#include <Porkholt/Core/PHAnimator.h>

class PHTimer : public PHAnimator 
{
private:
    bool valid;
    bool repeat;
    float time, dur;
    bool cboninvalidate;
    PHInvocation invocation;
    PHSemaphore * sem;
    bool sigm;

    virtual void _invalidate();

public:
    PHTimer();
    PHTimer(double duration);
    PHTimer(double duration, bool repeat);
    
    ~PHTimer();
    
    void advanceAnimation(float timeElapsed);

    void setDuration(float tm) { dur = time = tm; }
    float duration() { return dur; }
    void setTimeLeft(float tm) { time = tm; }
    float timeLeft() { return time; }
    bool repeats() { return repeat; }
    void setRepeats(bool rpt) { repeat = rpt; }
    bool callsBackOnInvalidate() { return cboninvalidate; }
    void setCallsBackOnInvalidate(bool b) { cboninvalidate = b; }

    void setSemaphore(PHSemaphore * semaphore); 
    PHSemaphore * semaphore() { return sem; }
    void setSignalMoreThanOnce(bool sm) { sigm = sm; }
    bool signalMoreThanOnce() { return sigm; }
    void signalSemaphore();
    
    void setCallback(const PHInvocation & inv) { invocation = inv; }
    bool isValid() { return valid; } 
    void invalidate() { if (valid) { if (cboninvalidate) timerFired(); _invalidate(); } }
    
    //waitForIt means that scheduleAction should block until the
    //invocation is performed and should only be used from 
    //another thread than the one on which the animator pool 
    //is running to avoid a deadlock
    static void scheduleAction(PHAnimatorPool * pool, const PHInvocation & inv, double time, bool repeat, bool waitForIt);
    static void scheduleAction(PHAnimatorPool * pool, const PHInvocation & inv, double time, bool repeat) {
        scheduleAction(pool, inv, time, repeat, false);
    }
    static void scheduleAction(PHAnimatorPool * pool, const PHInvocation & inv, double time) {
        scheduleAction(pool, inv, time, false, false);
    }
    static void scheduleAction(PHAnimatorPool * pool, const PHInvocation & inv) {
        scheduleAction(pool, inv, 0, false, false);
    }
    static void scheduleAction(PHAnimatorPool * pool, const PHInvocation & inv, bool waitForIt) {
        scheduleAction(pool, inv, 0, false, waitForIt);
    }

public:
    virtual void timerFired();
};

#endif
