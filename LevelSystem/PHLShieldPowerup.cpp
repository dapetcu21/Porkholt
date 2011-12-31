//
//  PHLShieldPowerup.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/25/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

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
