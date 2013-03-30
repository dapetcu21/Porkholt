/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHDISTANCEJOINT_H
#define PHDISTANCEJOINT_H

#include "PHJoint.h"

class PHDistanceJoint : public PHJoint
{
private:
    PHPoint anchor1;
    PHPoint anchor2;
    float freq,damp;
public:
    PHDistanceJoint(PHWorld * world);
    virtual ~PHDistanceJoint();
    virtual void loadFromLua(lua_State * L);
    virtual void recreateJoint();
};

#endif
