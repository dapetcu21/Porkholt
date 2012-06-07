/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLLEVELEND_H
#define PHLLEVELEND_H

#include "PHLSensor.h"

class PHLLevelEnd : public PHLSensor
{
public:
    PHLLevelEnd();
private:
    virtual void objectEntered(PHLObject * obj);
};

#endif
