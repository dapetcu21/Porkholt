/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHLAuxLayer.h"
#include "PHAuxLayerView.h"
#include "PHLua.h"

PHL_REGISTEROBJECT(PHLAuxLayer)

PHLAuxLayer::PHLAuxLayer()
{
    _class = "PHLAuxLayer";
}

PHLAuxLayer::~PHLAuxLayer()
{
    
}

void PHLAuxLayer::loadFromLua(lua_State * L, b2World * world, PHLevelController * lvlc)
{
    PHLObject::loadFromLua(L, world, lvlc);
    
    name = rand();
    PHLuaGetNumberField(name, "name");
}

void PHLAuxLayer::loadView()
{
    view = new PHAuxLayerView(name);
    view->setFrame(PHRect(pos.x, pos.y, 1.0f,1.0f));
    view->setRotationalCenter(PHOriginPoint);
    view->setRotation(rot);
}
