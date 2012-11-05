/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGKinematic.h"
#include "IGWorld.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHImageView.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Box2D/Box2D.h>

IGSCRIPTING_REGISTERCLASS("IGKinematic", IGKinematic)

IGKinematic::IGKinematic(IGWorld * w) : IGProp(w)
{
}

IGKinematic::~IGKinematic()
{
}

void IGKinematic::attachedToWorld()
{
    b2BodyDef def;
    def.type = b2_kinematicBody;
    def.position.Set(pos.x, pos.y);
    def.angle = rot;
    def.userData = this;
    body = world->physicsWorld()->CreateBody(&def);
}

//--- Lua Scripting ---
void IGKinematic::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGKinematic");

    lua_pop(L, 1);
}
