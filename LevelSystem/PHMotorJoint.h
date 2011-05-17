//
//  PHMotorJoint.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Home. All rights reserved.
//

#ifndef INSIDE_PHJOINT_H
#ifndef PHMOTORJOINT_H
#define PHMOTORJOINT_H
#define INSIDE_PHMOTORJOINT_H

#include "PHMain.h"

class PHMotorJoint : public PHJoint
{
protected:
    double _motorPower; //This can be either torque or force
    double _motorSpeed; //This can be set to zero to simulate friction
    bool enableMotor;
public:
    PHMotorJoint(PHWorld * world) : PHJoint(world), _motorPower(1.0f), _motorSpeed(0.0f), enableMotor(false) {};
    virtual ~PHMotorJoint();
    virtual void loadFromLua(void * L);
    
    bool motorEnabled() { return enableMotor; }
    void setMotorEnabled(bool val);
    double motorPower() { return _motorPower; }
    void setMotorPower(double val);
    double motorSpeed() { return _motorSpeed; }
    void setMotorSpeed(double val);
};

#undef INSIDE_PHMOTORJOINT_H
#endif
#endif