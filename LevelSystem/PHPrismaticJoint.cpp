//
//  PHPrismaticJoint.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/14/11.
//  Copyright 2011 Home. All rights reserved.
//

#include "PHMain.h"
#include <Box2D/Box2D.h>
#include "PHLua.h"

PHPrismaticJoint::~PHPrismaticJoint() {}

void PHPrismaticJoint::loadFromLua(void * l)
{
    PHMotorJoint::loadFromLua(l);
    lua_State * L = (lua_State*)l;

    anchor = PHOriginPoint;
    lua_pushstring(L,"anchor");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        anchor = PHPoint::pointFromLua(L);
    lua_pop(L, 1);
    
    _lowerTranslation = 0.0f;
    lua_pushstring(L, "lowerTranslation");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        _lowerTranslation = lua_isnumber(L, -1);
    lua_pop(L, 1);
    
    _upperTranslation = 1.0f;
    lua_pushstring(L, "upperTranslation");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        _upperTranslation = lua_isnumber(L, -1);
    lua_pop(L, 1);
    
    _enableLimit = true;
    lua_pushstring(L, "limitEnabled");
    lua_gettable(L, -2);
    if (lua_isboolean(L, -1))
        _enableLimit = lua_toboolean(L, -1);
    lua_pop(L, 1);
    
    axis.x = 0;
    axis.y = 1;
    lua_pushstring(L,"axis");
    lua_gettable(L, -2);
    if (lua_istable(L, -1))
        anchor = PHPoint::pointFromLua(L);
    lua_pop(L, 1);
}

void PHPrismaticJoint::recreateJoint()
{
    destroyJoint();
    b2PrismaticJointDef jointDef;
    b2Body * b1 = body1->getBody();
    b2Body * b2 = body2->getBody();
    
    b2Vec2 a;
    a.x = anchor.x;
    a.y = anchor.y;
    b2Vec2 ax;
    ax.x = axis.x;
    ax.y = axis.y;
    if (!worldCoord)
    {
        a = b1->GetWorldPoint(a);
    }
    jointDef.Initialize(b1,b2,a,ax);
    jointDef.collideConnected = collideConnected;
    jointDef.lowerTranslation = _lowerTranslation;
    jointDef.upperTranslation = _upperTranslation;
    jointDef.enableLimit = _enableLimit;
    joint = world->getPhysicsWorld()->CreateJoint(&jointDef);
}

void PHPrismaticJoint::setLimits(double lower, double upper)
{
    _lowerTranslation = lower;
    _upperTranslation = upper;
    if (joint)
        ((b2PrismaticJoint*)joint)->SetLimits(_lowerTranslation,_upperTranslation);
}

void PHPrismaticJoint::setLowerTranslation(double val)
{
    _lowerTranslation = val;
    if (joint)
        ((b2PrismaticJoint*)joint)->SetLimits(_lowerTranslation,_upperTranslation);
}
void PHPrismaticJoint::setUpperTranslation(double val)
{
    _upperTranslation = val;
    if (joint)
        ((b2PrismaticJoint*)joint)->SetLimits(_lowerTranslation,_upperTranslation);
}

void PHPrismaticJoint::setLimitEnabled(bool val)
{
    _enableLimit = val;
    if (joint)
        ((b2PrismaticJoint*)joint)->EnableLimit(val);
}