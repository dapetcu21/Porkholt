/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#ifndef PHLHPPOWERUP_H
#define PHLHPPOWERUP_H

#include "PHLPowerup.h"

class PHLHPPowerup : public PHLPowerup
{
public:
    PHLHPPowerup();
private:
    virtual void collected();
};

#endif
