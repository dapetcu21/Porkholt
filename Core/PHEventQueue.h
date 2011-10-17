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
public:
    PHEventQueue();
    virtual ~PHEventQueue();
    void schedule(PHInvocation inv, bool waitTillDone); //WARNING: don't use waitTillDone unless you call processQueue on another thread
    void processQueue();
};

#endif