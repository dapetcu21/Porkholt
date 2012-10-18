/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Geometry/PHRect.h>
#include <Porkholt/Geometry/PHPoint.h>
#include <Porkholt/Core/PHLua.h>

const PHRect PHWholeRect(0,0,1,1);
const PHRect PHInvalidRect(0,0,-1,-1);
const PHRect PHNullRect(0,0,0,0);

PHRect PHRect::fromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHWholeRect;
    PHRect pnt = PHWholeRect;
    
    lua_getfield(L, index, "x");
    if (lua_isnumber(L, -1))
        pnt.x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "y");
    if (lua_isnumber(L, -1))
        pnt.y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "height");
    if (lua_isnumber(L, -1))
        pnt.height = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "width");
    if (lua_isnumber(L, -1))
        pnt.width = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    return pnt;
}

void PHRect::saveToLua(lua_State * L) const
{
    lua_newtable(L);    
    lua_getglobal(L, "PHRect_meta");
    lua_setmetatable(L, -2);
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, width);
    lua_setfield(L, -2, "width");
    lua_pushnumber(L, height);
    lua_setfield(L, -2, "height");
}
