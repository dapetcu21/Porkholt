//
//  PHScriptableTimer.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/30/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHScriptableTimer.h"
#include "PHLua.h"
#include "PHWorld.h"

PHScriptableTimer::PHScriptableTimer()
{
    
}

PHScriptableTimer::~PHScriptableTimer()
{
    
}

void PHScriptableTimer::timerFired()
{
    lua_pushlightuserdata(L, (void*)this);
    lua_gettable(L, LUA_REGISTRYINDEX);

    lua_getfield(L, -1, "timerFired");
    lua_pushvalue(L, -2);
    if (lua_isfunction(L, -2))
    {
        int err = lua_pcall(L, 1, 0, 0);
        if (err)
        {
            PHLog("Lua: %s",lua_tostring(L, -1));
            lua_pop(L,1);
        }
    }

    lua_pop(L, 1);
}

static int PHTimer_schedule(lua_State *L)
{
    if ((lua_gettop(L)<1) || (!lua_istable(L, 1)))
    {
        lua_pushstring(L,"schedule() needs 1 table argument");
        return lua_error(L);
    }
    PHScriptableTimer * timer = new PHScriptableTimer;
    timer->setLuaState(L);
    
    lua_getfield(L, 1, "time");
    if (lua_isnumber(L,-1))
        timer->setTimeInterval(lua_tonumber(L,-1));
    lua_pop(L,1);

    lua_getfield(L, 1, "willrepeat");
    if (lua_isboolean(L,-1))
        timer->setRepeats(lua_toboolean(L,-1));
    lua_pop(L,1);
    
    lua_pushlightuserdata(L,(void*)timer);
    lua_setfield(L, 1, "ud");
    
    lua_pushlightuserdata(L, (void*)timer);
    lua_pushvalue(L, 1);
    lua_settable(L, LUA_REGISTRYINDEX);
    
    lua_getglobal(L, "PHWorld");
    lua_pushstring(L, "ud");
    lua_gettable(L,-2);
    ((PHWorld*)lua_touserdata(L, -1))->scheduleTimer(timer);
    lua_pop(L, 2);
    
    timer->release();
    
    return 0;
}

static int PHTimer_invalidate(lua_State *L)
{
    if ((lua_gettop(L)<1) || (!lua_istable(L, 1)))
    {
        lua_pushstring(L,"invalidate() needs 1 table argument");
        return lua_error(L);
    }
    
    lua_getfield(L, 1, "ud");
    if (!lua_isuserdata(L, -1))
    {
        lua_pushstring(L,"you need to run schedule() first");
        return lua_error(L);
    }
    PHScriptableTimer * timer = (PHScriptableTimer*)lua_touserdata(L, -1);
    lua_pop(L,1);
    if (!timer)
    {
        lua_pushstring(L,"WTF? NULL?");
        return lua_error(L);
    }
    timer->invalidate();
    
    return 0;
}

void PHScriptableTimer::registerLuaInterface(lua_State * L)
{
    lua_getglobal(L, "PHTimer");
    lua_pushcfunction(L, PHTimer_schedule);
    lua_setfield(L, -2, "schedule");
    lua_pushcfunction(L, PHTimer_invalidate);
    lua_setfield(L, -2, "invalidate");
    lua_pop(L,1);
}
