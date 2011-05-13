//
//  PHDistanceJoint.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 5/13/11.
//  Copyright 2011 Home. All rights reserved.
//

#ifndef PHDISTANCEJOINT_H
#define PHDISTANCEJOINT_H

#include "PHMain.h"

class PHDistanceJoint : public PHJoint
{
private:
    PHPoint anchor1;
    PHPoint anchor2;
public:
    PHDistanceJoint(PHWorld * world);
    virtual ~PHDistanceJoint();
    virtual void loadFromLua(void * L);
    virtual void recreateJoint();
};

#endif