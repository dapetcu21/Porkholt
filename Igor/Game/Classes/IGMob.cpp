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

void IGMob::loseHP(ph_float h)
{
    hp -= h;
    if (hp < 0)
        die();
}

void IGMob::die()
{
    removeFromWorld();
}

//--- Lua Scripting ---

PHLuaNumberGetter(IGMob, health);
PHLuaNumberSetter(IGMob, setHealth);
PHLuaNumberSetter(IGMob, loseHP);
PHLuaDefineCall(IGMob, die);

void IGMob::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGMob");

    PHLuaAddMethod(IGMob, health);
    PHLuaAddMethod(IGMob, setHealth);
    PHLuaAddMethod(IGMob, loseHP);
    PHLuaAddMethod(IGMob, die);

    lua_pop(L, 1);
}
