/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLPit.h"
#include "PHWorld.h"
#include "PHLPlayer.h"

PHL_REGISTEROBJECT(PHLPit)

void PHLPit::objectEntered(PHLObject * obj)
{
    PHLNPC * p = dynamic_cast<PHLNPC*>(obj);
    if (p)
        p->setHP(0);
}
