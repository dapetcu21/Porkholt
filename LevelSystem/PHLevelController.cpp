/*
 *  PHLevelController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

void PHLevelController::test(PHButtonView * sender, void * ud)
{
	paused = !paused;
	if (paused)
	{
		pauseMutex->lock();
		sender->setImage(PHImage::imageNamed("start"));
		sender->setPressedImage(PHImage::imageNamed("stop"));
	} else {
		pauseMutex->unlock();
		sender->setImage(PHImage::imageNamed("stop"));
		sender->setPressedImage(PHImage::imageNamed("start"));
	}
}

PHView * PHLevelController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	view->setUserInput(true);
	PHWorld * world = new PHWorld(PHMakeRect(0, 0, 1000, 1000));
	mutex = new PHMutex;
	pauseMutex = new PHMutex;
	view->addSubview(world->getView());
	thread = new PHThread;
	thread->setFunction(this,(PHCallback)&PHLevelController::auxThread, NULL);
	running = true;
	paused = false;
	thread->start();
	
	PHButtonView * bt = new PHButtonView(PHMakeRect(50, 50, 50, 50));
	bt->setImage(PHImage::imageNamed("stop"));
	bt->setPressedImage(PHImage::imageNamed("start"));
	bt->setDownCallBack(this, (PHCallback)&PHLevelController::test, NULL);
	bt->setUserInput(true);
	view->addSubview(bt);
	bt->release();
	
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