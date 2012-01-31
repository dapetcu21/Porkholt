//
//  PHPrismaticJoint.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHPrismaticJoint.h"
#include "PHWorld.h"
#include <Box2D/Box2D.h>
#include "PHLua.h"

PHPrismaticJoint::~PHPrismaticJoint() {}

void PHPrismaticJoint::loadFromLua(lua_State * L)
{
    PHMotorJoint::loadFromLua(L);

    anchor = PHOriginPoint;
    _lowerTranslation = _upperTranslation = 0.0f;
    _enableLimit = true;
    axis = PHPoint(0,1);
    
    PHLuaGetPointField(anchor, "anchor");
    PHLuaGetNumberField(_lowerTranslation, "lowerTranslation");
    PHLuaGetNumberField(_upperTranslation, "upperTranslation");
    PHLuaGetBoolField(_enableLimit, "limitEnabled");
    PHLuaGetPointField(axis, "axis");
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

void PHPrismaticJoint::setLimits(ph_float lower, ph_float upper)
{
    _lowerTranslation = lower;
    _upperTranslation = upper;
    if (joint)
        ((b2PrismaticJoint*)joint)->SetLimits(_lowerTranslation,_upperTranslation);
}

void PHPrismaticJoint::setLowerTranslation(ph_float val)
{
    _lowerTranslation = val;
    if (joint)
        ((b2PrismaticJoint*)joint)->SetLimits(_lowerTranslation,_upperTranslation);
}
void PHPrismaticJoint::setUpperTranslation(ph_float val)
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