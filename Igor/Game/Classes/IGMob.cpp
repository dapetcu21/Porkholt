/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGMob.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>

IGSCRIPTING_REGISTERCLASS("IGMob", IGMob)

IGMob::IGMob(IGWorld * w) : IGDampingProp(w)
{
}

IGMob::~IGMob()
{
}

void IGMob::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGMob");

    lua_pop(L, 1);
}
