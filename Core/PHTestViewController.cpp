/*
 *  PHTestViewController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

void PHTestViewController::callback1(PHObject * sender, void * ud)
{
	if (navController)
		navController->popViewController();
}

void PHTestViewController::callback2(PHObject * sender, void * ud)
{
	PHTestViewController * vc = new PHTestViewController();
	vc->init();
	if (navController)
		navController->pushViewController(vc);
	vc->release();
}

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
	
	view4->setUserInput(true);
	view->addSubview(view4);
	view4->release();
	
	
	PHButtonView * button1 = new PHButtonView(PHMakeRect(0, 0, 40, 40));
	PHButtonView * button2 = new PHButtonView(PHMakeRect(50, 0, 40, 40));
	button1->setImage(PHImage::imageNamed("stop"));
	button1->setPressedImage(PHImage::imageNamed("start"));
	
	button2->setPressedImage(PHImage::imageNamed("stop"));
	button2->setImage(PHImage::imageNamed("start"));
	
	button1->setUpCallBack(this, (PHCallback)&PHTestViewController::callback1, NULL);
	button2->setUpCallBack(this, (PHCallback)&PHTestViewController::callback2, NULL);
	
	view->addSubview(button1);
	view->addSubview(button2);
	
	button1->release();
	button2->release();
	
	return view;
}

void PHTestViewController::updateScene(double timeElapsed)
{
	PHTilt tilt = PHMotion::sharedInstance()->getTilt();
	view2->setRotation(-tilt.roll); // "geostationary" view
	view4->setRotation(tilt.pitch);
}