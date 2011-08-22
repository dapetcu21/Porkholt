//
//  PHAnimatorPool.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/30/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHAnimatorPool.h"
#include "PHImageAnimator.h"

PHAnimatorPool::PHAnimatorPool() : mutex(new PHMutex)
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
    animators.insert(a);
    mutex->unlock();
}

void PHAnimatorPool::removeAnimator(PHImageAnimator * a)
{
    mutex->lock();
    animators.erase(a);
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
    for (set<PHImageAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        if (!((*i)->advanceManually))
            (*i)->advanceAnimation(elapsedTime);
    }
    mutex->unlock();
}