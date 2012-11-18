/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "IGScripting.h"
#include "IGWorld.h"
#include "IGObject.h"
#include <Porkholt/Core/PHLua.h>
#include <Porkholt/Core/PHView.h>
#include <Porkholt/Core/PHAutoreleasePool.h>
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/IO/PHDirectory.h>

map<string, IGScriptingAllocator> * IGScripting::luaClasses;
list<IGScriptingIface> * IGScripting::luaInterfaces;

IGScripting::IGScripting(PHGameManager * _gm, PHDirectory * _dir, IGWorld * _world) : gm(_gm), dir(_dir), world(_world)
{
    PHAutoreleasePool pool;

    dir->retain();
    world->retain();

    L = lua_open();
    luaL_openlibs(L);

    PHDirectory * scripts = gm->resourceDirectory()->directoryAtPath("scripts");
    PHLuaAddIncludeDir(L, scripts);
    if (PHLuaLoadFile(L, scripts, "common.lua"))
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

PHPoint IGScripting::screenSize()
{
    return world->view()->bounds().size();
}

void IGScripting::classFromName(const string & s)
{
    map<string, IGScriptingAllocator>::iterator i = luaClasses->find(s);
    if (i == luaClasses->end())
    {
        PHLog("No class named \"%s\" found", s.c_str());
        lua_pushnil(L);
        return;
    }
    IGObject * obj = i->second(world);
    obj->getLuaObject(this);
}

static int IGScripting_classFromName(lua_State * L)
{
    IGScripting * s = (IGScripting*)PHLuaThisPointer(L); 
    s->classFromName(string(lua_tostring(L, 2)));
    return 1;
}

static int IGScripting_localPoint(lua_State * L)
{
    PHPoint p = PHPoint::fromLua(L, 1); 
    PHPoint pos = PHPoint::fromLua(L, 2); 
    float ang = lua_tonumber(L, 3);
    ((p-pos).rotated(-ang)).saveToLua(L);
    return 1;
}

static int IGScripting_worldPoint(lua_State * L)
{
    PHPoint p = PHPoint::fromLua(L, 1); 
    PHPoint pos = PHPoint::fromLua(L, 2); 
    float ang = lua_tonumber(L, 3);
    (pos + p.rotated(ang)).saveToLua(L);
    return 1;
}

static int IGScripting_PHLog(lua_State * L)
{
    PHLog("Lua: %s", lua_tostring(L, 1));
    return 0;
}

PHLuaPointGetter(IGScripting, screenSize);

void IGScripting::loadCInterface()
{
    lua_getglobal(L, "IGScripting");
    lua_pushlightuserdata(L, this);
    lua_setfield(L, -2, "ud");

    PHLuaAddMethod_(IGScripting, classFromName);
    PHLuaAddMethod(IGScripting, screenSize);

    lua_pop(L, 1);

    lua_pushvalue(L, LUA_GLOBALSINDEX);
    PHLuaAddMethod(IGScripting, worldPoint);
    PHLuaAddMethod(IGScripting, localPoint);
    PHLuaAddMethod_(IGScripting, PHLog);
    lua_pop(L, 1);

    for (list<IGScriptingIface>::iterator i = luaInterfaces->begin(); i != luaInterfaces->end(); i++)
        (*i)(this);
}

void IGScripting::advanceAnimation(ph_float elapsed)
{
    lua_getglobal(L, "frame");
    lua_pushnumber(L, elapsed);
    PHLuaCall(L, 1, 0);
}

