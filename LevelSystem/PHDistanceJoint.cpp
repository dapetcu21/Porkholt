//
//  PHDistanceJoint.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Home. All rights reserved.
//

#include "PHMain.h"
#include "PHLua.h"
#include <Box2D/Box2D.h>

PHDistanceJoint::PHDistanceJoint(PHWorld * world) : PHJoint(world)
{
    
}

PHDistanceJoint::~PHDistanceJoint()
{
    
}

void PHDistanceJoint::loadFromLua(void * l)
{
    PHJoint::loadFromLua(l);
    lua_State * L = (lua_State*)l;
    anchor1 = PHOriginPoint;
    anchor2 = PHOriginPoint;
    
    lua_pushstring(L,"anchor1");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        anchor1 = PHPoint::pointFromLua(L);
    lua_pop(L, 1);
    
    lua_pushstring(L, "anchor2");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        anchor2 = PHPoint::pointFromLua(L);
    lua_pop(L,1);
    
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
    joint = world->getPhysicsWorld()->CreateJoint(&jointDef);
}