/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGLevelController.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/IO/PHDirectory.h>

IGLevelController::IGLevelController(PHGameManager * gm, PHDirectory * lvldir) : PHViewController(gm), dir(lvldir), world(NULL), scripting(NULL)
{
    dir->retain();
}

PHView * IGLevelController::loadView(const PHRect & f)
{
    PHView * v = new PHView(f);
    world = new IGWorld(gm, dir, f);
    scripting = new IGScripting(gm, dir, world);

    v->addChild(world->view());
    return v;
}

IGLevelController::~IGLevelController()
{
    if (world)
        world->release();
    if (scripting)
        scripting->release();
    if (dir)
        dir->release();
}

void IGLevelController::updateScene(ph_float elapsed)
{
    scripting->frame(elapsed);
}


