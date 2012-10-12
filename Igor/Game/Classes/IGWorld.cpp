/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGWorld.h"
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/IO/PHDirectory.h>
#include <Porkholt/Core/PHView.h>

IGWorld::IGWorld(PHGameManager * _gm, PHDirectory * _dir, const PHRect & size) : gm(_gm), dir(_dir), _view(new PHView(size))
{
    dir->retain();
}

IGWorld::~IGWorld()
{
    _view->release();
    dir->release();
}

