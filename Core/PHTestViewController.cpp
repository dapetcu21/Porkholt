/*
 *  PHTestViewController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

PHView * PHTestViewController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	view->setBackgroundColor(PHGrayColor());
	view->setUserInput(true);
	
	view2 = new PHTestView(PHMakeRect(50, 50, 100, 100));
	view4 = new PHTestView(PHMakeRect(150, 150, 100, 100));
	PHView * view3 = new PHTestView(PHMakeRect(0,0,30,30));
	view3->setCenter(view2->boundsCenter());
	view3->setUserInput(true);
	view2->addSubview(view3);
	view3->release();
	view2->setUserInput(true);
	view->addSubview(view2);
	view2->release();
	view->addSubview(view4);
	view4->setUserInput(true);
	view4->release();
	
	return view;
}

void PHTestViewController::updateScene(double timeElapsed)
{
	PHTilt tilt = PHMotion::sharedInstance()->getTilt();
	view2->setRotation(-tilt.roll); // "geostationary" view
	view4->setRotation(tilt.pitch);
}