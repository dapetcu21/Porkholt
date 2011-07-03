//
//  PHDistanceJoint.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHDISTANCEJOINT_H
#define PHDISTANCEJOINT_H

#include "PHJoint.h"

class PHDistanceJoint : public PHJoint
{
private:
    PHPoint anchor1;
    PHPoint anchor2;
    double freq,damp;
public:
    PHDistanceJoint(PHWorld * world);
    virtual ~PHDistanceJoint();
    virtual void loadFromLua(lua_State * L);
    virtual void recreateJoint();
};

#endif