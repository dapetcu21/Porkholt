/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHAUTORELEASEPOOL_H
#define PHAUTORELEASEPOOL_H

#include <vector>
#include <map>
#include <list>

class PHObject;
class PHMutex;

#include <Porkholt/Core/PHThread.h>

class PHAutoreleasePool
{
    std::vector<PHObject*> objects;
    static std::map<PHThreadID, std::list<PHAutoreleasePool*> > pools;
    static PHMutex * mutex;

    public:
    PHAutoreleasePool();
    ~PHAutoreleasePool();

    static PHAutoreleasePool * currentPool();
    static void autorelease(PHObject * obj);

    void addObject(PHObject * obj) { objects.push_back(obj); }
    void drain();

    private:
    static void noPoolInPlace(PHObject * obj);
};

#endif
