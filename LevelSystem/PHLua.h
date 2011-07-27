/*
 *  PHLua.h
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#ifndef PHLUA_H
#define PHLUA_H

#include "PHMain.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

bool PHLuaLoadFile(lua_State * L, string fname);
bool PHLuaCall(lua_State * L,int inargs, int outargs);

void PHLuaSetIncludePath(lua_State * L, string path);
void PHLuaGetWeakRef(lua_State * L, void * ref);
void PHLuaSetWeakRef(lua_State * L, void * ref);
void PHLuaDeleteWeakRef(lua_State * L, void * ref);
void PHLuaGetHardRef(lua_State * L, void * ref);
void PHLuaSetHardRef(lua_State * L, void * ref);
void PHLuaDeleteHardRef(lua_State * L, void * ref);
void * PHLuaThisPointer(lua_State * L);
void PHLuaSeedRandom(lua_State * L);

#define PHLuaAddMethod(classn,method) lua_pushcfunction(L, classn ## _ ## method); lua_setfield(L, -2, #method);
#define PHLuaAddMethod_(classn,method) lua_pushcfunction(L, classn ## _ ## method); lua_setfield(L, -2, "_" #method);

#define PHLuaDefineCall(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    o->method(); \
    return 0; \
}

#define PHLuaBoolSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checktype(L, 2, LUA_TBOOLEAN); \
    o->method(lua_toboolean(L, 2)); \
    return 0; \
}

#define PHLuaBoolGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    lua_pushboolean(L, o->method()); \
    return 1; \
}

#define PHLuaNumberSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checknumber(L, 2); \
    o->method(lua_tonumber(L, 2)); \
    return 0; \
}

#define PHLuaNumberGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    lua_pushnumber(L, o->method()); \
    return 1; \
}

#define PHLuaAngleGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    lua_pushnumber(L, PHWarp(-toDeg(o->method()),360)); \
    return 1; \
}

#define PHLuaAngleSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checknumber(L, 2); \
    o->method(-toRad(lua_tonumber(L, 2))); \
    return 0; \
}

#define PHLuaStringSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checkstring(L, 2); \
    o->method(lua_tostring(L, 2)); \
    return 0; \
}

#define PHLuaStringGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    lua_pushstring(L, (o->method()).c_str()); \
    return 1; \
}

#define PHLuaPointSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checktype(L, 2, LUA_TTABLE); \
    o->method(PHPoint::fromLua(L, 2)); \
    return 0; \
}

#define PHLuaPointGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    (o->method()).saveToLua(L); \
    return 1; \
}

#define PHLuaColorSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checktype(L, 2, LUA_TTABLE); \
    o->method(PHColor::fromLua(L, 2)); \
    return 0; \
}

#define PHLuaColorGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    (o->method()).saveToLua(L); \
    return 1; \
}

#define PHLuaRectSetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    luaL_checktype(L, 2, LUA_TTABLE); \
    o->method(PHRect::fromLua(L, 2)); \
    return 0; \
}

#define PHLuaRectGetter(classn,method) \
static int classn ## _ ## method (lua_State * L) \
{ \
    classn * o = (classn *)PHLuaThisPointer(L); \
    (o->method()).saveToLua(L); \
    return 1; \
}

#define PHLuaGetNumberField(var,name) \
lua_getfield(L, -1, name); \
if (lua_isnumber(L, -1)) \
    var = lua_tonumber(L, -1); \
lua_pop(L,1)

#define PHLuaGetBoolField(var,name) \
lua_getfield(L, -1, name); \
if (lua_isboolean(L, -1)) \
    var = lua_toboolean(L, -1); \
lua_pop(L,1)

#endif