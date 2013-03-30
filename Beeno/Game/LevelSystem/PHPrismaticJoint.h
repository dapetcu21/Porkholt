/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPRISMATICJOINT_H
#define PHPRISMATICJOINT_H

#include "PHMotorJoint.h"

class PHPrismaticJoint : public PHMotorJoint
{
private:
    PHPoint anchor;
    float _lowerTranslation;
    float _upperTranslation;
    bool _enableLimit;
    PHPoint axis;
public:
    PHPrismaticJoint(PHWorld * world) : PHMotorJoint(world), anchor(PHOriginPoint), _lowerTranslation(0.0f), _upperTranslation(1.0f), _enableLimit(true) {};
    virtual ~PHPrismaticJoint();
    virtual void loadFromLua(lua_State * L);
    virtual void recreateJoint();
    
    float lowerTranslation() { return _lowerTranslation; }
    float upperTranslation() { return _upperTranslation; }
    bool limitEnabled() { return _enableLimit; }
    
    void setLimits(float lower, float upper);
    void setLowerTranslation(float val);
    void setUpperTranslation(float val);
    void setLimitEnabled(bool val);
};

#endif
