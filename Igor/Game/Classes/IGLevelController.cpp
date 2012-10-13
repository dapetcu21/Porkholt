/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGLevelController.h"
#include "IGScripting.h"
#include "IGBackground.h"
#include "IGWorld.h"
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHViewControllerHolder.h>

IGLevelController::IGLevelController(PHGameManager * gm, PHDirectory * lvldir) : PHViewController(gm), dir(lvldir), world(NULL), scripting(NULL)
{
    dir->retain();
}

PHView * IGLevelController::loadView(const PHRect & f)
{
    animpool = new PHAnimatorPool();

    PHView * v = new PHView(f);
    world = new IGWorld(gm, dir, f);
    scripting = new IGScripting(gm, dir, world);
    world->setAnimatorPool(animpool);
    scripting->setAnimatorPool(animpool);
    bg = new IGBackground(gm);
    PHViewControllerHolder * h = new PHViewControllerHolder(f);
    h->setViewController(bg);
    bg->release();
    v->addChild(h);
    h->release();
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
    if (animpool)
        animpool->release();
}

void IGLevelController::updateScene(ph_float elapsed)
{
    animpool->advanceAnimation(elapsed);
}


