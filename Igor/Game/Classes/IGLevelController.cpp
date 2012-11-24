/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGLevelController.h"
#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHViewControllerHolder.h>

IGLevelController::IGLevelController(PHGameManager * gm, PHDirectory * lvldir) : PHViewController(gm), dir(lvldir), world(NULL), scripting(NULL)
{
    dir->retain();
}

PHView * IGLevelController::loadView(const PHRect & f)
{
    animpool = new PHAnimatorPool();

    PHView * v = new PHView(f);
    v->setUserInput(true);
    world = new IGWorld(gm, dir, f, this);
    scripting = new IGScripting(gm, dir, world);
    scripting->setAnimatorPool(animpool);
    world->setAnimatorPool(animpool);
    PHViewControllerHolder * h = new PHViewControllerHolder(f);
    v->addChild(h);
    h->release();
    v->addChild(world->view());
    gm->setClearColor(PHColor((float)0xd9/0xff, (float)0x68/0xff, (float)0x68/0xff));

    gm->imageNamed("basic_mob");
    gm->imageNamed("knife");

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


