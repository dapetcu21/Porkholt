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
#include "PHLPlayer.h"
#include "PHLCamera.h"
#include "PHView.h"
#include "PHImageView.h"

PHScripting::PHScripting(PHWorld * _world,string level_dir) : world(_world)
{
    L = lua_open();
	luaL_openlibs(L);
    
	PHFileManager * fileMan = PHFileManager::singleton();
	string resourcePath = fileMan->resourcePath();
	
    world->setScripting(this);
    
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

static int PHWorld_fadeToColor(lua_State * L)
{
    PHWorld * world = (PHWorld*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    void * ud = NULL;
    if (lua_istable(L, 3))
    {
        ud = new uint8_t;
        lua_pushvalue(L, 3);
        PHLuaSetHardRef(L, ud);
    }
    world->fadeToColor(PHColor::colorFromLua(L, 2),ud);
    return 0;
}

void PHScripting::worldHasFadedAway(void *ud)
{
    if (!ud) return;
    lua_getglobal(L, "PHCallbackHelper");
    PHLuaGetHardRef(L, ud);
    PHLuaCall(L, 1, 0);
    delete (uint8_t*)ud;
}

static int PHWorld_dismissFading(lua_State * L)
{
    PHWorld * world = (PHWorld*)PHLuaThisPointer(L);
    void * ud = NULL;
    if (lua_istable(L, 2))
    {
        ud = new uint8_t;
        lua_pushvalue(L, 2);
        PHLuaSetHardRef(L, ud);
    }
    world->dismissFading(ud);
    return 0;
}

void PHScripting::loadWorld()
{
    lua_getglobal(L,"PHWorld");
    lua_pushstring(L, "ud");
    lua_pushlightuserdata(L, world);
    lua_settable(L, -3);
    
    lua_pushcfunction(L,PHWorld_insertObject);
    lua_setfield(L, -2, "_insertObject");
    lua_pushcfunction(L,PHWorld_fadeToColor);
    lua_setfield(L, -2, "_fadeToColor");
    lua_pushcfunction(L,PHWorld_dismissFading);
    lua_setfield(L, -2, "_dismissFading");
    
    lua_pop(L, 1);
    
    PHScriptableTimer::registerLuaInterface(L);
    PHLObject::registerLuaInterface(L);
    PHLNPC::registerLuaInterface(L);
    PHLAnimation::registerLuaInterface(L);
    PHLPlayer::registerLuaInterface(L);
    PHLCamera::registerLuaInterface(L);
    PHView::registerLuaInterface(L);
    PHImageView::registerLuaInterface(L);
}

void PHScripting::scriptingStep(double timeElapsed)
{
    
}