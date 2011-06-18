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

#endif