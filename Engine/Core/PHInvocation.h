/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHINVOCATION_H
#define PHINVOCATION_H

class PHObject;

typedef void (PHObject::*PHCallback) (PHObject * sender, void * ud);

#define PHInv(target,callback,userdata) PHInvocation((PHObject*)(target),(PHCallback)&callback,(void*)userdata)

#define PHInvN(target,callback) PHInvocation((PHObject*)(target),(PHCallback)&callback)

class PHInvocation {
public:
    PHObject * target;
    PHCallback callback;
    void * userdata;
    
    PHInvocation(PHObject * _target, PHCallback _callback, void * _userdata) : target(_target), callback(_callback), userdata(_userdata) {}
    PHInvocation(PHObject * _target, PHCallback _callback) : target(_target), callback(_callback), userdata(NULL) {}
    PHInvocation() : target(NULL), callback(NULL), userdata(NULL) {}
    PHInvocation(const PHInvocation & o) : target(o.target), callback(o.callback), userdata(o.userdata) {}
    
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
