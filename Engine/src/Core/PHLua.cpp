/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/IO/PHFile.h>
#include <Porkholt/IO/PHDirectory.h>

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

void PHLuaDeleteHardRef(lua_State * L, void * ref)
{
    lua_pushlightuserdata(L,ref);
    lua_pushnil(L);
    lua_settable(L,LUA_REGISTRYINDEX);
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

void PHLuaDeleteWeakRef(lua_State * L, void * ref)
{
    lua_getfield(L,LUA_REGISTRYINDEX,"ph");
    lua_pushlightuserdata(L,ref);
    lua_pushnil(L);
    lua_settable(L,-3);
    lua_pop(L,1);
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
        return NULL;
    }
    void * p = lua_touserdata(L, -1);
    lua_pop(L,1);
    return p;
}

bool PHLuaLoadFile(lua_State * L, string fname)
{
    int error = luaL_loadfile(L, fname.c_str()) || lua_pcall(L,0,0,0);
    if (error)
    {
        PHLog("Lua: %s",lua_tostring(L,-1));
        lua_pop(L, 1);
        return false;
    }
    return true;
}

bool PHLuaLoadFile(lua_State * L, PHDirectory * dir, const string & fname)
{
    PHAutoreleasePool ap;
    try { return PHLuaLoadFile(L, dir->fileAtPath(fname)); }
    catch(const string & ex) {
        PHLog("Lua: Can't load \"%s\": %s", fname.c_str(), ex.c_str()); 
    }
    return false;
}

bool PHLuaLoadFile(lua_State * L, PHFile * file)
{
    uint8_t * buf;
    size_t sz;
    try {
        buf = file->loadToBuffer(sz);
    } catch(string e) {
        PHLog(e);
        return false;
    }
    int error = luaL_loadbuffer(L, (const char*)buf, sz, file->name().c_str()) || lua_pcall(L,0,0,0);
    delete[] buf;
    if (error)
    {
        PHLog("Lua: %s",lua_tostring(L,-1));
        lua_pop(L, 1);
        return false;
    }
    return true;
}

bool PHLuaLoadString(lua_State * L, string s)
{
    int error = luaL_loadstring(L, s.c_str()) || lua_pcall(L,0,0,0);
    if (error)
    {
        PHLog("Lua: %s",lua_tostring(L,-1));
        lua_pop(L, 1);
        return false;
    }
    return true;
}

bool PHLuaLoadString(lua_State * L, const char * s)
{
    int error = luaL_loadstring(L, s) || lua_pcall(L,0,0,0);
    if (error)
    {
        PHLog("Lua: %s",lua_tostring(L,-1));
        lua_pop(L, 1);
        return false;
    }
    return true;
}


bool PHLuaCall(lua_State * L,int inargs, int outargs)
{
    int error = lua_pcall(L, inargs, outargs, 0);
    if (error)
    {
        if (error) {
            PHLog("Lua: %s",lua_tostring(L,-1));
            lua_pop(L, 1);
        } 
        return false;
    }
    return true;
}

void PHLuaSeedRandom(lua_State * L)
{
    lua_getglobal(L, "math");
    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "randomseed");
        lua_pushnumber(L, time(NULL));
        PHLuaCall(L, 1, 0);
    }
    lua_pop(L, 1);
}

PHLuaCallback::PHLuaCallback(lua_State * l, int index) : L(l)
{
    if (!lua_isfunction(L, index))
        throw "this is not a valid Lua function";
    lua_pushvalue(L, index);
    PHLuaSetHardRef(L, this);
}

PHLuaCallback::~PHLuaCallback()
{
    PHLuaDeleteHardRef(L, this);
}

void PHLuaCallback::call()
{
    PHLuaGetHardRef(L, this);
    PHLuaCall(L, 0, 0);
    inv.call(this);
}
