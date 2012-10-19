/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGDampingProp.h"
#include "IGScripting.h"
#include <Porkholt/Core/PHLua.h>

IGSCRIPTING_REGISTERCLASS("IGDampingProp", IGDampingProp)

IGDampingProp::IGDampingProp(IGWorld * w) : IGProp(w)
{
}

IGDampingProp::~IGDampingProp()
{
}

//--- Lua Scripting ---

void IGDampingProp::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGDampingProp");

    lua_pop(L, 1);
}

