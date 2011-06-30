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

PHScripting::PHScripting(PHWorld * _world,string level_dir) : world(_world)
{
    int error;
    L = lua_open();
	luaL_openlibs(L);
    
	PHFileManager * fileMan = PHFileManager::singleton();
	string resourcePath = fileMan->resourcePath();
	
	PHLuaSetIncludePath(L, level_dir+"/?.lua;"+resourcePath+"/scripts/?.lua");
    
    error = luaL_loadfile(L, (resourcePath+"/scripts/scripting_common.lua").c_str()) || lua_pcall(L, 0, 0, 0);
    if (error) {
		PHLog("Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
    } else {
        loadWorld();
        for (vector<PHLObject*>::iterator i = world->objects.begin(); i!=world->objects.end(); i++)
        {
            PHLObject * obj = *i;
            obj->scriptingInit((void*)L);
        }
        string path = level_dir+"/scripting.lua";
        if (PHFileManager::singleton()->fileExists(path))
        {
            error = luaL_loadfile(L, path.c_str()) || lua_pcall(L, 0, 0, 0);
            if (error) {
                PHLog("Lua: %s",lua_tostring(L,-1));
                lua_pop(L, 1);
            }
        }
    }
    
}

PHScripting::~PHScripting()
{
    lua_close(L);
}

void PHScripting::loadWorld()
{
    lua_getglobal(L,"PHWorld");
    lua_pushstring(L, "ud");
    lua_pushlightuserdata(L, world);
    lua_settable(L, -3);
    
//    luaL_Reg functions[2];
//    functions[0].name = "scheduleTimer"; functions[0].func = PHWorld_scheduleTimer;
//    functions[1].name = NULL;  functions[1].func = NULL;
//    luaL_register(L,NULL,functions);
    
    lua_pop(L, 1);
    
    PHScriptableTimer::registerLuaInterface(L);
}

void PHScripting::scriptingStep(double timeElapsed)
{
    
}