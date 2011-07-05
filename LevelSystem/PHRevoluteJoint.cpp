//
//  PHRevoluteJoint.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHRevoluteJoint.h"
#include "PHWorld.h"
#include <Box2D/Box2D.h>
#include "PHLua.h"

PHRevoluteJoint::~PHRevoluteJoint() {}

void PHRevoluteJoint::loadFromLua(lua_State * L)
{
    PHMotorJoint::loadFromLua(L);
    anchor = PHOriginPoint;
    
    lua_pushstring(L,"anchor");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        anchor = PHPoint::pointFromLua(L,-1);
    lua_pop(L, 1);
    
    _lowerAngle = 0.0f;
    lua_pushstring(L, "lowerAngle");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        _lowerAngle = lua_isnumber(L, -1);
    lua_pop(L, 1);
    
    _upperAngle = 0.0f;
    lua_pushstring(L, "upperAngle");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        _upperAngle = lua_isnumber(L, -1);
    lua_pop(L, 1);
    
    _enableLimit = false;
    lua_pushstring(L, "limitEnabled");
    lua_gettable(L, -2);
    if (lua_isboolean(L, -1))
        _enableLimit = lua_toboolean(L, -1);
    lua_pop(L, 1);
}

void PHRevoluteJoint::recreateJoint()
{
    destroyJoint();
    b2RevoluteJointDef jointDef;
    b2Body * b1 = body1->getBody();
    b2Body * b2 = body2->getBody();
    
    b2Vec2 a;
    a.x = anchor.x;
    a.y = anchor.y;
    if (!worldCoord)
        a = b1->GetWorldPoint(a);
    jointDef.Initialize(b1,b2,a);
    jointDef.collideConnected = collideConnected;
    jointDef.lowerAngle = _lowerAngle;
    jointDef.upperAngle = _upperAngle;
    jointDef.enableLimit = _enableLimit;
    joint = world->getPhysicsWorld()->CreateJoint(&jointDef);
}

void PHRevoluteJoint::setLimits(double lower, double upper)
{
    _lowerAngle = lower;
    _upperAngle = upper;
    if (joint)
        ((b2RevoluteJoint*)joint)->SetLimits(_lowerAngle,_upperAngle);
}

void PHRevoluteJoint::setLowerAngle(double val)
{
    _lowerAngle = val;
    if (joint)
        ((b2RevoluteJoint*)joint)->SetLimits(_lowerAngle,_upperAngle);
}
void PHRevoluteJoint::setUpperAngle(double val)
{
    _upperAngle = val;
    if (joint)
        ((b2RevoluteJoint*)joint)->SetLimits(_lowerAngle,_upperAngle);
}

void PHRevoluteJoint::setLimitEnabled(bool val)
{
    _enableLimit = val;
    if (joint)
        ((b2RevoluteJoint*)joint)->EnableLimit(val);
}