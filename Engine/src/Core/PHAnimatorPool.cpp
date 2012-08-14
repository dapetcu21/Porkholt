    /* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHAnimator.h>
#include <Porkholt/Core/PHTimer.h>
#include <Porkholt/Core/PHMutex.h>

PHAnimatorPool::PHAnimatorPool() : insideJob(false), mutex(new PHMutex(true))
{
    
}

PHAnimatorPool::~PHAnimatorPool()
{
    mutex->lock();
    for (set<PHAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        (*i)->pool = NULL;
        (*i)->releaseInPool();
    }
    mutex->unlock();
    mutex->release();
}

void PHAnimatorPool::insertAnimator(PHAnimator * a)
{
    mutex->lock();
    a->retainInPool();
    if (a->pool && (a->pool != this))
        a->pool->removeAnimator(a);
    a->pool = this;
    if (!insideJob)
    {
        if (!animators.insert(a).second)
            a->releaseInPool();
    }
    else
    {
        if (!insertQueue.insert(a).second)
            a->releaseInPool();
        deleteQueue.erase(a);
    }
    mutex->unlock();
}

void PHAnimatorPool::removeAnimator(PHAnimator * a)
{
    mutex->lock();
    a->pool = NULL;
    if (!insideJob)
    {
        if (animators.erase(a))
            a->releaseInPool();
    }
    else
    {
        deleteQueue.insert(a);
        if (insertQueue.erase(a))
            a->releaseInPool();
    }
    mutex->unlock();
}

void PHAnimatorPool::removeAllAnimators()
{
    mutex->lock();
    if (insideJob)
        deleteQueue.insert(animators.begin(),animators.end());
    else
    {
        for (set<PHAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
        {
            (*i)->pool = NULL;
            (*i)->releaseInPool();
        }
        animators.clear();
    }
    mutex->unlock();
}

void PHAnimatorPool::removeAnimatorsWithTag(int d)
{
    mutex->lock();
    for (set<PHAnimator*>::iterator i = animators.begin(); i != animators.end(); i++)
        if ((*i)->tag() == d)
            deleteQueue.insert(*i);
    if (!insideJob)
    {
        for (set<PHAnimator*>::iterator i = deleteQueue.begin(); i!=deleteQueue.end(); i++) 
            if (animators.erase(*i))
            {
                (*i)->pool = NULL;
                (*i)->releaseInPool();
            }
        deleteQueue.clear();
    }
    mutex->unlock();
}

void removeAllAnimators();
void remoteAnimatorsWithTag(int d);

void PHAnimatorPool::advanceAnimation(ph_float elapsedTime)
{
    mutex->lock();
    insideJob = true;
    for (set<PHAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        if (deleteQueue.count(*i))
            continue;
        if (!((*i)->advanceManually))
            (*i)->advanceAnimation(elapsedTime);
    }
    for (set<PHAnimator*>::iterator i = deleteQueue.begin(); i!=deleteQueue.end(); i++)
        if (animators.erase(*i))
        {
            (*i)->pool = NULL;
            (*i)->releaseInPool();
        }
    animators.insert(insertQueue.begin(),insertQueue.end());
    deleteQueue.clear();
    insertQueue.clear();
    insideJob = false;
    mutex->unlock();
}

void PHAnimatorPool::scheduleAction(const PHInvocation & inv, double time, bool repeat, bool waitForIt)
{
    PHTimer::scheduleAction(this, inv, time, repeat, waitForIt);
}

