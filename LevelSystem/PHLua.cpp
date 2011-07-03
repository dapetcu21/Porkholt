/*
 *  PHLua.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHLua.h"

void PHLuaSetIncludePath(lua_State * L, string path)
{
	lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );
	string cur_path(lua_tostring( L, -1 ));
	cur_path=cur_path+";"+path;
    lua_pop( L, 1 );
    lua_pushstring( L, cur_path.c_str());
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );
}

void PHLuaSetHardRef(lua_State * L, void * ref)
{
    lua_pushlightuserdata(L,ref);
    lua_insert(L,-2);
    lua_settable(L,LUA_REGISTRYINDEX);
}

void PHLuaGetHardRef(lua_State * L, void * ref)
{
    lua_pushlightuserdata(L,ref);
    lua_gettable(L,LUA_REGISTRYINDEX);
}

void PHLuaGetWeakRef(lua_State * L, void * ref)
{
    lua_getfield(L,LUA_REGISTRYINDEX,"ph");
    if (lua_isnil(L, -1)) return;
    lua_pushlightuserdata(L,ref);
    lua_gettable(L,-2);
    lua_remove(L,-2);
}

void PHLuaSetWeakRef(lua_State * L, void * ref)
{
    lua_getfield(L,LUA_REGISTRYINDEX,"ph");
    if (!lua_istable(L, -1))
    {
        lua_pop(L,1);
        lua_newtable(L);
        lua_pushvalue(L,-1);
        lua_setmetatable(L,-2);
        lua_pushstring(L,"v");
        lua_setfield(L,-2,"__mode");
        lua_pushvalue(L,-1);
        lua_setfield(L,LUA_REGISTRYINDEX,"ph");
    }
    lua_pushlightuserdata(L,ref);
    lua_pushvalue(L,-3);
    lua_settable(L,-3);
    lua_pop(L,2);
}

void * PHLuaThisPointer(lua_State * L)
{
    if (lua_gettop(L)<1 || !lua_istable(L, 1)) 
    {
        lua_pushstring(L, "this function requires the self argument");
        lua_error(L);
        return NULL;
    }
    lua_getfield(L, 1, "ud");
    if (!lua_isuserdata(L, -1))
    {
        lua_pop(L, 1);
        lua_pushstring(L, "this table doesn't have an \"ud\" field");
        lua_error(L);
    }
    void * p = lua_touserdata(L, -1);
    lua_pop(L,1);
    return p;
}