/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGWorld.h"
#include "IGObject.h"
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHView.h>
#include <Box2D/Box2D.h>

IGWorld::IGWorld(PHGameManager * _gm, PHDirectory * _dir, const PHRect & size) : gm(_gm), dir(_dir), _view(new PHView(size))
{
    _view->setUserInput(true);
    dir->retain();
    phyWorld = new b2World(b2Vec2(0, 0));
    b2BodyDef def;
    ground = phyWorld->CreateBody(&def);
}

IGWorld::~IGWorld()
{
    _view->release();
    for (list<IGObject*>::iterator i = _objects.begin(); i != _objects.end(); i++)
        (*i)->release();
    dir->release();
    phyWorld->DestroyBody(ground);
    delete phyWorld;
}

void IGWorld::advanceAnimation(ph_float elapsed)
{
    phyWorld->Step(gm->frameInterval(), 6, 2);
    phyWorld->ClearForces();
    for (list<IGObject*>::iterator i = _objects.begin(); i != _objects.end(); i++)
        (*i)->animate(elapsed);
}

void IGWorld::insertObject(IGObject * obj, bool before, IGObject * ref)
{
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

    obj->attachedToWorld(); 
}

void IGWorld::removeObject(IGObject * obj)
{
    if (obj->world != this) return;
    obj->world = NULL;
    PHDrawable * d = obj->getDrawable();
    if (d) 
        d->removeFromParent();
    obj->setPhysicsBody(NULL);
    obj->release();
}
