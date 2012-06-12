/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHTimer.h>
#include <Porkholt/Core/PHAnimatorPool.h>

PHTimer::PHTimer() : valid(true), repeat(false), time(0), dur(0), cboninvalidate(false), sem(NULL), sigm(false) { setRetainedInThePool(true); } 
PHTimer::PHTimer(double duration) : valid(true), repeat(false), time(duration), dur(duration), cboninvalidate(false), sem(NULL), sigm(false) { setRetainedInThePool(true); } 
PHTimer::PHTimer(double duration, bool rep) : valid(true), repeat(rep), time(duration), dur(duration), cboninvalidate(false), sem(NULL), sigm(false) { setRetainedInThePool(true); } 
    
PHTimer::~PHTimer()
{
    invalidate();
    setSemaphore(NULL);
}

void PHTimer::advanceAnimation(ph_float timeElapsed)
{
    time -= timeElapsed;
    if (time<=0)
    {
        timerFired();
        if (!repeat)
            _invalidate();
        else 
        {
            time+=(((int)((-time)/dur))+1)*dur;
            signalSemaphore();
        }
    }
}

void PHTimer::signalSemaphore()
{
    if (sem)
    {
        sem->signal();
        if (!sigm)
            setSemaphore(NULL);
    }
}

void PHTimer::_invalidate()
{
    if (!valid) return;
    valid = false;
    signalSemaphore();
    if (pool)
        setAnimatorPool(NULL);
}

void PHTimer::timerFired()
{
    invocation.call(this);
}


void PHTimer::scheduleAction(PHAnimatorPool * pool, const PHInvocation & inv, double time, bool repeat, bool waitForIt)
{
    PHTimer * t = new PHTimer(time, repeat);
    t->setCallback(inv);
    if (waitForIt)
    {
        PHSemaphore * sem = new PHSemaphore(0);
        t->setSemaphore(sem);
        pool->addAnimator(t);
        sem->wait();
        sem->release();
    } else
        pool->addAnimator(t);
    t->release();
}
