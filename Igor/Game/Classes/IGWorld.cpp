/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGWorld.h"
#include "IGObject.h"
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHView.h>

IGWorld::IGWorld(PHGameManager * _gm, PHDirectory * _dir, const PHRect & size) : gm(_gm), dir(_dir), _view(new PHView(size))
{
    dir->retain();
}

IGWorld::~IGWorld()
{
    _view->release();
    for (list<IGObject*>::iterator i = _objects.begin(); i != _objects.end(); i++)
        (*i)->release();
    dir->release();
}

void IGWorld::advanceAnimation(ph_float elapsed)
{
    for (list<IGObject*>::iterator i = _objects.begin(); i != _objects.end(); i++)
        (*i)->animate(elapsed);
}

void IGWorld::insertObject(IGObject * obj, bool before, IGObject * ref)
{
    if (obj->world) return;
    list<IGObject*>::iterator pos = _objects.end();
    if (before)
    {
        if (ref && ref->world)
            pos = ref->world_pos;
    } else {
        if (ref && ref->world)
        {
            pos = ref->world_pos;
            pos++;
        } else 
            pos = _objects.begin();
    }
    obj->world = this;
    obj->world_pos = _objects.insert(pos, obj);
    obj->retain();

    pos = obj->world_pos;
    PHDrawable * d = obj->getDrawable();
    if (d)
    {
        PHDrawable * lastD = NULL;
        for (pos++; (pos != _objects.end()) && !lastD; pos++)
            lastD = (*pos)->getDrawable();
        _view->addChildBefore(d, lastD);
    }

    //add physics here
}

void IGWorld::removeObject(IGObject * obj)
{
    if (obj->world != this) return;
    obj->world = NULL;
    PHDrawable * d = obj->getDrawable();
    if (d) 
        d->removeFromParent();
    //remove physics here
    obj->release();
}
