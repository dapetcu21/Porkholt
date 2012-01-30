//
//  PHEventQueue.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/3/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHEVENTQUEUE_H
#define PHEVENTQUEUE_H

#include "PHMain.h"

class PHTimer;
class PHEventQueue : public PHObject
{
private:
    struct event
    {
        PHInvocation invocation;
        PHSemaphore * sem;
    };
    list<event> q;
    PHMutex * mutex;
    multimap<void *,PHTimer*>timers;
public:
    PHEventQueue();
    virtual ~PHEventQueue();
    void schedule(const PHInvocation & inv, bool waitTillDone); //WARNING: don't use waitTillDone unless you call processQueue on another thread
    void processQueue();
    void updateTimers(double maxTime);
    void updateTimers() { updateTimers(INFINITY); }
    void update() { processQueue(); updateTimers(); }
    
    void scheduleTimer(PHTimer * timer) { scheduleTimer(timer, NULL); }
    void scheduleTimer(PHTimer * timer, void * ud);
    void invalidateTimersWithUserdata(void * ud);
    void invalidateAllTimers();

};

#endif