//
//  PHAnimatorPool.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/30/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHAnimatorPool.h"
#include "PHImageAnimator.h"

PHAnimatorPool::PHAnimatorPool() : mutex(new PHMutex(true)), insideJob(false)
{
    
}

PHAnimatorPool::~PHAnimatorPool()
{
    mutex->lock();
    for (set<PHImageAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        (*i)->pool = NULL;
    }
    mutex->unlock();
    mutex->release();
}

void PHAnimatorPool::insertAnimator(PHImageAnimator * a)
{
    mutex->lock();
    if (!insideJob)
        animators.insert(a);
    else
    {
        insertQueue.insert(a);
        deleteQueue.erase(a);
    }
    mutex->unlock();
}

void PHAnimatorPool::removeAnimator(PHImageAnimator * a)
{
    mutex->lock();
    if (!insideJob)
        animators.erase(a);
    else
    {
        deleteQueue.insert(a);
        insertQueue.erase(a);
    }
    mutex->unlock();
}

PHAnimatorPool * PHAnimatorPool::mainAnimatorPool()
{
    static PHAnimatorPool * main = NULL;
    if (!main)
        main = new PHAnimatorPool;
    return main;
}


void PHAnimatorPool::advanceAnimation(double elapsedTime)
{
    mutex->lock();
    insideJob = true;
    for (set<PHImageAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        if (deleteQueue.count(*i))
            continue;
        if (!((*i)->advanceManually))
            (*i)->advanceAnimation(elapsedTime);
    }
    for (set<PHImageAnimator*>::iterator i = deleteQueue.begin(); i!=deleteQueue.end(); i++)
        animators.erase(*i);
    animators.insert(insertQueue.begin(),insertQueue.end());
    insideJob = false;
    mutex->unlock();
}