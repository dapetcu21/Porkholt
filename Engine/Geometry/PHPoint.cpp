//
//  PHPoint.cpp
//  Porkholt
//
//  Created by Marius Petcu on 2/1/12.
//  Copyright (c) 2012 Porkholt Labs!. All rights reserved.
//

#include "PHMain.h"
#include "PHPoint.h"
#include "PHRect.h"
#include "PHLua.h"

const PHPoint PHOriginPoint(0,0);
const PHSize PHUnitSize(1,1);

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
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
}
