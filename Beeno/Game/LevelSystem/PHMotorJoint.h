/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHMOTORJOINT_H
#define PHMOTORJOINT_H

#include "PHJoint.h"

class PHMotorJoint : public PHJoint
{
protected:
    ph_float _motorPower; //This can be either torque or force
    ph_float _motorSpeed; //This can be set to zero to simulate friction
    bool enableMotor;
public:
    PHMotorJoint(PHWorld * world) : PHJoint(world), _motorPower(1.0f), _motorSpeed(0.0f), enableMotor(false) {};
    virtual ~PHMotorJoint();
    virtual void loadFromLua(lua_State * L);
    
    bool motorEnabled() { return enableMotor; }
    void setMotorEnabled(bool val);
    ph_float motorPower() { return _motorPower; }
    void setMotorPower(ph_float val);
    ph_float motorSpeed() { return _motorSpeed; }
    void setMotorSpeed(ph_float val);
};

#endif
