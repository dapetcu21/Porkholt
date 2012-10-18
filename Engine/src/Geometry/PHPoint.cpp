/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHMain.h>
#include <Porkholt/Geometry/PHPoint.h>
#include <Porkholt/Geometry/PHRect.h>
#include <Porkholt/Core/PHLua.h>

const PHPoint PHOriginPoint(0,0);
const PHSize PHUnitSize(1,1);
const PH3DPoint PH3DOriginPoint(0,0,0);
const PH3DSize PH3DUnitSize(1,1,1);

PHPoint::PHPoint(const PHRect & o) : x(o.x), y(o.y){};

PHPoint PHPoint::fromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHOriginPoint;
    PHPoint pnt = PHOriginPoint;
    
    lua_getfield(L, index, "x");
    if (lua_isnumber(L, -1))
        pnt.x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "y");
    if (lua_isnumber(L, -1))
        pnt.y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    return pnt;
}

void PHPoint::saveToLua(lua_State * L) const
{
    lua_newtable(L);    
    lua_getglobal(L, "PHVector2_meta");
    lua_setmetatable(L, -2);
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
}

PH3DPoint PH3DPoint::fromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHOriginPoint;
    PH3DPoint pnt = PH3DOriginPoint;
    
    lua_getfield(L, index, "x");
    if (lua_isnumber(L, -1))
        pnt.x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "y");
    if (lua_isnumber(L, -1))
        pnt.y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "z");
    if (lua_isnumber(L, -1))
        pnt.z = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    return pnt;
}

void PH3DPoint::saveToLua(lua_State * L) const
{
    lua_newtable(L);    
    lua_getglobal(L, "PHVector3_meta");
    lua_setmetatable(L, -2); 
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, z);
    lua_setfield(L, -2, "z");
}
