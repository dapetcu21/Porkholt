/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLShieldPowerup.h"
#include "PHLPlayer.h"
#include "PHWorld.h"

PHL_REGISTEROBJECT(PHLShieldPowerup)

PHLShieldPowerup::PHLShieldPowerup()
{
    _class = "PHLShieldPowerup";
}

void PHLShieldPowerup::collected()
{
    PHLPlayer * p = getWorld()->getPlayer();
    if (p)
        p->activateShield();
}
