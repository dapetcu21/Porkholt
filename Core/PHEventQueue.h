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
        PHCallback cb;
        PHObject * target;
        PHSemaphore * sem;
        void * ud;
    };
    list<event> q;
    PHMutex * mutex;
public:
    PHEventQueue();
    ~PHEventQueue();
    void schedule(PHObject * target, PHCallback cb, void * ud, bool waitTillDone); //WARNING: don't use waitTillDone unless you call processQueue on another thread
    void processQueue();
};

#endif