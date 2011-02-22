/*
 *  PHViewController.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 2/22/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHMain.h"

PHViewController::PHViewController(const PHRect & frame)
{
	_frame = frame;
}

PHViewController::PHViewController()
{
	_frame = PHMainEvents::sharedInstance()->screenBounds();
}

PHView * PHViewController::loadView(const PHRect & frame)
{
	PHView * view = new PHView(frame);
	return view;
}

void PHViewController::init()
{
	view = loadView(_frame);
}

void PHViewController::updateScene(double timeElapsed)
{
}