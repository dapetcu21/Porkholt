//
//  PHDistanceJoint.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef INSIDE_PHJOINT_H
#ifndef PHDISTANCEJOINT_H
#define PHDISTANCEJOINT_H

#include "PHMain.h"

class PHDistanceJoint : public PHJoint
{
private:
    PHPoint anchor1;
    PHPoint anchor2;
    double freq,damp;
public:
    PHDistanceJoint(PHWorld * world);
    virtual ~PHDistanceJoint();
    virtual void loadFromLua(void * L);
    virtual void recreateJoint();
};

#endif
#endif