//
//  PHScripting.cpp
//  Porkholt_iOS
//
//  Created by Marius Petcu on 6/27/11.
//  Copyright 2011 Porkholt Labs!. All rights reserved.
//

#include "PHMain.h"
#include "PHLua.h"

PHScripting::PHScripting(PHWorld * _world,string level_dir) : world(_world)
{
    int error;
	/*lua_State * */L = lua_open();
    //l = (void*)L;
	luaL_openlibs(L);
    
	PHFileManager * fileMan = PHFileManager::singleton();
	string resourcePath = fileMan->resourcePath();
	
	PHLuaSetIncludePath(L, level_dir+"/?.lua;"+resourcePath+"/scripts/?.lua");
    
    error = luaL_loadfile(L, (resourcePath+"/scripts/scripting_common.lua").c_str()) || lua_pcall(L, 0, 0, 0);
    if (error) {
		PHLog("Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);
    } else {
        loadLevelController();
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

void PHScripting::loadLevelController()
{
}

void PHScripting::scriptingStep(double timeElapsed)
{
    
}