/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLPOWERPOWERUP_H
#define PHLPOWERPOWERUP_H

#include "PHLPowerup.h"

class PHLPowerPowerup : public PHLPowerup
{
public:
    PHLPowerPowerup();
private:
    virtual void collected();
};

#endif
