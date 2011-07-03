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

void PHLuaSetIncludePath(lua_State * L, string path);
void PHLuaGetWeakRef(lua_State * L, void * ref);
void PHLuaSetWeakRef(lua_State * L, void * ref);
void PHLuaGetHardRef(lua_State * L, void * ref);
void PHLuaGetWeakRef(lua_State * L, void * ref);
void * PHLuaThisPointer(lua_State * L);
#endif