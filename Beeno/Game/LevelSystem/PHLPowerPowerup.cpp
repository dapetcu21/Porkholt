/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLPowerPowerup.h"
#include "PHWorld.h"
#include "PHLPlayer.h"

PHL_REGISTEROBJECT(PHLPowerPowerup)

PHLPowerPowerup::PHLPowerPowerup()
{
    _class = "PHLPowerPowerup";
}

void PHLPowerPowerup::collected()
{
    getWorld()->getPlayer()->activatePower();
}
