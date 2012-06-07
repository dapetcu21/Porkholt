/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLPlatform.h"
#include <Box2D/Box2D.h>

PHL_REGISTEROBJECT(PHLPlatform)

PHLPlatform::PHLPlatform() : PHLObject()
{
    
}

PHLPlatform::~PHLPlatform()
{
    
}

void PHLPlatform::contactPreSolve(bool b, b2Contact* contact, const b2Manifold* oldManifold)
{
    PHLObject::contactPreSolve(b, contact, oldManifold);
    if (contacts.count(contact))
    {
        contact->SetEnabled(false);
        return;
    }
    b2WorldManifold man;
    contact->GetWorldManifold(&man);
    b2Vec2 normal = man.normal;
    if (b)
        normal = -normal;
    if (normal.y<0.5)
    {
        contacts.insert(contact);
        contact->SetEnabled(false);
    }
}

void PHLPlatform::contactEnd(bool b, b2Contact * contact)
{
    PHLObject::contactEnd(b, contact);
    contacts.erase(contact);
}
