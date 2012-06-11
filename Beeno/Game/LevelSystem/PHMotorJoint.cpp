/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHMotorJoint.h"
#include "PHWorld.h"
#include <Box2D/Box2D.h>
#include <Porkholt/Core/PHLua.h>


PHMotorJoint::~PHMotorJoint() {}

void PHMotorJoint::loadFromLua(lua_State * L)
{
    PHJoint::loadFromLua(L);
    enableMotor = false;
    _motorPower = 1.0f;
    _motorSpeed = 1.0f;
    PHLuaGetBoolField(enableMotor,"motorEnabled");
    PHLuaGetNumberField(_motorPower,"motorMaxPower");
    PHLuaGetNumberField(_motorSpeed,"motorSpeed");
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
                break;
            default:
                break;
        }
    }
}
void PHMotorJoint::setMotorPower(ph_float val)
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
                break;
            default:
                break;
        }
    }
}

void PHMotorJoint::setMotorSpeed(ph_float val)
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
                break;
            default:
                break;
        }
    }
}
