//
//  PHLGround.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/25/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLGround.h"
#include "PHLPlayer.h"
#include "PHLCamera.h"
#include "PHWorld.h"
#include <Box2D/Box2D.h>

PHLGround::PHLGround()
{
    _class = "PHLGround";
}

PHLGround::~PHLGround()
{
    
}

void PHLGround::contactBegin(bool b,b2Contact* contact)
{
    PHLObject * o1 = (PHLObject*)contact->GetFixtureA()->GetBody()->GetUserData();
    PHLObject * o2 = (PHLObject*)contact->GetFixtureB()->GetBody()->GetUserData();
    PHWorld * w = getWorld();
    PHLPlayer * p = w->getPlayer();
    if (o1==p || o2==p)
    {
        PHLog("entered");
        w->getCamera()->incrementStrictTracking();
    }
}

void PHLGround::contactEnd(bool b,b2Contact* contact)
{
    PHLObject * o1 = (PHLObject*)contact->GetFixtureA()->GetBody()->GetUserData();
    PHLObject * o2 = (PHLObject*)contact->GetFixtureB()->GetBody()->GetUserData();
    PHWorld * w = getWorld();
    PHLPlayer * p = w->getPlayer();
    if (o1==p || o2==p)
    {
        PHLog("exited");
        w->getCamera()->decrementStrictTracking();
    }
}