/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHDistanceJoint.h"
#include "PHWorld.h"
#include <Porkholt/Core/PHLua.h>
#include <Box2D/Box2D.h>

PHDistanceJoint::PHDistanceJoint(PHWorld * world) : PHJoint(world), freq(0), damp(0)
{
    
}

PHDistanceJoint::~PHDistanceJoint()
{
    
}

void PHDistanceJoint::loadFromLua(lua_State * L)
{
    PHJoint::loadFromLua(L);
    anchor1 = PHOriginPoint;
    anchor2 = PHOriginPoint;
    freq = 0.0f;
    damp = 0.0f;
    
    PHLuaGetPointField(anchor1,"anchor1");
    PHLuaGetPointField(anchor2,"anchor2");
    PHLuaGetNumberField(freq,"frequency");
    PHLuaGetNumberField(damp,"dampening");    
}

void PHDistanceJoint::recreateJoint()
{
    destroyJoint();
    b2DistanceJointDef jointDef;
    b2Body * b1 = body1->getBody();
    b2Body * b2 = body2->getBody();
    
    b2Vec2 a1;
    b2Vec2 a2;
    a1.x = anchor1.x;
    a1.y = anchor1.y;
    a2.x = anchor2.x;
    a2.y = anchor2.y;
    if (!worldCoord)
    {
        a1 = b1->GetWorldPoint(a1);
        a2 = b2->GetWorldPoint(a2);
    }
    jointDef.Initialize(b1,b2,a1,a2);
    jointDef.collideConnected = collideConnected;
    jointDef.frequencyHz = freq;
    jointDef.dampingRatio = damp;
    joint = world->getPhysicsWorld()->CreateJoint(&jointDef);
}
