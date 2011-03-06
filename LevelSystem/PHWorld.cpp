/*
 *  PHWorld.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/5/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

PHWorld::PHWorld(const PHRect & size) : view(NULL)
{
	view = new PHView(PHMainEvents::sharedInstance()->screenBounds());
	worldView = new PHView(size);
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
	
}