/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGScripting.h"
#include "IGWorld.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/IO/PHDirectory.h>

IGScripting::IGScripting(PHGameManager * _gm, PHDirectory * _dir, IGWorld * _world) : gm(_gm), dir(_dir), world(_world)
{
    PHAutoreleasePool pool;

    dir->retain();
    world->retain();

    L = lua_open();
    luaL_openlibs(L);

    if (PHLuaLoadFile(L, gm->resourceDirectory()->directoryAtPath("scripts"), "common.lua"))
    {
        loadCInterface();
        PHLuaAddIncludeDir(L, dir);
        PHLuaLoadFile(L, dir, "main.lua");
    }
}

IGScripting::~IGScripting()
{
    lua_close(L);

    dir->release();
    world->release();
}

void IGScripting::loadCInterface()
{
}

void IGScripting::frame(ph_float elapsed)
{
    lua_getglobal(L, "frame");
    lua_pushnumber(L, elapsed);
    PHLuaCall(L, 1, 0);
}

