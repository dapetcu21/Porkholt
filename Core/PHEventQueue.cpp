//
//  PHEventQueue.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHEventQueue.h"

PHEventQueue::PHEventQueue()
{
    mutex = new PHMutex;
}

PHEventQueue::~PHEventQueue()
{
    mutex->release();
}

void PHEventQueue::schedule(PHObject * target, PHCallback cb, void * ud, bool waitTillDone)
{
    event evt;
    evt.target = target;
    evt.cb = cb;
    evt.ud = ud;
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
        ((evt.target)->*(evt.cb))(this,evt.ud);
        if (evt.sem)
            evt.sem->signal();
        mutex->lock();
    }
    mutex->unlock();
}