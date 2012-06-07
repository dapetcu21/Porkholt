/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

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
