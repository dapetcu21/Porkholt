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
#include "PHLMob.h"
#include "PHLBull.h"
#include "PHLSign.h"
#include "PHLPowerup.h"
#include "PHMessage.h"
#include "PHLBomberBird.h"
#include "PHLEggBomb.h"
#include "PHParticleAnimator.h"
#include "PHParticleView.h"
#include "PHKeyframeAnimatorGroup.h"

#ifdef PH_SIMULATOR
#include "PHLuaConsole.h"
#endif

PHScripting::PHScripting(PHWorld * _world,string level_dir) : world(_world)
{
    L = lua_open();
	luaL_openlibs(L);
    
	string resourcePath = _world->gameManager()->resourcePath();
	
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
        if (PHFileManager::fileExists(path))
            PHLuaLoadFile(L, path);
    }
    
#ifdef PH_SIMULATOR
    console = new PHLuaConsole(L);
#endif
}

PHScripting::~PHScripting()
{
#ifdef PH_SIMULATOR
    console->release();
#endif
    PHMessage::messageWithName("luaDestroy")->broadcast(this, L);
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
        obj->loadFromLua(L, world->getPhysicsWorld(),world->levelController());
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
    world->fadeToColor(PHColor::fromLua(L, 2),ud);
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

static int PHWorld_overlayText(lua_State * L)
{
    PHWorld * world = (PHWorld*)PHLuaThisPointer(L);
    luaL_checkstring(L, 2);
    luaL_checknumber(L, 3);
    world->overlayText(lua_tostring(L, 2), lua_tonumber(L, 3));
    return 0;
}

PHLuaStringGetter(PHWorld, resourcePath);

static int PHWorld_win(lua_State * L)
{
    PHWorld * world = (PHWorld*)PHLuaThisPointer(L);
    world->levelController()->endLevelWithOutcome(PHLevelController::LevelWon);
    return 0;
}

static int PHWorld_die(lua_State * L)
{
    PHWorld * world = (PHWorld*)PHLuaThisPointer(L);
    world->levelController()->endLevelWithOutcome(PHLevelController::LevelDied);
    return 0;
}

static int PHWorld_curtainText(lua_State * L)
{
    PHWorld * world = (PHWorld*)PHLuaThisPointer(L);
    luaL_checkstring(L, 2);
    lua_State * l = NULL;
    if (lua_istable(L, 3))
        l = L;
    world->levelController()->curtainText(lua_tostring(L, 2),l);
    return 0;
}

static int PHWorld_boom(lua_State * L)
{
    PHWorld * world = (PHWorld*)PHLuaThisPointer(L);
    luaL_checktype(L, 2, LUA_TTABLE);
    PHPoint loc = PHPoint::fromLua(L, 2);
    double magnitude = 1;
    double damage = 1;
    double radius = 1;
    if (lua_isnumber(L, 3))
        magnitude = lua_tonumber(L, 3);
    if (lua_isnumber(L, 4))
        damage = lua_tonumber(L, 4);
    if (lua_isnumber(L, 5))
        radius = lua_tonumber(L, 5);
    world->boom(loc,magnitude,damage,radius);
    return 0;
}

void PHScripting::loadWorld()
{
    lua_getglobal(L,"PHWorld");
    lua_pushstring(L, "ud");
    lua_pushlightuserdata(L, world);
    lua_settable(L, -3);
    
    PHLuaAddMethod_(PHWorld, insertObject);
    PHLuaAddMethod_(PHWorld, fadeToColor);
    PHLuaAddMethod_(PHWorld, dismissFading);
    PHLuaAddMethod(PHWorld, overlayText);
    PHLuaAddMethod_(PHWorld, curtainText);
    PHLuaAddMethod(PHWorld, resourcePath);
    PHLuaAddMethod(PHWorld, win);
    PHLuaAddMethod(PHWorld, die);
    PHLuaAddMethod(PHWorld, boom);

    lua_pop(L, 1);
    
    PHKeyframeAnimatorGroup::registerLuaInterface(L);
    PHScriptableTimer::registerLuaInterface(L);
    PHLObject::registerLuaInterface(L);
    PHLNPC::registerLuaInterface(L);
    PHLAnimation::registerLuaInterface(L);
    PHLPlayer::registerLuaInterface(L);
    PHLCamera::registerLuaInterface(L);
    PHView::registerLuaInterface(L);
    PHImageView::registerLuaInterface(L);
    PHParticleView::registerLuaInterface(L);
    PHParticleAnimator::registerLuaInterface(L);
    PHLMob::registerLuaInterface(L);
    PHLBull::registerLuaInterface(L);
    PHLSign::registerLuaInterface(L);
    PHLPowerup::registerLuaInterface(L);
    PHLBomberBird::registerLuaInterface(L);
    PHLEggBomb::registerLuaInterface(L);
}

void PHScripting::scriptingStep(double timeElapsed)
{
    
}

#ifdef PH_SIMULATOR
void PHScripting::executeConsole()
{
    console->execute();
}
#endif