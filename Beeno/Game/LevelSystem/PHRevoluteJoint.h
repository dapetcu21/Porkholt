//
//  PHRevoluteJoint.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHREVOLUTEJOINT_H
#define PHREVOLUTEJOINT_H

#include "PHMotorJoint.h"

class PHRevoluteJoint : public PHMotorJoint
{
private:
    PHPoint anchor;
    ph_float _lowerAngle;
    ph_float _upperAngle;
    bool _enableLimit;
public:
    PHRevoluteJoint(PHWorld * world) : PHMotorJoint(world), anchor(PHOriginPoint), _lowerAngle(0.0f), _upperAngle(0.0f), _enableLimit(false) {};
    virtual ~PHRevoluteJoint();
    virtual void loadFromLua(lua_State * L);
    virtual void recreateJoint();
    
    ph_float lowerAngle() { return _lowerAngle; }
    ph_float upperAngle() { return _upperAngle; }
    bool limitEnabled() { return _enableLimit; }
    
    void setLimits(ph_float lower, ph_float upper);
    void setLowerAngle(ph_float val);
    void setUpperAngle(ph_float val);
    void setLimitEnabled(bool val);
};

#endif