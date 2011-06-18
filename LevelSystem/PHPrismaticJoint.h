//
//  PHPrismaticJoint.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/14/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef INSIDE_PHJOINT_H
#ifndef INSIDE_PHMOTORJOINT_H
#ifndef PHPRISMATICJOINT_H
#define PHPRISMATICJOINT_H

#include "PHMain.h"

class PHPrismaticJoint : public PHMotorJoint
{
private:
    PHPoint anchor;
    double _lowerTranslation;
    double _upperTranslation;
    bool _enableLimit;
    PHPoint axis;
public:
    PHPrismaticJoint(PHWorld * world) : PHMotorJoint(world), anchor(PHOriginPoint), _lowerTranslation(0.0f), _upperTranslation(1.0f), _enableLimit(true) {};
    virtual ~PHPrismaticJoint();
    virtual void loadFromLua(void * L);
    virtual void recreateJoint();
    
    double lowerTranslation() { return _lowerTranslation; }
    double upperTranslation() { return _upperTranslation; }
    bool limitEnabled() { return _enableLimit; }
    
    void setLimits(double lower, double upper);
    void setLowerTranslation(double val);
    void setUpperTranslation(double val);
    void setLimitEnabled(bool val);
};

#endif
#endif
#endif