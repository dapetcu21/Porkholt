/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLSHIELDPOWERUP_H
#define PHLSHIELDPOWERUP_H

#include "PHLPowerup.h"

class PHLShieldPowerup : public PHLPowerup
{
public:
    PHLShieldPowerup();
private:
    virtual void collected();
};

#endif
