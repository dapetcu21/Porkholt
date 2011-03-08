/*
 *  PHLevelController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHLua.h"

void PHLevelController::viewDidAppear()
{
	resume();
}

void PHLevelController::viewWillDisappear()
{
	pause();
}

void PHLevelController::pause()
{
	if (paused) return;
	paused = true;
	pauseMutex->lock();
}

void PHLevelController::resume()
{
	if (!paused) return;
	paused = false;
	pauseMutex->unlock();
}

void PHLevelController::test(PHButtonView * sender, void * ud)
{
	if (!paused)
	{
		pause();
		sender->setImage(PHImage::imageNamed("start"));
		sender->setPressedImage(PHImage::imageNamed("stop"));
	} else {
		resume();
		sender->setImage(PHImage::imageNamed("stop"));
		sender->setPressedImage(PHImage::imageNamed("start"));
	}
}

PHView * PHLevelController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	view->setUserInput(true);
	world = new PHWorld(PHMakeRect(0, 0, 1000, 1000));
	mutex = new PHMutex;
	pauseMutex = new PHMutex;
	view->addSubview(world->getView());
	thread = new PHThread;
	thread->setFunction(this,(PHCallback)&PHLevelController::auxThread, NULL);
	running = true;
	paused = false;
	thread->start();
	
	return view;
}

PHLevelController::PHLevelController(string path) : PHViewController(), world(NULL)
{
	directory = path;
}

void PHLevelController::updateScene(double timeElapsed)
{
	mutex->lock();
	if (world)
		world->updateScene(timeElapsed);
	mutex->unlock();
}

PHLevelController::~PHLevelController()
{
	if (world)
	{
		world->getView()->removeFromSuperview();
		world->release();
	}
	running = false;
	thread->join();
	mutex->release();
	pauseMutex->release();
}

void PHLevelController::auxThread(PHThread * sender, void * ud)
{
	mutex->lock();
	string dir = directory;
	mutex->unlock();
	
	int error;
	lua_State *L = lua_open();   /* opens Lua */
	luaL_openlibs(L);
    
	error = luaL_loadfile(L, (dir+"/init.lua").c_str()) || lua_pcall(L, 0, 0, 0);
	if (error) {
		PHLog("Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);  /* pop error message from the stack */
	} 
	
	lua_getglobal(L,"objects");
	
	int n = 0;

	if (lua_istable(L , -1))
	{
		lua_pushstring(L, "n");
		lua_gettable(L, -2);
		if (lua_isnumber(L, -1))
			n = lua_tonumber(L, -1);
		lua_pop(L,1);
		
		for (int i=0; i<n; i++)
		{
			lua_pushnumber(L, i);
			lua_gettable(L, -2);
			if (lua_istable(L , -1))
			{
				lua_pushstring(L, "class");
				lua_gettable(L, -2);
				string clss = "PHLObject";
				if (lua_isstring(L, -1))
					clss = lua_tostring(L, -1);
				lua_pop(L,1);
				
				PHLObject * obj = PHLObject::objectWithClass(clss);
				obj->loadFromLUA(L,dir);
				obj->loadView();
				mutex->lock();
				world->addObject(obj);
				obj->release();
				mutex->unlock();
			}
			lua_pop(L,1);
		}

	}
	lua_pop(L,1); 
	
	lua_close(L);
	
	while (running)
	{
		pauseMutex->lock();
		mutex->lock();
		
		PHLog("Hello!");
		
		mutex->unlock();
		pauseMutex->unlock();
		
		PHTime::sleep(1);
	}
}