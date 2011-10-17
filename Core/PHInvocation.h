//
//  PHInvocation.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 10/16/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

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
    
    bool valid() 
    {
        return target && callback;
    }
    
    void clear()
    {
        target = NULL;
        callback = NULL;
        userdata = NULL;
    }
    
    void call()
    {
        if (target && callback)
            (target->*callback)(NULL,userdata);
    }
    
    void call(PHObject * sender) 
    {
        if (target && callback)
            (target->*callback)(sender,userdata);
    }
    
    void call(PHObject * sender, void * ud) 
    {
        if (target && callback)
            (target->*callback)(sender,ud);
    }
};

#endif
