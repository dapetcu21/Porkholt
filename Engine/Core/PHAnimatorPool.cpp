    /* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHAnimatorPool.h"
#include "PHAnimator.h"

map < PHThread*, list<PHAnimatorPool*> > PHAnimatorPool::stacks;
PHMutex * PHAnimatorPool::staticMutex = new PHMutex;

PHAnimatorPool::PHAnimatorPool() : insideJob(false), mutex(new PHMutex(true))
{
    
}

PHAnimatorPool::~PHAnimatorPool()
{
    mutex->lock();
    for (set<PHAnimator*>::iterator i = animators.begin(); i!=animators.end(); i++)
    {
        (*i)->pool = NULL;
    }
    mutex->unlock();
    mutex->release();
}

void PHAnimatorPool::insertAnimator(PHAnimator * a)
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

void PHAnimatorPool::removeAnimator(PHAnimator * a)
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

void PHAnimatorPool::removeAllAnimators()
{
    mutex->lock();
    if (insideJob)
        deleteQueue.insert(animators.begin(),animators.end());
    else
        animators.clear();
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
            animators.erase(*i);
        deleteQueue.clear();
    }
    mutex->unlock();
}

PHAnimatorPool * PHAnimatorPool::currentAnimatorPool()
{
    staticMutex->lock();
    map < PHThread*, list<PHAnimatorPool*> >::iterator it = stacks.find(PHThread::currentThread());
    if (it!=stacks.end())
    {
        if (!it->second.empty())
        {
            PHAnimatorPool * ap = it->second.back();
            staticMutex->unlock();
            return ap;
        }
    }
    staticMutex->unlock();
    return mainAnimatorPool();
}

void PHAnimatorPool::push()
{
    staticMutex->lock();
    map < PHThread*, list<PHAnimatorPool*> >::iterator it = stacks.find(PHThread::currentThread());
    if (it==stacks.end())
        it = stacks.insert( make_pair<PHThread*, list<PHAnimatorPool*> >(PHThread::currentThread(),list<PHAnimatorPool*>())).first;
    it->second.push_back(this);
    staticMutex->unlock();
}

void PHAnimatorPool::popPool()
{
    staticMutex->lock();
    map < PHThread*, list<PHAnimatorPool*> >::iterator it = stacks.find(PHThread::currentThread());
    if (it!=stacks.end())
    {
        it->second.pop_back();
    }
    staticMutex->unlock();
}

PHAnimatorPool * PHAnimatorPool::mainAnimatorPool()
{
    staticMutex->lock();
    static PHAnimatorPool * main = NULL;
    if (!main)
        main = new PHAnimatorPool;
    staticMutex->unlock();
    return main;
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
        animators.erase(*i);
    animators.insert(insertQueue.begin(),insertQueue.end());
    deleteQueue.clear();
    insertQueue.clear();
    insideJob = false;
    mutex->unlock();
}
