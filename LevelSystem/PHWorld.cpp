/*
 *  PHWorld.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

PHWorld::PHWorld(const PHRect & size) : view(NULL), camera(NULL), player(NULL)
{
	view = new PHView(PHMainEvents::sharedInstance()->screenBounds());
	worldView = new PHView(size);
	worldView->setUserInput(true);
	worldSize = size;
	view->addSubview(worldView);
}

PHWorld::~PHWorld()
{
	if (worldView)
	{
		worldView->removeFromSuperview();
		worldView->release();
	}
	if (view)
		view->release();
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
}

void PHWorld::addObject(PHLObject * obj)
{
	if (!obj) return;
	objects.push_back(obj);
	obj->retain();
	if (obj->getClass()=="PHLCamera")
		camera = (PHLCamera*)obj;
	if (obj->getClass()=="PHLPlayer")
		player = obj;
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