/*
 *  PHLua.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/6/11.
 *  Copyright 2011 Home. All rights reserved.
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