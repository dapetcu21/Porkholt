/* Copyright (c) Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Core/PHAutoreleasePool.h>

void PHObject::bindInvocation(PHInvocation * invo)
{
    set<PHInvocation*> * s = (set<PHInvocation*> *)inv;
    if (!s)
        inv = (void*)(s = new set<PHInvocation*> );
    s->insert(invo);
}

void PHObject::unbindInvocation(PHInvocation * invo)
{
    set<PHInvocation*> * s = (set<PHInvocation*> *)inv;
    if (s)
        s->erase(invo);
}

void PHObject::autorelease()
{
    PHAutoreleasePool::autorelease(this);
}

void PHObject::clearInvocations()
{
    set<PHInvocation*> * s = (set<PHInvocation*> *)inv;
    for (set<PHInvocation*>::iterator i = s->begin(); i!= s->end(); i++)
    {
        (*i)->bound = false;
        (*i)->clear();
    }
    delete s;
}

void PHObject::zombie()
{
    PHLog("HEY YOU! this(%p) is a zombie object. Set a breakpoint in PHObject::zombie() to debug\n", this);
}
