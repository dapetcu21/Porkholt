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
#include <Box2D/Box2D.h>

#define GAUGE_WIDTH 304
#define GAUGE_HEIGHT 19

PHWorld::PHWorld(const PHRect & size, PHLevelController * cntr) : view(NULL), camera(NULL), player(NULL), _jumpGauge(0.0f), maxJump(100), jumpGrowth(50), controller(cntr)
{
	PHRect bounds = PHMainEvents::sharedInstance()->screenBounds();
	view = new PHCaptureView(bounds);
	jumpGaugeView = new PHGaugeView(PHMakeRect(bounds.x+5, bounds.height-5-GAUGE_HEIGHT, GAUGE_WIDTH, GAUGE_HEIGHT));
	jumpGaugeView->setImage(PHImage::imageNamed("gauge"));
	PHMutex * mutex = cntr->mutex;
	view->setMutex(mutex);
	view->setQueue(&eventQueue);
	view->setUserInput(true);
	view->setSemaphores(cntr->pSem1, cntr->pSem2);
	view->setPaused(&cntr->paused);
	worldView = new PHView(size);
	worldView->setUserInput(true);
	worldView->setInputRouting(true);
	layerView = new PHView(bounds);
	worldSize = size;
	view->addSubview(layerView);
	view->addSubview(worldView);
	view->addSubview(jumpGaugeView);
	b2Vec2 grav(0,-10);
	physicsWorld = new b2World(grav,true);
}

PHWorld::~PHWorld()
{
	removeAllObjects();
    removeAllJoints();
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
			//pos.x=round(pos.x);
			//pos.y=round(pos.y);
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
	for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
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
	for (vector<PHLObject*>::iterator i = objects.begin(); i!=objects.end(); i++)
	{
		(*i)->wrld = NULL;
		(*i)->getView()->removeFromSuperview();
		(*i)->release();
	}
	objects.clear();
}

void PHWorld::addJoint(PHJoint * obj)
{
    if (!obj) return;
    joints.push_back(obj);
    obj->retain();
}
void PHWorld::removeJoint(PHJoint * obj)
{
    for (vector<PHJoint*>::iterator i = joints.begin(); i!=joints.end(); i++)
		if (*i == obj)
		{
            joints.erase(i);
            obj->release();
        }
}

void PHWorld::removeAllJoints()
{
    for (vector<PHJoint*>::iterator i = joints.begin(); i!=joints.end(); i++)
    {
        (*i)->release();
    }
    objects.clear();
}

PHWorld::layer * PHWorld::addLayer(double scale)
{
	layer tmp;
	tmp.container = new PHView(worldSize);
	layerView->addSubview(tmp.container);
	tmp.container->release();
	tmp.scale = scale;
	layers.push_back(tmp);
	return &(layers.back());
}

void PHWorld::addToLayer(layer * lyr, PHImageView * img)
{
	lyr->container->addSubview(img);
}