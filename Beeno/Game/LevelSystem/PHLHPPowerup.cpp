/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLHPPowerup.h"
#include "PHWorld.h"
#include "PHLPlayer.h"

PHL_REGISTEROBJECT(PHLHPPowerup)

PHLHPPowerup::PHLHPPowerup()
{
    _class = "PHLHPPowerup";
}

void PHLHPPowerup::collected()
{
    PHLPlayer * p = getWorld()->getPlayer();
    if (p)
        p->increaseHP(1.0f);
}
