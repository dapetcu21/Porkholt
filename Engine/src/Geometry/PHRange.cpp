/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Geometry/PHRange.h>
#include <Porkholt/Core/PHLua.h>

const PHRange PHInvalidRange(-1,-1);

PHRange PHRange::fromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHInvalidRange;
    PHRange rng = PHInvalidRange;
    
    lua_getfield(L, index, "start");
    if (lua_isnumber(L, -1))
        rng.start = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "length");
    if (lua_isnumber(L, -1))
        rng.length = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    return rng;
}

void PHRange::saveToLua(lua_State * L) const
{
    lua_newtable(L);    
    lua_pushnumber(L, start);
    lua_setfield(L, -2, "start");
    lua_pushnumber(L, length);
    lua_setfield(L, -2, "length");
}
