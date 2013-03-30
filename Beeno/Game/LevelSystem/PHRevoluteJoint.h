/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHREVOLUTEJOINT_H
#define PHREVOLUTEJOINT_H

#include "PHMotorJoint.h"

class PHRevoluteJoint : public PHMotorJoint
{
private:
    PHPoint anchor;
    float _lowerAngle;
    float _upperAngle;
    bool _enableLimit;
public:
    PHRevoluteJoint(PHWorld * world) : PHMotorJoint(world), anchor(PHOriginPoint), _lowerAngle(0.0f), _upperAngle(0.0f), _enableLimit(false) {};
    virtual ~PHRevoluteJoint();
    virtual void loadFromLua(lua_State * L);
    virtual void recreateJoint();
    
    float lowerAngle() { return _lowerAngle; }
    float upperAngle() { return _upperAngle; }
    bool limitEnabled() { return _enableLimit; }
    
    void setLimits(float lower, float upper);
    void setLowerAngle(float val);
    void setUpperAngle(float val);
    void setLimitEnabled(bool val);
};

#endif
