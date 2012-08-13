/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>

map<PHThread*, list<PHAutoreleasePool*> > PHAutoreleasePool::pools;
PHMutex * PHAutoreleasePool::mutex = new PHMutex();

void PHAutoreleasePool::noPoolInPlace(PHObject * obj)
{
    PHLog("Object 0x%llx is autoreleased with no pool in place. It will leak. Set a breakpoint in PHAutoreleasePool::noPoolInPlace to debug", (unsigned long long)(size_t)obj);
}

void PHAutoreleasePool::autorelease(PHObject * obj)
{
    PHAutoreleasePool * p = currentPool();
    if (p)
        p->addObject(obj);
    else
        noPoolInPlace(obj);
}

PHAutoreleasePool * PHAutoreleasePool::currentPool()
{
    PHAutoreleasePool * r = NULL;
    mutex->lock();

    map<PHThread*, list<PHAutoreleasePool*> >::iterator i = pools.find(PHThread::currentThread());
    if (i != pools.end())
    {
        list<PHAutoreleasePool*> & l = i->second;
        if (!l.empty())
            r = l.back();
    }

    mutex->unlock();
    return r;
}

void PHAutoreleasePool::drain()
{
    for (vector<PHObject*>::iterator i = objects.begin(); i!= objects.end(); i++)
        (*i)->release();
}

PHAutoreleasePool::PHAutoreleasePool()
{
    mutex->lock();
    PHThread * t = PHThread::currentThread();
    map<PHThread*, list<PHAutoreleasePool*> >::iterator i = pools.find(t);
    if (i == pools.end())
        i = pools.insert(make_pair(t, list<PHAutoreleasePool*>())).first;
    i->second.push_back(this);
    mutex->unlock();
}

PHAutoreleasePool::~PHAutoreleasePool()
{
    drain();

    mutex->lock();
    map<PHThread*, list<PHAutoreleasePool*> >::iterator i = pools.find(PHThread::currentThread());
    if (i != pools.end())
    {
        list<PHAutoreleasePool*> & l = i->second;
        if (!l.empty())
        {
            for (list<PHAutoreleasePool*>::reverse_iterator j = l.rbegin(); j != l.rend(); j++)
                if ((*j) == this)
                {
                    l.erase(--j.base());
                    break;
                }
            if (l.empty())
                pools.erase(i);
        }
    }
    mutex->unlock();
}
