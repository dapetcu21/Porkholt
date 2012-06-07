/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHPRISMATICJOINT_H
#define PHPRISMATICJOINT_H

#include "PHMotorJoint.h"

class PHPrismaticJoint : public PHMotorJoint
{
private:
    PHPoint anchor;
    ph_float _lowerTranslation;
    ph_float _upperTranslation;
    bool _enableLimit;
    PHPoint axis;
public:
    PHPrismaticJoint(PHWorld * world) : PHMotorJoint(world), anchor(PHOriginPoint), _lowerTranslation(0.0f), _upperTranslation(1.0f), _enableLimit(true) {};
    virtual ~PHPrismaticJoint();
    virtual void loadFromLua(lua_State * L);
    virtual void recreateJoint();
    
    ph_float lowerTranslation() { return _lowerTranslation; }
    ph_float upperTranslation() { return _upperTranslation; }
    bool limitEnabled() { return _enableLimit; }
    
    void setLimits(ph_float lower, ph_float upper);
    void setLowerTranslation(ph_float val);
    void setUpperTranslation(ph_float val);
    void setLimitEnabled(bool val);
};

#endif
