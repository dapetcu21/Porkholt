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
#include <Box2D/Box2D.h>



void PHLevelController::appSuspended()
{
	pause();
}

void PHLevelController::appResumed()
{
	resume();
}

void PHLevelController::pause()
{
	if (paused) return;
	paused = true;
	PHMainEvents::sharedInstance()->setIndependentTiming(false);
}

void PHLevelController::resume()
{
	if (!paused) return;
	paused = false;
	PHMainEvents::sharedInstance()->setIndependentTiming(true);
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
	PHMessage::messageWithName("appSuspended")->addListener(this,(PHCallback)&PHLevelController::appSuspended);
	PHMessage::messageWithName("appResumed")->addListener(this,(PHCallback)&PHLevelController::appResumed);
	
	PHView * view = new PHView(frame);
	view->setUserInput(true);
	mutex = new PHMutex;
	pSem1 = new PHSemaphore(0);
	pSem2 = new PHSemaphore(1);
	world = new PHWorld(PHMakeRect(0, 0, 1000, 1000),this);
	backgroundView = new PHImageView(frame);
	backgroundView->setImage(PHImage::imageFromPath(directory+"/bg.png"));
	view->addSubview(backgroundView);
	view->addSubview(world->getView());
	thread = new PHThread;
	thread->setFunction(this,(PHCallback)&PHLevelController::auxThread, NULL);
	running = true;
	paused = false;
	PHMainEvents::sharedInstance()->setIndependentTiming(true);
	thread->start();
	
	return view;
}

PHLevelController::PHLevelController(string path) : PHViewController(), world(NULL)
{
	directory = path;
}

void PHLevelController::updateScene(double timeElapsed)
{
//	mutex->lock();
//	mutex->unlock();
}

PHLevelController::~PHLevelController()
{
	if (backgroundView)
		backgroundView->release();
	running = false;
	thread->join();
	thread->release();
	if (world)
	{
		world->getView()->removeFromSuperview();
		world->release();
	}
	mutex->release();
	pSem1->release();
	pSem2->release();
	PHMessage::messageWithName("appSuspended")->removeListener(this);
	PHMessage::messageWithName("appResumed")->removeListener(this);
	PHMainEvents::sharedInstance()->setIndependentTiming(false);
}

void PHLevelController::auxThread(PHThread * sender, void * ud)
{
	mutex->lock();
	string dir = directory;
	b2World * fWorld = world->physicsWorld;
	mutex->unlock();
	
	int error;
	lua_State *L = lua_open();   /* opens Lua */
	luaL_openlibs(L);
    
	PHFileManager * fileMan = PHFileManager::singleton();
	string resourcePath = fileMan->resourcePath();
	
	PHLuaSetIncludePath(L, dir+"/?.lua;"+resourcePath+"/scripts/?.lua");
	
	error = luaL_loadfile(L, (dir+"/init.lua").c_str()) || lua_pcall(L, 0, 0, 0);
	if (error) {
		PHLog("Lua: %s",lua_tostring(L,-1));
		lua_pop(L, 1);  /* pop error message from the stack */
	} 
	
	lua_getglobal(L,"layers");
	
	int n = 0;
	
	if (lua_istable(L, -1))
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
			if (lua_istable(L, -1))
			{
				double scale = 1.0f;
				lua_pushstring(L, "scale");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					scale = lua_tonumber(L, -1);
				lua_pop(L, 1);
				
				int n = 0;
				lua_pushstring(L, "n");
				lua_gettable(L, -2);
				if (lua_isnumber(L, -1))
					n = lua_tonumber(L, -1);
				lua_pop(L,1);
				
				PHWorld::layer * lyr = NULL;
				
				for (int j=0; j<n; j++)
				{
					lua_pushnumber(L, j);
					lua_gettable(L, -2);
					if (lua_istable(L, -1))
					{
						bool isV = true;
						PHRect pos;
						string filename = dir + "/"; 
						lua_pushstring(L, "filename");
						lua_gettable(L, -2);
						if (isV = lua_isstring(L, -1))
							filename = filename + lua_tostring(L, -1);
						lua_pop(L, 1);
						
						if (isV)
						{
							lua_pushstring(L, "posX");
							lua_gettable(L, -2);
							if (lua_isnumber(L, -1))
								pos.x = lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							lua_pushstring(L, "posY");
							lua_gettable(L, -2);
							if (lua_isnumber(L, -1))
								pos.y = lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							lua_pushstring(L, "imgW");
							lua_gettable(L, -2);
							if (lua_isnumber(L, -1))
								pos.width = lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							lua_pushstring(L, "imgH");
							lua_gettable(L, -2);
							if (lua_isnumber(L, -1))
								pos.height = lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							PHRect portion = PHWholeRect;
							
							lua_pushstring(L, "texX");
							lua_gettable(L, -2);
							if (lua_isnumber(L, -1))
								portion.x = lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							lua_pushstring(L, "texY");
							lua_gettable(L, -2);
							if (lua_isnumber(L, -1))
								portion.y = lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							lua_pushstring(L, "texW");
							lua_gettable(L, -2);
							if (lua_isnumber(L, -1))
								portion.width = lua_tonumber(L, -1);
							lua_pop(L, 1);
							
							lua_pushstring(L, "texH");
							lua_gettable(L, -2);
							if (lua_isnumber(L, -1))
								portion.height = lua_tonumber(L, -1);
							lua_pop(L, 1);
					
							
							PHImage * img = PHImage::imageFromPath(filename);
							
							mutex->lock();
							if (!lyr)
								lyr = world->addLayer(scale);
							world->addToLayer(lyr, img, pos,portion);
							mutex->unlock();
							
						}						
					}
					lua_pop(L, 1);
				}
			}
			lua_pop(L, 1);
		}
	}
	lua_pop(L,1);
	
	lua_getglobal(L,"objects");
	
	n = 0;

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
				obj->loadFromLUA(L,dir,fWorld);
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
	
	mutex->lock();
	PHLPlayer * player = world->player;
	PHLCamera * camera = world->camera;
	list<PHPoint> * q = &world->eventQueue;
	mutex->unlock();
	
	double targetTime = PHTime::getTime();
//#define FRAME_TIMING
	
#ifdef FRAME_TIMING
	double ct = targetTime;
	int fcount = 0;
#endif
	
	int fps = PHMainEvents::sharedInstance()->framesPerSecond();
	double frameInterval = 1.0f/fps;
	
	while (running)
	{
		targetTime+=frameInterval;
		
		mutex->lock();
		player->updateControls(q);
		mutex->unlock();
		
		if (fps<=30)
			fWorld->Step(frameInterval, 10, 3);
		else
			fWorld->Step(frameInterval, 6, 2);

		fWorld->ClearForces();
		
		pSem2->wait();
		mutex->lock();
		for (list<PHLObject*>::iterator i = world->objects.begin(); i!=world->objects.end(); i++)
		{
			PHLObject * obj = *i;
			obj->updatePosition();
			obj->limitVelocity();
		}
		camera->updateCamera(player->position());
		world->updateScene();
		mutex->unlock();
		pSem1->signal();
		
		
		double currentTime = PHTime::getTime();
		double time = targetTime - currentTime;
		if (time>0)
			PHTime::sleep(time);
		else
			targetTime = currentTime;
#ifdef FRAME_TIMING		
		fcount++;
		if (fcount==fps)
		{
			fcount = 0;
			double ctt = PHTime::getTime();
			PHLog("%lf",ctt-ct);
			ct = ctt;
		}
#endif
	}
}