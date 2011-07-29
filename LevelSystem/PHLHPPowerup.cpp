//
//  PHLHPPowerup.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLHPPowerup.h"
#include "PHWorld.h"
#include "PHLPlayer.h"

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
