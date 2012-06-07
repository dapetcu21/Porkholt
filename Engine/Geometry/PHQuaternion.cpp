/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHMain.h"
#include "PHQuaternion.h"
#include "PHLua.h"

const PHQuaternion PHIdentityQuaternion(1,0,0,0);

PHMatrix PHQuaternion::rotationMatrix()
{
	return PHMatrix(q0*q0 + q1*q1 - q2*q2 - q3*q3, 2*(q2*q1 + q0*q3), 2*(q3*q1 - q0*q2), 0,
	 				2*(q1*q2 - q0*q3), q0*q0 - q1*q1 + q2*q2 - q3*q3, 2*(q2*q3 + q0*q1), 0,
					2*(q1*q3 + q0*q2), 2*(q2*q3 - q0*q1), q0*q0 - q1*q1 - q2*q2 + q3*q3, 0,
					0, 0, 0, q0*q0 + q1*q1 + q2*q2 + q3*q3);
}

PHQuaternion PHQuaternion::fromLua(lua_State * L, int index)
{
    if (!lua_istable(L, index)) return PHIdentityQuaternion;
    PHQuaternion q = PHIdentityQuaternion;
    
    lua_getfield(L, index, "r");
    if (lua_isnumber(L, -1))
        q.r = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "x");
    if (lua_isnumber(L, -1))
        q.x = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "y");
    if (lua_isnumber(L, -1))
        q.y = lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, index, "z");
    if (lua_isnumber(L, -1))
        q.z = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return q;
}

void PHQuaternion::saveToLua(lua_State * L) const
{
    lua_newtable(L);
    lua_pushnumber(L, r);
    lua_setfield(L, -2, "r");
    lua_pushnumber(L, x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, z);
    lua_setfield(L, -2, "z");
}
