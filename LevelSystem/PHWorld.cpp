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

PHWorld::PHWorld(const PHRect & size, PHLevelController * cntr) : view(NULL), camera(NULL), player(NULL), _jumpGauge(0.0f), maxJump(100), jumpGrowth(50), controller(cntr)
{
	PHRect bounds = PHMainEvents::sharedInstance()->screenBounds();
	view = new PHCaptureView(bounds);
	jumpGaugeView = new PHGaugeView(PHMakeRect(bounds.x+5, bounds.height-21, 256, 16));
	jumpGaugeView->setImage(PHImage::imageNamed("gauge"));
	PHMutex * mutex = cntr->mutex;
	view->setMutex(mutex);
	view->setQueue(&eventQueue);
	view->setUserInput(true);
	view->setSemaphores(cntr->pSem1, cntr->pSem2);
	view->setPaused(&cntr->paused);
	worldView = new PHView(size);
	layerView = new PHView(bounds);
	worldSize = size;
	view->addSubview(layerView);
	view->addSubview(worldView);
	//view->addSubview(jumpGaugeView);
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
	if (jumpGaugeView)
	{
		jumpGaugeView->removeFromSuperview();
		jumpGaugeView->release();
	}
	if (layerView)
	{
		layerView->removeFromSuperview();
		layerView->release();
	}
	if (view)
		view->release();
	delete physicsWorld;
}

void PHWorld::updateScene()
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
		pos.x=round(pos.x);
		pos.y=round(pos.y);
		worldView->setFrame(pos);
		worldView->setBounds(worldSize);
		for (list<layer>::iterator i = layers.begin(); i!=layers.end(); i++)
		{
			layer & ly = *i;
			PHRect pos = camera->size();
			PHPoint ps = camera->position();
			ps.x -= pos.width/2;			
			ps.y -= pos.height/2;
			double scaleX = view->bounds().width / pos.width * ly.scale;
			double scaleY = view->bounds().height / pos.height * ly.scale;
			pos.x = -ps.x * scaleX;
			pos.y = -ps.y * scaleY;
			pos.width = worldSize.width * scaleX;
			pos.height = worldSize.height * scaleY;
			pos.x=round(pos.x);
			pos.y=round(pos.y);
			ly.container->setFrame(pos);
			ly.container->setBounds(worldSize);
		}
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

void PHWorld::addLayer(PHImage * img, PHRect pos, double scale)
{
	layer tmp;
	tmp.container = new PHView(worldSize);
	PHImageView * view  = new PHImageView(pos);
	view->setImage(img);
	tmp.container->addSubview(view);
	layerView->addSubview(tmp.container);
	view->release();
	tmp.container->release();
	tmp.pos = pos;
	tmp.scale = scale;
	layers.push_back(tmp);
}