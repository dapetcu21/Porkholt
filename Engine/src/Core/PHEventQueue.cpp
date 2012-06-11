/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHEventQueue.h>
#include <Porkholt/Core/PHTimer.h>

PHEventQueue::PHEventQueue()
{
    mutex = new PHMutex(true);
}

PHEventQueue::~PHEventQueue()
{
    invalidateAllTimers();
    mutex->release();
}

void PHEventQueue::schedule(const PHInvocation & inv, bool waitTillDone)
{
    event evt;
    evt.invocation = inv;
    evt.sem = NULL;
    if (waitTillDone)
    {
        evt.sem = new PHSemaphore(0);
    }
    mutex->lock();
    q.push_back(evt);
    mutex->unlock();
    if (waitTillDone)
    {
        evt.sem->wait();
        evt.sem->release();
    }
}

void PHEventQueue::processQueue()
{
    mutex->lock();
    while (!q.empty())
    {
        event evt = q.front();
        q.pop_front();
        mutex->unlock();
        evt.invocation.call(this);
        if (evt.sem)
            evt.sem->signal();
        mutex->lock();
    }
    mutex->unlock();
}

void PHEventQueue::scheduleTimer(PHTimer * timer, void * ud)
{
    if (!timer || !timer->isValid())
        return;
    mutex->lock();
    timer->setLastUpdatedAt(PHTime::getTime());
    timers.insert(make_pair<void *,PHTimer*>(ud,timer));
    timer->retain();
    mutex->unlock();
}

void PHEventQueue::invalidateAllTimers()
{
    mutex->lock();
    for(multimap<void *,PHTimer*>::iterator i=timers.begin(); i!=timers.end(); i++)
        i->second->release();
    timers.clear();
    mutex->unlock();
}

void PHEventQueue::invalidateTimersWithUserdata(void * ud)
{
    mutex->lock();
    pair<multimap<void *,PHTimer*>::iterator,multimap<void *,PHTimer*>::iterator> range = timers.equal_range(ud);
    for(multimap<void *,PHTimer*>::iterator i = range.first; i!=range.second; i++)
        i->second->release();
    timers.erase(range.first,range.second);
    mutex->unlock();
}

void PHEventQueue::updateTimers(ph_float timeElapsed)
{
    mutex->lock();
    ph_float time = PHTime::getTime();
    
    multimap<void *,PHTimer*>::iterator i,nx;
    for (i=timers.begin(); i!=timers.end(); i=nx)
    {
        nx = i;
        nx++;
        PHTimer * timer = i->second;
        mutex->unlock();
        ph_float real = time-timer->lastUpdatedAt();
        if (real>timeElapsed)
            real=timeElapsed;
        timer->setLastUpdatedAt(time);
        timer->timePassed(real);
        mutex->lock();
        if (!(timer->isValid()))
        {
            timers.erase(i);
            timer->release();
        }
    }
    mutex->unlock();
}
