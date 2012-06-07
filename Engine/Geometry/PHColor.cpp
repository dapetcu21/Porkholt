/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHMain.h"
#include "PHColor.h"
#include "PHLua.h"

const PHColor PHClearColor(0,0,0,0);
const PHColor PHBlackColor(0,0,0,1);
const PHColor PHWhiteColor(1,1,1,1);
const PHColor PHGrayColor(0.5,0.5,0.5,1);
const PHColor PHInvalidColor(-1,-1,-1,-1);

PHColor PHColor::fromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHInvalidColor;
    PHColor color = PHWhiteColor;
    
    lua_getfield(L, index, "r");
    if (lua_isnumber(L, -1))
        color.r = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "g");
    if (lua_isnumber(L, -1))
        color.g = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "b");
    if (lua_isnumber(L, -1))
        color.b = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "a");
    if (lua_isnumber(L, -1))
        color.a = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return color;
}

void PHColor::saveToLua(lua_State * L) const
{
    lua_newtable(L);
    lua_pushnumber(L, r);
    lua_setfield(L, -2, "r");
    lua_pushnumber(L, g);
    lua_setfield(L, -2, "g");
    lua_pushnumber(L, b);
    lua_setfield(L, -2, "b");
    lua_pushnumber(L, a);
    lua_setfield(L, -2, "a");
}
