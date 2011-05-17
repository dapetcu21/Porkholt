//
//  PHRevoluteJoint.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Home. All rights reserved.
//

#ifndef INSIDE_PHJOINT_H
#ifndef INSIDE_PHMOTORJOINT_H
#ifndef PHREVOLUTEJOINT_H
#define PHREVOLUTEJOINT_H

#include "PHMain.h"

class PHRevoluteJoint : public PHMotorJoint
{
private:
    PHPoint anchor;
    double _lowerAngle;
    double _upperAngle;
    bool _enableLimit;
public:
    PHRevoluteJoint(PHWorld * world) : PHMotorJoint(world), anchor(PHOriginPoint), _lowerAngle(0.0f), _upperAngle(0.0f), _enableLimit(false) {};
    virtual ~PHRevoluteJoint();
    virtual void loadFromLua(void * L);
    virtual void recreateJoint();
    
    double lowerAngle() { return _lowerAngle; }
    double upperAngle() { return _upperAngle; }
    bool limitEnabled() { return _enableLimit; }
    
    void setLimits(double lower, double upper);
    void setLowerAngle(double val);
    void setUpperAngle(double val);
    void setLimitEnabled(bool val);
};

#endif
#endif
#endif