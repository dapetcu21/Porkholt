/*
 *  PHCaptureView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Home. All rights reserved.
 *
 */

#include "PHCaptureView.h"

void PHCaptureView::touchEvent(PHTouch * touch)
{
	if (!l) return;
	if (touch->phase() == PHTouch::touchMovedState)
	{
		PHPoint p1,p2,p;
		p1 = toMyCoordinates(touch->location());
		p2 = toMyCoordinates(touch->lastLocation());
		p.x = p1.x-p2.x;
		p.y = p1.y-p2.y;
		p.x/=_bounds.height;
		p.y/=_bounds.height;
		mutex->lock();
		l->push_back(p);
		mutex->unlock();
	}
}

void PHCaptureView::render()
{
	if (*paused)
		sm1->trywait();
	else
		sm1->wait();
	
	if (mutex)
		mutex->lock();
	PHView::render();
	if (mutex)
		mutex->unlock();
	
	if (!(*paused))
		sm2->signal();
}