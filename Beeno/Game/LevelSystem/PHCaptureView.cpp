/* Copyright (c) 2012 Marius Petcu, Porkholt Labs!. All rights reserved. */

#include "PHCaptureView.h"
#include <Porkholt/Core/PHGameManager.h>
#include <Porkholt/Core/PHAnimatorPool.h>
#include <Porkholt/Core/PHThreading.h>
#include <Porkholt/Core/PHDrawableCoordinates.h>

void PHCaptureView::setMutex(PHMutex * m) 
{ 
    if (m) 
        m->retain(); 
    if (mutex)
        mutex->release(); 
    mutex = m; 
}

PHCaptureView::~PHCaptureView() { if(mutex) mutex->release(); }

void PHCaptureView::touchEvent(PHEvent * event)
{
	if (!l) return;
    if (event->type() == PHEvent::touchDown)
        event->setHandled(true);
	if (event->type() == PHEvent::touchMoved)
	{
		PHPoint p1,p2,p;
		p1 = event->drawableLocation()->pointInView(this);
		p2 = event->lastDrawableLocation()->pointInView(this);
		p.x = p1.x-p2.x;
		p.y = p1.y-p2.y;
        ph_float dpi = gm->dotsPerInch();
		p.x/= dpi;
		p.y/= dpi;
		mutex->lock();
		l->push_back(p);
		mutex->unlock();
        event->setHandled(true);
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
    gm->animatorPool()->advanceAnimation(0);
    //PHThread::mainThread()->processQueue();
	PHView::render();
	if (mutex)
		mutex->unlock();
	
	if (!p)
    {
		sm2->signal();
        //PHLog("s2t1: signal");
    }
}
