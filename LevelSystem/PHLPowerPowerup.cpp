//
//  PHLPowerPowerup.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/29/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLPowerPowerup.h"
#include "PHWorld.h"
#include "PHLPlayer.h"


PHLPowerPowerup::PHLPowerPowerup()
{
    _class = "PHLPowerPowerup";
}

void PHLPowerPowerup::collected()
{
    getWorld()->getPlayer()->activatePower();
}
