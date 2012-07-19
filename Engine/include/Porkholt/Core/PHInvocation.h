/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHINVOCATION_H
#define PHINVOCATION_H

#include <Porkholt/Core/PHObject.h>

typedef void (PHObject::*PHCallback) (PHObject * sender, void * ud);

#define PHInv(target,callback,userdata) PHInvocation((PHObject*)(target),(PHCallback)&callback,(void*)userdata)

#define PHInvN(target,callback) PHInvocation((PHObject*)(target),(PHCallback)&callback)

#define PHInvBound(target,callback,userdata) PHInvocation((PHObject*)(target),(PHCallback)&callback,(void*)userdata, true)

#define PHInvBoundN(target,callback) PHInvocation((PHObject*)(target),(PHCallback)&callback, NULL, true)


class PHInvocation {
public:
    PHObject * target;
    PHCallback callback;
    void * userdata;
    bool bound;
    
    PHInvocation(PHObject * _target, PHCallback _callback, void * _userdata, bool _bound) : target(_target), callback(_callback), userdata(_userdata), bound(_bound) 
    {
        if (bound && target)
            target->bindInvocation(this);
    }
    PHInvocation(PHObject * _target, PHCallback _callback, void * _userdata) : target(_target), callback(_callback), userdata(_userdata), bound(false) {}
    PHInvocation(PHObject * _target, PHCallback _callback) : target(_target), callback(_callback), userdata(NULL), bound(false) {}
    PHInvocation() : target(NULL), callback(NULL), userdata(NULL), bound(false) {}
    PHInvocation(const PHInvocation & o) : target(o.target), callback(o.callback), userdata(o.userdata), bound(o.bound)
    {
        if (bound && target)
            target->bindInvocation(this);
    }

    ~PHInvocation()
    {
        if (bound && target)
            target->unbindInvocation(this);
    }
    
    bool valid() const
    {
        return target && callback;
    }
    
    void clear()
    {
        target = NULL;
        callback = NULL;
        userdata = NULL;
    }
    
    void call() const
    {
        if (target && callback)
            (target->*callback)(NULL,userdata);
    }
    
    void call(PHObject * sender) const
    {
        if (target && callback)
            (target->*callback)(sender,userdata);
    }
    
    void call(PHObject * sender, void * ud) const
    {
        if (target && callback)
            (target->*callback)(sender,ud);
    }
};

#endif
