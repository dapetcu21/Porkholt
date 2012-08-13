/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHAUTORELEASEPOOL_H
#define PHAUTORELEASEPOOL_H

#include <vector>
#include <map>
#include <list>

class PHObject;
class PHThread;
class PHMutex;

class PHAutoreleasePool
{
    std::vector<PHObject*> objects;
    static std::map<PHThread*, std::list<PHAutoreleasePool*> > pools;
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
