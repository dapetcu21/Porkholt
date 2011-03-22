/*
 *  PHWorld.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"
#include "PHCaptureView.h"
#include "PHGaugeView.h"

PHWorld::PHWorld(const PHRect & size, PHMutex * mutex) : view(NULL), camera(NULL), player(NULL), _jumpGauge(0.0f), maxJump(100), jumpGrowth(50)
{
	PHRect bounds = PHMainEvents::sharedInstance()->screenBounds();
	view = new PHCaptureView(bounds);
	bounds.y = bounds.height-21;
	bounds.height= 16;
	bounds.x+= 5;
	bounds.width = 256;
	jumpGaugeView = new PHGaugeView(bounds);
	jumpGaugeView->setImage(PHImage::imageNamed("gauge"));
	controlsMutex = mutex;
	mutex->retain();
	view->setMutex(controlsMutex);
	view->setQueue(&eventQueue);
	view->setUserInput(true);
	worldView = new PHView(size);
	//worldView->setUserInput(true);
	worldSize = size;
	view->addSubview(worldView);
	view->addSubview(jumpGaugeView);
	b2Vec2 grav(0,-10);
	physicsWorld = new b2World(grav,true);
}

PHWorld::~PHWorld()
{
	list<PHLObject*> l = objects;
	for (list<PHLObject*>::iterator i = l.begin(); i!=l.end(); i++)
		
	if (worldView)
	{
		worldView->removeFromSuperview();
		worldView->release();
	}
	view->setQueue(NULL);
	view->setMutex(NULL);
	if (view)
		view->release();
	if (jumpGaugeView)
		jumpGaugeView->release();
	controlsMutex->release();
	delete physicsWorld;
}

void PHWorld::updateScene(double time)
{
	if (camera)
	{
		PHRect pos = camera->size();
		PHPoint ps = camera->position();
		ps.x -= pos.width/2;			
		ps.y -= pos.height/2;
		double scaleX = view->bounds().width / pos.width;
		double scaleY = view->bounds().height / pos.height;
		pos.x = -ps.x * scaleX;
		pos.y = -ps.y * scaleY;
		pos.width = worldSize.width * scaleX;
		pos.height = worldSize.height * scaleY;
		worldView->setFrame(pos);
		worldView->setBounds(worldSize);
	}
	//PHLog("jumpGauge:%f",_jumpGauge);
	jumpGaugeView->setLevel(_jumpGauge/maxJump);
}

void PHWorld::addObject(PHLObject * obj)
{
	if (!obj) return;
	objects.push_back(obj);
	obj->retain();
	if (obj->getClass()=="PHLCamera")
		camera = (PHLCamera*)obj;
	if (obj->getClass()=="PHLPlayer")
		player = (PHLPlayer*)obj;
	obj->wrld = this;
	worldView->addSubview(obj->getView());
}

void PHWorld::removeObject(PHLObject * obj)
{
	if (!obj) return;
	if (obj==camera)
		camera = NULL;
	if (obj==player)
		player = NULL;
	for (list<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
		if (*i == obj)
		{
			objects.erase(i);
			obj->wrld = NULL;
			obj->getView()->removeFromSuperview();
			obj->release();
			break;
		}
}

void PHWorld::removeAllObjects()
{
	camera = NULL;
	player = NULL;
	for (list<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
	{
		(*i)->wrld = NULL;
		(*i)->getView()->removeFromSuperview();
		(*i)->release();
	}
	objects.clear();
}