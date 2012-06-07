/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLLevelEnd.h"
#include "PHWorld.h"
#include "PHLevelController.h"

PHL_REGISTEROBJECT(PHLLevelEnd)

PHLLevelEnd::PHLLevelEnd() 
{
    _class = "PHLLevelEnd"; 
}

void PHLLevelEnd::objectEntered(PHLObject * obj)
{
    getWorld()->levelController()->endLevelWithOutcome(PHLevelController::LevelWon);
}
