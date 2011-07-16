//
//  PHScripting.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/27/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHScripting.h"
#include "PHWorld.h"
#include "PHLua.h"
#include "PHFileManager.h"
#include "PHScriptableTimer.h"
#include "PHLObject.h"
#include "PHLevelController.h"
#include "PHLAnimation.h"
#include "PHLNPC.h"

PHScripting::PHScripting(PHWorld * _world,string level_dir) : world(_world)
{
    L = lua_open();
	luaL_openlibs(L);
    
	PHFileManager * fileMan = PHFileManager::singleton();
	string resourcePath = fileMan->resourcePath();
	
	PHLuaSetIncludePath(L, level_dir+"/?.lua;"+resourcePath+"/scripts/?.lua");
    
    if (PHLuaLoadFile(L, resourcePath+"/scripts/scripting_common.lua"))
    {
        loadWorld();
        for (vector<PHLObject*>::iterator i = world->objects.begin(); i!=world->objects.end(); i++)
        {
            PHLObject * obj = *i;
            obj->scriptingInit(L);
        }
        string path = level_dir+"/scripting.lua";
        if (PHFileManager::singleton()->fileExists(path))
            PHLuaLoadFile(L, path);
    }
    
}

PHScripting::~PHScripting()
{
    lua_close(L);
}

static int PHWorld_insertObject(lua_State * L)
{
    PHWorld * world = (PHWorld*)PHLuaThisPointer(L);
    int pos = 0;
    if (lua_isnumber(L, 3))
        pos = lua_tonumber(L, 3);
    PHLObject * obj2 = NULL;
    if (lua_isuserdata(L, 4))
        obj2 = (PHLObject*)lua_touserdata(L, 4);
    if (lua_istable(L, 2))
    {
        lua_getfield(L, 2, "class");
        string clss = "PHLObject";
        if (lua_isstring(L, -1))
            clss = lua_tostring(L, -1);
        lua_pop(L,1);
        PHLObject * obj = PHLObject::objectWithClass(clss);
        lua_pushvalue(L,2);
        obj->loadFromLua(L, world->levelController()->bundlePath(), world->getPhysicsWorld());
        lua_pop(L,1);
        obj->scriptingCreate(L);
        obj->defferedLoading(world, pos, obj2);
        obj->release();
    } else {
        lua_pushnil(L);
    }
    return 1;
}

void PHScripting::loadWorld()
{
    lua_getglobal(L,"PHWorld");
    lua_pushstring(L, "ud");
    lua_pushlightuserdata(L, world);
    lua_settable(L, -3);
    
    lua_pushcfunction(L,PHWorld_insertObject);
    lua_setfield(L, -2, "_insertObject");
    
    lua_pop(L, 1);
    
    PHScriptableTimer::registerLuaInterface(L);
    PHLObject::registerLuaInterface(L);
    PHLNPC::registerLuaInterface(L);
    PHLAnimation::registerLuaInterface(L);
}

void PHScripting::scriptingStep(double timeElapsed)
{
    
}