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

void IGMob::loseHealth(float h)
{
    hp -= h;
    if (hp <= 0)
        die();
}

void IGMob::die()
{
    if (L)
    {
        PHLuaGetWeakRef(L, this);
        if (lua_istable(L, -1))
        {
            lua_getfield(L, -1, "dieCallback");
            if (lua_isfunction(L, -1))
            {
                lua_pushvalue(L, -2);
                PHLuaCall(L, 1, 0);
                lua_pop(L, 1);
                return;
            }
            lua_pop(L, 1);
        } 
        lua_pop(L, 1);
    }
    removeFromWorld();
}

//--- Lua Scripting ---

PHLuaNumberGetter(IGMob, health);
PHLuaNumberSetter(IGMob, setHealth);
PHLuaNumberSetter(IGMob, loseHealth);
PHLuaDefineCall(IGMob, die);

void IGMob::loadLuaInterface(IGScripting * s)
{
    lua_State * L = s->luaState();
    lua_getglobal(L, "IGMob");

    PHLuaAddMethod(IGMob, health);
    PHLuaAddMethod(IGMob, setHealth);
    PHLuaAddMethod(IGMob, loseHealth);
    PHLuaAddMethod(IGMob, die);

    lua_pop(L, 1);
}
