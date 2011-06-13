//
//  PHLPlatform.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/24/11.
//  Copyright 2011 Home. All rights reserved.
//

#include "PHLPlatform.h"
#include <Box2D/Box2D.h>

PHLPlatform::PHLPlatform()
{
    
}

PHLPlatform::~PHLPlatform()
{
    
}

void PHLPlatform::contactPreSolve(bool b, b2Contact* contact, const b2Manifold* oldManifold)
{
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
    contacts.erase(contact);
}