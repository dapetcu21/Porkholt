//
//  PHMotorJoint.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Home. All rights reserved.
//

#include "PHMain.h"
#include <Box2D/Box2D.h>
#include "PHLua.h"


PHMotorJoint::~PHMotorJoint() {}

void PHMotorJoint::loadFromLua(void * l)
{
    PHJoint::loadFromLua(l);
    lua_State * L = (lua_State*)l;
    enableMotor = false;
    lua_pushstring(L, "motorEnabled");
    lua_gettable(L, -2);
    if (lua_isboolean(L, -1))
        enableMotor = lua_toboolean(L, -1);
    lua_pop(L,1);
    
    _motorPower = 1.0f;
    lua_pushstring(L, "motorMaxPower");
    lua_gettable(L, -2);
    if (lua_isnumber(L, -1))
        _motorPower = lua_tonumber(L, -1);
    lua_pop(L,1);
    
    _motorSpeed = 1.0f;
    lua_pushstring(L, "motorSpeed");
    lua_gettable(L, -2);
    if (lua_isnumber(L, -1))
        _motorSpeed = lua_tonumber(L, -1);
    lua_pop(L,1);
}

void PHMotorJoint::setMotorEnabled(bool val)
{
    enableMotor = val;
    if (joint)
    {
        switch(joint->GetType())
        {
            case e_revoluteJoint:
                ((b2RevoluteJoint*)joint)->EnableMotor(val);
                break;
            case e_prismaticJoint:
                ((b2PrismaticJoint*)joint)->EnableMotor(val);
        }
    }
}
void PHMotorJoint::setMotorPower(double val)
{
    _motorPower = val;
    if (joint)
    {
        switch(joint->GetType())
        {
            case e_revoluteJoint:
                ((b2RevoluteJoint*)joint)->SetMaxMotorTorque(val);
                break;
            case e_prismaticJoint:
                ((b2PrismaticJoint*)joint)->SetMaxMotorForce(val);
        }
    }
}

void PHMotorJoint::setMotorSpeed(double val)
{
    _motorPower = val;
    if (joint)
    {
        switch(joint->GetType())
        {
            case e_revoluteJoint:
                ((b2RevoluteJoint*)joint)->SetMotorSpeed(val);
                break;
            case e_prismaticJoint:
                ((b2PrismaticJoint*)joint)->SetMotorSpeed(val);
        }
    }
}