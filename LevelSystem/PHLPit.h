//
//  PHLPit.h
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/27/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#ifndef PHLPIT_H
#define PHLPIT_H

#include "PHLSensor.h"

class PHLPit : PHLSensor
{
public:
    PHLPit() { _class = "PHLPit"; }
private:
    virtual void objectEntered(PHLObject * obj);
};

#endif
