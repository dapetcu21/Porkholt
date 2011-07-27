//
//  PHLPit.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 7/27/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHLPit.h"
#include "PHWorld.h"
#include "PHLPlayer.h"

void PHLPit::objectEntered(PHLObject * obj)
{
    PHLPlayer * p = getWorld()->getPlayer();
    if (obj == p)
    {
        p->setHP(0);
    }
}