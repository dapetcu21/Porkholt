/*
 *  PHCaptureView.cpp
 *  Porkholt_iOS
 *
 *  Created by Marius Petcu on 3/21/11.
 *  Copyright 2011 Porkholt Labs!. All rights reserved.
 *
 */

#include "PHCaptureView.h"

void PHCaptureView::touchEvent(PHEvent * touch)
{
	if (!l) return;
	if (touch->type() == PHEvent::touchMoved)
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
    bool p = *paused;
	if (p || ((!p)&&lastPaused))
    {
		sm1->trywait();
        //PHLog("s1t1: trywait");
    }
	else
    {
        //PHLog("s1t1: wait");
		sm1->wait();
    }
    

	lastPaused = p;
	
	if (mutex)
		mutex->lock();
    PHThread::mainThread()->processQueue();
	PHView::render();
	if (mutex)
		mutex->unlock();
	
	if (!p)
    {
		sm2->signal();
        //PHLog("s2t1: signal");
    }
}