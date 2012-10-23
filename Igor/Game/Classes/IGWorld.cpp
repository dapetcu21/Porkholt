/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGWorld.h"
#include "IGObject.h"
#include "IGContactHandler.h"
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHView.h>
#include <Box2D/Box2D.h>

class IGContactListener : public b2ContactListener
{
    public:
        void BeginContact(b2Contact* contact)
        {
            IGContactHandler * ha = dynamic_cast<IGContactHandler*>((IGObject*)contact->GetFixtureA()->GetBody()->GetUserData());
            if (ha) ha->beginContact(true, contact);
            IGContactHandler * hb = dynamic_cast<IGContactHandler*>((IGObject*)contact->GetFixtureB()->GetBody()->GetUserData());
            if (hb) hb->beginContact(false, contact);
        }

        void EndContact(b2Contact* contact)
        {
            IGContactHandler * ha = dynamic_cast<IGContactHandler*>((IGObject*)contact->GetFixtureA()->GetBody()->GetUserData());
            if (ha) ha->endContact(true, contact);
            IGContactHandler * hb = dynamic_cast<IGContactHandler*>((IGObject*)contact->GetFixtureB()->GetBody()->GetUserData());
            if (hb) hb->endContact(false, contact);
        }
        
        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
        {
            IGContactHandler * ha = dynamic_cast<IGContactHandler*>((IGObject*)contact->GetFixtureA()->GetBody()->GetUserData());
            if (ha) ha->preSolve(true, contact, oldManifold);
            IGContactHandler * hb = dynamic_cast<IGContactHandler*>((IGObject*)contact->GetFixtureB()->GetBody()->GetUserData());
            if (hb) hb->preSolve(false, contact, oldManifold);
        }

        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
        {
            IGContactHandler * ha = dynamic_cast<IGContactHandler*>((IGObject*)contact->GetFixtureA()->GetBody()->GetUserData());
            if (ha) ha->postSolve(true, contact, impulse);
            IGContactHandler * hb = dynamic_cast<IGContactHandler*>((IGObject*)contact->GetFixtureB()->GetBody()->GetUserData());
            if (hb) hb->postSolve(false, contact, impulse);
        }
};

IGWorld::IGWorld(PHGameManager * _gm, PHDirectory * _dir, const PHRect & size) : gm(_gm), dir(_dir), _view(new PHView(size))
{
    _view->setUserInput(true);
    dir->retain();
    phyWorld = new b2World(b2Vec2(0, 0));
    phyWorld->SetAutoClearForces(false);
    b2BodyDef def;
    ground = phyWorld->CreateBody(&def);
    cl = new IGContactListener();
    phyWorld->SetContactListener(cl);
}

IGWorld::~IGWorld()
{
    _view->release();
    for (list<IGObject*>::iterator i = _objects.begin(); i != _objects.end(); i++)
        (*i)->release();
    dir->release();
    phyWorld->DestroyBody(ground);
    delete phyWorld;
    delete cl;
}

void IGWorld::advanceAnimation(ph_float elapsed)
{
    ph_float ts = gm->frameInterval();
    if (elapsed < ts/2) return;
    for (list<IGObject*>::iterator i = _objects.begin(); i != _objects.end(); i++)
        (*i)->adjustPhysics(elapsed);
    for (ph_float el = elapsed; el >= ts/2; el -= ts)
        phyWorld->Step(ts, 6, 2);
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
